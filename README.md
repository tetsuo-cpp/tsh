# Tetsuo's Shell (Tsh)
This is a toy shell I wrote to become more familar with C.
## Features
* Interactive commands (less, vi, etc).
* Piping.
* I/O redirection.
* Environment variables.
* Records command duration with SQLite (because why not).
## Improvements
* File globbing.
* More granular I/O redirection (choosing between stdout and stderr).
* More operators.
* Shell history.
* Make less buggy.
## Dependencies
* CMake.
* GNU ReadLine.
* SQLite Version 3.
* Attractive Chaos' Klib.
## Build
Bring in Git submodules.
```
git submodule init
git submodule update
```
Run ```build.sh``` to invoke the CMake build.
```
sh build.sh [BUILD_TYPE:debug/release/asan/ubsan/scan]
```
## Usage
To execute a single command.
```
build/[BUILD_TYPE]/tsh [COMMAND]
```
To run Tsh as the active shell (Ctrl-D to exit).
```
build/[BUILD_TYPE]/tsh
```
To run an Address Sanitized build.
```
ASAN_OPTIONS="symbolize=1" build/asan/tsh
```
To run an Undefined Behaviour Sanitized build.
```
UBSAN_OPTIONS="print_stacktrace=1" build/ubsan/tsh
```
## Built-in commands
* ```cd```: Change current working directory.
* ```help```: Display help text.
* ```exit```: Exit the shell.
* ```tshstats```: Display command duration information (backed by SQLite).
