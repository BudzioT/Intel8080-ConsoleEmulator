#include <iostream>
#include <cstdio>
#include <fstream>
#include "Emulator8080.h"

int main() {
    std::cout << "File path: " << std::endl;
    std::string path;
    std::cin >> path;

    FILE* file = fopen(path.c_str(), "rb");
    if (!file) {
        std::cerr << "Error: file not found" << std::endl;
        return 1;
    }

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    auto* buffer = new unsigned char[size];
    fseek(file, 0, SEEK_SET);

    fread(buffer, size, 1, file);
    fclose(file);

    Emulator8080 emulator(buffer);
    while(emulator.ProgramCounter() < size) {
        emulator.Emulate();
    }

    delete[] buffer;
    return 0;
}
