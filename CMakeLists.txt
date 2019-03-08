cmake_minimum_required(VERSION 3.9)
project(tsh)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

set(
  TSH_SOURCE_FILES
  lib/BuiltIn.c
  lib/Cmd.c
  lib/DataBase.c
  lib/Engine.c
  lib/Lex.c
  lib/Parse.c
  lib/Prompt.c
  lib/Tsh.c
  src/main.c
)

add_executable(tsh ${TSH_SOURCE_FILES})
target_include_directories(tsh PRIVATE lib external)
target_link_libraries(tsh readline sqlite3)

if (${SANITIZER_TYPE} STREQUAL "ASan")
  target_link_libraries(tsh -fsanitize=address)
elseif (${SANITIZER_TYPE} STREQUAL "UBSan")
  target_link_libraries(tsh -fsanitize=undefined)
endif()
