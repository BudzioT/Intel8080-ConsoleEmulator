#include "Emulator8080.h"

#include <cstdio>

/* Handle unimplemented instructions */
void Emulator8080::unimplementedInstruction() {
    printf("Error: Unimplemented Instruction!");
    running = false;
}

/* Is the emulator running? */
bool Emulator8080::Running() const {
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

        /* Unimplemented */
        default:
            unimplementedInstruction();
            break;
    }

    ++pc;
}