#ifndef MAPPER_H
#define MAPPER_H

#include "CPUOpcodes.h"
#include "Cartridge.h"
#include "Define.h"

NAMESPACE_BEGIN(sn)

enum NameTableMirroring {
	Horizontal  = 0,
	Vertical	= 1,
	FourScreen  = 8,
	OneScreenLower,
	OneScreenHigher,
};

using VFUNC = std::function<void()>;
using VVFUNC = std::function<void(void)>;

enum MapperType {
	NROM  = 0,
	SxROM = 1,
	UxROM = 2,
	CNROM = 3,
	MMC3  = 4,
	AxROM = 7,
	ColorDreams = 11,
	GxROM = 66,
};


class Mapper {
public:
	Mapper(Cartridge& cart, MapperType t): m_cartridge(cart), m_type(t) {
	};

	virtual ~Mapper() = default;
	
	virtual void writePRG (Address addr, Byte value) = 0;
	virtual Byte readPRG (Address addr) = 0;
	virtual Byte readCHR (Address addr) = 0;
	virtual void writeCHR (Address addr, Byte value) = 0;
	virtual NameTableMirroring getNameTableMirroring();
	
	bool inline hasExtendedRAM() {
		return m_cartridge.hasExtendedRAM();
	}
	
	virtual void scanlineIRQ(){}
	
	static std::unique_ptr<Mapper> createMapper(MapperType mapper_t, Cartridge& cart, VFUNC interrupt_cb, VVFUNC mirroring_cb);

protected:
	Cartridge& m_cartridge;
	MapperType m_type;
};

NAMESPACE_END(sn)

#endif //MAPPER_H
