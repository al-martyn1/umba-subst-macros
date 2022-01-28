@rem %UMBA_TOOLS% - eg F:\umba-tools

@if not exist %UMBA_TOOLS%\bin    mkdir %UMBA_TOOLS%\bin
@if not exist %UMBA_TOOLS%\conf   mkdir %UMBA_TOOLS%\conf

@copy /Y .out\msvc2019\x64\Release\umba-brief-scanner.exe    %UMBA_TOOLS%\bin\

@xcopy /Y /S /E /I /F /R _distr_conf\conf\*                  %UMBA_TOOLS%\conf
