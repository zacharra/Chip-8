#include <array>
#include <exception>
#include <filesystem>
#include <string>

#include <fmt/format.h>
#include <fmt/printf.h>
#include <cxxopts.hpp>
#include <tinyfiledialogs.h>

#include <chip8/chip8.h>

#include "window.h"

namespace fs = std::filesystem;

static fs::path get_file_from_dialog()
{
    std::array filters{"*", "*.ch8"};
    auto file = tinyfd_openFileDialog("Choose a Chip8 ROM file", "", filters.size(), filters.data(), "Chip-8 files", 0);
    if (!file) {
        return {};
    }

    return file;
}

int main(int argc, char* argv[])
try {
    cxxopts::Options options{argv[0], "Allowed options"};
    options.add_options()
        ("p,path", "Path to the ROM file", cxxopts::value<std::string>())
        ("h,help", "Print help")
    ;

    auto opts = options.parse(argc, argv);
    if (opts.count("help")) {
        fmt::print("{}\n", options.help({""}));
        return 0;
    }

    bool dialog_based = false;
    fs::path path;
    if (opts.count("path")) {
        path = opts["path"].as<std::string>();
    } else {
        path = get_file_from_dialog();
        if (path.empty()) {
            return 0;
        }
        dialog_based = true;
    }

    Chip8Cpu chip8;
    Window window{chip8, 640, 320};

    do {
        try {
            chip8.load_rom(path);
            window.run();
        } catch (const Exception& e) {
            tinyfd_messageBox("Error", fmt::format("{}: {}", e.what(), e.message()).c_str(), "ok", "error", 1);
        }
        if (dialog_based) {
            path = get_file_from_dialog();
        }
    } while (dialog_based && !path.empty());
    
    return 0;
} catch (const std::exception& e) {
    fmt::fprintf(stderr, "Error: %s\n", e.what());
    return 1;
} catch (...) {
    fmt::fprintf(stderr, "Unknown exception occured.\n");
    throw;
}
