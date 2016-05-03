LM Command SDK
==============

This SDK is comprised of a stand alone AT command generator/parser which operates on sequeces of bytes and a set of examples for using it. It is currently under heavy development and the code may change drastically between now and the first feature complete release.

Currently there are separate libraries for the LM95x/LM96x and the LM048/LM780/LM072 etc. The LM048 library is more complete. In the future these will be reorganised to increase code sharing where possible.

We hope to provide sample and template applications for the following languages and platforms resources permitting

- QT Windows & Linux (Partially done)
- Linux POSIX C
- Win32 VC++
- Windows .Net C#
- Android Java/JNI
- iOS Swift or Objective-C
- ARM Cortex C

The sample applications provided depends on demand and we are open to suggestions.

Directories
-----------

- examples: Contains examples for various platforms
- build: Contains the library files 
- src: Contains the Ragel sources which the library is generated from
- docs: Contains documentation, including a library overview and API reference

Usage
-----

Copy the files lm048lib.h and lm048lib.c from the build directory into your
project. See the examples for details concerning particular languages and
platforms.

Building
--------

The following instructions are for generating the library from the Ragel source, in most cases it is not necessary for you to do this. 

The library itself has no dependencies except the C standard library, however it requires three tools to build it:

- SCons: http://scons.org
- Ragel: http://colm.net/open-source/ragel
- A C compiler: e.g. gcc, clang, msvc; Clang is the default

To build the whole library from the Ragel sources up, run `scons` in the root directory.
*If you are not using Clang, set the `CC` variable in src/SConscript*

There are also some optional dependencies and build options:

- cldoc: https://jessevdk.github.io/cldoc/index.html 
- scan-build: http://clang-analyzer.llvm.org/scan-build.html

`scons -Q docs` will build the API documentation. The documentation is built with cldoc.

You can perform Clang's static analyses of the project with `scan-build -V --use-cc clang scons -Q --analyze`

The test suite can be ran with `scons check-lm048`.

Amusements
----------

Install the xdot program on your sytem and run `ragel -Vp -Satcmd src/lm951lib.rl | xdot` from the SDK folder. This will show you an interactive graph of the state machine produced by Ragel for parsing the AT commands.

Licensing
---------

See license.txt, the public code is licensed under the Adaptive Public License 1.0. Contact LM support if you cannot use this license in your project. All source files are covered under this license unless it is stated otherwise. This includes files preceeding the commits which added the license. 

