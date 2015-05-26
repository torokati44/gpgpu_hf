#include "ObjLoader.hpp"

#include <iostream>
#include <CL/cl_platform.h>
#include <algorithm>

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

    add_faces_as_edges();
    //connect_neighbors();
    connect_opposites();
    //connect_neighbors(0.001, 0.3);
}

void ObjLoader::add_faces_as_edges() {
    for (const auto &f : faces) {
        cl_int2 a, b, c;

        a.s[0] = f.s[0];
        a.s[1] = f.s[1];

        b.s[0] = f.s[1];
        b.s[1] = f.s[2];

        c.s[0] = f.s[2];
        c.s[1] = f.s[0];

        edges.push_back(a);
        edges.push_back(b);
        edges.push_back(c);
    }
}

void ObjLoader::connect_opposites() {

    auto is_corner = [](const cl_int4 &face, const cl_int &point) {
        return ((point == face.s[0]) || (point == face.s[1]) || (point == face.s[2]));
    };

    auto is_side = [](const cl_int4 &face, const cl_int2 &edge) {
        return ((edge.s[0] == face.s[0]) || (edge.s[0] == face.s[1]) || (edge.s[0] == face.s[2]))
            && ((edge.s[1] == face.s[0]) || (edge.s[1] == face.s[1]) || (edge.s[1] == face.s[2]));
    };


    for (size_t i = 0; i < edges.size(); ++i) {
        auto &e = edges[i];
        long sided = std::count_if(faces.begin(), faces.end(), [&e,&is_side](cl_int4 &f){ return  is_side(f, e); });
        if (sided != 2)
            std::cout << "Edge " << i << " (" << points[e.s[0]].s[0] << "," << points[e.s[0]].s[1] << "," << points[e.s[0]].s[2] << " - " <<
                    points[e.s[1]].s[0] << "," << points[e.s[1]].s[1] << "," << points[e.s[1]].s[2] <<
                    ") " << " has " << sided << " faces...\n";
    }



    auto &facesvar = faces;

    auto get_opposite_face = [&facesvar,&is_corner,&is_side](const cl_int4 &face, int corner) {
        cl_int2 e;
        e.s[0] = 0;
        e.s[1] = 0;
        switch (corner) {
            case 0:
                e.s[0] = face.s[1];
                e.s[1] = face.s[2];
                break;
            case 1:
                e.s[0] = face.s[0];
                e.s[1] = face.s[2];
                break;
            case 2:
                e.s[0] = face.s[0];
                e.s[1] = face.s[1];
                break;
            default:
                std::cerr << "lolwut" << std::endl;
                break;
        }

        for (auto &f : facesvar) {
            if (is_side(f, e) && !is_corner(f, face.s[corner])) {
                return f;
            }
        }
        std::cout << "FAIL\n";
        return facesvar[0];
    };

    auto get_opposite_corner = [&is_corner](const cl_int4 &of, const cl_int4 &to) {
        if (!is_corner(of, to.s[0])) {
            return to.s[0];
        } else if (!is_corner(of, to.s[1])) {
            return to.s[1];
        } else if (!is_corner(of, to.s[2])) {
            return to.s[2];
        }
        return 0;
    };

    for (const auto &f : faces) {
        cl_int2 edge;

        auto opp = get_opposite_face(f, 0);
        edge.s[0] = f.s[0];
        edge.s[1] = get_opposite_corner(f, opp);
        edges.push_back(edge);

        opp = get_opposite_face(f, 1);
        edge.s[0] = f.s[1];
        edge.s[1] = get_opposite_corner(f, opp);
        edges.push_back(edge);

        opp = get_opposite_face(f, 2);
        edge.s[0] = f.s[2];
        edge.s[1] = get_opposite_corner(f, opp);
        edges.push_back(edge);
    }

}

void ObjLoader::connect_neighbors() {
    std::vector<cl_int2> new_edges;

    for (int p = 0; p < (int)points.size(); ++p) {
        for (int e1 = 0; e1 < (int)edges.size(); ++e1) {
            if (edges[e1].s[0] == p || edges[e1].s[1] == p) {
                for (int e2 = 0; e2 < (int)edges.size(); ++e2) {

                    if (edges[e2].s[0] == edges[e1].s[1]) {
                        cl_int2 new_edge;
                        new_edge.s[0] = p;
                        new_edge.s[1] = edges[e2].s[1];

                        new_edges.push_back(new_edge);
                    }

                    if (edges[e2].s[1] == edges[e1].s[1]) {
                        cl_int2 new_edge;
                        new_edge.s[0] = p;
                        new_edge.s[1] = edges[e2].s[0];

                        new_edges.push_back(new_edge);
                    }

                }
            }
        }
    }

    for (const auto &e : new_edges) {
        edges.push_back(e);
    }

    std::unique(edges.begin(), edges.end(), [](const cl_int2 &a, const cl_int2 &b) {
        return ((a.s[0] == b.s[0]) && a.s[1] == b.s[1])
            || ((a.s[0] == b.s[1]) && a.s[1] == b.s[0]);
    });

    std::remove_if(edges.begin(), edges.end(), [](const cl_int2 &e){
        return e.s[0] == e.s[1];
    });
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
