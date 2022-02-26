#pragma once

#include "umba/umba.h"
#include "umba/simple_formatter.h"
#include "umba/string_plus.h"

#include <string>


//----------------------------------------------------------------------------
static const char *infoLogSectionSeparator = // "--------------------------------------"; // 38
"------------------------------------------------------------------------------"; // 78


//----------------------------------------------------------------------------
template<typename StreamType> inline
StreamType& printInfoLogSectionHeader( StreamType &s, std::string secCaption )
{
    using namespace umba::omanip;

    secCaption += ":";

    s << "\n";
    s << infoLogSectionSeparator << "\n";
    s << caption << secCaption << normal << "\n";
    s << std::string(secCaption.size(), '-');
    s << "\n";

    return s;
}

//----------------------------------------------------------------------------
template<typename StreamType> inline
StreamType& printInfoLogSectionHeader( StreamType &s, const char* secCaption )
{
    return printInfoLogSectionHeader( s, std::string(secCaption) );
}

//----------------------------------------------------------------------------

inline
std::string substTextRaw( std::string text, const std::vector< std::pair<std::string,std::string> >  &rawSubstitutions )
{
    std::vector< std::string::size_type > positions( rawSubstitutions.size(), 0 ); // инит нулями по количеству искомых строк

    // rawSubstitutions у нас сделаны из map с дефолтным less -> отсортированы по возрастанию, и те имена, что длиннее при одинаковой начальной части - будут в конце
    // а мы хотим искать сначала самые длинные - значит, надо искать с конца

    // Тут мы полагаемся на тот порядок, который получен из map - но это однократная функция "по месту", просто чтобы не захламлять main

    auto findNextPosition = [&]() // -> std::vector< std::string::size_type >::size_type
        {
            std::vector< std::string::size_type >::size_type idxMinFound = positions.size();

            std::vector< std::string::size_type >::size_type i = positions.size();
            for( ; i-->0; )
            {
                if (positions[i]==std::string::npos)
                    continue; // данная строка была ранее не найдена в тексте, дальнейший поиск не имеет смысла

                positions[i] = text.find(rawSubstitutions[i].first, positions[i]); // ищем очередную строку с предыдущей позиции
                if (positions[i]==std::string::npos)
                    continue; // не нашли

                // Обе ветки одинаковы, но в отладке удобнее понимать, по какому условию нашли результат
                if (idxMinFound>=positions.size())
                {
                    idxMinFound = i; // первое найденное
                }
                else if (positions[i]<positions[idxMinFound]) // найденное меньше, чем найденное ранее
                {
                    idxMinFound = i;
                }
            }

            return idxMinFound;

        };

    
    auto updateStartPositions = [&]( std::string::size_type pos )
        {
            // Похуй, откуда начинать, пусть цикл будет одинаковый
            std::vector< std::string::size_type >::size_type i = positions.size();
            for( ; i-->0; )
            {
                if (positions[i]==std::string::npos)
                    continue; // данная строка была ранее не найдена в тексте, не надо обновлять стартовую позицию

                positions[i] = pos;
            }
        };


    std::vector< std::string::size_type >::size_type 
    foundStringIdx = findNextPosition();

    while(foundStringIdx<rawSubstitutions.size())
    {
        std::string::size_type replacePos  = positions[foundStringIdx];
        std::string::size_type replaceSize = rawSubstitutions[foundStringIdx].first.size();
        text.replace(replacePos, replaceSize, rawSubstitutions[foundStringIdx].second);

        std::string::size_type replacementSize = rawSubstitutions[foundStringIdx].second.size();
        std::string::size_type newStartPos = replacePos + replacementSize;

        updateStartPositions(newStartPos);
        foundStringIdx = findNextPosition();
    }

    return text;

}







