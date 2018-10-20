#include <cstdlib>
#include "fastpoly2tri.h"
#define internal_static extern
#define MPE_POLY2TRI_IMPLEMENTATION
#include "MPE_fastpoly2tri.h"

void Free::operator()(void* ptr) {
    std::free(ptr);
}

MPEPolyMemory MPE_PolyAllocateMem(u32 max_points) {
    size_t mem_req = MPE_PolyMemoryRequired(max_points);
    return MPEPolyMemory(std::calloc(mem_req, 1));
}
