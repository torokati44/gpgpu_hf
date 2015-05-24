#ifndef GPGPU_HF_OBJLOADER_H
#define GPGPU_HF_OBJLOADER_H

#include <string>
#include <vector>
#include <utility>
#include <tuple>
#include <fstream>
#include <sstream>

#include <CL/cl.h>

class ObjLoader {
public:
    ObjLoader(std::string filename);
    std::vector<cl_float4> points;
    std::vector<cl_float4> normals;
    std::vector<cl_int2> edges;
    std::vector<cl_int4> faces;

    void connect_neighbors(float mindist, float maxdist);
};


#endif //GPGPU_HF_OBJLOADER_H
