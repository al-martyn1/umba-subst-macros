@rem %UMBA_TOOLS% - eg F:\umba-tools

@if "%UMBA_TOOLS%"=="" goto UMBA_TOOLS_VAR_NOT_SET
@goto UMBA_TOOLS_VAR_IS_SET

:UMBA_TOOLS_VAR_NOT_SET
@echo UMBA_TOOLS environmetnt variable is not set
@exit /B 1

:UMBA_TOOLS_VAR_IS_SET


@if not exist %UMBA_TOOLS%\bin    mkdir %UMBA_TOOLS%\bin
@if not exist %UMBA_TOOLS%\conf   mkdir %UMBA_TOOLS%\conf

@copy /Y .out\msvc2019\x64\Release\umba-subst-macros.exe       %UMBA_TOOLS%\bin\
@rem copy /Y .out\msvc2019\x64\Debug\umba-subst-macros.exe         %UMBA_TOOLS%\bin\

@rem xcopy /Y /S /E /I /F /R _distr_conf\conf\*                    %UMBA_TOOLS%\conf

@rem https://stackoverflow.com/questions/3068929/how-to-read-file-contents-into-a-variable-in-a-batch-file
@rem set /P VERSION= |umba-subst-macros.exe -v
@umba-subst-macros.exe -v >version.txt
@set /P VERSION=<version.txt
@echo VERSION: %VERSION%
