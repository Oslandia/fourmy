#pragma once

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/corefinement.h>

namespace boolean 
{
typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Point;
typedef CGAL::Surface_mesh<Point> Mesh;
namespace PMP = CGAL::Polygon_mesh_processing;

inline
Mesh create_mesh(const boost::python::list &vtx, const boost::python::list &idx)
{
    Mesh out;
    std::vector<Mesh::Vertex_index> vtx_idx; 
    for (int i=0; i<len(vtx); i++)
    {
        vtx_idx.push_back(out.add_vertex(Point(
            boost::python::extract<double>(vtx[i][0]),
            boost::python::extract<double>(vtx[i][1]),
            boost::python::extract<double>(vtx[i][2]))));
    }

    for (int i=0; i<len(idx); i++)
    {
          const int a = boost::python::extract<int>(idx[i][0]);
          const int b = boost::python::extract<int>(idx[i][1]);
          const int c = boost::python::extract<int>(idx[i][2]);
          std::cout << a << " " << b << " " << c << "\n";
          out.add_face(vtx_idx[a], vtx_idx[b], vtx_idx[c]);
    }
    return out;
}

inline
Mesh union_(Mesh &a, Mesh &b)
{
    Mesh out;
    const bool valid_union = PMP::corefine_and_compute_union(a, b, out);
    if (!valid_union)
        throw std::runtime_error("union could not be computed");
    return out;
}

}

