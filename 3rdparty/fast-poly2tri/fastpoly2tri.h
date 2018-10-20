#ifndef _FASTPOLY2TRI_H
#define _FASTPOLY2TRI_H

#include <memory>

#define internal_static extern
#include "MPE_fastpoly2tri.h"

struct Free {
    void operator()(void* ptr);
};

using MPEPolyMemory = std::unique_ptr<void, Free>;

MPEPolyMemory MPE_PolyAllocateMem(u32 max_points);

#endif
