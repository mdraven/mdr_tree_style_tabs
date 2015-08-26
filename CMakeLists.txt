cmake_minimum_required (VERSION 2.6)

project(TreeStyleTabs)

#### Check --------------------------------

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)

#### Configure ----------------------------

set(EXEC "tree")

# set(TST_LIBRARY_NAME "TreeStyleTabs")

set(EXEC_SOURCES
  main.cpp
  TreeStyleTab.cpp
  TreeStyleTabs.cpp
  TreeStyleTabsProxy.cpp
  )

# set(LIB_SOURCES
#   TreeStyleTab.cpp
#   TreeStyleTabs.cpp
#   TreeStyleTabsProxy.cpp
#   )

set(FOR_MOC_HEADERS
  DragAndDropFilter.hpp
  TreeStyleTab.hpp
  TreeStyleTabs.hpp
  )

set(QRC_FILES
  TreeStyleTabs.qrc)

qt5_add_resources(QRC_RES ${QRC_FILES})

add_executable(${EXEC} ${EXEC_SOURCES} ${MOC_HEADERS} ${QRC_RES})
# add_library(${TST_LIBRARY_NAME} STATIC ${LIB_SOURCES} ${MOC_HEADERS} ${QRC_RES})

target_link_libraries(${EXEC} Qt5::Widgets)
# target_link_libraries(${TST_LIBRARY_NAME} Qt5::Widgets)
