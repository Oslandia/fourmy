#pragma once

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#   define FOURMY_API_IMPORT __declspec(dllimport)
#   define FOURMY_API_EXPORT __declspec(dllexport)
#else
#   define FOURMY_API_IMPORT
#   define FOURMY_API_EXPORT
#endif

#include <stddef.h>

#ifdef __cplusplus
extern "C" 
{
#endif
    typedef void* ( *fourmy_alloc_handler_t ) ( size_t );
    typedef void ( *fourmy_free_handler_t ) ( void* );

    typedef void fourmy_geometry_t;


    typedef enum {
        FOURMY_TYPE_EMPTY               = 0,
        FOURMY_TYPE_POINT               = 1,
        FOURMY_TYPE_LINESTRING          = 2,
        FOURMY_TYPE_POLYGON             = 3,
        FOURMY_TYPE_MULTIPOINT          = 4,
        FOURMY_TYPE_MULTILINESTRING     = 5,
        FOURMY_TYPE_MULTIPOLYGON        = 6,
        //FOURMY_TYPE_GEOMETRYCOLLECTION  = 7,
        //     TYPE_CIRCULARSTRING      = 8,
        //     TYPE_COMPOUNDCURVE       = 9,
        //     TYPE_CURVEPOLYGON        = 10,
        //     TYPE_MULTICURVE          = 11, //abstract
        //     TYPE_MULTISURFACE        = 12, //abstract
        //     TYPE_CURVE               = 13, //abstract
        //     TYPE_SURFACE             = 14, //abstract
        //FOURMY_TYPE_POLYHEDRALSURFACE   = 15,
        //FOURMY_TYPE_TRIANGULATEDSURFACE = 16,

        //-- not official codes
        //FOURMY_TYPE_TRIANGLE            = 100, //17 in Wikipedia???
        //FOURMY_TYPE_SOLID               = 101,
        //FOURMY_TYPE_MULTISOLID          = 102
        FOURMY_TYPE_INVALID               = 255
    } fourmy_geometry_type_t;

    //extern const long fourmy_type_point;
    //extern const long fourmy_type_linestring;
    //extern const long fourmy_type_polygon;
    //extern const long fourmy_type_multipoint;
    //extern const long fourmy_type_multilinestring;
    //extern const long fourmy_type_multipolygon;
    //extern const long fourmy_type_empty;

    FOURMY_API_EXPORT fourmy_geometry_t * fourmy_from_wkb(const unsigned char* wkb);

    FOURMY_API_EXPORT void fourmy_delete(fourmy_geometry_t * geom);

    FOURMY_API_EXPORT fourmy_geometry_type_t fourmy_type(fourmy_geometry_t * geom);

    FOURMY_API_EXPORT fourmy_geometry_t * fourmy_tesselate(fourmy_geometry_t * geom);

    FOURMY_API_EXPORT void fourmy_set_alloc_handlers( fourmy_alloc_handler_t alloc_handler, fourmy_free_handler_t free_handler);

#ifdef __cplusplus
}
#endif

