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
Use CMake to generate a platform specific build script and then invoke that:
```
cmake .
make
```
## Built-in commands.
* ```cd```: Change current working directory.
* ```help```: Display help text.
* ```exit```: Exit the shell.
* ```tshstats```: Display command duration information (backed by SQLite).
