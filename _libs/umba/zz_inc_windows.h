#pragma once

#if defined(UMBA_WIN32_USED) || defined(WIN32) || defined(_WIN32)

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif

    #ifndef STRICT
        #define STRICT
    #endif


    // Делаем вид, что WinSock.h уже подключен, иначе, если потом подключается 
    // WinSock2.h, то будет конфликт

    #ifndef _WINSOCKAPI_
        #define _WINSOCKAPI_
    #endif

    #include "windows.h"

    #if defined(min)
        #undef min
    #endif

    #if defined(max)
        #undef max
    #endif

#endif

