#include <iostream>
#include <cstdio>
#include <fstream>
#include "Emulator8080.h"
#include "Disassembler8080.h"

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    /*
    std::cout << "File path: " << std::endl;
    std::string path;
    std::cin >> path;
    */
    std::string path = "D:\\Pobrane\\Github_projects\\Intel8080-ConsoleEmulator\\ROMS\\cpudiag.bin";

    FILE* file = fopen(path.c_str(), "rb");
    if (!file) {
        std::cerr << "Error: file not found" << std::endl;
        return 1;
    }

    fseek(file, 0, SEEK_END);
    unsigned int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    auto* memory = new uint8_t [0x10000];
    uint8_t* buffer = &memory[0x100];
    fread(buffer, size, 1, file);
    fclose(file);

    int line = 0;

    /* Fix the stack pointer from 0x6ad to 0x7ad */
    buffer[368] = 0x7;

    Emulator8080 emulator(memory, 0x0100);
    while(emulator.ProgramCounter() < size) {
        printf("%d: ", ++line);
        emulator.Emulate();
        disassembler(memory, emulator.ProgramCounter());
    }

    delete[] memory;
    return 0;
}
