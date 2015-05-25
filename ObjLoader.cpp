#include "ObjLoader.hpp"

#include <iostream>
#include <CL/cl_platform.h>

ObjLoader::ObjLoader(std::string filename) {
    std::ifstream f(filename);
    while (!f.eof()) {
        std::string line;
        std::getline(f, line);

        if (f.eof()) {
            break;
        }

        if (!line.empty()) {
            std::stringstream ss(line);
            std::string type;
            ss >> type;
            if (type == "#") {
                continue;
            } else if (type == "v") {
                float x, y, z;
                ss >> x >> y >> z;
                cl_float4 point;
                point.s[0] = x;
                point.s[1] = y;
                point.s[2] = z;
                point.s[3] = 0;
                points.push_back(point);
            } else if (type == "vn") {
                float x, y, z;
                ss >> x >> y >> z;
                cl_float4 normal;
                normal.s[0] = x;
                normal.s[1] = y;
                normal.s[2] = z;
                normal.s[3] = 0;
                normals.push_back(normal);
            } else if (type == "l") {
                int a, b;
                ss >> a >> b;
                cl_int2 edge;
                edge.s[0] = a - 1;
                edge.s[1] = b - 1;
                edges.push_back(edge);
            } else if (type == "f") {
                int a, b, c;
                ss >> a >> b >> c;
                cl_int4 face;
                face.s[0] = a - 1;
                face.s[1] = b - 1;
                face.s[2] = c - 1;
                face.s[3] = 0;
                faces.push_back(face);
            } else if (type == "s") {
                // nothing...
            } else {
                std::cerr << "Unknown line type: '" << type << "'";
            }
        }
    }
    std::cout << "Loaded " <<
            points.size() << " points, " <<
            edges.size() << " edges and " <<
            faces.size() << " faces " <<
            "from " << filename << std::endl;

    connect_neighbors(0.01, 0.32);
}

void ObjLoader::connect_neighbors(float mindist, float maxdist) {
    float mindistSqr = mindist * mindist;
    float maxdistSqr = maxdist * maxdist;

    int n = 0;

    for (size_t i = 0; i < points.size(); ++i) {
        const auto &p1 = points[i];
        for (size_t j = i + 1; j < points.size(); ++j) {
            const auto &p2 = points[j];

            float dx = p1.s[0] - p2.s[0];
            float dy = p1.s[1] - p2.s[1];
            float dz = p1.s[2] - p2.s[2];

            float dstSqr = (dx * dx + dy*dy + dz*dz);

            if ((dstSqr >= mindistSqr) && (dstSqr <= maxdistSqr)) {
                cl_int2 edge;
                edge.s[0] = (int)i;
                edge.s[1] = (int)j;
                edges.push_back(edge);

                ++n;
            }
        }
    }

    std::cout << "Added " << n << " new edges." << std::endl;
}
