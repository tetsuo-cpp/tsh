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
## Dependencies
* CMake.
* GNU ReadLine.
* SQLite Version 3.
* Attractive Chaos' KLib.
## Build
Bring in Git submodules.
```
git submodule init
git submodule update
```
Use CMake to generate a platform specific build script and then invoke that:
```
cmake .
make
```
## Usage
To execute a single command.
```
./tsh [COMMAND]
```
To run Tsh as the active shell (Ctrl-D to exit).
```
./tsh
```
## Built-in commands.
* ```cd```: Change current working directory.
* ```help```: Display help text.
* ```exit```: Exit the shell.
* ```tshstats```: Display command duration information (backed by SQLite).
