std::string appFullName   = "Umba Brief Scanner";
std::string appVersion    = "0.1";
std::string appCommitHash;  //UNDONE
std::string appBuildDate  = __DATE__;
std::string appBuildTime  = __TIME__;

const char *appHomeUrl    = "https://github.com/al-martyn1/umba-brief-scanner";
const char *appistrPath   = "";

#if defined(WIN32) || defined(_WIN32)
    const char *appSubPath    = "bin/umba-brief-scanner.exe";
#else
    const char *appSubPath    = "bin/umba-brief-scanner";
#endif

