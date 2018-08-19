#include "chip8.h"

#include <algorithm>
#include <fstream>

#include "utils/random.h"

static constexpr std::array<std::uint8_t, 80> chip8_fontset = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

std::array<Chip8Cpu::InterpreterFn, 16> Chip8Cpu::instructions = {
    [](Chip8Cpu& cpu) {
        switch (cpu.opcode) {
        // clear screen
        case 0x00E0:
            cpu.flags.cls = true;
            break;
        // return from subroutine
        case 0x00EE:
            if (cpu.sp == 0) {
                throw InterpreterException("Stack underflowed");
            }
            cpu.pc = cpu.stack[--cpu.sp];
            break;
        default:
            throw InterpreterException("Instruction 0x0NNN is unsupported");
        }
        cpu.pc += 2;
    },

    // 1NNN: jump to address NNN
    [](Chip8Cpu& cpu) { // 0x1
        cpu.pc = cpu.opcode & 0x0FFF;
    },
    
    // 2NNN: call subroutine at address NNN
    [](Chip8Cpu& cpu) { // 0x2
        if (cpu.sp == stack_size) {
            throw InterpreterException("Stack overflowed");
        }
        cpu.stack[cpu.sp++] = cpu.pc;
        cpu.pc = cpu.opcode & 0x0FFF;
    },
    
    // 3XNN: skip next instruction if VX == NN
    [](Chip8Cpu& cpu) { // 0x3
        const auto x = (cpu.opcode & 0x0F00) >> 8;
        if (cpu.V[x] == (cpu.opcode & 0x00FF)) {
            cpu.pc += 2;
        }
        cpu.pc += 2;
    },
    
    // 4XNN: skip next instruction if VX != NN
    [](Chip8Cpu& cpu) {
        const auto x = (cpu.opcode & 0x0F00) >> 8;
        if (cpu.V[x] != (cpu.opcode & 0x00FF)) {
            cpu.pc += 2;
        }
        cpu.pc += 2;
    },
    
    
    // 5XY0: skip next instruction if VX == VY
    [](Chip8Cpu& cpu) {
        const auto x = (cpu.opcode & 0x0F00) >> 8;
        const auto y = (cpu.opcode & 0x00F0) >> 4;
        if (cpu.V[x] == cpu.V[y]) {
            cpu.pc += 2;
        }
        cpu.pc += 2;
    },

    // 6XNN: set VX to NN
    [](Chip8Cpu& cpu) {
        const auto x = (cpu.opcode & 0x0F00) >> 8;
        cpu.V[x] = cpu.opcode & 0x00FF;
        cpu.pc += 2;
    },
    
    // 7XNN: adds NN to VX
    [](Chip8Cpu& cpu) {
        const auto x = (cpu.opcode & 0x0F00) >> 8;
        cpu.V[x] += cpu.opcode & 0x00FF;
        cpu.pc += 2;
    },
    
    [](Chip8Cpu& cpu) {
        const auto x = (cpu.opcode & 0x0F00) >> 8;
        const auto y = (cpu.opcode & 0x00F0) >> 4;
        switch (cpu.opcode & 0x000F) {
        // 8XY0: Set VX to the value of VY
        case 0:
            cpu.V[x] = cpu.V[y];
            break;
        case 1:
            cpu.V[x] = cpu.V[x] | cpu.V[y];
            break;
        case 2:
            cpu.V[x] = cpu.V[x] & cpu.V[y];
            break;
        case 3:
            cpu.V[x] = cpu.V[x] ^ cpu.V[y];
            break;
        case 4:
            cpu.V[0xF] = cpu.V[y] > (0xFF - cpu.V[x]) ? 1 : 0;
            cpu.V[x] += cpu.V[y];
            break;
        case 5:
            cpu.V[0xF] = cpu.V[x] < cpu.V[y] ? 0 : 1;
            cpu.V[x] -= cpu.V[y];
            break;
        case 6:
            cpu.V[0xF] = cpu.V[x] & 0x01;
            cpu.V[x] >>= 1;
            break;
        case 7:
            cpu.V[0xF] = cpu.V[y] < cpu.V[x] ? 1 : 0;
            cpu.V[x] = cpu.V[y] - cpu.V[x];
            break;
        case 0xE:
            cpu.V[0xF] = cpu.V[x] >> 7;
            cpu.V[x] <<= 1;
            break;
        default:
            throw InterpreterException("Instruction {0:#X} is not a Chip8 opcode", cpu.opcode);
        }
        cpu.pc += 2;
    },
    
    // 9XY0: skip next instruction if VX != VY
    [](Chip8Cpu& cpu) {
        const auto x = (cpu.opcode & 0x0F00) >> 8;
        const auto y = (cpu.opcode & 0x00F0) >> 4;
        if (cpu.V[x] != cpu.V[y]) {
            cpu.pc += 2;
        }
        cpu.pc += 2;
    },

    // ANNN: set I to address NNN
    [](Chip8Cpu& cpu) {
        cpu.I = cpu.opcode & 0x0FFF;
        cpu.pc += 2;
    },
    
    // BNNN: jump to address NNN + V0
    [](Chip8Cpu& cpu) {
        cpu.pc = cpu.opcode & 0x0FFF;
        cpu.pc += cpu.V[0];
    },

    // CXNN: store bitwise AND operation of NN and random number in VX
    [](Chip8Cpu& cpu) {
        const auto i = (cpu.opcode & 0x0F00) >> 8;
        cpu.V[i] = (cpu.opcode & 0x00FF) & utils::random<std::uint16_t>();
        cpu.pc += 2;
    },

    [](Chip8Cpu& cpu) {
        const auto xpos = cpu.V[(cpu.opcode & 0x0F00) >> 8];
        const auto ypos = cpu.V[(cpu.opcode & 0x00F0) >> 4];
        const int height = cpu.opcode & 0x000F;
        
        cpu.V[0xF] = 0;
        for (int y = 0; y < height; y++) {
            const auto pixel = cpu.memory[cpu.I + y];
            for (int x = 0; x < 8; x++) {
                if (pixel & (0x80 >> x)) {
                    if (cpu.gfx[ypos + y][xpos + x]) {
                        cpu.V[0xF] = 1;
                    }
                    cpu.gfx[ypos + y][xpos + x] ^= 1;
                }
            }
        }
        
        cpu.flags.draw = true;
        cpu.pc += 2;
    },

    [](Chip8Cpu& cpu) {
        const auto x = (cpu.opcode & 0x0F00) >> 8;
        const auto k = cpu.V[x];
        if (k >= keys_size) {
            throw InterpreterException("Key stored in register V[{0:d}] out of range", x);
        }
        switch (cpu.opcode & 0x00FF) {
        // skip next instruction if key stored in VX is pressed
        case 0x9E:
            if (cpu.keys[k]) {
                cpu.pc += 2;
            }
            break;
        // skip next instruction if key stored in VX isn't pressed
        case 0xA1:
            if (!cpu.keys[k]) {
                cpu.pc += 2;
            }
            break;
        default:
            throw InterpreterException("Instruction {0:#X} is not a Chip8 opcode", cpu.opcode);
        }

        cpu.pc += 2;
    },

    [](Chip8Cpu& cpu) {
        const auto x = (cpu.opcode & 0x0F00) >> 8;
        switch (cpu.opcode & 0x00FF) {
        // set VX to the value of the delay timer
        case 0x07:
            cpu.V[x] = cpu.delay_timer;
            break;
        // await key press and then store it in VX
        case 0x0A:
            cpu.pc -= 2;
            for (std::uint8_t i = 0; i < keys_size; i++) {
                if (cpu.keys[i]) {
                    cpu.V[x] = i;
                    cpu.pc += 2;
                    break;
                }
            }
            break;
        // set delay timer to VX
        case 0x15:
            cpu.delay_timer = cpu.V[x];
            break;
        // set sound timer to VX
        case 0x18:
            cpu.sound_timer = cpu.V[x];
            break;
        // add VX to I
        case 0x1E:
            cpu.I += cpu.V[x];
            break;
        // set I to the location of the sprite for the character in VX
        case 0x29:
            if (cpu.V[x] > 0xF) {
                throw InterpreterException("Character in register V[{0:d}] not representable", x);
            }
            cpu.I = cpu.V[x] * 5;
            break;
        // store the binary-coded decimal representation of VX
        case 0x33:
        {
            const auto i = cpu.I;
            const auto vx = cpu.V[x];
            cpu.memory[i] = vx / 100;
            cpu.memory[i + 1] = (vx % 100) / 10;
            cpu.memory[i + 2] = vx % 10;
            break;
        }
        // store V0 to VX (inclusive) in memory starting at address I
        case 0x55:
            if (cpu.I >= memory_size - x) {
                throw InterpreterException("Can't copy registers to memory: address register out of range");
            }
            std::copy_n(cpu.V, x + 1, cpu.memory + cpu.I);
            break;
        // fill V0 to VX (inclusive) with values from memory starting at address I
        case 0x65:
            if (cpu.I >= memory_size - x) {
                throw InterpreterException("Can't copy memory to registers: address register out of range");
            }
            std::copy_n(cpu.memory + cpu.I, x + 1, cpu.V);
            break;
        default:
            throw InterpreterException("Instruction {0:#X} is not a Chip8 opcode", cpu.opcode);
        }
        cpu.pc += 2;
    }
};

