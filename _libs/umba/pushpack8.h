/*! \file
    \brief Сохранение текущих настроек и установка упаковки структур по 8
*/

// http://www.keil.com/support/man/docs/armcc/armcc_chr1359124990875.htm
// http://www.keil.com/support/man/docs/armcc/armcc_chr1359124982232.htm
// http://www.keil.com/support/man/docs/armcc/armcc_chr1359124980173.htm
// http://www.keil.com/support/man/docs/armcc/armcc_chr1359124968737.htm
#if defined(_WIN32)
    #include <pshpack8.h>
#else
    #pragma pack(push,8)
#endif

