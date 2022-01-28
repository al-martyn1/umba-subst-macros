#pragma once

//----------------------------------------------------------------------------

/*! \file
    \brief Совместимая версия alloca/_alloca
*/

//----------------------------------------------------------------------------

#include "umba/zz_detect_environment.h"


#ifdef _WIN32

    #if !defined(_INC_MALLOC) && !defined(_MALLOC_H_) && !defined(_MALLOC_H)
        #include <malloc.h>
    #endif
    #ifndef alloca
        #define alloca  _alloca
    #endif

#else

    #include <alloca.h>
    #ifndef _alloca
        #define _alloca  alloca
    #endif

#endif
