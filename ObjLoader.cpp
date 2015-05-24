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
            char type;
            ss >> type;
            switch (type) {
                case '#':
                    continue;
                case 'v':
                    float x, y, z;
                    ss >> x >> y >> z;
                    points.push_back((cl_float4){x, y, z, 0.0f});
                    break;
                case 'l':
                    int a, b;
                    ss >> a >> b;
                    edges.push_back((cl_int2){a - 1, b - 1});
                    break;
                default:
                    std::cerr << "Unknown line type: '" << type << "'";
                    break;
            }
        }
    }

    std::cout << "Loaded " << points.size() << " points and " << edges.size() << " edges from " << filename << std::endl;
}

void ObjLoader::connect_neighbors(float mindist, float maxdist) {
    float mindistSqr = mindist * mindist;
    float maxdistSqr = maxdist * maxdist;

    int n = 0;

    for (int i = 0; i < points.size(); ++i) {
        const auto &p1 = points[i];
        for (int j = i + 1; j < points.size(); ++j) {
            const auto &p2 = points[j];

            float dx = p1.s[0] - p2.s[0];
            float dy = p1.s[1] - p2.s[1];
            float dz = p1.s[2] - p2.s[2];

            float dstSqr = (dx * dx + dy*dy + dz*dz);

            if ((dstSqr >= mindistSqr) && (dstSqr <= maxdistSqr)) {
                edges.push_back((cl_int2){i, j});

                ++n;
            }
        }
    }

    std::cout << "Added " << n << " new edges." << std::endl;
}
