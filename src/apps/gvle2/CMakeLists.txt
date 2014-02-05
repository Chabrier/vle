set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
find_package(PkgConfig REQUIRED)
pkg_check_modules(XMLDEPS libxml-2.0)
include_directories(${XMLDEPS_INCLUDE_DIRS})
if (VLE_VERSION)
  include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS})
  set(V_LIB vlelib)
  if (WIN32)
    link_directories(${VLEDEPS_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})
  endif (WIN32)
else (VLE_VERSION)
  message(STATUS "No VLE_VERSION detected, run standalone build")
  pkg_check_modules(VLEDEPS REQUIRED vle-1.1)
  include_directories(${VLEDEPS_INCLUDE_DIRS})
  link_directories(${VLEDEPS_LIBRARY_DIRS})
  set(V_LIB "vle-1.1")
endif (VLE_VERSION)

if (WIN32)
  set(INTL_LIB "intl")
else (WIN32)
  set(INTL_LIB "")
endif (WIN32)

set(GVLE2_SOURCE_DIR ${VLE_SOURCE_DIR}/src/vle/gvle2)

include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS} ${GVLE2_SOURCE_DIR}
   ${VLE_SOURCE_DIR}/src/apps/gvle2)

SET(gvle2_SOURCES
  ${GVLE2_SOURCE_DIR}/gvle2win.cpp
  ${GVLE2_SOURCE_DIR}/aboutbox.cpp
  ${GVLE2_SOURCE_DIR}/help.cpp
  ${GVLE2_SOURCE_DIR}/filevpzview.cpp
  ${GVLE2_SOURCE_DIR}/simulation.cpp
  ${GVLE2_SOURCE_DIR}/widgetprojecttree.cpp
  ${GVLE2_SOURCE_DIR}/logger.cpp
  main.cpp)

#SET(gvle2_FORMS gvle2win.ui aboutbox.ui help.ui filevpzview.ui
#  simulationview.ui)

QT4_WRAP_UI(ui_gvle2win_h ${GVLE2_SOURCE_DIR}/gvle2win.ui)
QT4_WRAP_UI(aboutbox_h ${GVLE2_SOURCE_DIR}/aboutbox.ui)
QT4_WRAP_UI(help_h ${GVLE2_SOURCE_DIR}/help.ui)
QT4_WRAP_UI(filevpzview_h ${GVLE2_SOURCE_DIR}/filevpzview.ui)
QT4_WRAP_UI(simulationview_h ${GVLE2_SOURCE_DIR}/simulationview.ui)
QT4_WRAP_UI(simulationrtool_h ${GVLE2_SOURCE_DIR}/simulationrtool.ui)

QT4_ADD_RESOURCES(gvle2_RES gvle2.qrc)

add_executable(gvle2 ${gvle2_SOURCES} ${gvle2_RES}
  ${ui_gvle2win_h} ${aboutbox_h} ${help_h} ${filevpzview_h}
  ${simulationview_h} ${simulationrtool_h} )

target_link_libraries(gvle2 ${V_LIB} ${QT_LIBRARIES} ${INTL_LIB})

install(FILES
  ${GVLE2_SOURCE_DIR}/logger.h
  ${GVLE2_SOURCE_DIR}/plugin_sim.h
  DESTINATION ${VLE_INCLUDE_DIRS}/gvle2)

install(TARGETS gvle2 DESTINATION bin)
install(FILES "transalations/tr_en.qm" DESTINATION
  "${VLE_SHARE_DIRS}/translations" RENAME "gvle2_en_US.qm")
install(FILES "transalations/tr_fr.qm" DESTINATION
  "${VLE_SHARE_DIRS}/translations" RENAME "gvle2_fr_FR.qm")
