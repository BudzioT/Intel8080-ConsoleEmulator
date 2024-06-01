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
    std::string path = "D:\\Pobrane\\Github_projects\\Intel8080-ConsoleEmulator\\ROMS\\SpaceInvaders\\invaders.concatenated";

    FILE* file = fopen(path.c_str(), "rb");
    if (!file) {
        std::cerr << "Error: file not found" << std::endl;
        return 1;
    }

    fseek(file, 0, SEEK_END);
    unsigned int size = ftell(file);
    auto* buffer = new unsigned char[size];
    fseek(file, 0, SEEK_SET);

    fread(buffer, size, 1, file);
    fclose(file);

    int pc = 0;
    int line = 0;

    Emulator8080 emulator(buffer);
    while(emulator.ProgramCounter() < size) {
        printf("%d: ", ++line);
        disassembler(buffer, emulator.ProgramCounter());
        emulator.Emulate();
    }

    delete[] buffer;
    return 0;
}