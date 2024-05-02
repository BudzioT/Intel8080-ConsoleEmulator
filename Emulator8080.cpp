#include "Emulator8080.h"

#include <cstdio>

/* Handle unimplemented instructions */
void Emulator8080::unimplementedInstruction() {
    printf("Error: Unimplemented Instruction!");
    running = false;
}

/* Is the emulator running? */
bool Emulator8080::Running() {
    return running;
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
        case 0x58:
            e = b;
            break;
        /* Unimplemented */
        default:
            unimplementedInstruction();
            break;
    }

    ++pc;
}