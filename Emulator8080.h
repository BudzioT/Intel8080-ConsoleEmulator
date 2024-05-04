#ifndef EMULATOR8080_H
#define EMULATOR8080_H

#include <cstdint>

struct ConditionCodes
{
    uint8_t z = 1;
    uint8_t s = 1;
    uint8_t p = 1;
    uint8_t cy = 1;
    uint8_t ac = 1;
};

class Emulator8080
{
public:
    void Emulate();
    bool Running() const;

private:
    void UnimplementedInstruction();
    uint8_t Parity(uint16_t ans) const;

private:
    uint8_t a, b, c, d, e, h, l;
    uint16_t sp, pc;
    uint8_t *memory;
    uint8_t intEnable;
    ConditionCodes cc;

    bool running;
};

#endif