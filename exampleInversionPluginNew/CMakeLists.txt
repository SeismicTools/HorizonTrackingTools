cmake_minimum_required(VERSION 3.10)

set(CMAKE_PREFIX_PATH C:/Qt/5.15.2/winrt_x64_msvc2019/lib/cmake/Qt5Core)
set(Qt5Widgets_DIR C:/Qt/5.15.2/winrt_x64_msvc2019/lib/cmake/Qt5Widgets)
set(Qt5_DIR C:/Qt/5.15.2/winrt_x64_msvc2019/lib/cmake/Qt5)

IF(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of
build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug
Release RelWithDebInfo MinSizeRel.")
ELSE()
   SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build,
options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
RelWithDebInfo MinSizeRel.")
ENDIF()

project(ExampleInversionPlugin LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(
    "includes"
    "src"
)

set(NothingValue std::numeric_limits<int32_t>::min())

set(Base
	includes/common.h
	includes/inversionplugininterfaces.h
	includes/pluginprogressreporterinterface.h 
	"src/FotiadiMathReturnCodes.h")
set(Plugin_SRCs
	src/ex1form.ui
	src/ex2form.ui
	src/exampleinversionplugin.h
	src/exampleinversionplugin.cpp
	src/FotiadiConnector.cpp 
	src/FotiadiConnector.h 
	src/PathHelper.h 
	src/PathHelper.cpp 
	"src/FotiadiMathFunctions.h"  
	"src/FileSaverHelper.cpp" "src/FileSaverHelper.h" 
	"src/FotiadiMathReturnCodes.h" "src/AppSettings.h")

set(Plugin_NAME "ExampleInversionPlugin")

ADD_DEFINITIONS(-DQT_PLUGIN)

ADD_LIBRARY(${Plugin_NAME} SHARED ${Base} ${Plugin_SRCs})
TARGET_LINK_LIBRARIES(${Plugin_NAME} Qt5::Widgets)

set(Folder_Desmana_Plugin "C:/Users/nikit/source/repos/W-SEIS_DEV_NEW/plugins")
add_custom_command(TARGET ${Plugin_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/ExampleInversionPlugin.dll"  ${Folder_Desmana_Plugin}/.
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/ExampleInversionPlugin.pdb"  ${Folder_Desmana_Plugin}/.
)