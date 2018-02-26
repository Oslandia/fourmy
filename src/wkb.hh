#pragma once

#include "utility.hh"
#include "geometry.hh"

#include<iostream>

using namespace utility;

namespace wkb
{
    enum byteorder_t {
        big_endian = 0,
        little_endian =1
    };

    byteorder_t system_byteoder()
    {
        union {
            uint32_t i;
            char c[4];
        } bint = {0x01020304};

        return bint.c[0] == 1 ? big_endian : little_endian; 
    }     

    enum type_t {
        point = 1,
        linestring = 2,
        polygon = 3,
        multipoint = 4,
        multilinestring = 5,
        multipolygon = 6,
        geometrycollection = 7
    };


    template< class W >
    struct buffer
    {
        buffer(W && wkb)
            :_wkb(std::move(wkb))
        {
            byteorder() == system_byteoder() || die("byteorder is different than system byteorder");
        }

        type_t type() const
        {
            return type(get_int(bytes() + 1) & 0xff);
        }

        bool has_z() const
        {
            return (get_int(bytes() + 1) & 0x80000000) != 0;
        }

        bool has_m() const
        {
            return (get_int(bytes() + 1) & 0x40000000) != 0;
        }

        bool has_srid() const
        {
            return (get_int(bytes() + 1) & 0x20000000) != 0;
        }

        int srid() const
        {
            return has_srid() ? get_int(bytes() + 1 + 4) : 0;
        }

        const unsigned char * geom_bytes() const
        {
            return bytes() + 1 + 4 + (has_srid() ? 4 : 0);
        }

    protected:
        W _wkb;

        const unsigned char * bytes() const 
        {
            return _wkb.bytes();
        }


        int get_int(const unsigned char *buf) const
        {
            return *reinterpret_cast<const int*>(buf);
        }

        byteorder_t byteorder() const
        {
            return byteorder_t(*bytes());
        }
    };

    unsigned char pop_byte(const unsigned char * & buffer)
    {
        const unsigned char res = *buffer;
        buffer += 1;
        return res;
    }

    int pop_int(const unsigned char * & buffer)
    {
        const int res = *reinterpret_cast<const int *>(buffer);
        buffer += 4;
        return res;
    }

    unsigned int pop_uint(const unsigned char * & buffer)
    {
        const unsigned int res = *reinterpret_cast<const unsigned int *>(buffer);
        buffer += 4;
        return res;
    }

    double pop_double(const unsigned char * & buffer)
    {
        const double res = *reinterpret_cast<const double *>(buffer);
        buffer += 8;
        return res;
    }

    template< typename T >
    geometry::point<T> pop_point(const unsigned char * & buffer)
    {
        const double x = pop_double(buffer);
        const double y = pop_double(buffer);
        return std::move(geometry::point<T>( {x, y} ));
    }

    template< typename T >
    geometry::linestring<T> pop_linestring(const unsigned char * & buffer)
    {
        const size_t npoints = pop_uint(buffer);
        geometry::linestring<T> result;
        for (size_t p=0; p<npoints; p++)
            result.push_back(pop_point<T>(buffer));
        return std::move(result);
    }

    template< typename T >
    geometry::polygon<T> pop_polygon(const unsigned char * & buffer)
    {
        const size_t nrings = pop_uint(buffer);
        geometry::polygon<T> result;
        for (size_t r=0; r<nrings; r++)
            result.push_back(pop_linestring<T>(buffer));
        return std::move(result);
    }

    template<typename T>
    typename geometry::geometry<T> loads(const unsigned char * buffer)
    {
        const byteorder_t byteorder = byteorder_t(pop_byte(buffer)); 
        byteorder == system_byteoder() || die("wkb byteorder isn't the same as system byteorder");
        const unsigned int flags = pop_uint(buffer);
        const type_t type  = type_t(flags  & 0xff);
        const bool has_z = (flags & 0x80000000) != 0;
        const bool has_m = (flags & 0x40000000) != 0;
        const bool has_srid = (flags & 0x20000000) != 0;
        const int srid = has_srid ? pop_int(buffer) : 0;

        (void) srid;

        switch (type)
        {
            case point:
                if (!has_z && !has_m)
                    return pop_point<T>(buffer);
                else if (has_z && !has_m)
                    die("incomplete implementation");
                else if (!has_z && has_m)
                    die("incomplete implementation");
                else
                    die("incomplete implementation");
            case linestring:
                if (!has_z && !has_m)
                    return pop_linestring<T>(buffer);
                else if (has_z && !has_m)
                    die("incomplete implementation");
                else if (!has_z && has_m)
                    die("incomplete implementation");
                else
                    die("incomplete implementation");
            case polygon:
                if (!has_z && !has_m)
                    return pop_polygon<T>(buffer);
                else if (has_z && !has_m)
                    die("incomplete implementation");
                else if (!has_z && has_m)
                    die("incomplete implementation");
                else
                    die("incomplete implementation");
            default:
                die("incomplete implementation");
                return geometry::geometry<T>();
        }
    }
}
