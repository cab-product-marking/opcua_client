#pragma once

#include <ctime>

#ifdef __linux__
#define DIR_RES         "../res/"
#define DIR_BUILD       "../build/"
#else
#define DIR_RES         "../../res/"
#define DIR_BUILD       "../../build/"    
#endif

namespace oswrapper
{
    /* Operating system wrappers */
    
    /* Work with time */
    void
    localtime(const time_t* time_time_t, tm* time_tm);

    /* More local cases ... */

} // namespace osw

/* Eof */