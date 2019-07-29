#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_2_algorithms.h>

#include <boost/python.hpp>
#include <boost/python/overloads.hpp>
#include <iostream>
#include <string>

namespace fourmy {

typedef CGAL::Exact_predicates_inexact_constructions_kernel       K;
typedef CGAL::Triangulation_vertex_base_2<K>                      Vb;
typedef CGAL::Constrained_triangulation_face_base_2<K>            Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>               TDS;
typedef CGAL::Exact_predicates_tag                                Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag>  CDT;
typedef CDT::Point                                                Point;
typedef CGAL::Polygon_2<K>                                        Polygon_2;
typedef std::vector< Polygon_2 >                                  Rings;


Polygon_2 polygon_from_ring(const boost::python::object & coords)
{
    using namespace boost::python;
    Polygon_2 poly;
    const size_t sz = extract<size_t>(coords.attr("__len__")());
    for (size_t i=0; i<sz-1; i++)
        poly.push_back(Point(extract<double>(coords[i][0]), extract<double>(coords[i][1])));
    return std::move(poly);
}

bool is_inside(const Point & point, const Rings & rings)
{
    int nesting = 0;
    for(const Polygon_2 & ring : rings)
    {
        if (CGAL::ON_BOUNDED_SIDE == CGAL::bounded_side_2(ring.vertices_begin(), ring.vertices_end(), point, K()))
        {
            ++nesting;
        }
    }

    return nesting == 1;
}


boost::python::object tessellate(const boost::python::object & polygon, const boost::python::object & lines = boost::python::object(), const boost::python::object & points = boost::python::object())
{
    using namespace boost::python;
    Rings rings;

    const char * type = extract<const char *>(polygon.attr("type"));
    if (type != std::string("Polygon"))
        throw std::runtime_error("tessellate only accepts shapely Polygons");

    CDT cdt;

    {
        Polygon_2 poly(polygon_from_ring(polygon.attr("exterior").attr("coords")));
        cdt.insert_constraint(poly.vertices_begin(), poly.vertices_end(), true);
        rings.push_back(poly);
    }
    const size_t nrings = extract<size_t>(polygon.attr("interiors").attr("__len__")());
    for (size_t r=0; r<nrings; r++)
    {
        Polygon_2 poly(polygon_from_ring(polygon.attr("interiors")[r].attr("coords")));
        cdt.insert_constraint(poly.vertices_begin(), poly.vertices_end(), true);
        rings.push_back(poly);
    }


    // insert points
    if (points !=  boost::python::object())
    {
        const size_t sz = extract<size_t>(points.attr("__len__")());
        for (size_t i=0; i<sz; i++)
            cdt.push_back(Point(extract<double>(points[i].attr("coords")[0][0]), extract<double>(points[i].attr("coords")[0][1])));
    }


    // insert line constrains
    if (lines !=  boost::python::object())
    {
        const size_t nlines = extract<size_t>(lines.attr("__len__")());
        for (size_t l=0; l<nlines; l++)
        {
            const boost::python::object & coords = lines[l].attr("coords");
            const size_t sz = extract<size_t>(coords.attr("__len__")());
            for (size_t i=1; i<sz; i++)
            {
                cdt.insert_constraint(Point(extract<double>(coords[i-1][0]), extract<double>(coords[i-1][1])),
                                      Point(extract<double>(coords[i][0]), extract<double>(coords[i][1])));
            }
        }
    }

    //mark_domains(cdt);

    object shapely_geom = import("shapely.geometry");
    list triangles;
    for (CDT::Finite_faces_iterator fit=cdt.finite_faces_begin();
                                    fit!=cdt.finite_faces_end();++fit)
    {
        const Point c((fit->vertex(0)->point().x()+fit->vertex(1)->point().x()+fit->vertex(2)->point().x())/3,
                              (fit->vertex(0)->point().y()+fit->vertex(1)->point().y()+fit->vertex(2)->point().y())/3);

        if (is_inside(c, rings))
        {
            list ring;
            for (size_t i=0; i<4; i++)
                ring.attr("append")(make_tuple(fit->vertex(i%3)->point().x(), fit->vertex(i%3)->point().y()));
            triangles.attr("append")(shapely_geom.attr("Polygon")(ring));
        }
    }
    return shapely_geom.attr("MultiPolygon")(triangles);
}
}

BOOST_PYTHON_FUNCTION_OVERLOADS(tessellate_overloads, fourmy::tessellate, 1, 3)

BOOST_PYTHON_MODULE(_fourmy)
{
    using namespace boost::python;
    def("tessellate", &fourmy::tessellate, tessellate_overloads(args("polygon", "lines", "points"), "This is tesselate's docstring"));
}

