#pragma once

#include <iostream>

namespace utility
{
    template< typename E >
    bool die(E exc)
    {
        throw exc;
        return true;
    }

    bool die(const char * msg)
    {
        throw std::runtime_error(msg);
        return true;
    }

}

