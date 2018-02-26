// c++ -shared cgal.cpp -lCGAL -lgmp -I/usr/include/python3.5m/ -lboost_python-py35 -fPIC -o cgal.so
//
#include "geometry.hh"
#include "wkb.hh"
#include "delaunay.hh"

#include <iostream>
#include <vector>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

//#include "union.hh"
//#include "delaunay.hh"

typedef unsigned char byte;

namespace boost 
{ 
    namespace python {
        bool hasattr(object o, const char* name) 
        {
            return PyObject_HasAttrString(o.ptr(), name);
        }
    } 
}

void translate_out_of_range(const std::out_of_range & e)
{
    PyErr_SetString(PyExc_IndexError, e.what());
}

namespace python
{
    geometry::geometry<double> loads(const boost::python::object & object)
    {
        object != boost::python::object() || die("cannot load wkb from None");
        if (boost::python::hasattr(object, "wkb"))
        {
            const boost::python::object wkb = object.attr("wkb");
            return wkb::loads<double>( reinterpret_cast<const unsigned char *>(PyBytes_AsString(wkb.ptr())));

        }
        else if (PyBytes_Check(object.ptr()))
        {
            return wkb::loads<double>( reinterpret_cast<const unsigned char *>(PyBytes_AsString(object.ptr())));
        }
        die("cannot convert python obbjet to geometry");
        return geometry::geometry<double>();
    }

    geometry::multipolygon<double> tessellate(const boost::python::object & object)
    {
        return delaunay::tessellate(boost::get< geometry::polygon<double> >(loads(object)));
    }

    double point_getitem(const geometry::point<double> & p, int i)
    {
        return p[i];
    }

    size_t point_len(const geometry::point<double> & p)
    {
        return p.size();
    }

    boost::python::str point_repr(const geometry::point<double> & p)
    {
        return ("("+std::to_string(p[0])+", "+std::to_string(p[1])+")").c_str();
    }

    boost::python::object point_array_interface(const geometry::point<double> & p)
    {
        boost::python::dict result;
        result["version"] = 3;
        result["typestr"] = wkb::system_byteoder() == wkb::little_endian ? "<f8" : ">f8";
        result["shape"] = boost::python::make_tuple(1, 2);
        result["data"] = &p[0];
        return result;
    }

    boost::python::object linestring_array_interface(const geometry::linestring<double> & l)
    {
        boost::python::dict result;
        result["version"] = 3;
        result["typestr"] = wkb::system_byteoder() == wkb::little_endian ? "<f8" : ">f8";
        result["shape"] = boost::python::make_tuple(l.size(), 2);
        sizeof(geometry::point<double>) == 16 || die("big bug here");
        result["data"] = &l[0][0];
        return result;
    }

}

BOOST_PYTHON_MODULE(fourmy)
{
    using namespace boost::python;

    register_exception_translator<std::out_of_range>(translate_out_of_range);
    
    enum_<wkb::byteorder_t>("wkb_byteorder")
        .value("big_endian", wkb::big_endian)
        .value("little_endian", wkb::little_endian)
        ;

    enum_<wkb::type_t>("wkb_type")
        .value("Point", wkb::point)
        .value("LineString", wkb::linestring)
        .value("Polygon", wkb::polygon)
        .value("MultiPoint", wkb::multipoint)
        .value("MultiLineString", wkb::multilinestring)
        .value("MultiPolygon", wkb::multipolygon)
        .value("GeometryCollection", wkb::geometrycollection)
        ;

    class_<geometry::point<double>>("Point", init<>())
        .def("__len__", &python::point_len) 
        .def("__getitem__", &python::point_getitem) 
        .def("__repr__", &python::point_repr) 
        .add_property("__array_interface__", &python::point_array_interface) 
        ;
    class_<geometry::linestring<double>>("LineString", init<>())
        .def(vector_indexing_suite<geometry::linestring<double>>() )
        .add_property("__array_interface__", &python::linestring_array_interface) 
        ;

    class_<geometry::polygon<double>>("Polygon", init<>())
        .def(vector_indexing_suite<geometry::polygon<double>>() )
        ;

    class_<geometry::multipolygon<double>>("MultiPolygon", init<>())
        .def(vector_indexing_suite<geometry::multipolygon<double>>() )
        ;

    class_<geometry::geometry<double>>("Geometry", init<>())
        ;

    def("loads", &python::loads);
    def("tessellate", &python::tessellate);
}

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
        ;


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
