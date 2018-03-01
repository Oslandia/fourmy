#pragma once

#include <vector>
#include <array>
#include <boost/variant.hpp>

namespace geometry
{
    struct empty {};
    template<typename T> using xy = std::array<T, 2>;
    template<typename T> using point = xy<T>;
    template<typename T> struct linestring : std::vector< xy<T> >{using std::vector< xy<T> >::vector;};
    template<typename T> struct polygon : std::vector< linestring<T> >{using std::vector< linestring<T> >::vector;};
    template<typename T> struct multipoint : std::vector< xy<T> >{using  std::vector< xy<T> >::vector;};
    template<typename T> struct multilinestring : std::vector< linestring<T> >{using std::vector< linestring<T> >::vector;};
    template<typename T> struct multipolygon : std::vector< polygon<T> >{using std::vector< polygon<T> >::vector;};
    template<typename T> using geometry = boost::variant<
        empty,
        point<T>,
        linestring<T>,
        polygon<T>,
        multipoint<T>,
        multilinestring<T>,
        multipolygon<T>
        >;
}
