cmake_minimum_required(VERSION 3.16)
project(c_fullstack_template C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
option(USE_BUNDLED_SQLITE "Build with bundled SQLite amalgamation source" ON)

add_executable(c_fullstack_server
  server/src/main.c
  server/src/http.c
  server/src/db.c
  server/src/static.c
)

target_include_directories(c_fullstack_server PRIVATE server/src)

if (USE_BUNDLED_SQLITE)
  add_library(sqlite3_bundled STATIC
    third_party/sqlite-amalgamation-3510200/sqlite3.c
  )
  target_include_directories(sqlite3_bundled PUBLIC
    third_party/sqlite-amalgamation-3510200
  )
  target_link_libraries(c_fullstack_server PRIVATE sqlite3_bundled)
else()
  set(SQLITE3_INCLUDE_DIR "" CACHE PATH "Path to sqlite3 include directory")
  set(SQLITE3_LIBRARY "" CACHE FILEPATH "Path to sqlite3 library file")
  if (SQLITE3_INCLUDE_DIR)
    set(SQLite3_INCLUDE_DIR "${SQLITE3_INCLUDE_DIR}")
  endif()
  if (SQLITE3_LIBRARY)
    set(SQLite3_LIBRARY "${SQLITE3_LIBRARY}")
  endif()
  find_package(SQLite3 REQUIRED)
  target_link_libraries(c_fullstack_server PRIVATE SQLite::SQLite3)
endif()

add_library(c_fullstack_client
  client_c/src/api_client.c
)
target_include_directories(c_fullstack_client PUBLIC client_c/include)

add_executable(client_example client_c/src/example.c)
target_link_libraries(client_example PRIVATE c_fullstack_client)
target_include_directories(client_example PRIVATE client_c/include)

if (WIN32)
  target_link_libraries(c_fullstack_server PRIVATE ws2_32)
  target_link_libraries(client_example PRIVATE ws2_32)
endif()
