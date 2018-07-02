#pragma once

#include <array>
#include <cstdint>
#include <filesystem>

#include "exceptions.h"

class Chip8Cpu
{
public:
    Chip8Cpu();

    void load_rom(const std::filesystem::path& path);
    void step();
    void clear_screen();
    void reset();
    void count_down();

    struct {
        bool cls;
        bool draw;
    } flags{};

    using InterpreterFn = void(*)(Chip8Cpu&);

    static constexpr int screen_width = 64;
    static constexpr int screen_height = 32;

private:
    static constexpr std::uint16_t memory_size = 4096;
    static constexpr std::uint16_t stack_size = 16;
    static constexpr std::uint16_t reg_size = 16;
    static constexpr std::uint16_t keys_size = 16;

    static constexpr std::uint16_t max_rom_size = 0x1000 - 0x200;

    static const std::array<InterpreterFn, 16> instructions;
    std::uint16_t opcode = 0;
    std::uint16_t I = 0;
    std::uint16_t pc = 0x200;
    std::uint16_t stack[stack_size];
    std::uint8_t sp = 0;
    std::uint8_t V[reg_size]; //registers
    std::uint8_t delay_timer = 0;
    std::uint8_t sound_timer = 0;

public:
    std::uint8_t keys[keys_size] = {};
    std::uint8_t gfx[screen_height][screen_width] = {};

private:
    std::uint8_t memory[memory_size];
};

class InterpreterException
    : public Exception
{
public:
    using Exception::Exception;
};
