#pragma once

#include "utility.hh"
#include "geometry.hh"

#include<iostream>


namespace wkb
{
    using namespace utility;

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
    std::vector<unsigned char> dump(const geometry::geometry<T> & geom, int srid=0)
    {
        std::vector<unsigned char> result;
        result.push_back('t');
        result.push_back('o');
        result.push_back('t');
        result.push_back('o');
        return std::move(result);
    }

    template<typename T>
    typename geometry::geometry<T> load(const unsigned char * buffer, int * srid)
    {
        const byteorder_t byteorder = byteorder_t(pop_byte(buffer)); 
        byteorder == system_byteoder() || die("wkb byteorder isn't the same as system byteorder");
        const unsigned int flags = pop_uint(buffer);
        const type_t type  = type_t(flags  & 0xff);
        const bool has_z = (flags & 0x80000000) != 0;
        const bool has_m = (flags & 0x40000000) != 0;
        const bool has_srid = (flags & 0x20000000) != 0;

        if (srid)
            *srid = has_srid ? pop_int(buffer) : 0;
        else if (has_srid)
            pop_int(buffer);

        std::cout << type << has_z << has_m << has_srid << " flags\n";

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
