cmake_minimum_required(VERSION 3.16)

project(BlueExposureTest VERSION 1.0.0)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

add_library(BlueExposureTest SHARED
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
)
target_sources(BlueExposureTest PRIVATE
    BoolBeResult.h
    StdAfx.h
    TestAttributes.h
    TestCallbacks.h
    TestMethods.h
    TestMultipleInterfaces.h
    TestOptionalArgs.h
    TestProperties.h
    TestReferences.h
)

target_precompile_headers(BlueExposureTest PRIVATE StdAfx.h)

if(APPLE)
    target_link_libraries(BlueExposureTest PRIVATE "-framework Foundation")
endif()

target_compile_definitions(BlueExposureTest PUBLIC BLUEBUILD BLUE_WITH_PYTHON)

target_link_libraries(BlueExposureTest
    PRIVATE BlueExposure
    PUBLIC Python
)

target_include_directories(BlueExposureTest
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

set_target_properties(BlueExposureTest PROPERTIES
    FOLDER "Tests"
    OUTPUT_NAME "_blueexposuretest"
)

# Copy binary runtime dependencies on DLL platforms
add_custom_command(TARGET BlueExposureTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_RUNTIME_DLLS:BlueExposureTest>
    $<TARGET_FILE_DIR:BlueExposureTest>
)

set(TEST_DESTINATION test/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/)

if(APPLE)
    # on macOS we like to still use the .so naming convention, without a prefix
    set_target_properties(BlueExposureTest
        PROPERTIES
            PREFIX ""
            SUFFIX ".so"
    )
    install(
        TARGETS BlueExposureTest
        EXPORT BlueExposureTestTargets
        CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
        LIBRARY DESTINATION test/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/
    )
endif()
if(WIN32)
    install(
        TARGETS BlueExposureTest
        EXPORT BlueExposureTestTargets
        CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
        RUNTIME DESTINATION test/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/
    )
endif()
if(WIN32)
    # Discover Python tests. This may be a little bit brittle since at this point we are missing missing the `_blueexposuretest`
    # binary. As in, test discovery happens during the configure stage at which the binary does not exist yet.
    set(PYTHON_EXECUTABLE "${BRANCH_ROOT_DIR}/vendor/python/2.7.1+ccp-stackless/bin/Windows/x64/v141/python.exe")
    set(ENV{PYTHONPATH} "${BRANCH_ROOT_DIR}/carbon/common/stdlib")
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import ctesthelper;ctesthelper.discover_tests()"
        OUTPUT_VARIABLE PYTHON_TESTS_STR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/python
    )
    separate_arguments(PYTHON_TESTS UNIX_COMMAND ${PYTHON_TESTS_STR})
    foreach(PYTHON_TEST ${PYTHON_TESTS})
        add_test(NAME ${PYTHON_TEST}
            COMMAND ${PYTHON_EXECUTABLE} -m unittest -v ${PYTHON_TEST}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/python
        )
        set_tests_properties(${PYTHON_TEST} PROPERTIES
            ENVIRONMENT "PYTHONPATH=${BRANCH_ROOT_DIR}/carbon/common/stdlib\\\;$<TARGET_FILE_DIR:BlueExposureTest>"
        )
    endforeach()
endif()
