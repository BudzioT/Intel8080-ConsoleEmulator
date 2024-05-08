#include "Emulator8080.h"

#include <cstdio>

/* Handle unimplemented instructions */
void Emulator8080::UnimplementedInstruction() {
    printf("Error: Unimplemented Instruction!");
    running = false;
}

/* Is the emulator running? */
bool Emulator8080::Running() const {
    return running;
}

/* Check if the number of even bits is even */
uint8_t Emulator8080::Parity(uint16_t ans) {
    uint8_t num = ans & 0xFF; /* Take the lower bits */
    int size = 8;
    int count = 0;

    /* Check every bit. If it's 1, add to the even count */
    for (int i = 0; i < size; i++) {
        if (num & 0x1)
            ++count;
        num >>= 1;
    }

    /* Return whether count is odd or even */
    return (count & 0x1) == 0;
}

/* Set the basic flags */
void Emulator8080::setFlags(uint16_t ans) {
    cc.z = ((ans & 0xFF) == 0 ? 1 : 0);
    cc.s = ((ans & 0x80) ? 1 : 0);
    cc.p = Parity(ans & 0xFF);
}

/* Add a register to the accumulator */
void Emulator8080::addRegister(uint8_t reg) {
    uint16_t ans = a + static_cast<uint16_t>(reg);
    setFlags(ans);
    /* Set the rest of flags */
    cc.cy = ans > 0xFF;
    cc.ac = ((a & 0xF) + (reg & 0xF)) > 0xF;

    a = ans & 0xFF;
}

/* Add a register and the carry bit to the accumulator */
void Emulator8080::addRegisterCarry(uint8_t reg) {
    uint16_t ans = a + static_cast<uint16_t>(reg) + static_cast<uint16_t>(cc.cy);
    setFlags(ans);
    /* Set the rest of flags */
    cc.cy = ans > 0xFF;
    cc.ac = ((a & 0xF) + ((reg + cc.cy) & 0xF)) > 0xF;

    a = ans & 0xFF;
}

/* Subtract a register from the accumulator */
void Emulator8080::subtractRegister(uint8_t reg) {
    uint16_t ans = static_cast<uint16_t>(a) - reg;
    setFlags(ans);
    /* Set the rest of flags */
    cc.cy = a < reg;
    cc.ac = ((a & 0xF) < (b & 0xF));

    a = ans & 0xFF;
}

/* Subtract a register and the carry bit from the accumulator */
void Emulator8080::subtractRegisterBorrow(uint8_t reg) {
    uint16_t ans = static_cast<uint16_t>(a) - static_cast<uint16_t>(reg) - cc.cy;
    setFlags(ans);
    /* Set the rest of flags */
    cc.cy = a < (reg - cc.cy);
    cc.ac = ((a & 0xF) < ((reg - cc.cy) & 0xF));

    a = ans & 0xFF;
}

/* Increment the register */
void Emulator8080::incrementRegister(uint8_t& reg) {
    uint16_t ans = static_cast<uint16_t>(reg) + 1;
    setFlags(ans);
    /* Set the rest of flags */
    cc.ac = ((reg & 0xF) + 1) > 0xF;

    reg = ans & 0xFF;
}

/* Increment the register pair */
void Emulator8080::incrementRegPair(uint8_t& reg1, uint8_t& reg2) {
    uint16_t pair = ((reg1 << 8) | reg2);
    uint16_t ans = pair + 1;

    reg1 = (ans & 0xFF00) >> 8;
    reg2 = (ans & 0xFF);
}

/* Decrement the register */
void Emulator8080::decrementRegister(uint8_t& reg) {
    uint16_t ans = static_cast<uint16_t>(reg) - 1;
    setFlags(ans);
    /* Set the rest of flags */
    cc.ac = (reg & 0xF) > 1;

    reg = ans & 0xFF;
}

