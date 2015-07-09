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

The library itself has no dependencies, however it requires three tools to build it:

- SCons: http://scons.org
- Ragel: http://colm.net/open-source/ragel
- A C compiler: e.g. gcc, clang, msvc

To build the whole library from the Ragel sources up, run `scons -Q` in the root directory.

*If you are not using Clang, set the `CC` variable in src/SConscript*
