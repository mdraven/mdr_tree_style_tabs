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

# set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)

#### Configure ----------------------------

set(EXEC "tree")

set(TST_LIBRARY_NAME "mdrTreeStyleTabs")

set(SOURCE_DIR "src")

include_directories(${SOURCE_DIR} ${SOURCE_DIR}/include)

set(LIB_SOURCES
  ${SOURCE_DIR}/ITabWidget.cpp
  ${SOURCE_DIR}/Node.cpp
  ${SOURCE_DIR}/TabWidgetFactory.cpp
  ${SOURCE_DIR}/TreeStyleTabs.cpp
  ${SOURCE_DIR}/details/NodeUtils.cpp
  ${SOURCE_DIR}/details/TabWidget.cpp
  ${SOURCE_DIR}/details/DragAndDropFilter.cpp
  )

set(EXEC_SOURCES
  main.cpp
  )


set(FOR_MOC_HEADERS
  # ${SOURCE_DIR}/include/mdrTreeStyleTabs/details/DragAndDropFilter.hpp
  ${SOURCE_DIR}/include/mdrTreeStyleTabs/TreeStyleTabs.hpp
  )

set(QRC_FILES
  TreeStyleTabs.qrc)

qt5_wrap_cpp(MOC_HEADERS ${FOR_MOC_HEADERS})
qt5_add_resources(QRC_RES ${QRC_FILES})

add_library(${TST_LIBRARY_NAME} SHARED ${LIB_SOURCES} ${MOC_HEADERS} ${QRC_RES})

add_executable(${EXEC} ${EXEC_SOURCES} ${MOC_HEADERS} ${QRC_RES})

target_link_libraries(${TST_LIBRARY_NAME} Qt5::Widgets)
target_link_libraries(${EXEC} mdrTreeStyleTabs Qt5::Widgets)

##### ----------------------

include(GNUInstallDirs)

install(TARGETS ${TST_LIBRARY_NAME}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY ${SOURCE_DIR}/include/mdrTreeStyleTabs
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT development)
