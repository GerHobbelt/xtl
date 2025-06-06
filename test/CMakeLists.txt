############################################################################
# Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          #
# Copyright (c) QuantStack                                                 #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

cmake_minimum_required(VERSION 3.29)

find_package(doctest            REQUIRED)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(xtl-test)

    enable_testing()

    find_package(xtl REQUIRED CONFIG)
    find_package(nlohmann_json QUIET CONFIG)
    set(XTL_INCLUDE_DIR ${xtl_INCLUDE_DIRS})
endif ()

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting tests build type to Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
else()
    message(STATUS "Tests build type is ${CMAKE_BUILD_TYPE}")
endif()

include(CheckCXXCompilerFlag)

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

if(nlohmann_json_FOUND)
  add_definitions(-DHAVE_NLOHMANN_JSON)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES GNU OR CMAKE_CXX_COMPILER_ID MATCHES Intel)
    add_compile_options(-Wunused-parameter -Wextra -Wreorder -Wconversion -Wsign-conversion)

    if(NOT CMAKE_CXX_FLAGS MATCHES "-march")
        CHECK_CXX_COMPILER_FLAG(-march=native HAS_MARCH_NATIVE)
        if (HAS_MARCH_NATIVE)
            add_compile_options(-march=native)
        endif()
    endif()
    if (XTL_DISABLE_EXCEPTIONS)
        add_compile_options(-fno-exceptions)
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    add_compile_options(/EHsc /MP /bigobj)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
    if (XTL_DISABLE_EXCEPTIONS)
        add_compile_options(/EHs-c-)
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  if(NOT WIN32)
    add_compile_options(-Wunused-parameter -Wextra -Wreorder -Wconversion -Wsign-conversion)

    if(NOT CMAKE_CXX_FLAGS MATCHES "-march")
        CHECK_CXX_COMPILER_FLAG(-march=native HAS_MARCH_NATIVE)
        if (HAS_MARCH_NATIVE)
            add_compile_options(-march=native)
        endif()
    endif()
    if (XTL_DISABLE_EXCEPTIONS)
        add_compile_options(-fno-exceptions)
    endif()
  else() # we are using clang-cl
    add_compile_options(/EHsc /bigobj -fms-compatibility)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
    if (XTL_DISABLE_EXCEPTIONS)
        add_compile_options(/EHs-c-)
    endif()
  endif()
endif()

find_package(Threads)

set(XTL_TESTS
    test_xbase64.cpp
    test_xbasic_fixed_string.cpp
    test_xcomplex.cpp
    test_xcompare.cpp
    test_xcomplex_sequence.cpp
    test_xclosure.cpp
    test_xdynamic_bitset.cpp
    test_xfunctional.cpp
    test_xhalf_float.cpp
    test_xhash.cpp
    test_xhierarchy_generator.cpp
    test_xiterator_base.cpp
    test_xmasked_value.cpp
    test_xmeta_utils.cpp
    test_xmultimethods.cpp
    test_xoptional.cpp
    test_xsequence.cpp
    test_xtype_traits.cpp
    test_xplatform.cpp
    test_xproxy_wrapper.cpp
    test_xsystem.cpp
    test_xvisitor.cpp
)

if(nlohmann_json_FOUND)
    # Version up to 3.1.2 export the target `nlohmann_json`
    if(TARGET nlohmann_json)
      set(nlohmann_json_TARGET nlohmann_json)
    # Newer versions export the namespaced target `nlohmann_json::nlohmann_json`
    elseif(TARGET nlohmann_json::nlohmann_json)
      set(nlohmann_json_TARGET nlohmann_json::nlohmann_json)
    endif()
endif()

foreach(filename IN LISTS XTL_TESTS)
    get_filename_component(targetname ${filename} NAME_WE)

    add_executable(${targetname} main.cpp  ${filename} ${XTL_HEADERS})
    target_include_directories(${targetname} PRIVATE ${XTL_INCLUDE_DIR})
    target_link_libraries(${targetname} xtl doctest::doctest Threads::Threads ${nlohmann_json_TARGET})

    add_test(NAME ${targetname} COMMAND ${targetname})
endforeach()

add_executable(test_xtl main.cpp ${XTL_TESTS} ${XTL_HEADERS})
target_include_directories(test_xtl PRIVATE ${XTL_INCLUDE_DIR})
target_link_libraries(test_xtl xtl doctest::doctest Threads::Threads ${nlohmann_json_TARGET})

add_custom_target(xtest COMMAND test_xtl DEPENDS test_xtl)
add_test(NAME xtest COMMAND test_xtl)
