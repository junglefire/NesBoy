#include "Mapper.h"
#include "CPUOpcodes.h"
#include "MapperNROM.h"
#include "MapperSxROM.h"
#include "MapperMMC3.h"
#include "MapperUxROM.h"
#include "MapperCNROM.h"
#include "MapperAxROM.h"
#include "MapperColorDreams.h"
#include "MapperGxROM.h"

NAMESPACE_BEGIN(sn)

NameTableMirroring Mapper::getNameTableMirroring() {
	return static_cast<NameTableMirroring>(m_cartridge.getNameTableMirroring());
}

std::unique_ptr<Mapper> Mapper::createMapper(MapperType mapper_t, sn::Cartridge& cart, VFUNC interrupt_cb, VVFUNC mirroring_cb) {
	std::unique_ptr<Mapper> ret(nullptr);
	switch (mapper_t) {
	case NROM:
		_info("Mapper type: NROM");
		ret.reset(new MapperNROM(cart));
		break;
	case SxROM:
		_info("Mapper type: SxROM");
		ret.reset(new MapperSxROM(cart, mirroring_cb));
		break;
	case UxROM:
		_info("Mapper type: UxROM");
		ret.reset(new MapperUxROM(cart));
		break;
	case CNROM:
		_info("Mapper type: CNROM");
		ret.reset(new MapperCNROM(cart));
		break;
	case MMC3:
		_info("Mapper type: MMC3");
		ret.reset(new MapperMMC3(cart, interrupt_cb, mirroring_cb));
		break;
	case AxROM:
		_info("Mapper type: AxROM");
		ret.reset(new MapperAxROM(cart, mirroring_cb));
		break;
	case ColorDreams:
		_info("Mapper type: ColorDreams");
		ret.reset(new MapperColorDreams(cart, mirroring_cb));
		break;
	case GxROM:
		_info("Mapper type: GxROM");
		ret.reset(new MapperGxROM(cart, mirroring_cb));
		break;
	default:
		_info("Mapper type is unsupported!");
		break;
	}
	return ret;
}

NAMESPACE_END(sn)
