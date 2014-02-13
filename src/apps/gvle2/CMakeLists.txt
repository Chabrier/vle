set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

find_package(PkgConfig REQUIRED)

pkg_check_modules(XMLDEPS libxml-2.0)
include_directories(${XMLDEPS_INCLUDE_DIRS})


  include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS})
  set(V_LIB vlelib)
  if (WIN32)
    link_directories(${VLEDEPS_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})
  endif (WIN32)

if (WIN32)
  set(INTL_LIB "intl")
else (WIN32)
  set(INTL_LIB "")
endif (WIN32)

include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS})

link_directories(${VLEDEPS_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

set(GVLE2_SOURCE_DIR ${VLE_SOURCE_DIR}/src/vle/gvle2)

QT4_ADD_RESOURCES(gvle2_RES gvle2.qrc)

if (WIN32)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gvle2.o
    COMMAND ${CMAKE_RC_COMPILER}
    -I${CMAKE_BINARY_DIR}/share
    -i${CMAKE_BINARY_DIR}/share/gvle.rc
    -o${CMAKE_CURRENT_BINARY_DIR}/gvle2.o)
  add_executable(gvle2 WIN32 main.cpp ${gvle2_RES}
    ${CMAKE_CURRENT_BINARY_DIR}/gvle2.o)

  set_target_properties(gvle PROPERTIES
    COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")

else (WIN32)
  add_executable(gvle2 WIN32 main.cpp ${gvle2_RES})

  set_target_properties(gvle2 PROPERTIES
    SOVERSION "${VLE_MAJOR}.${VLE_MINOR}"
    VERSION "${VLE_MAJOR}.${VLE_MINOR}"
    COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")
endif (WIN32)

target_link_libraries(gvle2 gvle2lib vlelib ${VLEDEPS_LIBRARIES}
  ${Boost_LIBRARIES} ${OS_SPECIFIC_LIBRARIES})

install(TARGETS gvle2 DESTINATION bin)

install(FILES "transalations/tr_en.qm" DESTINATION
  "${VLE_SHARE_DIRS}/translations" RENAME "gvle2_en_US.qm")
install(FILES "transalations/tr_fr.qm" DESTINATION
  "${VLE_SHARE_DIRS}/translations" RENAME "gvle2_fr_FR.qm")
