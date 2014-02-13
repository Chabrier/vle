set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

find_package(PkgConfig REQUIRED)

pkg_check_modules(XMLDEPS libxml-2.0)
include_directories(${XMLDEPS_INCLUDE_DIRS})

if (WIN32)
  set(INTL_LIB "intl")
else (WIN32)
  set(INTL_LIB "")
endif (WIN32)

include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS} ${GVLE2_SOURCE_DIR}
  ${VLE_SOURCE_DIR}/src/apps/gvle2)

SET(GVLE2_SOURCES_CPP gvle2win.cpp aboutbox.cpp help.cpp
  filevpzview.cpp simulation.cpp widgetprojecttree.cpp logger.cpp)

QT4_WRAP_UI(ui_gvle2win_h gvle2win.ui)
QT4_WRAP_UI(aboutbox_h aboutbox.ui)
QT4_WRAP_UI(help_h help.ui)
QT4_WRAP_UI(filevpzview_h filevpzview.ui)
QT4_WRAP_UI(simulationview_h simulationview.ui)
QT4_WRAP_UI(simulationrtool_h simulationrtool.ui)

SET(GVLE2_SOURCES_HPP ${ui_gvle2win_h} ${aboutbox_h} ${help_h}
  ${filevpzview_h} ${simulationview_h} ${simulationrtool_h} logger.h
  plugin_sim.h)

if (WIN32)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vfl.o
    COMMAND ${CMAKE_RC_COMPILER}
    -I${CMAKE_BINARY_DIR}/share
    -i${CMAKE_BINARY_DIR}/share/vfl.rc
    -o${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
  set(OS_SPECIFIC_PATH_IMPL ${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
  set(OS_SPECIFIC_LIBRARIES ws2_32)
else (WIN32)
  set(OS_SPECIFIC_PATH_IMPL)
  set(OS_SPECIFIC_LIBRARIES dl)
endif (WIN32)

add_library(gvle2lib SHARED ${GVLE2_SOURCES_CPP} ${GVLE2_SOURCES_HPP})

set_target_properties(gvle2lib PROPERTIES
  VERSION 0
  OUTPUT_NAME "gvle2-${VLE_VERSION_SHORT}"
  DEFINE_SYMBOL "gvle2lib_EXPORTS"
  COMPILE_DEFINITIONS "GVLE2_DLL"
#  COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden"
  CLEAN_DIRECT_OUTPUT 1)

if (WIN32)
  set_target_properties(gvle2lib PROPERTIES
    LINK_FLAGS " -mwindows")
endif ()

target_link_libraries(gvle2lib vlelib ${QT_LIBRARIES} ${INTL_LIB})

install(FILES ${GVLE2_SOURCES_HPP}
  DESTINATION ${VLE_INCLUDE_DIRS}/gvle2)

install(TARGETS gvle2lib RUNTIME DESTINATION bin LIBRARY DESTINATION
  lib ARCHIVE DESTINATION lib)