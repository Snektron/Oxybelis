#ifndef _FASTPOLY2TRI_H
#define _FASTPOLY2TRI_H

#include <memory>
#include <cstdint>

#define internal_static extern
#define MPE_POLY2TRI_USE_DOUBLE
#include "MPE_fastpoly2tri.h"

struct Free {
    void operator()(void* ptr);
};

using MPEPolyMemory = std::unique_ptr<void, Free>;

MPEPolyMemory MPE_PolyAllocateMem(u32 max_points);

#endif
