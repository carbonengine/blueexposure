# Copyright © 2026 CCP ehf.

cmake_minimum_required(VERSION 3.16)

project(BlueExposure VERSION 1.0.0)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)
if(NOT DEFINED ENV{CCP_EVE_PERFORCE_BRANCH_PATH})
    message(FATAL_ERROR "Missing required environment variable CCP_EVE_PERFORCE_BRANCH_PATH")
elseif(NOT IS_ABSOLUTE $ENV{CCP_EVE_PERFORCE_BRANCH_PATH})
    message(FATAL_ERROR "CCP_EVE_PERFORCE_BRANCH_PATH needs to be an absolute path")
elseif(NOT EXISTS $ENV{CCP_EVE_PERFORCE_BRANCH_PATH})
    message(FATAL_ERROR "CCP_EVE_PERFORCE_BRANCH_PATH is pointing at a non-existing location $ENV{CCP_EVE_PERFORCE_BRANCH_PATH}")
elseif(NOT IS_DIRECTORY $ENV{CCP_EVE_PERFORCE_BRANCH_PATH})
    message(FATAL_ERROR "CCP_EVE_PERFORCE_BRANCH_PATH needs to be a directory")
else()
    message(STATUS "Using CCP_EVE_PERFORCE_BRANCH_PATH located at $ENV{CCP_EVE_PERFORCE_BRANCH_PATH}")
endif()

file(TO_CMAKE_PATH "$ENV{CCP_EVE_PERFORCE_BRANCH_PATH}" BRANCH_ROOT_DIR)
list(INSERT CMAKE_MODULE_PATH 0 ${BRANCH_ROOT_DIR}/cmake)
include(CcpGlobalSettings)
include(CcpPackageConfigHelpers)

find_package(CcpCore REQUIRED NO_CMAKE_PATH)
find_package(Python REQUIRED NO_CMAKE_PATH)

ccp_add_library(BlueExposure STATIC
    BlueClasses.cpp
    BlueClasses.h
    BlueClasses_Blue.cpp
    BlueExposure.cpp
    BlueExtractArgument.cpp
    BlueListUtil.cpp
    BlueMemberIterator.cpp
    BluePythonThunkers.cpp
    BluePythonThunkers.h
    BluePyWrap.cpp
    BluePyWrap.h
    BlueRegistration.cpp
    BlueRegistrationPython.cpp
    BlueScriptCallback.cpp
    BlueSharedString.cpp
    BlueStdResult.cpp
    BlueStructureList.cpp
    BlueTypes.cpp
    BlueUtil.cpp
    BlueVariable.cpp
    Copier.cpp
    Copier_Blue.cpp
    Find.cpp
    InterfaceDefinitions.cpp
    PythonKlass.cpp
    TypeInfo.cpp
    TypeInfo.h
)

set(PUBLIC_HEADER_FILES
    include/BlueCallFunction.h
    include/BlueCallFunctionHelpers.h
    include/BlueCallFunctionWithBeResult.h
    include/BlueClass.h
    include/BlueDict.h
    include/BlueExposure.h
    include/BlueExposureHelpers.h
    include/BlueExposureHelpersPython.h
    include/BlueExposureMacros.h
    include/BlueExposureMacrosDeprecated.h
    include/BlueExposureMacrosPython.h
    include/BlueExposureTypeSignature.h
    include/BlueExtractArgument.h
    include/BlueExtractArgumentPython.h
    include/BlueFunctionTraits.h
    include/BlueListUtil.h
    include/BlueMemberFunctionAsCallable.h
    include/BlueMemberFunctionTraits.h
    include/BlueMemberIterator.h
    include/BluePythonObject.h
    include/BlueRegistration.h
    include/BlueScriptCallback.h
    include/BlueScriptValue.h
    include/BlueSharedString.h
    include/BlueSmartPtr.h
    include/BlueSmartPy.h
    include/BlueStdResult.h
    include/BlueStringTable.h
    include/BlueStructureList.h
    include/BlueTypes.h
    include/BlueTypeTraits.h
    include/BlueUtil.h
    include/BlueVariable.h
    include/BlueVectorTypes.h
    include/BlueWeakRef.h
    include/BlueWrapReturnValue.h
    include/BlueWrapReturnValuePython.h
    include/Copier.h
    include/Find.h
    include/IBlueClasses.h
    include/IBlueDict.h
    include/IBlueRtti.h
    include/IBlueStructureList.h
    include/ICopier.h
    include/ICustomPersist.h
    include/IInitialize.h
    include/IList.h
    include/INotify.h
    include/IPythonMethods.h
    include/IPythonNumeric.h
    include/PythonKlass.h
    include/StringBeResult.h
)

target_sources(BlueExposure PRIVATE ${PUBLIC_HEADER_FILES})

target_compile_definitions(BlueExposure
    PUBLIC
        BLUE_WITH_PYTHON
    PRIVATE
        BLUEBUILD
)

target_link_libraries(BlueExposure PUBLIC CcpCore Python)

target_include_directories(BlueExposure
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(BUILD_TESTING "Build tests. Enabled by default." ON)
    include(CTest)
    if(BUILD_TESTING)
	    add_subdirectory(tests)
    endif()
endif()

# Provide an install target iff this is the top level project only
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Install rule to ensure that our runtime and linker files are in the expected, platform-specific folders
    install(
        TARGETS BlueExposure
        EXPORT BlueExposureTargets
        CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
        ARCHIVE DESTINATION lib/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/
        RUNTIME DESTINATION bin/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/
    )
    # Install rule for available public headers
    install(FILES ${PUBLIC_HEADER_FILES} DESTINATION include)

    configure_ccp_vendor_config_file(
        TARGET BlueExposure
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/BlueExposureConfig.cmake"
        DEPENDENCIES "CcpCore;Python"
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/BlueExposureConfig.cmake DESTINATION .)
endif()
