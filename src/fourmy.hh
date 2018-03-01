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
    
    typedef void ( *fourmy_error_handler_t ) ( const char* );

    //! @brief fourmy_geometry_t is an opaque pointer type that is used to represent a pointer to fourmy::geometry
    //! @ingroup capi

    typedef void fourmy_geometry_t;

    typedef enum {
        FOURMY_TYPE_POINT               = 1,
        FOURMY_TYPE_LINESTRING          = 2,
        FOURMY_TYPE_POLYGON             = 3,
        FOURMY_TYPE_MULTIPOINT          = 4,
        FOURMY_TYPE_MULTILINESTRING     = 5,
        FOURMY_TYPE_MULTIPOLYGON        = 6,
        FOURMY_TYPE_INVALID             = 255 // used for debug only
    } fourmy_geometry_type_t;

    //@brief call fourmy_delete on the returned pointer once done
    FOURMY_API_EXPORT fourmy_geometry_t * fourmy_from_wkb(const unsigned char* wkb, int* srid);

    //@brief call fourmy_free on the buffer once done
    FOURMY_API_EXPORT void fourmy_as_wkb(fourmy_geometry_t * geom, char** buffer, size_t* len, int srid);

    FOURMY_API_EXPORT void fourmy_delete(fourmy_geometry_t * geom);

    FOURMY_API_EXPORT void fourmy_free(char * mem);

    FOURMY_API_EXPORT fourmy_geometry_type_t fourmy_type(fourmy_geometry_t * geom);

    FOURMY_API_EXPORT fourmy_geometry_t * fourmy_tesselate(fourmy_geometry_t * geom);

    FOURMY_API_EXPORT void fourmy_set_alloc_handlers( fourmy_alloc_handler_t alloc_handler, fourmy_free_handler_t free_handler);

    FOURMY_API_EXPORT void fourmy_set_error_handlers( fourmy_error_handler_t warning_handler, fourmy_error_handler_t error_handler );

#ifdef __cplusplus
}
#endif

