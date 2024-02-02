#pragma once

/**
 * Case Windows and Linux
 * ^^^^^^^^^^^^^^^^^^^^^^
*/

#include <ctime>

#ifdef __linux__
#define DIR_RES         "../res/"
#define DIR_BUILD       "../build/"
#else
#define DIR_RES         "../../res/"
#define DIR_BUILD       "../../build/"    
#endif

namespace lset
{
    /* Work with time */
    void
    wrapper_localtime (const time_t* time_time_t, tm* time_tm);

    /* More local cases */




} // namespace lset

/* Eof */