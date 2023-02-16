#ifndef MEMORY_H
#define MEMORY_H

#include <unordered_map>
#include <functional>
#include <vector>
#include <memory>
#include "Cartridge.h"
#include "Mapper.h"

namespace sn {

    enum IORegisters {
        PPUCTRL = 0x2000,
        PPUMASK,
        PPUSTATUS,
        OAMADDR,
        OAMDATA,
        PPUSCROL,
        PPUADDR,
        PPUDATA,
        OAMDMA = 0x4014,
        JOY1 = 0x4016,
        JOY2 = 0x4017,
    };
    
    struct IORegistersHasher {
        std::size_t operator()(sn::IORegisters const & reg) const noexcept {
            return std::hash<std::uint32_t>{}(reg);
        }
    };

    using WFunc = std::function<void(Byte)>;
    using RFunc = std::function<Byte(void)>;
    using WFuncMap = std::unordered_map<IORegisters, WFunc, IORegistersHasher>;
    using RFuncMap = std::unordered_map<IORegisters, RFunc, IORegistersHasher>;

    class MainBus {
        public:
            MainBus();
            Byte read(Address addr);
            void write(Address addr, Byte value);
            bool setMapper(Mapper* mapper);
            bool setWriteCallback(IORegisters reg, WFunc callback);
            bool setReadCallback(IORegisters reg, RFunc callback);
            const Byte* getPagePtr(Byte page);
        private:
            std::vector<Byte> m_RAM;
            std::vector<Byte> m_extRAM;
            Mapper* m_mapper;
            WFuncMap m_writeCallbacks;
            RFuncMap m_readCallbacks;;
    };

}; //sn

#endif // MEMORY_H
