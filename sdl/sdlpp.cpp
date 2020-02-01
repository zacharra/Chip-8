#include "sdlpp.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "cert-err58-cpp"

namespace sdl {
    Init Init::init{SDL_INIT_VIDEO};
}

#pragma clang diagnostic pop
