# Fragment Audio Server launcher

A [wxWidgets](https://www.wxwidgets.org/) launcher for [Fragment Audio Server](https://github.com/grz0zrg/fas) built for the [Fragment Synthesizer](https://github.com/grz0zrg/fsynth), a [web-based Collaborative Spectral Synthesizer](https://www.fsynth.com)

This program should compile on most platforms.

The [Fragment Audio Server](https://github.com/grz0zrg/fas) executable should be put alongside the launcher application.

## Description

This program is a simple native launcher which provide an easy interface to start the Fragment Audio Server, the launcher also provide a convenient way to configure the audio server for individual sessions and provide a direct way to launch the web. application pre-linked to the native audio server.

## Package

A .deb package is available in the **package** folder along with the [Debreate](https://antumdeluge.github.io/debreate-web/) file project used to build the package for the amd64 platform, this package bundle the audio server and the launcher, this is the solution distributed on the Fragment homepage.

## Build

This program was made with the [CodeLite IDE](https://codelite.org/) to ease the process of creating the wxWidgets UI with [wxCrafter](https://wxcrafter.codelite.org/).

Requirements :

 * [PortAudio](http://www.portaudio.com/download.html)
 * [wxWidgets](https://www.wxwidgets.org/)
 * [tiny-process-library](https://github.com/eidheim/tiny-process-library)

This also make use of [SQLite](https://www.sqlite.org/) (this is bundled)

### Linux

This is easily compiled under GNU/Linux systems with the "Debug" or "Release" CodeLite target.

### Cross-compiling for Windows under Linux

The launcher was successfully built for Windows systems under Linux (Ubuntu) by using the **mingw-w64** package, **x86_64-w64-mingw32** was tested successfully, the following guide will be written for this target but there should be minor changes needed for other targets.

A CodeLite target named "Release_WinCross" contain the configuration needed to build the application for Windows via cross-compilation.

There may be some issues building the needed libraries so here are some hints

- CodeLite may not detect automatically the compiler environment so you must add it manually via the build settings (for example, mine was located at /usr/x86_64-w64-mingw32)

#### Cross-compiling wxWidgets

Download the library somewhere (a folder named **cross** for example)

##### Configure step

`./configure --host=x86_64-w64-mingw32 --build=x86_64-linux CFLAGS='-I/usr/share/mingw-w64/include/ -I/home/julien/crosscompile/wxWidgets/lib/wx/include/x86_64-w64-mingw32-msw-unicode-3.1 -I/home/julien/crosscompile/wxWidgets/include -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXMSW__'`

Then `make` & `sudo make install`

#### Cross-compiling PortAudio (with wmme/dx/asio support)

Download the library somewhere (a folder named **cross** for example)

Download the ASIO SDK (you will need to modify the ASIO SDK path appearing in the configure step below)

##### Configure step

`./configure --host=x86_64-w64-mingw32 --build=x86_64-linux CFLAGS='-I/usr/share/mingw-w64/include/ -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXMSW__' --with-asiodir=/home/julien/crosscompile/ASIOSDK2.3/ --with-winapi=wmme,directx,asio`

Then `make` & `sudo make install`

#### Cross-compiling tiny-process-library

Download the library somewhere (a folder named **cross** for example)

##### Compilation step

`/usr/bin/x86_64-w64-mingw32-g++ -O2 -std=c++11 -D_WIN32 -D_WINCROSS_w64 -I/usr/x86_64-w64-mingw32/include -c process.cpp`

`/usr/bin/x86_64-w64-mingw32-g++ -O2 -std=c++11 -D_WIN32 -D_WINCROSS_w64 -I/usr/x86_64-w64-mingw32/include -c process_win.cpp`

##### Library step

`/usr/bin/x86_64-w64-mingw32-ar crf libtiny-process-library.a process_win.o`

##### Incomplete support for < thread > and < mutex > and UNICODE problems

Some issues may arise when compiling tiny-process-library due to missing C++11 thread and mutex library and unicode support, this can be solved by using [mingw-std-threads](https://github.com/meganz/mingw-std-threads) headers and by undefining unicode.

In **process.hpp** replace #include <mutex> and #include <thread> by

`#ifdef _WINCROSS_w64`
`#include <windows.h>`
`#include "mingw.mutex.h"`
`#include "mingw.thread.h"`
`#undef UNICODE`
`#else`
`#include <mutex>`
`#include <thread>`
`#endif`

## Screenshots

[![FAS launcher](https://www.fsynth.com/data/fas_launcher.png)](https://github.com/grz0zrg/fas_launcher)
