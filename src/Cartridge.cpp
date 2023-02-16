#include "Cartridge.h"
#include "Mapper.h"

NAMESPACE_BEGIN(sn)

Cartridge::Cartridge() : m_nameTableMirroring(0), m_mapperNumber(0), m_extendedRAM(false) {
}

const std::vector<Byte>& Cartridge::getROM() {
	return m_PRG_ROM;
}

const std::vector<Byte>& Cartridge::getVROM() {
	return m_CHR_ROM;
}

Byte Cartridge::getMapper() {
	return m_mapperNumber;
}

Byte Cartridge::getNameTableMirroring() {
	return m_nameTableMirroring;
}

bool Cartridge::hasExtendedRAM() {
	// Some ROMs don't have this set correctly, plus there's 
	// no particular reason to disable it.
	return true;
}

bool Cartridge::loadFromFile(std::string path) {
	std::ifstream romFile(path, std::ios_base::binary|std::ios_base::in);
	if (!romFile) {
		_error("Could not open ROM file from path: {}, path");
		return false;
	}

	_info("Reading ROM from path: {}", path);
		
	//Header
	std::vector<Byte> header;
	header.resize(0x10);
	if (!romFile.read(reinterpret_cast<char*>(&header[0]), 0x10)) {
		_error("Reading iNES header failed...");
		return false;
	}

	if (std::string{&header[0], &header[4]} != "NES\x1A") {
		_error("Invalid NES image, magic number: `{:c} {:c} {:c} {:x}`, valid magic number: `N E S 1a`",
			header[0], header[1], header[2], int(header[3]));
		return false;
	}

	_info("Reading header, it dictates:");

	Byte banks = header[4];
	_info(" - 16KB PRG-ROM Banks: {}", +banks);
	if (!banks) {
		_error("ROM has no PRG-ROM banks. Loading ROM failed!");
		return false;
	}

	Byte vbanks = header[5];
	_info(" - 8KB CHR-ROM Banks: {}", +vbanks);

	if (header[6] & 0x8) {
		m_nameTableMirroring = NameTableMirroring::FourScreen;
		_info(" - Name Table Mirroring: `FourScreen`");
	} else {
		m_nameTableMirroring = header[6] & 0x1;
		_info(" - Name Table Mirroring: {}", (m_nameTableMirroring == 0 ? "Horizontal" : "Vertical"));
	}

	m_mapperNumber = ((header[6] >> 4) & 0xf) | (header[7] & 0xf0);
	_info(" - Mapper #: {}", +m_mapperNumber);

	m_extendedRAM = header[6] & 0x2;
	_info(" - Extended (CPU) RAM: {}", m_extendedRAM);

	if (header[6] & 0x4) {
		_error("Trainer is not supported.");
		return false;
	}

	if ((header[0xA] & 0x3) == 0x2 || (header[0xA] & 0x1)) {
		_error("PAL ROM not supported.");
		return false;
	} else {
		_info("ROM is NTSC compatible");
	}

	//PRG-ROM 16KB banks
	m_PRG_ROM.resize(0x4000 * banks);
	if (!romFile.read(reinterpret_cast<char*>(&m_PRG_ROM[0]), 0x4000 * banks)) {
		_error("Reading PRG-ROM from image file failed.");
		return false;
	}

	//CHR-ROM 8KB banks
	if (vbanks) {
		m_CHR_ROM.resize(0x2000 * vbanks);
		if (!romFile.read(reinterpret_cast<char*>(&m_CHR_ROM[0]), 0x2000 * vbanks)) {
			_error("Reading CHR-ROM from image file failed.");
			return false;
		}
	} else { 
		_info("Cartridge with CHR-RAM.");
	}

	return true;
}

NAMESPACE_END(sn)
