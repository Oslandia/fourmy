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
    //std::cout << "mark_domains\n";
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
                    //if (!ct.is_infinite(e)) std::cout << ct.segment(e);
                    //else std::cout << "infinit";
                    if(ct.is_constrained(e) && std::find(rings.begin(), rings.end(), ct.segment(e)) != rings.end() ){
                        //std::cout << (std::find(rings.begin(), rings.end(), 
                        //            std::make_pair(fh->vertex(i)->point(), fh->vertex((i+1)%3)->point()) 
                        //            ) != rings.end() ? "ok" : "not ok") << " " << border.size() << "\n";
                        //std::cout << " constrainted segment\n";
                        border.push_back(e);
                    }
                    else {
                        //std::cout << " unconstrained\n";
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

    //for (CDT::Constrained_edges_iterator e=cdt.constrained_edges_begin(); e != cdt.constrained_edges_end(); e++){
    //    std::cout << cdt.segment(*e);
    //    if (std::find(border.begin(), border.end(), cdt.segment(*e)) != border.end()){
    //        std::cout << " border";
    //    }
    //    std::cout << "\n";
    //}

    // insert points
    if (points !=  boost::python::object())
    {
        const size_t sz = extract<size_t>(points.attr("__len__")());
        for (size_t i=0; i<sz; i++)
            cdt.push_back(Point(extract<double>(points[i].attr("coords")[0][0]), extract<double>(points[i].attr("coords")[0][1])));
    }


    //int i = 0;
    //for (CDT::Constrained_edges_iterator  e=cdt.constrained_edges_begin(); e != cdt.constrained_edges_end(); ++e, ++i){
    //    std::cout << i << " " << (std::find(border.begin(), border.end(), *e) != border.end() ? "ring" : "not ring") 
    //        << " " << (cdt.is_constrained(*e) ? "constrained": "unconstrained") <<"\n";
    //}

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
}

BOOST_PYTHON_FUNCTION_OVERLOADS(tessellate_overloads, fourmy::tessellate, 1, 3)

BOOST_PYTHON_MODULE(_fourmy)
{
    using namespace boost::python;
    def("tessellate", &fourmy::tessellate, tessellate_overloads(args("polygon", "lines", "points"), "This is tesselate's docstring"));
}

//    register_exception_translator<std::out_of_range>(translate_out_of_range);
//
//    def("from_wkb", &fourmy_from_wkb, return_internal_reference<>());
//    //def("delete", &fourmy_delete);
//    //def("tesselate", &fourmy_tesselate, return_internal_reference<>());
//    
//    enum_<wkb::byteorder_t>("wkb_byteorder")
//        .value("big_endian", wkb::big_endian)
//        .value("little_endian", wkb::little_endian)
//        ;
//
//    enum_<wkb::type_t>("wkb_type")
//        .value("Point", wkb::point)
//        .value("LineString", wkb::linestring)
//        .value("Polygon", wkb::polygon)
//        .value("MultiPoint", wkb::multipoint)
//        .value("MultiLineString", wkb::multilinestring)
//        .value("MultiPolygon", wkb::multipolygon)
//        .value("GeometryCollection", wkb::geometrycollection)
//        ;
//
//    class_<geometry::point<double>>("Point", init<>())
//        .def("__len__", &python::point_len) 
//        .def("__getitem__", &python::point_getitem) 
//        .def("__repr__", &python::point_repr) 
//        .add_property("__array_interface__", &python::point_array_interface) 
//        ;
//    class_<geometry::linestring<double>>("LineString", init<>())
//        //.def(vector_indexing_suite<geometry::linestring<double>>() )
//        .def("__len__", &geometry::linestring<double>::size) 
//        .def("__getitem__", &python::linestring_getitem) 
//        .add_property("__array_interface__", &python::linestring_array_interface) 
//        ;
//
//    class_<geometry::polygon<double>>("Polygon", init<>())
//        .def(vector_indexing_suite<geometry::polygon<double>>() )
//        ;
//
//    class_<geometry::multipolygon<double>>("MultiPolygon", init<>())
//        .def(vector_indexing_suite<geometry::multipolygon<double>>() )
//        ;
//
//    class_<geometry::geometry<double>>("Geometry", init<>())
//        ;
//
//    def("loads", &python::loads);
//    def("tessellate", &python::tessellate);
//}

//void translate_runtime_error(const std::runtime_error & e)
//{
//    PyErr_SetString(PyExc_UserWarning, e.what());
//}
//
//
//struct wrapper
//{
//    wrapper(boost::python::object wkb)
//        : _wkb(PyBytes_Check(wkb.ptr()) ? wkb : wkb.attr("wkb"))
//    {
//        _wkb != boost::python::object() || die("cannot create wrapper from None");
//        // @todo check conformance of object else raise
//    }
//
//    const unsigned char * bytes() const
//    {
//        return reinterpret_cast<const unsigned char *>(PyBytes_AsString(_wkb.ptr()));
//    }
//
//private:
//    // storage is either const from a python object, or non const from other ctor
//    const boost::python::object _wkb;
//
//};
//
    //class_<geometry::geometry>("Geometry", init<object>())
    //    .add_property("type", &geometry::geometry::type)
    //    .add_property("has_z", &geometry::geometry::has_z)
    //    .add_property("has_srid", &geometry::geometry::has_srid)
    //    .add_property("has_m", &geometry::geometry::has_m)
    //    .add_property("srid", &geometry::geometry::srid)
    //    .add_property("wkb", &geometry::geometry::wkb)
    //    ;
        //.def("__len__", &geometry::point<double>::size)
        //.def("__getitem__", const double &(geometry::point<double>::*operator{])() =  &geometry::point<double>::operator[])
    //    ;


//    class_<geometry::coordinate>("Coordinate", no_init)
//        .def("__len__", &geometry::coordinate::size)
//        .def("__getitem__", &geometry::coordinate::operator[])
//        ;
//
//    class_<geometry::coordinate_sequence>("CoordinateSequence", no_init)
//        .def("__len__", &geometry::coordinate_sequence::size)
//        .def("__getitem__", &geometry::coordinate_sequence::operator[])
//        ;
//
//    class_<geometry::point, bases<geometry::geometry> >("Point", init<object>())
//        //.add_property("x", &geometry::point::x)
//        //.add_property("y", &geometry::point::y)
//        //.add_property("z", &geometry::point::z)
//        //.add_property("m", &geometry::point::m)
//        .add_property("coords", &geometry::point::coords)
//        ;
//
//    class_<geometry::linestring, bases<geometry::geometry> >("LineString", init<object>())
//        .add_property("coords", &geometry::linestring::coords)
//        ;


    //def("delaunay", delaunay::tessellate);

//    class_<boolean::Mesh>("Mesh", init<>())
//        .add_property("number_of_vertices", &boolean::Mesh::number_of_vertices)
//        .add_property("number_of_faces", &boolean::Mesh::number_of_faces);
//    def("union", boolean::union_);
//    def("create_mesh", boolean::create_mesh);
//
//    class_<geometry::CoordinateSequence>("CoordinateSequence", init<>())
//        .add_property("__getitem__", &geometry::CoordinateSequence::operator[])
//        .add_property("__len__", &geometry::CoordinateSequence::size)
//        ;
//boost::python::object do_it(const boost::python::object in)
//{
//    const char *data = PyBytes_AsString(in.ptr());
//    const std::vector<unsigned char> v(data, data+PyBytes_Size(in.ptr()));
//    return boost::python::object(boost::python::handle<>(PyBytes_FromStringAndSize(reinterpret_cast<const char*>(&v[0]), v.size())));
//}
//
