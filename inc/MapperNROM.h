#ifndef MAPPERNROM_H
#define MAPPERNROM_H
#include "Mapper.h"

NAMESPACE_BEGIN(sn)

class MapperNROM : public Mapper {
public:
    MapperNROM(Cartridge& cart);
    void writePRG (Address addr, Byte value);
    Byte readPRG (Address addr);
    Byte readCHR (Address addr);
    void writeCHR (Address addr, Byte value);
private:
    bool m_oneBank;
    bool m_usesCharacterRAM;
    std::vector<Byte> m_characterRAM;

};

NAMESPACE_END(sn)

#endif // MAPPERNROM_H
