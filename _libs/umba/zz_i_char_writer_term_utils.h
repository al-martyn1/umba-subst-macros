#pragma once

#if !defined(UMBA_I_CHAR_WRITER_H)
    #error "zz_i_char_writer_term_utils.h must be included from zz_i_char_writer_term.h only"
#endif



//! Утилиты для работы с терминалами
namespace utils
{

//! Форматирование unsigned в сырой char буфер. Буфера должно быть достаточно (32 хватает для 64х бит с большим запасом)
inline
char* formatUnsignedDec( char *pBuf, unsigned u)
{
    char *pBufOrg = pBuf;
    while(u)
    {
        unsigned d = u % 10;
        *pBuf++ = '0' + d;
        u /= 10;
    }

    if (pBufOrg == pBuf)
    {
        *pBuf++ = '0';
        return pBuf;
    }

    //*pBuf = 0;
    //std::strrev(pBufOrg);
    std::reverse(pBufOrg, pBuf);
    return pBuf;
}


}; // namespace utils

