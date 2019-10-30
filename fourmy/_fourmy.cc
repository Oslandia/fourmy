#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Polygon_2.h>

#include <boost/python.hpp>
#include <boost/python/overloads.hpp>
#include <iostream>
#include <algorithm>
#include <string>

namespace fourmy {

struct FaceInfo2
{
    FaceInfo2(){}
    int nesting_level;
    bool in_domain(){ 
        return nesting_level%2 == 1;
    }
};

typedef CGAL::Exact_predicates_inexact_constructions_kernel       K;
typedef CGAL::Triangulation_vertex_base_2<K>                      Vb;
typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2,K>    Fbb;
typedef CGAL::Constrained_triangulation_face_base_2<K,Fbb>        Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>               TDS;
typedef CGAL::Exact_predicates_tag                                Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag>  CDT;
typedef CDT::Point                                                Point;
typedef CDT::Segment                                              Segment;
typedef CGAL::Polygon_2<K>                                        Polygon_2;

void 
mark_domains(CDT& ct, 
             CDT::Face_handle start, 
             int index, 
             std::list<CDT::Edge>& border,
             const std::vector< Segment >& rings)
{
    if(start->info().nesting_level != -1){
        return;
    }
    std::list<CDT::Face_handle> queue;
    queue.push_back(start);
    while(! queue.empty()){
        CDT::Face_handle fh = queue.front();
        queue.pop_front();
        if(fh->info().nesting_level == -1){
            fh->info().nesting_level = index;
            for(int i = 0; i < 3; i++){
                CDT::Edge e(fh,i);
                CDT::Face_handle n = fh->neighbor(i);
                if(n->info().nesting_level == -1){
                    if(ct.is_constrained(e) && std::find(rings.begin(), rings.end(), ct.segment(e)) != rings.end() ){
                        border.push_back(e);
                    }
                    else {
                        queue.push_back(n);
                    }
                }
            }
        }
    }
}

//explore set of facets connected with non constrained edges,
//and attribute to each such set a nesting level.
//We start from facets incident to the infinite vertex, with a nesting
//level of 0. Then we recursively consider the non-explored facets incident 
//to constrained edges bounding the former set and increase the nesting level by 1.
//Facets in the domain are those with an odd nesting level.
void
mark_domains(CDT& cdt, const std::vector< Segment >& rings)
{
    for(CDT::All_faces_iterator it = cdt.all_faces_begin(); it != cdt.all_faces_end(); ++it){
        it->info().nesting_level = -1;
    }
    std::list<CDT::Edge> border;
    mark_domains(cdt, cdt.infinite_face(), 0, border, rings);
    while(! border.empty()){
        CDT::Edge e = border.front();
        border.pop_front();
        CDT::Face_handle n = e.first->neighbor(e.second);
        if(n->info().nesting_level == -1){
            mark_domains(cdt, n, e.first->info().nesting_level+1, border, rings);
        }
    }
}

Polygon_2 polygon_from_ring(const boost::python::object & coords)
{
    using namespace boost::python;
    Polygon_2 poly;
    const size_t sz = extract<size_t>(coords.attr("__len__")());
    for (size_t i=0; i<sz-1; i++)
        poly.push_back(Point(extract<double>(coords[i][0]), extract<double>(coords[i][1])));
    return std::move(poly);
}

boost::python::object tessellate(const boost::python::object & polygon, const boost::python::object & lines = boost::python::object(), const boost::python::object & points = boost::python::object())
{
    using namespace boost::python;

    const char * type = extract<const char *>(polygon.attr("type"));
    if (type != std::string("Polygon"))
        throw std::runtime_error("tessellate only accepts shapely Polygons");

    CDT cdt;

    {
        Polygon_2 poly(polygon_from_ring(polygon.attr("exterior").attr("coords")));
        cdt.insert_constraint(poly.vertices_begin(), poly.vertices_end(), true);
    }
    const size_t nrings = extract<size_t>(polygon.attr("interiors").attr("__len__")());
    for (size_t r=0; r<nrings; r++)
    {
        Polygon_2 poly(polygon_from_ring(polygon.attr("interiors")[r].attr("coords")));
        cdt.insert_constraint(poly.vertices_begin(), poly.vertices_end(), true);
    }

    std::vector< Segment > border;
    for (CDT::Constrained_edges_iterator e=cdt.constrained_edges_begin(); e != cdt.constrained_edges_end(); e++){
        const Segment s(cdt.segment(*e));
        border.push_back(s);
        border.push_back(s.opposite());
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

    // insert points
    if (points !=  boost::python::object())
    {
        const size_t sz = extract<size_t>(points.attr("__len__")());
        for (size_t i=0; i<sz; i++)
            cdt.push_back(Point(extract<double>(points[i].attr("coords")[0][0]), extract<double>(points[i].attr("coords")[0][1])));
    }

    mark_domains(cdt, border);

    object shapely_geom = import("shapely.geometry");
    list triangles;
    for (CDT::Finite_faces_iterator fit=cdt.finite_faces_begin();
                                    fit!=cdt.finite_faces_end();++fit)
    {
        if (fit->info().in_domain())
        {
            list ring;
            for (size_t i=0; i<4; i++)
                ring.attr("append")(make_tuple(fit->vertex(i%3)->point().x(), fit->vertex(i%3)->point().y()));
            triangles.attr("append")(shapely_geom.attr("Polygon")(ring));
        }
    }
    return shapely_geom.attr("MultiPolygon")(triangles);
}

} // namespace fourmy

BOOST_PYTHON_FUNCTION_OVERLOADS(tessellate_overloads, fourmy::tessellate, 1, 3)

BOOST_PYTHON_MODULE(_fourmy)
{
    using namespace boost::python;
    def("tessellate", &fourmy::tessellate, tessellate_overloads(args("polygon", "lines", "points"), "This is tesselate's docstring"));
}
