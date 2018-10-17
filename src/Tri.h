#ifndef _OXYBELIS_TRI_H
#define _OXYBELIS_TRI_H

#include <vector>
#include "math/Mat.h"
#include "math/Vec.h"
#include "graphics/camera/FreeCam.h"
#include "graphics/VertexArray.h"
#include "graphics/Buffer.h"
#include "graphics/shader/Program.h"

class Tri {
    Program shader;
    Uniform perspective;
    Uniform model;

    std::vector<Vec3F> vertices;

    VertexArray tri_vao;
    Buffer tri_buffer;

public:
    Tri();
    void advance();
    void reupload();
    void render(const Mat4F& proj, const FreeCam& cam);
};

#endif
