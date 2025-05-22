CALL "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"


SET INIT_DIR=%cd%
SET SOURCE=.\src\
SET WIN_BUILD_DIR=%SOURCE%x64\Release\
SET LINUX_BUILD_DIR=%SOURCE%FotiadiMath.linux\bin\x64\Release\
SET DISTRIB_DIR=%1

@ECHO ON
echo ===========================Nuget Restore======================================
nuget restore %SOURCE%FotiadiMath.sln
echo ===========================Build============================================== 
msbuild %SOURCE%FotiadiMath.sln /t:Clean;Rebuild /p:Configuration=Release;Platform=x64 /p:DebugSymbols=false /p:DebugType=None /p:DefineConstants="__FOTIADI_MATH__" 

cd %WIN_BUILD_DIR%
echo ===========================Tests==============================================
BasicAlg.test.exe  
WaveAlgorithm.test.exe 
FotiadiMath.test.exe

cd %INIT_DIR%
echo ===========================Copy files=========================================

COPY %LINUX_BUILD_DIR%FotiadiMath.linux.so %DISTRIB_DIR%
COPY %WIN_BUILD_DIR%FotiadiMath.win.dll %DISTRIB_DIR%
COPY %WIN_BUILD_DIR%FotiadiMath.win.lib %DISTRIB_DIR%
COPY c:\windows\system32\vcomp140.dll %DISTRIB_DIR%
COPY %SOURCE%FotiadiMath.share\fotiadi_math.h %DISTRIB_DIR%
( type %SOURCE%\Doxyfile & echo OUTPUT_DIRECTORY=%DISTRIB_DIR%manuals ) | doxygen.exe -

@ECHO OFF

rem doxygen.exe %SOURCE%Doxyfile