@echo off
REM		Build Script

REM Set Compiler Settings Here

cls

set CPP=c++
set GPP=g++
set BIN2CPP=bin2cpp


set OUTPUT=program.exe
set DEBUGMODE=1

set REBUILD_RESOURCES=0
set REBUILD_LIBRARIES=0
set REBUILD_COMPLETED=0
set LINK_ONLY=0
set VERBOSE=0

set ASYNC_BUILD=1


REM -DNO_RELEASE
set COMPILER_FLAGS=-std=c++20
set ADDITIONAL_LIBRARIES=-static-libstdc++ -lpthread -lportaudio -lsetupapi -lwinmm -lcomdlg32 -luser32 -lgdi32 -lopengl32 -lShlwapi -ldwmapi -lstdc++fs -lpng -lz -lfreetype
set ADDITIONAL_LIBDIRS=-Llibrary
set ADDITIONAL_INCLUDEDIRS=-Ilibrary -Ilibrary\helpers -Isrc -Isrc\completed -Iresources -Iresources\build

set ARCHIVER_DIR=.\resources
set ARCHIVER=%ARCHIVER_DIR%\archiver
set RESOURCE_BUILD_DIR=resources\build
set RESOURCE_ARCHIVE_NAME=archive.dat
set RESOURCE_OUTPUT_NAME=archive_data



del %OUTPUT% 2>nul
setlocal enabledelayedexpansion


if %LINK_ONLY% GTR 0 (
	goto linker
)

if %REBUILD_RESOURCES% GTR 0 (
	echo Packing Archive...
	start /B /WAIT "Archive" %ARCHIVER% %RESOURCE_BUILD_DIR%\%RESOURCE_ARCHIVE_NAME% Puzzle-Assets

	echo Generating Precompiled Archive Data Buffer...

	if %VERBOSE% GTR 0 (
		echo start /B /D %RESOURCE_BUILD_DIR% /WAIT "Archive" %BIN2CPP% %RESOURCE_ARCHIVE_NAME% puzzle_asset_archive %RESOURCE_OUTPUT_NAME%
	)
	
	start /B /D %RESOURCE_BUILD_DIR% /WAIT "Archive" %BIN2CPP% %RESOURCE_ARCHIVE_NAME% puzzle_asset_archive %RESOURCE_OUTPUT_NAME%
)

if %DEBUGMODE% GTR 0 (
	set DEBUG_INFO=-ggdb -g
) else (
	set DEBUG_INFO=-s
)

if %ASYNC_BUILD% GTR 0 (
	set WAIT=
) else (
	set WAIT=/WAIT
)

if not exist .objs64 (
	echo Creating Object Directory Structure...
	mkdir .objs64
)

del /Q "src\*.o" >nul 2>nul

if %REBUILD_RESOURCES% GTR 0 (
	del /S /Q "%RESOURCE_BUILD_DIR%\*.o" >nul 2>nul
)

if %REBUILD_LIBRARIES% GTR 0 (
	del /S /Q "library\helpers\*.o" >nul 2>nul
)

if %REBUILD_COMPLETED% GTR 0 (
	del /S /Q "src\completed\*.o" >nul 2>nul
)


echo Building Library API Files...
for %%F in (library\helpers\*.cpp) do (
	if not exist library\helpers\%%~nF.o (
		echo Building %%~nF.o
		if %VERBOSE% GTR 0 (
			echo %CPP% %ADDITIONAL_INCLUDEDIRS% %COMPILER_FLAGS% %DEBUG_INFO% -c %%F -o library\helpers\%%~nF.o
		)
		start /B %WAIT% "%%~nF.o" %CPP% %ADDITIONAL_INCLUDEDIRS% %COMPILER_FLAGS% %DEBUG_INFO% -c %%F -o library\helpers\%%~nF.o
	)
)

echo Building Game Files...
for %%F in (src\*.cpp) do (
	if not exist src\%%~nF.o (
		echo Building %%~nF.o
		if %VERBOSE% GTR 0 (
			echo %CPP% %ADDITIONAL_INCLUDEDIRS% %COMPILER_FLAGS% %DEBUG_INFO% -c %%F -o src\%%~nF.o
		)
		start /B %WAIT% "%%~nF.o" %CPP% %ADDITIONAL_INCLUDEDIRS% %COMPILER_FLAGS% %DEBUG_INFO% -c %%F -o src\%%~nF.o
	)
)

for %%F in (src\completed\*.cpp) do (
	if not exist src\completed\%%~nF.o (
		echo Building %%~nF.o
		if %VERBOSE% GTR 0 (
			echo %CPP% %ADDITIONAL_INCLUDEDIRS% %COMPILER_FLAGS% %DEBUG_INFO% -c %%F -o src\completed\%%~nF.o
		)
		start /B %WAIT% "%%~nF.o" %CPP% %ADDITIONAL_INCLUDEDIRS% %COMPILER_FLAGS% %DEBUG_INFO% -c %%F -o src\completed\%%~nF.o
	)
)

echo Building Resource Files...
for %%F in (%RESOURCE_BUILD_DIR%\*.cpp) do (
	if not exist %RESOURCE_BUILD_DIR%\%%~nF.o (
		echo Building %%~nF.o
		if %VERBOSE% GTR 0 (
			echo %CPP% %ADDITIONAL_INCLUDEDIRS% %COMPILER_FLAGS% %DEBUG_INFO% -c %%F -o %RESOURCE_BUILD_DIR%\%%~nF.o
		)
		start /B %WAIT% "%%~nF.o" %CPP% %ADDITIONAL_INCLUDEDIRS% %COMPILER_FLAGS% %DEBUG_INFO% -c %%F -o %RESOURCE_BUILD_DIR%\%%~nF.o
	)
)

REM Wait for building process to finish
:loop
for /f %%G in ('tasklist ^| find /c "%CPP%"') do ( set count=%%G )
if %count%==0 (
	goto linker
) else (
	timeout /t 2 /nobreak>nul
	goto loop
)

:linker

set "files="
for /f "delims=" %%A in ('dir /b /a-d "src\*.o" ') do set "files=!files! src\%%A"
for /f "delims=" %%A in ('dir /b /a-d "src\completed\*.o" ') do set "files=!files! src\completed\%%A"
for /f "delims=" %%A in ('dir /b /a-d "%RESOURCE_BUILD_DIR%\*.o" ') do set "files=!files! %RESOURCE_BUILD_DIR%\%%A"
for /f "delims=" %%A in ('dir /b /a-d "library\helpers\*.o" ') do set "files=!files! library\helpers\%%A"

:link
echo Linking Executable...

if %DEBUGMODE% GTR 0 (
	set MWINDOWS=
) else (
	set MWINDOWS=-mwindows
)

if %VERBOSE% GTR 0 (
	echo %GPP% %ADDITIONAL_LIBDIRS% -o %OUTPUT% %files% %ADDITIONAL_LIBRARIES% %MWINDOWS%
)

%GPP% %ADDITIONAL_LIBDIRS% -o %OUTPUT% %files% %ADDITIONAL_LIBRARIES% %MWINDOWS%

:finish
if exist .\%OUTPUT% (
	echo Build Success!
) else (
	echo Build Failed!
)