/* Decrement the register pair */
void Emulator8080::decrementRegPair(uint8_t& reg1, uint8_t& reg2) {
    uint16_t pair = ((reg1 << 8) | reg2);
    uint16_t ans = pair - 1;

    reg1 = (ans & 0xFF00) >> 8;
    reg2 = (ans & 0xFF);
}

/* Add a register pair to the H and L registers */
void Emulator8080::addPairToHL(uint8_t reg1, uint8_t reg2) {
    uint16_t pair = ((reg1 << 8) | reg2);
    uint16_t hl = ((h << 8) | l);
    uint32_t ans = pair + hl;

    cc.cy = ans > 0xFFFF;

    h = (ans & 0xFF00) >> 8;
    l = (ans & 0xFF);
}

/* Decimal adjust the accumulator */
void Emulator8080::decimalAdjustAcc() {
    uint16_t ans = a;
    if ((a & 0x0F) > 9 || cc.ac)
        ans += 6;

    if ((ans & 0xF0) > 0x90 || cc.cy)
        ans += 0x60;

    setFlags(ans);
    cc.cy = ans > 0xFF;
    cc.ac = ((a & 0x0F) > 9);

    a = ans & 0xFF;
}

/* Set the accumulator to the result of logical AND with a register */
void Emulator8080::logicalAndRegister(uint8_t reg) {
    uint16_t ans = a & reg;
    setFlags(ans);
    /* Reset the Carry flags */
    cc.cy = 0;
    cc.ac = 0;
    a = ans;
}

/* Set the accumulator to the result of logical XOR with a register */
void Emulator8080::logicalXOrRegister(uint8_t reg) {
    uint16_t ans = a ^ reg;
    setFlags(ans);
    /* Reset the Carry flags */
    cc.cy = 0;
    cc.ac = 0;
    a = ans;
}

/* Set the accumulator to the result of logical OR with a register */
void Emulator8080::logicalOrRegister(uint8_t reg) {
    uint16_t ans = a | reg;
    setFlags(ans);
    /* Reset the Carry flags */
    cc.cy = 0;
    cc.ac = 0;
    a = ans;
}

/* Compare the accumulator with a register, set flags */
void Emulator8080::compareRegister(uint8_t reg) {
    uint16_t ans = a - reg;
    setFlags(ans);
    /* Set the rest of flags */
    cc.cy = a < reg;
    cc.ac = ((a & 0x0F) < (reg & 0x0F));
}

/* Rotate content of the accumulator one place left, update the Carry flag */
void Emulator8080::rotateLeft() {
    uint8_t oldVal = a;
    a = (((oldVal & 0x80) >> 7) | (oldVal << 1));
    cc.cy = (a & 1);
}

/* Rotate content of the accumulator one place left,
 * set the LSB bit to the Carry flag, update it */
void Emulator8080::rotateLeftCarry() {
    uint8_t oldVal = a;
    a = ((cc.cy >> 7) | (oldVal << 1));
    cc.cy = (oldVal & 0x80);
}

/* Rotate content of the accumulator one place right, update the Carry flag */
void Emulator8080::rotateRight() {
    uint8_t oldVal = a;
    a = (((oldVal & 1) << 7) | (oldVal >> 1));
    cc.cy = (a & 0x80);
}

/* Rotate content of the accumulator one place right,
 * set the MSB to the Carry flag, update it */
void Emulator8080::rotateRightCarry() {
    uint8_t oldVal = a;
    a = ((cc.cy << 7) | (oldVal >> 1));
    cc.cy = (oldVal & 1);
}

void Emulator8080::call(uint8_t byte1, uint8_t byte2) {
    uint16_t nextIns = pc + 3;
    memory[sp - 1] = ((nextIns >> 8) & 0xFF);
    memory[sp - 2] = (nextIns & 0xFF);
    sp -= 2;
    pc = ((byte2 << 8) | byte1);
}


