#pragma once

//----------------------------------------------------------------------------

/*! 
    \file
    \brief Разбор INI-файла
 */

//----------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <map>

#include "string_plus.h"
#include "exception.h"
#include "numeric_version.h"
#include "name_version.h"
#include "rgbquad.h"

//----------------------------------------------------------------------------

#if defined(min)
    #undef min
#endif

#if defined(max)
    #undef max
#endif

//----------------------------------------------------------------------------

#include <algorithm>

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------

namespace umba
{

//! return len of comment chars or 0 for non-comment string
inline
size_t isIniCommentLine( std::string str )
{
    namespace strplus = umba::string_plus;

    strplus::ltrim( str );
    if (strplus::starts_with( str, "#"))
        return 1;
    if (strplus::starts_with( str, ";"))
        return 1;
    if (strplus::starts_with( str, "//"))
        return 2;
    if (strplus::starts_with( str, "--"))
        return 2;
    return 0;
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Статические строки для фильтров. Возможно, надо переделать будет, но так было проще на скорую руку
namespace ini_static_strings
{

static const char* filterLatinUpper          = " -ABCDEFGHIJKLMNOPQRSTUVWXYZ";                                    //!< Латиница в верхнем регистре и '-', как допустимый в имени символ
static const char* filterLatinLower          = " -abcdefghijklmnopqrstuvwxyz";                                    //!< Латиница в нижнем регистре и '-', как допустимый в имени символ
static const char* filterLatin               = " -ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";          //!< Латиница и '-', как допустимый в имени символ
static const char* filterLatinOnlyUpper      = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";                                      //!< Только латиница в верхнем регистре
static const char* filterLatinOnlyLower      = "abcdefghijklmnopqrstuvwxyz";                                      //!< Только латиница в нижнем регистре
static const char* filterLatinOnly           = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";            //!< Только латиница
static const char* filterIdentifier          = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_"; //!< Только латиница, цифры и подчёркивание - допустимые для идентификатора последующие символы
static const char* filterIdentifierFirst     = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";           //!< Допустимый для идентификатора первый символ

} // namespace ini_static_strings

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Интерфейс читателя файлов для движка парсера INI-файлов
/*! Был сделан на скорую руку как абстракция для доступа к файловой системе.
    На тот момент ещё не имелось абстракции FileCache.

    \todo 
    //TODO: Нужно весь INI-reader переделать через LineView, а конкретно 
    IniFileReaderInterface упразднить и работать через FileCache.

    \todo 
    //TODO: Замечание по FileCache: нужно научить его не кешировать (вернее, разучить кешировать)
    содержимое файлов. Это нужно, чтобы можно было на MCU обрабатывать INI-файлы, которые великоваты для
    помещения в оперативную память. По крайней мере, после того, как файл обработан, хорошо бы вымарать его из кеша -
    даже если один отдельный файл не жирный, то развесистые инклюды на MCU в кеш не влезут
 */
class IniFileReaderInterface
{

public:

    //------------------------------
    //! Чтение файла в вектор строк. Какой используется перевод строки - зависит от реализации
    virtual 
    std::vector<std::string> readLines( std::string fileName ) = 0;


//protected:

    //------------------------------
    //! Собирает полное имя файла из указанного имени файла и имени базового файла (того, который производит включение)
    /*! Было сделано на скорую руку.
        На тот момент ещё не имелось абстракции IncludeFinder.

        \todo 
        //TODO: Нужно весь INI-reader переделать через LineView, а конкретно 
        данный метод упразднить/переделать через IncludeFinder.
     */
    virtual
    std::string composeFileName( std::string fileName, std::string baseName ) const
    {
        if (fileName.empty())
        {
            //throw umba::FileException( "Invalid (empty) file name", const std::string& fileName )
            throw std::invalid_argument("Invalid (empty) file name");
        }

        bool isAbs = false;
        if (fileName[0]=='\\' || fileName[0]=='/')  // unix abs path or windows current drive abs path
            isAbs = true;
        #if defined(WIN32) || defined(_WIN32)
        if (!isAbs && fileName.size()>1 && fileName[1]==':' )
           isAbs = true; // drive letter found
        if (!isAbs && fileName.size()>1 && fileName[0]=='\\' && fileName[1]=='\\')
           isAbs = true; // network or NT name
        #endif

        if (isAbs)
            return fileName;

        if (baseName.empty())
            return fileName;

        std::string basePath;
        
        size_t slashPos = baseName.find_last_of("/\\");
        
        if (slashPos!=std::string::npos)
        {
            basePath.assign( baseName, 0, slashPos);
        }

        if (basePath.empty())
            return fileName;

        if (fileName[0]=='/' || fileName[0]=='\\')
            fileName.erase(0,1);
        
        #if defined(WIN32) || defined(_WIN32)
        return basePath + std::string("\\") + fileName;
        #else
        return basePath + std::string("/") + fileName;
        #endif
    }

    //------------------------------
    //! Нормализует имя файла - разделители частей пути приводятся к одному виду
    /*! Было сделано на скорую руку.
        На тот момент ещё не имелось абстракции IncludeFinder.

        \todo 
        //TODO: Нужно весь INI-reader переделать через LineView, а конкретно 
        данный метод упразднить/переделать через IncludeFinder.
     */
    virtual
    std::string normalizeFileName( std::string fileName ) const
    {
        std::string res;
        for( auto ch : fileName )
        {
            if (ch=='\\' || ch=='/')
            {
                #if defined(WIN32) || defined(_WIN32)
                res.append(1,'\\');
                #else
                res.append(1,'/');
                #endif
            }
            else
            {
                res.append(1,ch);
            }
        }

        return res;
    }

    //------------------------------
    //! Делает из имени файла строковый key для его поиска в map
    /*! Было сделано на скорую руку.
        На тот момент ещё не имелось абстракций IncludeFinder / FileCache.

        \todo 
        //TODO: Нужно весь INI-reader переделать через LineView, а конкретно 
        данный метод упразднить/переделать через IncludeFinder / FileCache.
     */
    virtual
    std::string makeLookupKeyFromFileName( std::string fileName ) const
    {
        return umba::string_plus::tolower_copy(normalizeFileName(fileName));
    }

};

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
//! Реализация читателя файлов для движка парсера INI-файлов с использованием std::ifstream
class SimpleStdIfstreamIniFileReader : public IniFileReaderInterface
{
    //! Чтение файла в вектор строк. Какой используется перевод строки - зависит от реализации
    virtual 
    std::vector<std::string> readLines( std::string fileName ) override
    {
                                               // makeLookupKeyFromFileName
        std::string filenameToOpen = fileName; // normalizeFileName( composeFileName(fileName,baseName) );

        std::ifstream in(filenameToOpen.c_str());
        if (!in)
            throw umba::FileException( "Failed to open file", filenameToOpen );

        std::vector<std::string> lines;
       
        std::string s;
       
        while( std::getline( in, s ) )
        {
            lines.push_back(s);
        }

        return lines;

    }

};

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Класс разбора INI-файлов
/*! 
    Данный класс читает INI-файл, и дополнительно:
    - парсит include'ы, позволяя раскидать конфиг по разным файлам (опционально)
    - умеет в изначально предопределенные условия - ifdef/ifndef (опционально) - чтобы одни и те же конфиги можно было бы использовать 
      в разных сходных прогах на одну тему
    - умеет в define для задания условий на лету (опционально)

    Обработка происходит в два этапа:
    -# Чтение INI-файла и разбивка на строки, обработка условных конструкций и дефайнов, обработка подключаемых файлов
    -# Разбор полученного массива строк. Строки лежат в одном массиве (векторе), для разбора вызывается шаблонный метод parse,
       который принимает одним из своих аргументов объект-обработчик строки INI-файла.

    \note
    //NOTE:
    Иерархические INI-файлы (с древовидной структурой) не поддерживаются, но пока удавалось их симулировать при помощи плоских
    INI-файлов с отступами (которые на самом деле не мобязательны). Все любители иерархических древесных INI были удовлетворены,
    и никто не заметил найопки.

    \todo 
    //TODO: Нужно хорошечно его переделать, слишком многое втащено прямо сюда за неимением над тот момент
    адекватных изолированных абстракций

 */
class Ini
{

public:

    //! Тип строки
    enum class LineType
    {
        normal,
        empty,
        comment,
        section
    
    };

    //! Настроечный конструктор
    Ini( bool mergeMultilines = true, bool parseIncludes = true, std::string separators = ":=" )
    : m_fileNames() //("-")
    , m_lines()
    , m_mergeMultilines(mergeMultilines)
    , m_parseIncludes(parseIncludes)
    , m_separators( separators )
    , m_pFileReader(&m_defaultReader)
    {
        m_fileNames.push_back("-");
    }

    //! Конструктор копирования - можно использовать как для копирования только что инициализированного настроечного Ini, так и для копирования уже распарщенного
    Ini(const Ini &ini)
    : m_fileNames(ini.m_fileNames) // (ini.m_fileName)
    , m_lines(ini.m_lines)
    , m_mergeMultilines(ini.m_mergeMultilines)
    , m_parseIncludes(ini.m_parseIncludes)
    , m_separators(ini.m_separators)
    , m_pFileReader(&m_defaultReader)
    {
        if (ini.m_pFileReader != &ini.m_defaultReader)
            m_pFileReader = ini.m_pFileReader;
    }

    //! Конструктор из вектора строк - уже готовых в памяти, остальные параметры - настроечные
    Ini( const std::vector<std::string> &lines, bool mergeMultilines = true, bool parseIncludes = true, std::string separators = ":=")
    : m_fileNames() // ("-")
    , m_lines()
    , m_mergeMultilines(mergeMultilines)
    , m_parseIncludes(parseIncludes)
    , m_separators(separators)
    , m_pFileReader(&m_defaultReader)
    {
        m_fileNames.push_back("-");
        fromLines(lines);
    }

    //! Конструктор из вектора строк - уже готовых в памяти, остальные параметры - настроечные, и задаёт имя базового файла (для последующей обработки инклудов и прочего такого)
    Ini( const std::vector<std::string> &lines, const std::string &fileName, bool mergeMultilines = true, bool parseIncludes = true, std::string separators = ":=")
    : m_fileNames() // (fileName)
    , m_lines()
    , m_mergeMultilines(mergeMultilines)
    , m_parseIncludes(parseIncludes)
    , m_separators(separators)
    , m_pFileReader(&m_defaultReader)
    {
        m_fileNames.push_back(fileName);
        fromLines(lines);
    }

    //! Устанавливает файл-ридер объект
    void setFileReader( IniFileReaderInterface *pFileReader )
    {
        m_pFileReader = pFileReader;
    }

    //! Читает и препарсит файл из вектора строк
    void readFrom( const std::vector<std::string> &lines, const std::string &fileName = "-" )
    {
        //m_fileName = fileName;
        m_fileNames.clear();
        m_fileNames.push_back(fileName);
        fromLines(lines);
        //return true;
    }

    /*
    bool readFrom( std::istream &in, const std::string &fileName = "-" )
    {
        std::vector<std::string> lines;
       
        std::string s;
       
        while( std::getline( in, s ) )
        {
            lines.push_back(s);
        }

        return readFrom(lines, fileName);
    }
    */

    //! Читает и препарсит заданный файл
    void readFrom( const std::string &fileName )
    {
        //auto lines = m_pFileReader->readLines( fileName, std::string(), false );
        auto lines = m_pFileReader->readLines( fileName );
        readFrom(lines, fileName);
        /*
        std::ifstream in(fileName.c_str());
        if (!in)
            return false;
        return readFrom(in, fileName);
        */
    }

protected:
    #if 0
    void readFrom( const std::string &fileName, const std::string &baseName, bool lookupSystemFirst )
    {
        auto lines = m_pFileReader->readLines( fileName /* , baseName, lookupSystemFirst */ );
        //return 
        readFrom(lines, fileName);
    }
    #endif

    //! Какой-то хелпер - установить сет прочитанных файлов
    void setReadedFiles( const std::set<std::string> &s )
    {
        m_readedFiles = s;
    }

public:

    //! Если парсим из массива строк, то какое-то базовое имя надо задать руками
    void setFileName( const std::string &fileName )
    {
        //m_fileName = fileName;
        m_fileNames.clear();
        m_fileNames.push_back(fileName);
    }

    //! Возвращает набор строк - видимо, это после распарса кондишенов
    std::vector<std::string> getLines() const
    {
        std::vector<std::string> res;

        for( auto line : m_lines )
        {
            line.merge();
            res.push_back(line.text);
        }

        return res;
    }

    //! Оператор преобразования в вектор строк - то же, что и getLines - возвращает набор строк - видимо, это после распарса кондишенов. Нафуа такой implicit? XZ
    operator std::vector<std::string>() const
    {
        return getLines();
    }

    //! Собсно, сам разбор инишечки
    /*! Обработчик строки должен иметь вид
        bool handler( const std::string &fileName, const Ini::LineInfo &lineInfo )
        Обработчик возвращает false чтобы остановить разбор.
        В этом случае возвращаем также false
     */
    template<typename LineHandler>
    bool parse( const LineHandler &lineHandler )
    {
        for( const auto &line : m_lines )
        {
            size_t fileId = line.fileId;
            if (fileId<m_fileNames.size())
                line.fileName = m_fileNames[fileId];
            else
                line.fileName = "";

            bool res = lineHandler( line );

            line.fileName.clear();

            if (!res)
                return false;
        }

        return true;
    }


    //! Строка INI-файла с допками
    struct LineInfo;

    //! Установка кондишнсов для условного разбора INI-конфигов
    void setConditionals( const std::set<std::string> &conditionalTags, bool allowDefines = true )
    {
        m_useConditionals = true;
        m_allowDefines    = allowDefines;
        m_conditionalTags = conditionalTags;
    }



protected:

    //! Возвращает список строк с условными директивами
    std::vector<std::string> getConditionalDirectives() const
    {
        static std::vector<std::string> directives{ "", "#ifdef ", "#ifndef ", "#endif", "#define ", "#redefine ", "#undef ", "#else", "#include " };
        //if (directives.empty())
        return directives;
    }

    //! Какой-то startsWith хелпер
    size_t startsWithParsingHelper( const std::string &str, const std::vector<std::string> &names, size_t defRes ) const
    {
        size_t idx = 0;
        for(; idx!=names.size(); ++idx)
        {
            if (names[idx].empty())
                continue;

            if (umba::string_plus::starts_with(str,names[idx]))
               return idx;
        }

        return defRes;
    }

    //! Возвращает имя файла по его ID
    /*! 
        \todo 
        //TODO: Втащено сюда, потому что не было IncludeFinder / FileCache. А вообще оно тут ненужное.
     */
    std::string getFileName( size_t idx ) const
    {
        if (idx>=m_fileNames.size())
            return std::string();
        return m_fileNames[idx];
    }

    //! Возвращает true, если строка определена (как тэг/дефайн)
    /*! Строка может начинаться с символа '!' - тогда производится инверсия результата. Так:

        \code
        #ifdef  SOME_TAG -- сработает, если SOME_TAG определен
        #ifdef !SOME_TAG -- сработает, если SOME_TAG не определен
        \endcode

        Хз, нужно ли, но, имхо, если конструкция без \b else ветки, 
        то
        \code
        #ifdef !SOME_TAG
        \endcode

        выглядит нагляднее, чем

        \code
        #ifndef SOME_TAG
        \endcode
     */
    bool isDefined( std::string str ) const
    {
        bool bInverseResult = false;
        if (!str.empty() && str[0]=='!')
        {
            bInverseResult = true;
            str.erase(0,1);
        }

        umba::string_plus::trim( str );

        if (str.empty())
            return false;

        bool bDefined = m_conditionalTags.find(str)!=m_conditionalTags.end();

        if (bInverseResult)
            bDefined = !bDefined;

        return bDefined;
    }

    //! Задаёт дефайн/таг
    void setDefine( std::string str, std::string directive, size_t fileId, size_t lineNumber, bool redefine )
    {
        if (!m_allowDefines)
        {
            throw umba::FileParsingException( redefine ? "Unexpected #redefine" : "Unexpected #define", getFileName(fileId), lineNumber );
        }

        str.erase(0, directive.size());
        umba::string_plus::trim( str );

        if (str.empty())
            throw umba::FileParsingException( redefine ? "Missing #redefine value" : "Missing #define value", getFileName(fileId), lineNumber );

        if (!redefine)
        {
            if (m_conditionalTags.find(str)!=m_conditionalTags.end())
                throw umba::FileParsingException( "Value already defined", getFileName(fileId), lineNumber );
        }
        
        m_conditionalTags.insert(str);
    
    }

    //! Парсит инклюжу
    std::vector< LineInfo > processInclude( std::string includeFileName, size_t fileId, size_t lineNumber )
    {
        umba::string_plus::trim(includeFileName);

        if (includeFileName.empty())
            throw umba::FileParsingException( std::string("No file name taken for include"), getFileName(fileId), lineNumber );

        std::string baseName;
        if (!m_fileNames.empty() && m_fileNames[0]!="-")
            baseName = m_fileNames[0];

        std::string filenameToOpen   = m_pFileReader->normalizeFileName( m_pFileReader->composeFileName( includeFileName, baseName ) );
        std::string filenameToLookup = m_pFileReader->makeLookupKeyFromFileName(filenameToOpen);

        if (m_readedFiles.find(filenameToLookup)!=m_readedFiles.end())
            throw umba::FileParsingException( std::string("Recursive include derective detected - file '") + filenameToOpen + ("' already included"), getFileName(fileId), lineNumber );

        auto readedFiles = m_readedFiles;
        readedFiles.insert(filenameToLookup);

        Ini ini( m_mergeMultilines, m_parseIncludes, m_separators);
        ini.setFileReader(m_pFileReader);
        ini.setReadedFiles(readedFiles);

        if (m_useConditionals)
        {
            ini.setConditionals( m_conditionalTags, m_allowDefines );
        }

        try
        {
            ini.readFrom( filenameToOpen /* , baseName, false */  );
        }
        catch(const umba::FileException &e)
        {
            throw umba::FileParsingException( std::string("Failed to include file '") + e.getFileName() + std::string("'"), getFileName(fileId), lineNumber );
        }
        catch(...)
        {
            throw;
        }

        for( auto & iniLine : ini.m_lines )
        {
            if (iniLine.fileId!=(size_t)-1)
                iniLine.fileId += m_fileNames.size();
        }

        m_fileNames.insert( m_fileNames.end(), ini.m_fileNames.begin(), ini.m_fileNames.end() );
        //m_lines.insert( m_lines.end(), ini.m_lines.begin(), ini.m_lines.end() );

        return ini.m_lines;
            
    }

    //! Подсчитывает трушность текущего состояния, получая на входе стек условий
    bool conditionCalc( const std::vector<bool> &cVec ) const
    {
        bool res = true;
        for(auto c : cVec)
        {
            res = res && c;
        }
        return res;
    }

    //! Всовывает текущее вычисленное состояние условия в стек состояний/условий
    bool conditionPush( std::vector<bool> &cVec, bool c ) const
    {
        cVec.push_back(c);
        return conditionCalc(cVec);
    }

    //! Попает условие стека состояний/условий
    bool conditionPop( std::vector<bool> &cVec ) const
    {
        if (!cVec.empty())
           cVec.pop_back();
        return conditionCalc(cVec);
    }



public:

    //! Чтение INI из набора строк
    void fromLines( std::vector<std::string> lines )
    {
        m_lines.clear();

        //----------------------------
        // Convert to LineInfo's

        size_t lineNum = 0;
        //for( const auto& l : lines )
        for( auto l : lines )
        {
            LineInfo iniLine;
            iniLine.text   = l;
            iniLine.lineNumber = lineNum++;
            if (!m_fileNames.empty())
                iniLine.fileId = 0;

            if (isIniCommentLine(l))
                iniLine.type = LineType::comment;
            else
                iniLine.type = LineType::normal;

            m_lines.push_back(iniLine);
        }

        //----------------------------
        // Parse conditionals

        enum ConditionalsCompareResult
        {
            noMatch   = 0,
            ifdef     = 1,
            ifndef    = 2,
            endif     = 3,
            define    = 4,
            redefine  = 5,
            undef     = 6,
            ifdefElse = 7,
            include   = 8
        };
        // static std::vector<std::string> directives{ "", "#ifdef ", "#ifndef ", "#endif", "#define ", "#redefine ", "#undef ", "#else", "#include " };

        const std::vector<std::string> & directives = getConditionalDirectives();

        if (m_useConditionals)
        {
            //size_t startsWithParsingHelper( const std::string &str, const std::vector<std::string> &names, size_t defRes ) const
            std::vector<bool>       conditions;
            std::vector< LineInfo > tmpLineInfos; tmpLineInfos.reserve(m_lines.size());

            bool curCondition = conditionCalc(conditions);

            std::vector< LineInfo >::const_iterator it = m_lines.begin();
            for(; it != m_lines.end(); ++it )
            {
                std::string testStr = it->text;
                umba::string_plus::ltrim( testStr );

                ConditionalsCompareResult lineCond = (ConditionalsCompareResult)( startsWithParsingHelper( testStr, directives, 0 /* defRes */ ) );

                if (lineCond==noMatch) // not a directive
                {
                    if (curCondition)
                    {
                        tmpLineInfos.push_back(*it);
                    }
                }
                else if (lineCond==ifdef || lineCond==ifndef)
                {
                    testStr.erase(0, directives[lineCond].size());
                    umba::string_plus::trim( testStr );
                    
                    if (testStr.empty())
                        throw umba::FileParsingException( "Invalid (empty) condition", getFileName(it->fileId), it->lineNumber );

                    if (testStr=="!")
                        throw umba::FileParsingException( "Invalid condition", getFileName(it->fileId), it->lineNumber );

                    bool condition = isDefined(testStr);

                    if (lineCond==ifndef)
                         condition = !condition;

                    curCondition = conditionPush( conditions, condition );
                }
                else if (lineCond==ifdefElse)
                {
                    if (conditions.empty())
                        throw umba::FileParsingException( "Unexpected #else", getFileName(it->fileId), it->lineNumber );
                    conditions.back() = !conditions.back();
                    curCondition = conditionCalc(conditions);
                }
                else if (lineCond==endif)
                {
                    if (conditions.empty())
                        throw umba::FileParsingException( "Unexpected #endif", getFileName(it->fileId), it->lineNumber );
                    curCondition = conditionPop(conditions);
                }
                else if (lineCond==define)
                {
                    if (curCondition)
                        setDefine( testStr, directives[lineCond], it->fileId, it->lineNumber, false /* !redefine */  );
                }
                else if (lineCond==redefine)
                {
                    if (curCondition)
                        setDefine( testStr, directives[lineCond], it->fileId, it->lineNumber, true /* redefine */  );
                }
                else if (lineCond==undef)
                {
                    if (curCondition)
                    {
                        if (!m_allowDefines)
                        {
                            throw umba::FileParsingException( "Unexpected #undef", getFileName(it->fileId), it->lineNumber );
                        }

                        testStr.erase(0, directives[lineCond].size());
                        umba::string_plus::trim( testStr );
                        m_conditionalTags.erase(testStr);
                    }
                }
                else if (lineCond==include)
                {
                    if (curCondition && m_parseIncludes)
                    {
                        testStr.erase(0, directives[lineCond].size());
                        std::vector< LineInfo > includedLines = processInclude( testStr, it->fileId, it->lineNumber );
                        tmpLineInfos.insert( tmpLineInfos.end(), includedLines.begin(), includedLines.end() );
                    }
                }
                else
                {
                    throw umba::FileParsingException( std::string("Unexpected directive - '") + directives[lineCond] + std::string("'"), getFileName(it->fileId), it->lineNumber );
                }

            } // for

            std::swap(tmpLineInfos, m_lines);

        } // // Parse conditionals

        // Parse includes here
        if (m_parseIncludes && !m_useConditionals)
        {
            std::vector< LineInfo > tmpLineInfos; tmpLineInfos.reserve(m_lines.size());

            std::vector< LineInfo >::const_iterator it = m_lines.begin();
            for(; it != m_lines.end(); ++it )
            {
                std::string testStr = it->text;
                umba::string_plus::ltrim( testStr );

                ConditionalsCompareResult lineCond = (ConditionalsCompareResult)( startsWithParsingHelper( testStr, directives, 0 /* defRes */ ) );

                if (lineCond==include)
                {
                    if (!tmpLineInfos.empty())
                    {
                        auto &lastLineText = tmpLineInfos.back().text;
                        umba::string_plus::rtrim( lastLineText );
                        if (!lastLineText.empty() && lastLineText.back()=='\\')
                            lastLineText.erase( lastLineText.end()-1 );
                    }

                    testStr.erase(0, directives[lineCond].size());
                    std::vector< LineInfo > includedLines = processInclude( testStr, it->fileId, it->lineNumber );
                    tmpLineInfos.insert( tmpLineInfos.end(), includedLines.begin(), includedLines.end() );

                    if (!tmpLineInfos.empty())
                    {
                        auto &lastLineText = tmpLineInfos.back().text;
                        umba::string_plus::rtrim( lastLineText );
                        if (!lastLineText.empty() && lastLineText.back()=='\\')
                            lastLineText.erase( lastLineText.end()-1 );
                    }
                }
                else
                {
                    tmpLineInfos.push_back(*it);
                }

            } // for

            std::swap(tmpLineInfos, m_lines);

        } // // Parse includes here


        // Continue job here
        if (m_mergeMultilines)
            m_lines = mergeMultilines(m_lines);

        // Here we parse into parts
        for( auto& line : m_lines )
        {
            line.split(m_separators);
        }
    }

    //! Обработка концевиков бекслэш для склеивания строк
    static
    std::vector< LineInfo > mergeMultilines( const std::vector< LineInfo > &iniLines )
    {
        namespace strplus = umba::string_plus;

        std::vector< LineInfo > res;
    
        for( auto line : iniLines )
        {
            if (res.empty() || res.back().type != LineType::normal )
            {
                res.push_back(line);
                continue;
            }
    
            auto prevText = strplus::trim_copy(res.back().text);
            if (prevText.empty())
            {
                res.push_back(line);
                continue;
            }
    
            if (prevText.back()=='\\')
            {
                prevText.erase( prevText.end()-1 );
                prevText.append(line.text);
                res.back().text = prevText;
                continue;
            }
    
            res.push_back(line);
            
        }
    
        return res;
    }


    //! Анализ плоскостопия INI-файла
    size_t analyzeFlatness( size_t                   indentLim
                          , unsigned                 distributionPercentLim
                          , size_t                   *pIndentsDistribution
                          , size_t                   *pIndentsDistribution2
                          , size_t                   *pIndentsDistribution3
                          , size_t                   *pTotalSections
                          , size_t                   *pTotalValues
                          , size_t                   *pTotalSignificantLines
                          , std::map<size_t, size_t> *pSectionIndentCounts
                          , std::map<size_t, size_t> *pValueIndentCounts
                          , std::map<size_t, size_t> *pSignificantLineIndentCounts
                          )
    {
        //

        size_t                   limCount = 0;

        size_t                   totalSections               = 0;
        size_t                   totalValues                 = 0;
        size_t                   totalSignificantLines       = 0;
        std::map<size_t, size_t> sectionIndentCounts         ;
        std::map<size_t, size_t> valueIndentCounts           ;
        std::map<size_t, size_t> significantLineIndentCounts ;

        size_t maxIndent = 0;

        for( const auto &line : m_lines )
        {
            if (line.isEmpty() || line.isComment())
               continue;

            size_t level = line.getLevel();

            if (level>maxIndent)
                maxIndent = level;

            if (level>indentLim)
                ++limCount;

            ++totalSignificantLines;
            ++significantLineIndentCounts[level];

            if (line.isSection())
            {
                ++totalSections;
                ++sectionIndentCounts[level];
            }
            else
            {
                ++totalValues;
                ++valueIndentCounts[level];
            }
        }

       if (pTotalSections)
           *pTotalSections               = totalSections               ;
       if (pTotalValues)
           *pTotalValues                 = totalValues                 ;
       if (pTotalSignificantLines)
           *pTotalSignificantLines       = totalSignificantLines       ;
       if (pSectionIndentCounts)
           *pSectionIndentCounts         = sectionIndentCounts         ;
       if (pValueIndentCounts)
           *pValueIndentCounts           = valueIndentCounts           ;
       if (pSignificantLineIndentCounts)
           *pSignificantLineIndentCounts = significantLineIndentCounts ;

       if (pIndentsDistribution)
       {
           size_t totalSignificantCounts = 0;
           for( auto p : significantLineIndentCounts)
           {
               unsigned percent = (unsigned)((double)p.second / (double)totalSignificantLines * 100.0 + 0.5);
               if (percent>distributionPercentLim)
               {
                   ++totalSignificantCounts;
               }
           }

           *pIndentsDistribution  = (size_t)(((double)significantLineIndentCounts.size() / (double)(maxIndent+1)) * 100.0 + 0.5);
           *pIndentsDistribution2 = (size_t)(((double)totalSignificantCounts             / (double)(maxIndent+1)) * 100.0 + 0.5);
           *pIndentsDistribution3 = (size_t)(((double)totalSignificantCounts             / (double)(significantLineIndentCounts.size())) * 100.0 + 0.5);
       }

       return limCount;

    }

    //! Возврщает true ля плоских INI
    bool isFlat( size_t indentLim = 3, unsigned limPercent = 10 )
    {
        size_t totalSignificantLines;
        size_t limCount = analyzeFlatness( indentLim, 1, 0, 0, 0, 0, 0, &totalSignificantLines, 0, 0, 0 );

        double ratio = ((double)limCount / (double)totalSignificantLines);
        unsigned percent = (unsigned)(ratio*100.0 + 0.5);
        //s<<"Total lines with indent greater than "<<indentLim<<" : "<<percent<<"%\n";
        return percent < limPercent;
    }

    //--------------------------------------

            //! Строка с допками
            struct LineInfo
            {
        
                friend class Ini;
        
            public:
        
                bool isEmpty()         const { return type==LineType::empty; }     //!< Пустая строка?
                bool isValueLine()     const { return type==LineType::normal; }    //!< Строка со значением?
                bool isValue()         const { return type==LineType::normal; }    //!< Строка со значением?
                bool isComment()       const { return type==LineType::comment; }   //!< Строка коментария?
                bool isSection()       const { return type==LineType::section; }   //!< Строка с секцией?
                bool isNameEqual( const std::string &n ) const { return nameCompare(n)==0; }              //!< Сравнение имени секции/параметра
                bool isSection( const std::string &n )   const { return isSection() && isNameEqual(n); }  //!< Секция с указанным именем?
                bool isValue  ( const std::string &n )   const { return isValue() && isNameEqual(n); }    //!< Параметр с указанным именем?
        
                std::string getFileName() const { return fileName; }    //!< Возвращает имя файла, изолированныхкоторого строка
                size_t getLineNumber()    const { return lineNumber; }  //!< Возвращает номер строки
                size_t getLevel()         const { return startPos; }    //!< Возвращает уровень вложенности
                std::string getText()     const { return text; }        //!< Возвращает текст строки
        
                int nameCompare( std::string compareWith ) const        //!< Сравнение имени
                {
                    return umba::string_plus::toupper_copy(name).compare( umba::string_plus::toupper_copy(compareWith) );
                }
        
                int nameStartsWith( std::string startsWith ) const      //!< Сравнение начала имени
                {
                    return umba::string_plus::starts_with( umba::string_plus::toupper_copy(name), umba::string_plus::toupper_copy(startsWith) );
                }
        
                int nameEndsWith( std::string endsWith ) const          //!< Сравнение конца имени
                {
                    return umba::string_plus::ends_with( umba::string_plus::toupper_copy(name), umba::string_plus::toupper_copy(endsWith) );
                }
        
                std::string getName() const                             //!< Возвращает имя раздела или параметра
                {
                    return name;
                }
        
                template<typename T>
                T getValue() const                                      //!< Шаблон получения значения
                {
                    static_assert ( false , "getValue<T> requres specialization" );
                }
        
                template<typename T>
                T getValue( int base ) const                            //!< Специализация шаблона получения значения для целого 
                {
                    static_assert ( false , "getValue<T>(base) requres specialization" );
                }
        
                //---
                template<> int getValue<int>( int base ) const          //!< Специализация шаблона получения значения для целого знакого
                {
                    checkCanGetValue();
                    int res = stoi( getValueNumericSeparatorStripped(), 0, base );
                    return res;
                }
        
                template<> int getValue<int>( ) const { return getValue<int>( 0 ); } //!< Специализация шаблона получения значения для целого знакого

                //---
                template<> unsigned getValue<unsigned>( int base ) const //!< Специализация шаблона получения значения для целого беззнакого
                {
                    checkCanGetValue();
                    unsigned res = (unsigned)stoul( getValueNumericSeparatorStripped(), 0, base );
                    return res;
                }
        
                template<> unsigned getValue<unsigned>( ) const { return getValue<int>( 0 );  } //!< Специализация шаблона получения значения для целого беззнакого

                //---
                template<> long long int getValue<long long int>( int base ) const //!< Специализация шаблона получения значения для целого очень длинного
                {
                    checkCanGetValue();
                    long long int res = stoll( getValueNumericSeparatorStripped(), 0, base );
                    return res;
                }
        
                template<> long long int getValue<long long int>( ) const { return getValue<long long int>( 0 ); } //!< Специализация шаблона получения значения для целого очень длинного

                //---
                template<> unsigned long long getValue<unsigned long long>( int base ) const //!< Специализация шаблона получения значения для целого очень длинного беззнакого
                {
                    checkCanGetValue();
                    unsigned long long res = stoull( getValueNumericSeparatorStripped(), 0, base );
                    return res;
                }
        
                template<> unsigned long long getValue<unsigned long long>( ) const { return getValue<unsigned long long>( 0 );  } //!< Специализация шаблона получения значения для целого очень длинного беззнакого

                //---
        
                template<typename Validator>
                int getValue( int base, const Validator &validator ) const            //!< Геттер-конвертер
                {
                    return validator( *this, getValue<int>( base ) );
                }
        
                template<typename Validator>
                unsigned getValue( unsigned base, const Validator &validator ) const  //!< Геттер-конвертер
                {
                    return validator( *this, getValue<unsigned>( (int)base ) );
                }
        
                template<>
                std::string getValue<std::string>( ) const                            //!< Геттер-конвертер
                {
                    checkCanGetValue();
                    return getValueUnquoted();
                }
        
                template<typename Validator>
                std::string getValue( const Validator &validator ) const              //!< Геттер-конвертер
                {
                    checkCanGetValue();
                    return validator( *this, getValueUnquoted());
                }
        
                template<>
                bool getValue<bool>( ) const                                          //!< Геттер-конвертер
                {
                    checkCanGetValue();
                    auto V = umba::string_plus::toupper_copy(value);
                    if (V=="0" || V=="FALSE" /* || V=="F" */ || V=="NO"  || V=="N")
                        return false;
                    if (V=="1" || V=="TRUE"  /* || V=="T" */ || V=="YES" || V=="Y")
                        return true;
                    throw std::invalid_argument("Value can't be converted to bool");
                }
        
                bool isVersionStringAnyVersion( std::string strVer ) const            //!< Геттер-конвертер
                {
                    std::string strUpper = umba::string_plus::toupper_copy(strVer);
                    if (strUpper=="ANY" || strUpper=="VANY" || strUpper=="VERSIONANY" || strUpper=="ANYVERSION")
                        return true;
                    return false;
                }
        
                NumericVersion parseNumericVersion( std::string strVer ) const        //!< Геттер-конвертер
                {
                    if (isVersionStringAnyVersion(strVer))
                        return NumericVersion::any();
        
                    size_t pos = 0;
                    uint32_t verMajor = (uint32_t)stoul( strVer, &pos, 10 /* base */ ); // version is decimal
                    if (pos==0)
                        throw std::invalid_argument("Value can't be converted to NumericVersion");
        
                    strVer.erase( 0, pos ); pos = 0;
        
                    uint32_t verMinor = 0;
        
                    if (!strVer.empty())
                    {
                        if (strVer[0]!='.')
                            throw std::invalid_argument("Value can't be converted to NumericVersion");
        
                        strVer.erase(0, 1);
                        umba::string_plus::trim(strVer);
        
                        if (!strVer.empty())
                        {
                            verMinor = (uint32_t)stoul( strVer, &pos, 10 /* base */ ); // version is decimal
                            if (pos==0)
                                throw std::invalid_argument("Value can't be converted to NumericVersion");
                        }
                    }
        
                    if (verMajor>65535 || verMinor>65535)
                        throw std::invalid_argument("Value can't be converted to NumericVersion - too big version number part");
                    return NumericVersion{ (uint16_t)verMajor, (uint16_t)verMinor };
                }
        
        
                NumericVersionEx parseNumericVersionEx( std::string strVer ) const    //!< Геттер-конвертер
                {
                    if (isVersionStringAnyVersion(strVer))
                        return NumericVersionEx::any();
        
                    size_t pos = 0;
                    uint32_t verMajor = (uint32_t)stoul( strVer, &pos, 10 /* base */ ); // version is decimal
                    if (pos==0)
                        throw std::invalid_argument("Value can't be converted to NumericVersionEx");
        
                    strVer.erase( 0, pos ); pos = 0;
        
                    uint32_t verMinor = 0;
                    uint32_t verBuild = 0;
        
                    if (!strVer.empty())
                    {
                        if (strVer[0]!='.')
                            throw std::invalid_argument("Value can't be converted to NumericVersionEx");
        
                        strVer.erase(0, 1);
                        umba::string_plus::trim(strVer);
        
                        verMinor = (uint32_t)stoul( strVer, &pos, 10 /* base */ ); // version is decimal
                        if (pos==0)
                            throw std::invalid_argument("Value can't be converted to NumericVersionEx");
        
                        strVer.erase( 0, pos ); pos = 0;
        
                        if (!strVer.empty())
                        {
                            if (strVer[0]!='.')
                                throw std::invalid_argument("Value can't be converted to NumericVersionEx");
                       
                            strVer.erase(0, 1);
                            umba::string_plus::trim(strVer);
        
                            if (!strVer.empty())
                            {
                                verBuild = (uint32_t)stoul( strVer, &pos, 10 /* base */ ); // version is decimal
                                if (pos==0)
                                    throw std::invalid_argument("Value can't be converted to NumericVersionEx");
                            }
                        }
                    }
        
                    if (verMajor>65535 || verMinor>65535)
                        throw std::invalid_argument("Value can't be converted to NumericVersionEx - too big version number part");
                    return NumericVersionEx{ (uint16_t)verMajor, (uint16_t)verMinor, (uint32_t)verBuild };
                }
        
                template<>
                NumericVersion getValue<NumericVersion>( ) const                      //!< Геттер-конвертер
                {
                    checkCanGetValue();
                    //std::string strVer = getValueNumericSeparatorStripped();
                    return parseNumericVersion( getValueUnquoted() );
                }
        
                template<>
                NumericVersionEx getValue<NumericVersionEx>( ) const                  //!< Геттер-конвертер
                {
                    checkCanGetValue();
                    return parseNumericVersionEx( getValueUnquoted() );
                }
        
                template<>
                NameVersion getValue<NameVersion>( ) const                            //!< Геттер-конвертер
                {
                    checkCanGetValue();
                    std::string strVer = getValueUnquoted();
                    std::string::size_type sepPos = strVer.find_first_of( " /" );
        
                    if (sepPos==std::string::npos)
                        throw std::invalid_argument("Value can't be converted to NameVersion - missing version number");
        
                    std::string name = std::string( strVer, 0, sepPos );
                    umba::string_plus::trim( name );
                    
                    strVer.erase( 0, sepPos+1 );
                    umba::string_plus::trim( strVer );
        
                    if (strVer.empty())
                        throw std::invalid_argument("Value can't be converted to NameVersion - missing version number");
        
                    return NameVersion{ name, parseNumericVersion( strVer ) };
                }
        
                template<>
                NameVersionEx getValue<NameVersionEx>( ) const                        //!< Геттер-конвертер
                {
                    checkCanGetValue();
                    std::string strVer = getValueUnquoted();
                    std::string::size_type sepPos = strVer.find_first_of( " /" );
        
                    if (sepPos==std::string::npos)
                        throw std::invalid_argument("Value can't be converted to NameVersionEx - missing version number");
        
                    std::string name = std::string( strVer, 0, sepPos );
                    umba::string_plus::trim( name );
                    
                    strVer.erase( 0, sepPos+1 );
                    umba::string_plus::trim( strVer );
        
                    if (strVer.empty())
                        throw std::invalid_argument("Value can't be converted to NameVersionEx - missing version number");
        
                    return NameVersionEx{ name, parseNumericVersionEx( strVer ) };
                }
        
        
                size_t findEnumValue( const std::vector<std::string> &nameList
                                    , std::string name 
                                    ) const                                           //!< Геттер-конвертер
                {
                    umba::string_plus::trim( name );
                    name = umba::string_plus::toupper_copy(name);
        
                    size_t idx = 0, size = nameList.size();
                    for(; idx!=size; ++idx)
                    {
                        std::string nameFromList = umba::string_plus::toupper_copy(nameList[idx]);
                        if (nameFromList==name)
                            return idx;
                    }
        
                    return (size_t)-1;
                }
        
                template< typename EnumType >
                EnumType getValue( const std::vector< EnumType > &valList
                                 , const std::vector<std::string> &nameList
                                 , bool asFlags
                                 ) const                                              //!< Геттер-конвертер
                {
                    if (valList.size()!=nameList.size())
                        throw std::invalid_argument("Name list not match values list");
        
                    std::string upperValue = umba::string_plus::toupper_copy(value);
                    umba::string_plus::trim( upperValue );
        
                    if (!asFlags)
                    {
                        size_t foundAt = findEnumValue( nameList, upperValue );
                        if (foundAt==(size_t)-1)
                            throw std::invalid_argument( std::string("Unknown enumeration value - '") + upperValue + std::string("'"));
        
                        return valList[foundAt];
                    }
        
                    typedef typename std::underlying_type< EnumType >::type    EnumUnderlyingType;
        
                    EnumUnderlyingType res = 0;
        
                    std::vector< std::string> flagsStrings = umba::string_plus::split( upperValue, '|',  /* skipEmpty */  true );
        
                    for( const auto &flagStr : flagsStrings)
                    {
                        size_t foundAt = findEnumValue( nameList, flagStr );
                        if (foundAt==(size_t)-1)
                            throw std::invalid_argument( std::string("Unknown enumeration value - '") + flagStr + std::string("'"));
        
                        res |= (EnumUnderlyingType)valList[foundAt];
                    }
        
                    return (EnumType)res;
                    
                }
                //enumValuesToVector( EnumType eb, EnumType ee, bool inclusiveEnd = false )
                
                template<>
                RgbQuad getValue<RgbQuad>( ) const                                    //!< Геттер-конвертер
                {
                    RgbQuad rgbq;
                    if (!rgbq.fromString(value))
                        throw std::invalid_argument( std::string("Value can't be converted to color"));
        
                    return rgbq;
                }
        
                std::string getCommentText() const                                    //!< Геттер-конвертер
                {
                    size_t eraseCharsN = isIniCommentLine( text );
                    if (!eraseCharsN)
                        throw std::invalid_argument("Comment text can't be retrieved on non-comment lines");
        
                    std::string str = std::string(text, eraseCharsN );
        
                    return umba::string_plus::trim_copy(str);
                }
        
                std::string getValueStringAsIs() const                                //!< Геттер-конвертер
                {
                    return value;
                }
        
        
                std::vector< LineInfo > splitMultiParam( std::map<std::string, size_t> *pNameMap = 0
                                                       , char paramsSeparator = ','
                                                       , const std::string &nameValSeparators = ":="
                                                       , const std::string &quotes = "\'\""
                                                       ) const                        //!< Геттер-конвертер
                {
                    std::vector< LineInfo > resLines;
                    if (type!=LineType::normal)
                        return resLines;

                    //std::vector< std::string> paramPairs = umba::string_plus::split( getValueStringAsIs(), paramsSeparator,  /* skipEmpty */  true );
                    std::vector< std::string> paramPairs;

                    {
                        const auto & text = getValueStringAsIs();
                        std::string buf;

                        char quot = 0;
                       
                        for ( auto ch : text )
                        {
                            if (quot==0 && ch==paramsSeparator)
                            {
                                umba::string_plus::trim(buf);
                                if (!buf.empty())
                                {
                                    paramPairs.push_back(buf);
                                    buf.clear();
                                }
                                continue;
                            }

                            if (ch!=0 && ch==quot)
                            {
                                quot = 0;
                            }
                            else
                            {
                                auto pos = quotes.find(ch);
                                if (pos!=quotes.npos)
                                    quot = quotes[pos];
                            }

                            buf.append(1,ch);

                        } // for
                    
                        umba::string_plus::trim(buf);
                        if (!buf.empty())
                        {
                            paramPairs.push_back(buf);
                        }
                    }

        
                    for( const auto &pvPair : paramPairs )
                    {
                        LineInfo li;
        
                        li.text        = pvPair;
                        li.lineNumber  = lineNumber;
                        li.type        = type;
                        li.fileId      = fileId  ;
                        li.fileName    = fileName;
        
                        li.split(nameValSeparators);

                        if (pNameMap)
                        {
                            (*pNameMap)[li.getName()] = resLines.size();
                        }
        
                        resLines.push_back(li);
                    }
        
                    return resLines;
                }

                std::vector< LineInfo > splitMultiParam( bool converNamesToUpper // true - upper, false - lower
                                                       , std::map<std::string, size_t> *pNameMap = 0
                                                       , char paramsSeparator = ','
                                                       , const std::string &nameValSeparators = ":="
                                                       , const std::string &quotes = "\'\""
                                                       ) const                        //!< Геттер-конвертер
                {
                    std::vector< LineInfo > res = splitMultiParam( pNameMap, paramsSeparator, nameValSeparators, quotes );

                    if (pNameMap)
                    {
                        std::map<std::string, size_t> resMap;
                        std::map<std::string, size_t>::const_iterator nit = pNameMap->begin();
                        for ( ; nit != pNameMap->end(); ++nit)
                        {
                            std::string name = converNamesToUpper ? umba::string_plus::toupper_copy(nit->first) : umba::string_plus::tolower_copy(nit->first);
                            resMap[ name ] = nit->second;
                        }

                        *pNameMap = resMap;
                    }

                    return res;
                }
        
        
            protected:
        
                std::string    text;                          //!< Текст строки
                size_t         lineNumber;                    //!< Номер строки
                LineType       type;                          //!< Тип строки
        
                size_t              fileId = (size_t)-1;      //!< ID файла
                mutable std::string fileName;                 //!< Имя файла
        
                std::string    name;                          //!< Имя параметра/секции
                std::string    value;                         //!< Строковое значение парамета
                std::string    sep;                           //!< Найденный разделитель
                size_t         startPos = 0;                  //!< Стартовая позиция имени
                size_t         sepPos   = std::string::npos;  //!< Позиция разделителя
                size_t         valPos   = 0;                  //!< Позиция значения
        
                //! Проверка, можно ли взять значение
                void checkCanGetValue() const
                {
                    if (type!=LineType::normal)
                        throw std::runtime_error("Can't get value on this type of INI line");
                }
        
                //! Возвращает true, если value'шка заквотена
                bool isValueQuoted() const
                {
                    if (value.size()<2)
                        return false;
        
                    if ( (value.front()=='\"' && value.back()=='\"')
                      || (value.front()=='\'' && value.back()=='\'')
                       )
                       return true;
                    return false;
                }
        
                std::string getValueUnquoted() const //!< Возвращает расковыченное строковое значение 
                {
                    if (!isValueQuoted())
                        return value;
        
                    std::string res = value;
                    res.erase(res.size()-1, 1);
                    res.erase(0, 1);
                    return res;
                }
        
                std::string getValueNumericSeparatorStripped() const //!< Возвращает строку с целочисленным значением, убирая разделители разрядов
                {
                    // Числа не должны быть закавычены, получим исключение
                    if (isValueQuoted())
                        return value;
        
                    std::string res;
        
                    for( auto ch : value )
                    {
                        if (ch=='\'' || ch==' ')
                            continue;
                        res.append(1, ch);
                    }
        
                    return res;
                }
        
                //! Производит разделение имени и значения параметра
                void split( std::string seps = ":=")
                {
                    namespace strplus = umba::string_plus;
        
                    if (seps.empty())
                        seps = ":=";
        
                    startPos = 0;
                    sepPos   = std::string::npos;
                    valPos   = 0;
        
                    if (type!=LineType::normal)
                        return;
        
                    std::string tmp = strplus::trim_copy(text);
                    if (tmp.empty())
                    {
                        type = LineType::empty;
                        return;
                    }
        
                    startPos = text.find_first_not_of( ' ' );
        
                    if (tmp.front()=='[' && tmp.back()==']')
                    {
                        type = LineType::section;
                        tmp.erase(tmp.size()-1, 1);
                        tmp.erase(0, 1);
                        //startPos = text.find('[');
                        name = tmp;
                    }
                    else
                    {
                        sep.clear();
                        for( auto sepCh : seps )
                        {
                            size_t tmpPos = text.find(sepCh);
                            if (tmpPos<sepPos)
                            {
                                sepPos = tmpPos;
                                sep    = std::string( 1, sepCh );
                                valPos = text.find_first_not_of( ' ', sepPos+1 );
                            }
                            //sp = std::min( sp, text.find(sepCh) );
                        }
        
                        if (sepPos == std::string::npos)
                        {
                            name = strplus::trim_copy(text); 
                            value.clear();
                        }
                        else
                        {
                            name  = strplus::trim_copy( std::string( text, 0, sepPos) );
                            value = strplus::trim_copy( std::string( text, sepPos+1 ) );
                        }
        
                    }
                         
                } // split
        
                //! Чота мержит
                void merge()
                {
                    namespace strplus = umba::string_plus;
        
                    switch(type)
                       {
                        case LineType::normal:
                             text.clear();
                             strplus::expand(text, startPos, ' ');
                             text.append(name);
                             if (sepPos != std::string::npos)
                             {
                                 text.append(sep);
                                 strplus::expand(text, valPos, ' ');
                                 text.append(value);
                             }
                             break;
        
                        case LineType::section:
                             text.clear();
                             strplus::expand(text, startPos, ' ');
                             text.append(1,'[');
                             text.append(name);
                             text.append(1,']');
                             break;
                        //case LineType::empty:
                        //case LineType::comment:
                        //default:
        
                       }
                } // merge
        
        
        
            }; // struct LineInfo

    //--------------------------------------


    //------------------
    // Validators
    //------------------

    //! Интерфейс валидатора значений для INI-файла
    struct ValidatorBase
    {

    protected:

        //! Валидация символа
        virtual bool validateChar( char ch ) const { return true; }

        //! Тровер
        void doThrow( const LineInfo &lineInfo, std::string value, const std::string &validatorName ) const
        {
            throw umba::FileParsingException( std::string("Invalid value '") 
                                            + value + std::string("' in parameter '") 
                                            + lineInfo.getName() + std::string("' - ") 
                                            + validatorName + std::string(" required")
                                            , lineInfo.getFileName()
                                            , lineInfo.getLineNumber()
                                            );
        }

        //! Валидатор
        std::string validate( const LineInfo &lineInfo, std::string value, const std::string &validatorName ) const
        {
            if (value.empty())
                doThrow( lineInfo, value, validatorName );

            for( auto ch : value )
            {
                if (!validateChar(ch))
                {
                    doThrow( lineInfo, value, validatorName );
                }
            }
            return value;
        }

    }; // struct ValidatorBase


    //! Базовая реализация валидатора значений для INI-файла по строке символов
    struct CharListValidatorBase : public ValidatorBase
    {
        //! Конструктор
        CharListValidatorBase( const char *pCharList )
        : m_pFirstCharList(pCharList)
        , m_pNextCharList(pCharList)
        {}

        //! Конструктор
        CharListValidatorBase( const char *pFirstCharList, const char *pNextCharList )
        : m_pFirstCharList(pFirstCharList)
        , m_pNextCharList(pNextCharList)
        {}


    protected:

        const char *m_pFirstCharList; //!< Строка допустимых символов для первого символа
        const char *m_pNextCharList;  //!< Строка допустимых символов для последующих символов

        //! Расширенная валидация
        virtual bool validateCharEx( char ch, const char *pCharList ) const
        {
            if ( std::strchr(pCharList, ch) )
                return true;
            return false;
        }

        //! Валидация
        std::string validate( const LineInfo &lineInfo, std::string value, const std::string &validatorName ) const
        {
            if (value.empty())
                doThrow( lineInfo, value, validatorName );

            size_t pos = 0, size = value.size();
            if (!validateCharEx( value[pos++], m_pFirstCharList ))
                doThrow( lineInfo, value, validatorName );

            for( ; pos!=size; ++pos )
            {
                if (!validateCharEx(value[pos],m_pNextCharList))
                {
                    doThrow( lineInfo, value, validatorName );
                }
            }
            return value;
        }

    }; // struct ValidatorBase



    //! Реализация валидатора имени латиницей для значений из INI-файла по строке символов
    struct LatinNameValidator : CharListValidatorBase
    {
        //! Конструктор
        LatinNameValidator() : CharListValidatorBase(ini_static_strings::filterLatin) {}

        //! Оператор функционального вызова, осуществляет валидацию
        std::string operator()( const LineInfo &lineInfo, std::string value ) const
        {
            return validate( lineInfo, value, "\'Latin name\'" );
        }

    }; // struct LatinNameValidator

    //! Реализация валидатора имени латиницей без пробелов для значений из INI-файла по строке символов
    struct LatinNameNoSpacesValidator : CharListValidatorBase
    {
        //! Конструктор
        LatinNameNoSpacesValidator() : CharListValidatorBase(ini_static_strings::filterLatinOnly) {}

        //! Оператор функционального вызова, осуществляет валидацию
        std::string operator()( const LineInfo &lineInfo, std::string value ) const
        {
            return validate( lineInfo, value, "\'Latin name (no spaces)\'" );
        }

    }; // struct LatinNameNoSpacesValidator

    //! Реализация валидатора C++ имени для значений из INI-файла по строке символов
    struct CppNameValidator : CharListValidatorBase
    {
        //! Конструктор
        CppNameValidator() : CharListValidatorBase(ini_static_strings::filterIdentifierFirst, ini_static_strings::filterIdentifier) {}

        //! Оператор функционального вызова, осуществляет валидацию
        std::string operator()( const LineInfo &lineInfo, std::string value ) const
        {
            return validate( lineInfo, value, "\'Cpp name\'" );
        }

    }; // struct LatinNameNoSpacesValidator




protected:

    //std::string               m_fileName;
    std::vector<std::string>  m_fileNames;                   //!< Имена файлов
    std::vector< LineInfo >   m_lines;                       //!< Строки
    bool                      m_mergeMultilines = false;     //!< Опция - склеивать мультистроки
    bool                      m_parseIncludes   = false;     //!< Опция - парсить инклуды
    std::string               m_separators;                  //!< Разделители имени от значения

    IniFileReaderInterface    *m_pFileReader;                //!< Указатель на ридер

    SimpleStdIfstreamIniFileReader m_defaultReader;          //!< Ридер по умолчанию

    std::set<std::string>     m_readedFiles;                 //!< Прочитанные файлы

    std::set<std::string>     m_conditionalTags;             //!< Условные тэги
    bool                      m_useConditionals = false;     //!< Опция - парсить условия
    bool                      m_allowDefines    = false;     //!< Опция - разрешить дефайны

    //std::stack<bool>

};


//! Печатаем типа аналитику по плоскостопию INI-шки
template< typename Stream >
inline
void printFlatnessAnalyzingResults( Stream                   &s
                                  , size_t                   indentLim
                                  , size_t                   indentLimCount
                                  , size_t                   *pIndentsDistribution
                                  , size_t                   *pIndentsDistribution2
                                  , size_t                   *pIndentsDistribution3
                                  , size_t                   *pTotalSections
                                  , size_t                   *pTotalValues
                                  , size_t                   *pTotalSignificantLines
                                  , std::map<size_t, size_t> *pSectionIndentCounts
                                  , std::map<size_t, size_t> *pValueIndentCounts
                                  , std::map<size_t, size_t> *pSignificantLineIndentCounts
                                  )
{
    if (pTotalSignificantLines)
    {
        double ratio = ((double)indentLimCount / (double)*pTotalSignificantLines);
        unsigned percent = (unsigned)(ratio*100.0 + 0.5);
        s<<"Total lines with indent greater than "<<indentLim<<" : "<<percent<<"%\n";
    }

    if (pIndentsDistribution)
        s<<"Indent distribution:  "<<*pIndentsDistribution<<"%\n";
    if (pIndentsDistribution2)
        s<<"Indent distribution2: "<<*pIndentsDistribution2<<"%\n";
    if (pIndentsDistribution3)
        s<<"Indent distribution3: "<<*pIndentsDistribution3<<"%\n";

    if (pTotalSignificantLines)
        s<<"Total Significant Lines: "<<*pTotalSignificantLines<<"\n";
    if (pTotalSections)
        s<<"Total Sections         : "<<*pTotalSections<<"\n";
    if (pTotalValues)
        s<<"Total Values           : "<<*pTotalValues<<"\n";

    if (pSignificantLineIndentCounts)
    {
        s<<"Significant Line Indent Counts\n";
        s<<"  Indent   # times\n";
        for( auto p : *pSignificantLineIndentCounts )
        {
            s<<"  "<<p.first<<" :    "<<p.second<<"\n";
        }
    }

    if (pSectionIndentCounts)
    {
        s<<"Section Indent Counts\n";
        for( auto p : *pSectionIndentCounts )
        {
            s<<"  "<<p.first<<" :    "<<p.second<<"\n";
        }
    }

    if (pValueIndentCounts)
    {
        s<<"Value Indent Counts\n";
        for( auto p : *pValueIndentCounts )
        {
            s<<"  "<<p.first<<" :    "<<p.second<<"\n";
        }
    }

}


} // namespace umba

