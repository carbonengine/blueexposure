# Copyright (c) 2026 CCP Games

ccp_add_library(BlueExposureTest SHARED
    BlueExposureTest.cpp
    TestAttributes.cpp
    TestAttributes_Blue.cpp
    TestCallbacks.cpp
    TestCallbacks_Blue.cpp
    TestFunctions.cpp
    TestMethods.cpp
    TestMethods_Blue.cpp
    TestMultipleInterfaces.cpp
    TestMultipleInterfaces_Blue.cpp
    TestOptionalArgs.cpp
    TestOptionalArgs_Blue.cpp
    TestProperties.cpp
    TestProperties_Blue.cpp
    TestReferences.cpp
    TestReferences_Blue.cpp
    TestSuperclassAttributes_Blue.cpp
    TestSuperclassMethods_Blue.cpp
)
target_precompile_headers(BlueExposureTest
    PRIVATE
        <BlueExposure.h>
        "BoolBeResult.h"
)
target_sources(BlueExposureTest PRIVATE
    BoolBeResult.h
    TestAttributes.h
    TestCallbacks.h
    TestMethods.h
    TestMultipleInterfaces.h
    TestOptionalArgs.h
    TestProperties.h
    TestReferences.h
    TestSuperclassAttributes.h
    TestSuperclassMethods.h
)

if(APPLE)
    target_link_libraries(BlueExposureTest PRIVATE "-framework Foundation")
endif()

if(CMAKE_IMPORT_LIBRARY_SUFFIX)
    add_custom_command(TARGET BlueExposureTest POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:BlueExposureTest> $<TARGET_FILE_DIR:BlueExposureTest>
            COMMAND_EXPAND_LISTS
            VERBATIM
    )
endif()

target_compile_definitions(BlueExposureTest PUBLIC BLUEBUILD BLUE_WITH_PYTHON)

target_link_libraries(BlueExposureTest
    PRIVATE BlueExposure
    PUBLIC Python3::Python
)

set_target_properties(BlueExposureTest PROPERTIES
    FOLDER "Tests"
    OUTPUT_NAME "_blueexposuretest"
)

# Construct python path for tests
if(WIN32)
    set(PYTHONPATH_ENV "$<TARGET_FILE_DIR:BlueExposureTest>\;${CMAKE_CURRENT_SOURCE_DIR}/python")
elseif(APPLE OR LINUX)
    set(PYTHONPATH_ENV "$<TARGET_FILE_DIR:BlueExposureTest>:${CMAKE_CURRENT_SOURCE_DIR}/python")
endif()

set(ENV{PYTHONPATH} ${PYTHONPATH_ENV})
message("-- Python Path for tests: " ${PYTHONPATH_ENV})

# discover python tests ... this may be a little bit brittle since we're working around the missing `_blueexposuretest`
# binary. As in, test discovery happens during the configure stage at which the binary does not exist yet.
execute_process(COMMAND ${Python3_EXECUTABLE} discover.py
    OUTPUT_VARIABLE PYTHON_TESTS_STR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE PYTHON_TEST_DISCOVERY_RETVAL
    ERROR_VARIABLE PYTHON_TEST_DISCOVERY_ERRORS
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/python/BlueExposureTest
    )

if(${PYTHON_TEST_DISCOVERY_RETVAL})
    message(FATAL_ERROR "Error encountered during test discovery:\n=== stderr: ===\n${PYTHON_TEST_DISCOVERY_ERRORS}\n=== stdout: ===\n${PYTHON_TESTS_STR}")
endif()

separate_arguments(PYTHON_TESTS UNIX_COMMAND ${PYTHON_TESTS_STR})
foreach(PYTHON_TEST ${PYTHON_TESTS})
    add_test(NAME ${PYTHON_TEST}
        COMMAND ${Python3_EXECUTABLE} -m unittest -v ${PYTHON_TEST}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/python/BlueExposureTest/test
    )
    if(WIN32)
        set_tests_properties(${PYTHON_TEST} PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH_ENV};BUILDFLAVOR=$<LOWER_CASE:$<CONFIG>>")
    elseif(APPLE OR LINUX)
        set_tests_properties(${PYTHON_TEST} PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH_ENV};BUILDFLAVOR=$<LOWER_CASE:$<CONFIG>>")
    else()
        message(FATAL_ERROR "Unsupported platform")
    endif()
endforeach()

add_subdirectory(cpp)