/* Emulate the 8080 using saved memory buffer */
void Emulator8080::Emulate() {
    unsigned char* opCode = &memory[pc];

    switch (*opCode) {
        /* NOP */
        case 0x00:
        case 0x10:
        case 0x20:
        case 0x30:
        case 0x08:
        case 0x18:
        case 0x28:
        case 0x38:
            break;

        case 0x01: /* LXI B, d16 */
            b = opCode[2];
            c = opCode[1];
            pc += 2;
            break;
        case 0x11: /* LXI D, d16 */
            d = opCode[2];
            e = opCode[1];
            pc += 2;
            break;
        case 0x21: /* LXI H, d16 */
            h = opCode[2];
            l = opCode[1];
            pc += 2;
            break;
        case 0x31: /* LXI SP, d16 */
            sp = (opCode[2] << 8) | opCode[1];
            pc += 2;
            break;

        case 0x40: /* MOV B, B */
            break;
        case 0x41: /* MOV B, C */
            b = c;
            break;
        case 0x42: /* MOV B, D */
            b = d;
            break;
        case 0x43: /* MOV B, E */
            b = e;
            break;
        case 0x44: /* MOV B, H */
            b = h;
            break;
        case 0x45: /* MOV B, L */
            b = l;
            break;
        case 0x46: /* MOV B, M */
            b = memory[(h << 8) | l];
            break;
        case 0x47: /* MOV B, A */
            b = a;
            break;
        case 0x48: /* MOV C, B */
            c = b;
            break;
        case 0x49: /* MOV C, C */
            break;
        case 0x4A: /* MOV C, D */
            c = d;
            break;
        case 0x4B: /* MOV C, E */
            c = e;
            break;
        case 0x4C: /* MOV C, H */
            c = h;
            break;
        case 0x4D: /* MOV C, L */
            c = l;
            break;
        case 0x4E: /* MOV C, M */
            c = memory[(h << 8) | l];
            break;
        case 0x4F: /* MOV C, A */
            c = a;
            break;
        case 0x50: /* MOV D, B */
            d = b;
            break;
        case 0x51: /* MOV D, C */
            d = c;
            break;
        case 0x52: /* MOV D, D */
            break;
        case 0x53: /* MOV D, E */
            d = e;
            break;
        case 0x54: /* MOV D, H */
            d = h;
            break;
        case 0x55: /* MOV D, L */
            d = l;
            break;
        case 0x56: /* MOV D, M */
            d = memory[(h << 8) | l];
            break;
        case 0x57: /* MOV D, A */
            d = a;
            break;
        case 0x58: /* MOV E, B */
            e = b;
            break;
        case 0x59: /* MOV E, C */
            e = c;
            break;
        case 0x5A: /* MOV E, D */
            e = d;
            break;
        case 0x5B: /* MOV E, E */
            break;
        case 0x5C: /* MOV E, H */
            e = h;
            break;
        case 0x5D: /* MOV E, L */
            e = l;
            break;
        case 0x5E: /* MOV E, M */
            e = memory[(h << 8) | l];
            break;
        case 0x5F: /* MOV E, A */
            e = a;
            break;
        case 0x60: /* MOV H, B */
            h = b;
            break;
        case 0x61: /* MOV H, C */
            h = c;
            break;
        case 0x62: /* MOV H, D */
            h = d;
            break;
        case 0x63: /* MOV H, E */
            h = e;
            break;
        case 0x64: /* MOV H, H */
            break;
        case 0x65: /* MOV H, L */
            h = l;
            break;
        case 0x66: /* MOV H, M */
            h = memory[(h << 8) | l];
            break;
        case 0x67: /* MOV H, A */
            h = a;
            break;
        case 0x68: /* MOV L, B */
            l = b;
            break;
        case 0x69: /* MOV L, C */
            l = c;
            break;
        case 0x6A: /* MOV L, D */
            l = d;
            break;
        case 0x6B: /* MOV L, E */
            l = e;
            break;
        case 0x6C: /* MOV L, H */
            l = h;
            break;
        case 0x6D: /* MOV L, L */
            break;
        case 0x6E: /* MOV L, M */
            l = memory[(h << 8) | l];
            break;
        case 0x6F: /* MOV L, A */
            l = a;
            break;
        case 0x70: /* MOV M, B */
            memory[(h << 8) | l] = b;
            break;
        case 0x71: /* MOV M, C */
            memory[(h << 8) | l] = c;
            break;
        case 0x72: /* MOV M, D */
            memory[(h << 8) | l] = d;
            break;
        case 0x73: /* MOV M, E */
            memory[(h << 8) | l] = e;
            break;
        case 0x74: /* MOV M, H */
            memory[(h << 8) | l] = h;
            break;
        case 0x75: /* MOV M, L */
            memory[(h << 8) | l] = l;
            break;
        case 0x77: /* MOV M, A */
            memory[(h << 8) | l] = a;
            break;
        case 0x78: /* MOV A, B */
            a = b;
            break;
        case 0x79: /* MOV A, C */
            a = c;
            break;
        case 0x7A: /* MOV A, D */
            a = d;
            break;
        case 0x7B: /* MOV A, E */
            a = e;
            break;
        case 0x7C: /* MOV A, H */
            a = h;
            break;
        case 0x7D: /* MOV A, L */
            a = l;
            break;
        case 0x7E: /* MOV A, M */
            a = memory[(h << 8) | l];
            break;
        case 0x7F: /* MOV A, A */
            break;

        case 0x80: /* ADD B */
            addRegister(b);
            break;
        case 0x81: /* ADD C */
            addRegister(c);
            break;
        case 0x82: /* ADD D */
            addRegister(d);
            break;
        case 0x83: /* ADD E */
            addRegister(e);
            break;
        case 0x84: /* ADD H */
            addRegister(h);
            break;
        case 0x85: /* ADD L */
            addRegister(l);
            break;
        case 0x86: /* ADD M */
            addRegister(memory[(h << 8) | l]);
            break;
        case 0x87: /* ADD A */
            addRegister(a);
            break;

        case 0x88: /* ADC B */
            addRegisterCarry(b);
            break;
        case 0x89: /* ADC C */
            addRegisterCarry(c);
            break;
        case 0x8A: /* ADC D */
            addRegisterCarry(d);
            break;
        case 0x8B: /* ADC E */
            addRegisterCarry(e);
            break;
        case 0x8C: /* ADC H */
            addRegisterCarry(h);
            break;
        case 0x8D: /* ADC L */
            addRegisterCarry(l);
            break;
        case 0x8E: /* ADC M */
            addRegisterCarry(memory[(h << 8) | l]);
            break;
        case 0x8F: /* ADC A */
            addRegisterCarry(a);
            break;

        case 0x90: /* SUB B */
            subtractRegister(b);
            break;
        case 0x91: /* SUB C */
            subtractRegister(c);
            break;
        case 0x92: /* SUB D */
            subtractRegister(d);
            break;
        case 0x93: /* SUB E */
            subtractRegister(e);
            break;
        case 0x94: /* SUB H */
            subtractRegister(h);
            break;
        case 0x95: /* SUB L */
            subtractRegister(l);
            break;
        case 0x96: /* SUB M */
            subtractRegister(memory[(h << 8) | l]);
            break;
        case 0x97: /* SUB A */
            subtractRegister(a);
            break;

        case 0x98: /* SBB B */
            subtractRegisterBorrow(b);
            break;
        case 0x99: /* SBB C */
            subtractRegisterBorrow(c);
            break;
        case 0x9A: /* SBB D */
            subtractRegisterBorrow(d);
            break;
        case 0x9B: /* SBB E */
            subtractRegisterBorrow(e);
            break;
        case 0x9C: /* SBB H */
            subtractRegisterBorrow(h);
            break;
        case 0x9D: /* SBB L */
            subtractRegisterBorrow(l);
            break;
        case 0x9E: /* SBB M */
            subtractRegisterBorrow(memory[(h << 8) | l]);
            break;
        case 0x9F: /* SBB A */
            subtractRegisterBorrow(a);
            break;

        case 0xA0: /* ANA B */
            logicalAndRegister(b);
            break;
        case 0xA1: /* ANA C */
            logicalAndRegister(c);
            break;
        case 0xA2: /* ANA D */
            logicalAndRegister(d);
            break;
        case 0xA3: /* ANA E */
            logicalAndRegister(e);
            break;
        case 0xA4: /* ANA H */
            logicalAndRegister(h);
            break;
        case 0xA5: /* ANA L */
            logicalAndRegister(l);
            break;
        case 0xA6: /* ANA M */
            logicalAndRegister(memory[(h << 8) | l]);
            break;
        case 0xA7: /* ANA A */
            logicalAndRegister(a);
            break;

        case 0xA8: /* XRA B */
            logicalXOrRegister(b);
            break;
        case 0xA9: /* XRA C */
            logicalXOrRegister(c);
            break;
        case 0xAA: /* XRA D */
            logicalXOrRegister(d);
            break;
        case 0xAB: /* XRA E */
            logicalXOrRegister(e);
            break;
        case 0xAC: /* XRA H */
            logicalXOrRegister(h);
            break;
        case 0xAD: /* XRA L */
            logicalXOrRegister(l);
            break;
        case 0xAE: /* XRA M */
            logicalXOrRegister(memory[(h << 8) | l]);
            break;
        case 0xAF: /* XRA A */
            logicalXOrRegister(a);
            break;

        case 0xB0: /* ORA B */
            logicalOrRegister(b);
            break;
        case 0xB1: /* ORA C */
            logicalOrRegister(c);
            break;
        case 0xB2: /* ORA D */
            logicalOrRegister(d);
            break;
        case 0xB3: /* ORA E */
            logicalOrRegister(e);
            break;
        case 0xB4: /* ORA H */
            logicalOrRegister(h);
            break;
        case 0xB5: /* ORA L */
            logicalOrRegister(l);
            break;
        case 0xB6: /* ORA M */
            logicalOrRegister(memory[(h << 8) | l]);
            break;
        case 0xB7: /* ORA A */
            logicalOrRegister(a);
            break;

        case 0xB8: /* CMP B */
            compareRegister(b);
            break;
        case 0xB9: /* CMP C */
            compareRegister(c);
            break;
        case 0xBA: /* CMP D */
            compareRegister(d);
            break;
        case 0xBB: /* CMP E */
            compareRegister(e);
            break;
        case 0xBC: /* CMP H */
            compareRegister(h);
            break;
        case 0xBD: /* CMP L */
            compareRegister(l);
            break;
        case 0xBE: /* CMP M */
            compareRegister(memory[(h << 8) | l]);
            break;
        case 0xBF: /* CMP A */
            compareRegister(a);
            break;

        case 0x06: /* MVI B, d8 */
            b = opCode[1];
            ++pc;
            break;
        case 0x16: /* MVI D, d8 */
            d = opCode[1];
            ++pc;
            break;
        case 0x26: /* MVI H, d8 */
            h = opCode[1];
            ++pc;
            break;
        case 0x36: /* MVI M, d8 */
            memory[(h << 8) | l] = opCode[1];
            ++pc;
            break;
        case 0x0E: /* MVI C, d8 */
            c = opCode[1];
            ++pc;
            break;
        case 0x1E: /* MVI E, d8 */
            e = opCode[1];
            ++pc;
            break;
        case 0x2E: /* MVI L, d8 */
            l = opCode[1];
            ++pc;
            break;
        case 0x3E: /* MVI A, d8 */
            a = opCode[1];
            ++pc;
            break;

        case 0x3A: /* LDA addr */
            a = memory[(opCode[2] << 8) | opCode[1]];
            pc += 2;
            break;

        case 0x0A: /* LDAX B */
            a = memory[(b << 8) | c];
            break;
        case 0x1A: /* LDAX D */
            b = memory[(d << 8) | e];
            break;

        case 0x2A: /* LHLD addr */
            l = memory[(opCode[2] << 8) | opCode[1]];
            h = memory[((opCode[2] << 8) | opCode[1]) + 1];
            pc += 2;
            break;

        case 0x32: /* STA addr */
            memory[(opCode[2] << 8) | opCode[1]] = a;
            pc += 2;
            break;

        case 0x02: /* STAX B */
            memory[(b << 8) | c] = a;
            break;
        case 0x12: /* STAX D */
            memory[(d << 8) | c] = a;
            break;

        case 0x22: /* SHLD addr */
            memory[(opCode[2] << 8) | opCode[1]] = l;
            memory[((opCode[2] << 8) | opCode[1]) + 1] = h;
            pc += 2;
            break;

        case 0xEB: /* XCHG */
            h = e;
            l = d;
            break;

        case 0xC6: /* ADI, d8 */
            addRegister(opCode[1]);
            ++pc;
            break;

        case 0xCE: /* ACI, d8 */
            addRegisterCarry(opCode[1]);
            ++pc;
            break;

        case 0xD6: /* SUI, d8 */
            subtractRegister(opCode[1]);
            ++pc;
            break;

        case 0xDE: /* SBI, d8 */
            subtractRegisterBorrow(opCode[1]);
            ++pc;
            break;

        case 0x03: /* INX B */
            incrementRegPair(b, c);
            break;
        case 0x13: /* INX D */
            incrementRegPair(d, e);
            break;
        case 0x23: /* INX H */
            incrementRegPair(h, l);
            break;
        case 0x33: /* INX SP */
            ++sp;
            break;

        case 0x09: /* DAD B */
            addPairToHL(b, c);
            break;
        case 0x19: /* DAD D */
            addPairToHL(d, e);
            break;
        case 0x29: /* DAD H */
            addPairToHL(h, l);
            break;
        case 0x39: /* DAD SP */
            addPairToHL((sp & 0xFF00) >> 8, sp & 0xFF);
            break;

        case 0x0B: /* DCX B */
            decrementRegPair(b, c);
            break;
        case 0x1B: /* DCX D */
            decrementRegPair(d, e);
            break;
        case 0x2B: /* DCX H */
            decrementRegPair(h, l);
            break;
        case 0x3B: /* DCX SP */
            --sp;
            break;

        case 0x04: /* INR B */
            incrementRegister(b);
            break;
        case 0x0C: /* INR C */
            incrementRegister(c);
            break;
        case 0x14: /* INR D */
            incrementRegister(d);
            break;
        case 0x1C: /* INR E */
            incrementRegister(e);
            break;
        case 0x24: /* INR H */
            incrementRegister(h);
            break;
        case 0x2C: /* INR L */
            incrementRegister(l);
            break;
        case 0x34: /* INR M */
            incrementRegister(memory[(h << 8) | l]);
            break;
        case 0x3C: /* INR A */
            incrementRegister(a);
            break;

        case 0x05: /* DCR B */
            decrementRegister(b);
            break;
        case 0x0D: /* DCR C */
            decrementRegister(c);
            break;
        case 0x15: /* DCR D */
            decrementRegister(d);
            break;
        case 0x1D: /* DCR E */
            decrementRegister(e);
            break;
        case 0x25: /* DCR H */
            decrementRegister(h);
            break;
        case 0x2D: /* DCR L */
            decrementRegister(l);
            break;
        case 0x35: /* DCR M */
            decrementRegister(memory[(h << 8) | l]);
            break;
        case 0x3D: /* DCR A */
            decrementRegister(a);
            break;

        case 0x27: /* DAA */
            decimalAdjustAcc();
            break;

        case 0xE6: /* ANI, d8 */
            logicalAndRegister(opCode[1]);
            ++pc;
            break;

        case 0xEE: /* XRI, d8 */
            logicalXOrRegister(opCode[1]);
            ++pc;
            break;

        case 0xF6: /* ORI, d8 */
            logicalOrRegister(opCode[1]);
            ++pc;
            break;

        case 0xFE: /* CPI, d8 */
            compareRegister(opCode[1]);
            ++pc;
            break;

        case 0x07: /* RLC */
            rotateLeft();
            break;
        case 0x0F: /* RRC */
            rotateRight();
            break;

        case 0x17: /* RAL */
            rotateLeftCarry();
            break;
        case 0x1F: /* RAR */
            rotateRightCarry();
            break;

        case 0x2F: /* CMA */
            a = ~a;
            break;

        case 0x37: /* STC */
            cc.cy = 1;
            break;

        case 0x3F: /* CMC */
            cc.cy = ~cc.cy;
            break;

        /* JMP, addr */
        case 0xC3:
        case 0xCB:
            pc = ((opCode[2] << 8) | opCode[1]);
            break;

        case 0xC2: /* JNZ, addr */
            if (cc.z == 0)
                pc = ((opCode[2] << 8) | opCode[1]);
            else
                pc += 2;
            break;
        case 0xCA: /* JZ, addr */
            if (cc.z == 1)
                pc = ((opCode[2] << 8) | opCode[1]);
            else
                pc += 2;
            break;
        case 0xD2: /* JNC, addr */
            if (cc.cy == 0)
                pc = ((opCode[2] << 8) | opCode[1]);
            else
                pc += 2;
            break;
        case 0xDA: /* JC, addr */
            if (cc.cy == 1)
                pc = ((opCode[2] << 8) | opCode[1]);
            else
                pc += 2;
            break;
        case 0xE2: /* JPO, addr */
            if (cc.p == 0)
                pc = ((opCode[2] << 8) | opCode[1]);
            else
                pc += 2;
            break;
        case 0xEA: /* JPE, addr */
            if (cc.p == 1)
                pc = ((opCode[2] << 8) | opCode[1]);
            else
                pc += 2;
            break;
        case 0xF2: /* JP, addr */
            if (cc.s == 0)
                pc = ((opCode[2] << 8) | opCode[1]);
            else
                pc += 2;
            break;
        case 0xFA: /* JM, addr */
            if (cc.s == 1)
                pc = ((opCode[2] << 8) | opCode[1]);
            else
                pc += 2;
            break;

        /* CALL, addr */
        case 0xCD:
        case 0xDD:
        case 0xED:
        case 0xFD:
            call(opCode[1], opCode[2]);
            break;

        case 0xC4: /* CNZ, addr */
            if (cc.z == 0)
                call(opCode[1], opCode[2]);
            else
                pc += 2;
            break;
        case 0xCC: /* CZ, addr */
            if (cc.z == 1)
                call(opCode[1], opCode[2]);
            else
                pc += 2;
            break;
        case 0xD4:  /* CNC, addr */
            if (cc.cy == 0)
                call(opCode[1], opCode[2]);
            else
                pc += 2;
            break;
        case 0xDC:  /* CC, addr */
            if (cc.cy == 1)
                call(opCode[1], opCode[2]);
            else
                pc += 2;
            break;
        case 0xE4: /* CPO, addr */
            if (cc.p == 0)
                call(opCode[1], opCode[2]);
            else
                pc += 2;
            break;
        case 0xEC: /* CPE, addr */
            if (cc.p == 1)
                call(opCode[1], opCode[2]);
            else
                pc += 2;
            break;
        case 0xF4: /* CP, addr */
            if (cc.s == 0)
                call(opCode[1], opCode[2]);
            else
                pc += 2;
            break;
        case 0xFC: /* CM, addr */
            if (cc.s == 1)
                call(opCode[1], opCode[2]);
            else
                pc += 2;
            break;

        /* Unimplemented */
        default:
            UnimplementedInstruction();
            break;
    }

    ++pc;
}