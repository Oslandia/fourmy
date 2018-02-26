#pragma once

#include <cassert>
#include <exception>
#include <string>
#include <vector>
#include <array>
#include <boost/variant.hpp>

namespace geometry
{

    
    struct empty {};
    template<typename T> using xy = std::array<T, 2>;
    template<typename T> using point = xy<T>;
    template<typename T> using linestring = std::vector< xy<T> >;
    template<typename T> using polygon = std::vector< linestring<T> >;
    template<typename T> using multipoint = std::vector< xy<T> >;
    template<typename T> using multilinestring = std::vector< linestring<T> >;
    template<typename T> using multipolygon = std::vector< polygon<T> >;
    template<typename T> using geometry = boost::variant<
        empty,
        point<T>,
        linestring<T>,
        polygon<T>,
        multipoint<T>,
        multilinestring<T>,
        multipolygon<T>
        >;

    //template <typename T>
    //struct xy
    //{
    //    size_t size() const {return 2;}
    //    T& operator[](size_t i) {return _coords[i];}
    //    T& x() {return _x;}
    //    T& y() {return _y;}
    //    T x() const {return _x;}
    //    T y() const {return _y;}
    //private:
    //    union {
    //    struct {T _x; T _y;};
    //    T _coords[2];
    //    };
    //};
//
//    // storage for geometries
//    struct geometry
//    {
//        geometry(boost::python::object wkb)
//            : _wkb(PyBytes_Check(wkb.ptr()) ? wkb : wkb.attr("wkb"))
//        {
//            _wkb != boost::python::object() || die("cannot create geometry from None");
//            // @todo check conformance of object else raise
//            byteorder() == system_byteoder() || die("geometry byteorder is different than system byteorder");
//        }
//
//        geometry(wkb_type type)
//            : _data(1, static_cast<unsigned char>(system_byteoder()))
//        {
//            push_back_int(int(type));
//        }
//
//        wkb_type type() const
//        {
//            return wkb_type(get_int(bytes() + 1) & 0xff);
//        }
//
//        bool has_z() const
//        {
//            return (get_int(bytes() + 1) & 0x80000000) != 0;
//        }
//
//        bool has_m() const
//        {
//            return (get_int(bytes() + 1) & 0x40000000) != 0;
//        }
//
//        bool has_srid() const
//        {
//            return (get_int(bytes() + 1) & 0x20000000) != 0;
//        }
//
//        int srid() const
//        {
//            return has_srid() ? get_int(bytes() + 1 + 4) : 0;
//        }
//
//        boost::python::object wkb() const
//        {
//            return _wkb != boost::python::object()
//                ? _wkb
//                : boost::python::object(boost::python::handle<>(PyBytes_FromStringAndSize(reinterpret_cast<const char*>(&_data[0]), _data.size())));
//        }
//
//    protected:
//        // storage is either const from a python object, or non const from other ctor
//        const boost::python::object _wkb;
//        std::vector<unsigned char> _data;
//
//        const unsigned char * bytes() const
//        {
//            return _wkb != boost::python::object()
//                ? reinterpret_cast<const unsigned char *>(PyBytes_AsString(_wkb.ptr()))
//                : &_data[0];
//        }
//
//        const unsigned char * geom_bytes() const
//        {
//            return bytes() + 1 + 4 + (has_srid() ? 4 : 0);
//        }
//
//        int get_int(const unsigned char *buf) const
//        {
//            return *reinterpret_cast<const int*>(buf);
//        }
//
//        void push_back_int(int value)
//        {
//            _wkb != boost::python::object() || die();
//            const size_t sz = _data.size();
//            _data.resize(sz+4);
//            *reinterpret_cast<int*>(&_data[sz]) = value;
//        }
//
//        wkb_byteorder byteorder() const
//        {
//            return wkb_byteorder(*bytes());
//        }
//    };
//
//    template<typename T, size_t DIM>
//    struct coordinate
//    {
//        coordinate(size_t dim, T * data)
//            : _dim(dim)
//            , _data(data)
//        {}
//
//        T operator[](int i) const
//        {
//            const int j = i>=0 ? i : DIM - i;
//            (j < int(DIM) && j >= 0) || die(std::out_of_range("coordinate index "+std::to_string(i)+" is out of range"));
//            return _data[j];
//        }
//
//        T & operator[](int i)
//        {
//            const int j = i>=0 ? i : _dim - i;
//            (j < int(_dim) && j >= 0) || die(std::out_of_range("coordinate index "+std::to_string(i)+" is out of range"));
//            return _data[j];
//        }
//
//        size_t size() const
//        {
//            return _dim;
//        }
//
//    private:
//        T * _data;
//    };
//
//    template<typename T, size_t DIM>
//    struct coordinate_sequence
//    {
//        coordinate_sequence(size_t size, T * data)
//            : _size(size)
//            , _data(data)
//        {}
//
//        coordinate<T, DIM> operator[](int i)
//        {
//            const int j = i>=0 ? i : _size - i;
//            (j < int(_size) && j >= 0) || die(std::out_of_range("coordinate_sequence index "+std::to_string(i)+" is out of range"));
//            return coordinate<T, DIM>(_dim, _data + j*_dim);
//        }
//
//        coordinate<T, DIM> operator[](int i) const
//        {
//            const int j = i>=0 ? i : _size - i;
//            (j < int(_size) && j >= 0) || die(std::out_of_range("coordinate_sequence index "+std::to_string(i)+" is out of range"));
//            return coordinate<T, DIM>(_dim, _data + j*_dim);
//        }
//
//        size_t size() const
//        {
//            return _size;
//        }
//
//    private:
//        size_t _size;
//        T * _data;
//    };
//
//    struct point: geometry
//    {
//        point(const geometry & geom)
//            : geometry(geom)
//        {
//            type()==wkb_point || die("geometry is not a point");
//            has_z() && die("point have no z dimension, use pointz[m]");
//            has_m() && die("point have no m dimension, use point[z]m");
//        }
//
//        coordinate_sequence<const double, 2> coords() const
//        {
//            return coordinate_sequence(2, reinterpret_cast<const double *>(geom_bytes()));
//        }
//    };
//
//    struct linestring: geometry
//    {
//        linestring(const geometry & geom)
//            : geometry(geom)
//        {
//            type()==wkb_linestring || die("geometry is not a linestring");
//            has_z() && die("linestring have no z dimension, use linestringz[m]");
//            has_m() && die("linestring have no m dimension, use linestring[z]m");
//        }
//
//        coordinate_sequence coords() const
//        {
//            return coordinate_sequence(get_int(geom_bytes()), 2+(has_z() ? 1 : 0)+(has_m() ? 1 : 0), reinterpret_cast<const double *>(geom_bytes()+4));
//        }
//    };
}
