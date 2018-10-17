#ifndef _FASTPOLY2TRI_H
#define _FASTPOLY2TRI_H

#include <memory>
#include <cstdlib>

#define internal_static extern
#include "MPE_fastpoly2tri.h"

struct Free {
    void operator()(void* ptr) {
        std::free(ptr);
    }
};

using MPEPolyMemory = std::unique_ptr<void, Free>;

MPEPolyMemory MPE_PolyAllocateMem(u32 max_points) {
    size_t mem_req = MPE_PolyMemoryRequired(max_points);
    return MPEPolyMemory(std::calloc(mem_req, 1));
}

#endif
