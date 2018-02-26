#pragma once

#include "geometry.hh"
#include "wkb.hh"
#include "delaunay.hh"
#include "utility.hh"

#include <boost/python.hpp>

namespace boost 
{ 
    namespace python {
        bool hasattr(object o, const char* name) 
        {
            return PyObject_HasAttrString(o.ptr(), name);
        }
    } 
}

namespace python
{
    using namespace utility;

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

    geometry::point<double> linestring_getitem(const geometry::linestring<double> & l, int i)
    {
        return l[i];
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
        result["data"] = &(l[0][0]);
        return result;
    }

}


