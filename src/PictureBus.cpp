#include "PictureBus.h"

NAMESPACE_BEGIN(sn)

PictureBus::PictureBus(): m_palette(0x20), m_RAM(0x800), m_mapper(nullptr) {
}

Byte PictureBus::read(Address addr) {
	if (addr < 0x2000) {
		return m_mapper->readCHR(addr);
	} else if (addr < 0x3eff) {
		const auto index = addr & 0x3ff;

		// Name tables upto 0x3000, then mirrored upto 3eff
		auto normalizedAddr = addr;
		if (addr >= 0x3000) {
			normalizedAddr -= 0x1000;
		}

		if (NameTable0 >= m_RAM.size()) {
			return m_mapper->readCHR(normalizedAddr);
		} else if (normalizedAddr < 0x2400) { //NT0
			return m_RAM[NameTable0 + index];
		} else if (normalizedAddr < 0x2800) { //NT1
			return m_RAM[NameTable1 + index];
		} else if (normalizedAddr < 0x2c00) { //NT2
			return m_RAM[NameTable2 + index];
		} else { /* if (normalizedAddr < 0x3000)*/ //NT3
			return m_RAM[NameTable3 + index];
		}
	} else if (addr < 0x3fff) {
		auto paletteAddr = addr & 0x1f;
		return readPalette(paletteAddr);
	}
	return 0;
}

Byte PictureBus::readPalette(Byte paletteAddr) {
	// Addresses $3F10/$3F14/$3F18/$3F1C are mirrors of $3F00/$3F04/$3F08/$3F0C
	if (paletteAddr >= 0x10 && paletteAddr % 4 == 0) {
		paletteAddr = paletteAddr & 0xf;
	}
	return m_palette[paletteAddr];
}

void PictureBus::write(Address addr, Byte value) {
	if (addr < 0x2000) {
		m_mapper->writeCHR(addr, value);
	} else if (addr < 0x3eff) {
		const auto index = addr & 0x3ff;
		// Name tables upto 0x3000, then mirrored upto 3eff
		auto normalizedAddr = addr;
		if (addr >= 0x3000) {
			normalizedAddr -= 0x1000;
		}

		if (NameTable0 >= m_RAM.size()) {
			m_mapper->writeCHR(normalizedAddr, value);
		} else if (normalizedAddr < 0x2400) { //NT0
			m_RAM[NameTable0 + index] = value;
		} else if (normalizedAddr < 0x2800) { //NT1
			m_RAM[NameTable1 + index] = value;
		} else if (normalizedAddr < 0x2c00) { //NT2
			m_RAM[NameTable2 + index] = value;
		} else { //NT3
			m_RAM[NameTable3 + index] = value;
		}
	} else if (addr < 0x3fff) {
		auto palette = addr & 0x1f;
		// Addresses $3F10/$3F14/$3F18/$3F1C are mirrors of $3F00/$3F04/$3F08/$3F0C
		if (palette >= 0x10 && addr % 4 == 0) {
			palette = palette & 0xf;
		}
		m_palette[palette] = value;
	}
}

void PictureBus::updateMirroring() {
	switch (m_mapper->getNameTableMirroring()) {
	case Horizontal:
		NameTable0 = NameTable1 = 0;
		NameTable2 = NameTable3 = 0x400;
		_info("Horizontal Name Table mirroring set. (Vertical Scrolling)");
		break;
	case Vertical:
		NameTable0 = NameTable2 = 0;
		NameTable1 = NameTable3 = 0x400;
		_info("Vertical Name Table mirroring set. (Horizontal Scrolling)");
		break;
	case OneScreenLower:
		NameTable0 = NameTable1 = NameTable2 = NameTable3 = 0;
		_info("Single Screen mirroring set with lower bank.");
		break;
	case OneScreenHigher:
		NameTable0 = NameTable1 = NameTable2 = NameTable3 = 0x400;
		_info("Single Screen mirroring set with higher bank.");
		break;
	case FourScreen:
		NameTable0 = m_RAM.size();
		_info("FourScreen mirroring.");
		break;
	default:
		NameTable0 = NameTable1 = NameTable2 = NameTable3 = 0;
		_error("Unsupported Name Table mirroring: {}", m_mapper->getNameTableMirroring());
	}
}

bool PictureBus::setMapper(Mapper *mapper) {
	if (!mapper) {
		_error("Mapper argument is nullptr");
		return false;
	}

	m_mapper = mapper;
	updateMirroring();
	return true;
}

void PictureBus::scanlineIRQ() {
	m_mapper->scanlineIRQ();
}

NAMESPACE_END(sn)