Chip8Cpu::Chip8Cpu()
{
    std::copy(chip8_fontset.begin(), chip8_fontset.end(), memory);
}

void Chip8Cpu::load_rom(const std::filesystem::path& path)
{
    namespace fs = std::filesystem;
    
    if (!fs::exists(path) || !fs::is_regular_file(path)) {
        throw FileNotFoundException(path.u8string());
    }
    
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs) {
        throw IOException("Can't read file " + path.u8string());
    }

    ifs.seekg(0, std::ios::end);
    const auto bytes = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    if (bytes > max_rom_size) {
        throw IOException("Size of loaded ROM exceeds max memory size");
    }

    ifs.read(reinterpret_cast<char*>(memory) + 0x200, bytes);
}

void Chip8Cpu::step()
{
    if (pc >= memory_size) {
        throw IOException("Program counter exceeded memory size");
    }

    opcode = memory[pc];
    opcode <<= 8;
    opcode |= memory[pc + 1];

    instructions[(opcode & 0xF000) >> 12](*this);
}

void Chip8Cpu::clear_screen()
{
    std::fill_n(&gfx[0][0], screen_width * screen_height, 0);
}

void Chip8Cpu::reset()
{
    clear_screen();
    pc = 0x200;
    I = 0;
    delay_timer = 0;
    sound_timer = 0;
}

void Chip8Cpu::count_down()
{
    if (delay_timer > 0) {
        --delay_timer;
    }
    
    if (sound_timer > 0) {
        if (sound_timer == 1) {
            fmt::print("BEEP\a");
            std::fflush(stdout);
        }
        --sound_timer;
    }
}

