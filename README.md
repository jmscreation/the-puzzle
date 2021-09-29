# The Puzzle
 
 -- A Simple And Fun Puzzle Game --


Hey!
This game is currently under very early development, so don't expect much, but check back later!


## How To Build

Currently this game is only available with a Windows build toolchain.

To build, make sure MinGW64 is installed and you have [correctly configured your bin path](https://docs.microsoft.com/en-us/previous-versions/office/developer/sharepoint-2010/ee537574(v=office.14)).

Run the *build.bat* script to build the project.

## Troubleshooting Building

For first time build, you must make sure that `REBUILD_RESOURCES=1` in the *build.bat* script is set.

You cannot build with a UCRT version of MinGW64. Make sure your MinGW64 version is configured for MSVCRT.
You can find [prebuilt binaries of MinGW64 here](https://github.com/brechtsanders/winlibs_mingw/releases)
You should avoid any *pre-release* version, and don't download the UCRT version.

Libraries included in this project were compiled for 64bit; make sure your compiler is the 64bit version, and not the 32bit version.

This project requires the C++20 ISO standard to be available. Make sure your compiler is C++20 compatible.

If you are still having trouble with your compiler, here's a direct download for a working compiler: [MingW64-9.0.0 gcc-11.2.0 r1](https://github.com/brechtsanders/winlibs_mingw/releases/download/11.2.0-12.0.1-9.0.0-r1/winlibs-x86_64-posix-seh-gcc-11.2.0-mingw-w64-9.0.0-r1.7z)



## License For Use

Main project license:
[License](https://github.com/jmscreation/GameAPIs/blob/main/LICENSE)


Additional Licenses this project uses:
[OLC License](https://github.com/jmscreation/GameAPIs/blob/main/OLC_LICENSE)
[Freetype License](https://github.com/freetype/freetype/blob/master/docs/FTL.TXT)
[PortAudio License](https://github.com/PortAudio/portaudio/blob/master/LICENSE.txt)
