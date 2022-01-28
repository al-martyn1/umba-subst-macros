/*! \file
    \brief Стаб, для того, чтобы windeployqt подсосал нужный рантайм для MSVC - а то совсем без Qt он не работает
 */

#include <iostream>

#include <QCoreApplication>
#include <QString>
#include <QSettings>
//#include <QtCore/QDir>
#include <QDir>


#if defined(DEBUG) || defined(_DEBUG)
    #pragma comment(lib, "Qt5Cored")
#else
    #pragma comment(lib, "Qt5Core")
#endif


int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("test001");
    QCoreApplication::setApplicationVersion("1.0");

    QCoreApplication::setOrganizationName("al-martyn1");
    QCoreApplication::setOrganizationDomain("https://github.com/al-martyn1/");

    using std::cout;
    using std::endl;


    cout<<"Launched from : "<<QDir::currentPath().toStdString()<<endl;
    cout<<"Launched exe  : "<<QCoreApplication::applicationFilePath().toStdString()<<endl;
    cout<<"Path to exe   : "<<QCoreApplication::applicationDirPath().toStdString()<<endl;
   
    return 0;
}