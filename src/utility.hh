#pragma once

#include <iostream>

namespace utility
{
    template< typename E >
    bool die_(const char * file, int line, E exc)
    {
        throw E(std::string(file)+":"+std::to_string(line)+" "+exc.what());
        return true;
    }

    bool die_(const char * file, int line, const char * msg)
    {
        throw std::runtime_error(std::string(file)+":"+std::to_string(line)+" "+std::string(msg));
        return true;
    }

#define die(msg) die_(__FILE__, __LINE__, (msg))

}

