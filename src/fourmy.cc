// c++ -shared cgal.cpp -lCGAL -lgmp -I/usr/include/python3.5m/ -lboost_python-py35 -fPIC -o cgal.so
//
#include "fourmy.hh"

#include "geometry.hh"
#include "wkb.hh"
#include "delaunay.hh"

#include "Python.h"

#include <iostream>

//#include <boost/python.hpp>
//#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

//#include "union.hh"
//#include "delaunay.hh"

typedef unsigned char byte;

void prt(const char * msg)
{
    printf("%s\n", msg);
}

static fourmy_alloc_handler_t __fourmy_alloc_handler = malloc;
static fourmy_free_handler_t __fourmy_free_handler = free;
static fourmy_error_handler_t __fourmy_warning_handler = prt;
static fourmy_error_handler_t __fourmy_error_handler = prt;

#define FOURMY_WARNING __fourmy_warning_handler
#define FOURMY_ERROR __fourmy_error_handler

//const long fourmy_type_point =           FOURMY_TYPE_POINT;
//const long fourmy_type_linestring =      FOURMY_TYPE_LINESTRING;
//const long fourmy_type_polygon =         FOURMY_TYPE_POLYGON;
//const long fourmy_type_multipoint =      FOURMY_TYPE_MULTIPOINT;
//const long fourmy_type_multilinestring = FOURMY_TYPE_MULTILINESTRING;
//const long fourmy_type_multipolygon =    FOURMY_TYPE_MULTIPOLYGON;
//const long fourmy_type_invalid =         FOURMY_TYPE_INVALID;         

extern "C" {


    void fourmy_set_alloc_handlers( fourmy_alloc_handler_t alloc_handler, fourmy_free_handler_t free_handler )
    {
        __fourmy_alloc_handler = alloc_handler;
        __fourmy_free_handler = free_handler;
    }

    void fourmy_set_error_handlers( fourmy_error_handler_t warning_handler, fourmy_error_handler_t error_handler )
    {
        __fourmy_warning_handler = warning_handler;
        __fourmy_error_handler = error_handler;
    }

    fourmy_geometry_t * fourmy_from_wkb(const unsigned char* wkb)
    {
        try
        {
            return new geometry::geometry<double>(wkb::loads<double>(wkb));
        }
        catch (std::exception & e)
        {
            FOURMY_ERROR(e.what());
            return NULL;
        }
    }

    void fourmy_delete(fourmy_geometry_t * geom)
    {
        delete reinterpret_cast<geometry::geometry<double> *>(geom);
    }

    fourmy_geometry_type_t fourmy_type(fourmy_geometry_t * geom)
    {
        const geometry::geometry<double> * g = reinterpret_cast<geometry::geometry<double> *>(geom);
        switch (g->which())
        {
            case 0: return FOURMY_TYPE_EMPTY;
            case 1: return FOURMY_TYPE_POINT;
            case 2: return FOURMY_TYPE_LINESTRING;
            case 3: return FOURMY_TYPE_POLYGON;
            case 4: return FOURMY_TYPE_MULTIPOINT;
            case 5: return FOURMY_TYPE_MULTILINESTRING;
            case 6: return FOURMY_TYPE_MULTIPOLYGON;
        }
        // @todo handle error
        return FOURMY_TYPE_INVALID;
    }


    fourmy_geometry_t * fourmy_tesselate(fourmy_geometry_t * geom)
    {
        return new geometry::geometry<double>(delaunay::tessellate<double>( boost::get< const geometry::polygon<double> &>(*reinterpret_cast<const geometry::geometry<double> *>(geom))));
    }


    void translate_out_of_range(const std::out_of_range & e)
    {
        PyErr_SetString(PyExc_IndexError, e.what());
    }

}



//extern "C" PyMethodDef fourmy_module_methods[] = { 
//    {   
//        "from_wkb",
//        python_fourmy_from_wkb,
//        METH_VARARGS,
//        "Print 'hello world' from a method defined in a C extension."
//    },  
//    {NULL, NULL, 0, NULL}
//};
//
//extern "C" struct PyModuleDef fourmy_module_definition = { 
//    PyModuleDef_HEAD_INIT,
//    "_fourmy",
//    "expose fourmy c API to python",
//    -1, 
//    fourmy_module_methods
//};
//
//extern "C" PyMODINIT_FUNC PyInit__fourmy(void)
//{
//    Py_Initialize();
//
//    return PyModule_Create(&fourmy_module_definition);
//}

//BOOST_PYTHON_MODULE(fourmy)
//{
//    using namespace boost::python;
//
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
