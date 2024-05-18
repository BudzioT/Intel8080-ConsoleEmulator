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

    void setFlags(uint16_t ans);

    void addRegister(uint8_t reg);
    void addRegisterCarry(uint8_t reg);
    void subtractRegister(uint8_t reg);
    void subtractRegisterBorrow(uint8_t reg);
    void addPairToHL(uint8_t reg1, uint8_t reg2);
    void decimalAdjustAcc();
    void incrementRegister(uint8_t& reg);
    static void incrementRegPair(uint8_t& reg1, uint8_t& reg2);
    void decrementRegister(uint8_t& reg);
    static void decrementRegPair(uint8_t& reg1, uint8_t& reg2);

    void logicalAndRegister(uint8_t reg);
    void logicalXOrRegister(uint8_t reg);
    void logicalOrRegister(uint8_t reg);
    void compareRegister(uint8_t reg);
    void rotateLeft();
    void rotateLeftCarry();
    void rotateRight();
    void rotateRightCarry();

    void call(uint8_t byte1, uint8_t byte2);
    void ret();
    void rst(int nnn);

    void pushPair(uint8_t reg1, uint8_t reg2);
    void pushPSW();
    void popPair(uint8_t& reg1, uint8_t& reg2);
    void popPSW();

    static uint8_t Parity(uint16_t ans);

private:
    uint8_t a, b, c, d, e, h, l;
    uint16_t sp, pc;
    uint8_t *memory;
    uint8_t intEnable;
    ConditionCodes cc;

    bool running;
};

#endif