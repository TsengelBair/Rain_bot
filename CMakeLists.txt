cmake_minimum_required(VERSION 3.14)

project(Rain_Bot LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${CURL_INCLUDE_DIRS})

add_executable(Rain_Bot
  main.cpp
  Config.h
  Db/IDataBase.h
  Bot/rainbot.h
  Bot/rainbot.cpp
  Db/sqlitedbhandler.h
  Db/sqlitedbhandler.cpp
  Logger/logger.h
  Logger/logger.cpp
  Scheduler/scheduler.h
  Scheduler/scheduler.cpp
  Request/requesthandler.h
  Request/requesthandler.cpp
  TimeBoundary/timeboundarymaker.h
  TimeBoundary/timeboundarymaker.cpp
  Parser/parser.h
  Parser/parser.cpp
)
target_link_libraries(Rain_Bot TgBot ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${CURL_LIBRARIES} pthread sqlite3)
