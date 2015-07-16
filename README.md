LM Command SDK
==============

This SDK is comprised of a stand alone AT command generator/parser which operates on sequeces of bytes and a set of examples for using it.

Directories
-----------

- examples: Contains examples for various platforms
- build: Contains the library files 
- src: Contains the Ragel sources which the library is generated from

Usage
-----

TODO

Building
--------

The library itself has no dependencies except the C standard library, however it requires three tools to build it:

- SCons: http://scons.org
- Ragel: http://colm.net/open-source/ragel
- A C compiler: e.g. gcc, clang, msvc; Clang is the default

To build the whole library from the Ragel sources up, run `scons -Q` in the root directory.
*If you are not using Clang, set the `CC` variable in src/SConscript*

There are also some optional dependencies and build options:

- cldoc: https://jessevdk.github.io/cldoc/index.html 
- scan-build: http://clang-analyzer.llvm.org/scan-build.html

`scons -Q docs` will build the API documentation and `scons -Q docs/API` will copy it from /build/docs to /docs/API. The documentation is built with cldoc.

You can perform Clang's static analyses of the project with `scan-build -V --use-cc clang scons -Q --analyze`


