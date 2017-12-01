cmake_minimum_required(VERSION 3.8)

project(UEngine)

#option(UCORE_BUILD_SHARED "dll or static" ON)
#option(UENGINE_BUILD_SHARED "dll or static?" ON)
#option(UGFXDIRECTX_BUILD_SHARED "" ON)
#option(UGFXVULKAN_BUILD_SHARED "" ON)

set(PhysXSDKDir "D:/Lib/PhysX-3.3-master/PhysXSDK"  CACHE PATH "")
set(QT5Dir "E:/Qt5/5.6.3/msvc2015_64"  CACHE PATH "")

if("${QT5Dir}" STREQUAL "")
	message(WARNING "QT5Dir is empty")
endif()

if("${PhysXSDKDir}" STREQUAL "")
	message(WARNING "PhysXSDKDir is empty")
endif()

#this add a definition to target as ${Target}_BUILD 
macro(SetTargetExport Target)
	set_target_properties(${Target} PROPERTIES COMPILE_FLAGS "-D${Target}_BUILD")
endmacro()

macro(AdjustTargetDebugWorkingDir Target)
	set_target_properties(${Target} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/../${UPLATFORM_NAME}/Debug")
endmacro()


#message(${CMAKE_BINARY_DIR})
#message(${CMAKE_SOURCE_DIR})

#if(UCORE_BUILD_SHARED)
#	#all projects will have UCORE_SHARED preprocessor
#	add_definitions(-DUCORE_SHARED)
#	set(UCORE_BUILD_TYPE SHARED)
#else()
#	set(UCORE_BUILD_TYPE STATIC)
#endif()


set(UCORE_BUILD_TYPE SHARED)
set(UENGINE_BUILD_TYPE SHARED)
set(UENGINE2D_BUILD_TYPE SHARED)

set(UGFXDIRECTX_BUILD_TYPE SHARED)
set(UGFXVULKAN_BUILD_TYPE SHARED)
set(UGFXCORE_BUILD_TYPE SHARED)
set(USCENERENDERER_BUILD_TYPE SHARED)
set(UEDITORCOMMON_BUILD_TYPE SHARED)

add_definitions(-DUNICODE -D_UNICODE)

add_definitions(
-DUCORE_SHARED 
-DUENGINE_SHARED 
-DUENGINE2D_SHARED
-DUGFXDIRECTX_SHARED 
-DUGFXVULKAN_SHARED 
-DUGFXCORE_SHARED 
-DUSCENERENDERER_SHARED
-DUEDITORCOMMON_SHARED
-DIMGUI_SHARED
-DUCORE_HAS_ZLIB
)



#SET(CMAKE_BUILD_TYPE Debug CACHE STRING "default to debug" FORCE)

if(WIN32)
	if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(UPLATFORM_NAME "Win32")
	else()
		set(UPLATFORM_NAME "Win64")
	endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../${UPLATFORM_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../${UPLATFORM_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../${UPLATFORM_NAME})

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${UPLATFORM_NAME})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${UPLATFORM_NAME})
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${UPLATFORM_NAME})

#wall == enable all warning

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++latest")





# project version
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)

#However, the file(GLOB...) allows for wildcard additions:

file(GLOB_RECURSE SOURCES_Core "Core/*.cpp")
file(GLOB_RECURSE SOURCES_CoreTest "CoreTest/*.cpp")
file(GLOB_RECURSE SOURCES_Engine "Engine/*.cpp")
file(GLOB_RECURSE SOURCES_Launcher "Launcher/*.cpp")

file(GLOB_RECURSE SOURCES_EditorCommon "EditorCommon/*.cpp")
file(GLOB_RECURSE HEADERS_EditorCommon "EditorCommon/*.h")

file(GLOB_RECURSE SOURCES_Editor "Editor/*.cpp")
file(GLOB_RECURSE SOURCES_GFXDirectX "GFXDirectX/*.cpp")
file(GLOB_RECURSE SOURCES_GFXVulkan "GFXVulkan/*.cpp")
file(GLOB_RECURSE SOURCES_GFXCore "GFXCore/*.cpp")
file(GLOB_RECURSE SOURCES_SceneRenderer "SceneRenderer/*.cpp")
file(GLOB_RECURSE SOURCES_IMGUI "IMGUI/*.cpp")
file(GLOB_RECURSE SOURCES_DXTest "DXTest/*.cpp")
file(GLOB_RECURSE SOURCES_LauncherTest "ULauncherTest/*.cpp")

file(GLOB_RECURSE HEADERS_Core "Core/*.h")
file(GLOB_RECURSE HEADERS_CoreTest "CoreTest/*.h")
file(GLOB_RECURSE HEADERS_Engine "Engine/*.h")
file(GLOB_RECURSE HEADERS_Launcher "Launcher/*.h")


#IMGUI was removed from project
#file(GLOB_RECURSE HEADERS_EditorBaseIM "EditorBaseIM/*.h")
#file(GLOB_RECURSE SOURCES_EditorBaseIM "EditorBaseIM/*.cpp")

file(GLOB_RECURSE HEADERS_Editor "Editor/*.h")
file(GLOB_RECURSE HEADERS_GFXDirectX "GFXDirectX/*.h")
file(GLOB_RECURSE HEADERS_GFXVulkan "GFXVulkan/*.h")
file(GLOB_RECURSE HEADERS_GFXCore "GFXCore/*.h")
file(GLOB_RECURSE HEADERS_SceneRenderer "SceneRenderer/*.h")
file(GLOB_RECURSE HEADERS_IMGUI "IMGUI/*.h")
file(GLOB_RECURSE HEADERS_DXTest "DXTest/*.h" "DXTest/*.hlsl")
file(GLOB_RECURSE HEADERS_LauncherTest "ULauncherTest/*.h")



include_directories("${CMAKE_SOURCE_DIR}/ThirdParty/")
include_directories("${CMAKE_SOURCE_DIR}/ThirdParty/assimp/include")

set(TPPATH_DEBUG ${CMAKE_SOURCE_DIR}/ThirdParty/Lib/${UPLATFORM_NAME}/Debug)
set(TPPATH_RELEASE ${CMAKE_SOURCE_DIR}/ThirdParty/Lib/${UPLATFORM_NAME}/Release)

#libraries that UCore uses
set(LIBS_UCORE
zlibstatic
ConvertUTF
)

#UCore ---------------------
add_library(UCore ${UCORE_BUILD_TYPE}
${SOURCES_Core} 
${HEADERS_Core} )


#add definition to a target
set_target_properties(UCore PROPERTIES COMPILE_FLAGS -DUCORE_BUILD)

target_link_libraries(UCore 
debug ${TPPATH_DEBUG}/zlibstatic.lib optimized ${TPPATH_RELEASE}/zlibstatic.lib
debug ${TPPATH_DEBUG}/convertUTF.lib optimized ${TPPATH_RELEASE}/convertUTF.lib)



#UCoreTest -----------------
add_executable(UCoreTest 
${SOURCES_CoreTest} 
${HEADERS_CoreTest} 
)
target_link_libraries(UCoreTest UCore)
AdjustTargetDebugWorkingDir(UCoreTest)

#UEngine --------------------
add_library(UEngine ${UENGINE_BUILD_TYPE} 
${SOURCES_Engine} 
${HEADERS_Engine} )
set_target_properties(UEngine PROPERTIES COMPILE_FLAGS -DUENGINE_BUILD)
#Names passed into this can be libraries fullpaths to libraries, or the name of a library from an add_library command
target_link_libraries(UEngine UCore UGFXCore
debug ${TPPATH_DEBUG}/assimp.lib optimized ${TPPATH_RELEASE}/assimp.lib)


target_include_directories(UEngine PUBLIC ${PhysXSDKDir}/include)

#ULauncherTest----------------------------------------------------------------------
add_executable(ULauncherTest
${HEADERS_LauncherTest}
${SOURCES_LauncherTest}
)
target_link_libraries(ULauncherTest UCore UEngine UGFXDirectX IMGUI UEditorBaseIM
debug ${TPPATH_DEBUG}/FileWatcher.lib optimized ${TPPATH_RELEASE}/FileWatcher.lib
debug ${TPPATH_DEBUG}/assimp.lib optimized ${TPPATH_RELEASE}/assimp.lib
)

AdjustTargetDebugWorkingDir(ULauncherTest)


#IMGUI ----------------------------------------------------------------
add_library(IMGUI SHARED 
${SOURCES_IMGUI}
${HEADERS_IMGUI})
set_target_properties(IMGUI PROPERTIES COMPILE_FLAGS -DIMGUI_BUILD)
target_link_libraries(IMGUI UCore)

#EditorCommon -------------------------------------------------------------------------
add_library(UEditorCommon ${UEDITORCOMMON_BUILD_TYPE}
${SOURCES_EditorCommon}
${HEADERS_EditorCommon}
)
set_target_properties(UEditorCommon PROPERTIES COMPILE_FLAGS -DUEDITORCOMMON_BUILD)

target_link_libraries(UEditorCommon UCore UEngine UGFXDirectX)
target_link_libraries(UEditorCommon
debug ${QT5Dir}/lib/qt5cored.lib optimized ${QT5Dir}/lib/qt5core.lib
debug ${QT5Dir}/lib/qt5guid.lib optimized ${QT5Dir}/lib/qt5gui.lib
debug ${QT5Dir}/lib/qt5widgetsd.lib optimized ${QT5Dir}/lib/qt5widgets.lib
)
target_include_directories(UEditorCommon PUBLIC "${QT5Dir}/include")


#EditorBaseIM not supported anymore, I am focusing on QT
#EditorBaseIM -------------------------------------------------------------------------
#add_library(UEditorBaseIM STATIC
#${SOURCES_EditorBaseIM}
#${HEADERS_EditorBaseIM}
#)
#
#set_target_properties(UEditorBaseIM PROPERTIES COMPILE_FLAGS -DUEDITORBASEIM_BUILD)
#target_link_libraries(UEditorBaseIM UCore UEngine)



#Editor-------------------------------------------------------------------------------
add_executable(UEditor 
${SOURCES_Editor}
${HEADERS_Editor}
)
#note UGFXDirectX must be loaded dynamically in future
target_link_libraries(UEditor UEditorCommon UEngine UGFXDirectX)
AdjustTargetDebugWorkingDir(UEditor)

#UGFXCore -------------------------------------------------------------------------------------
add_library(UGFXCore ${UGFXCORE_BUILD_TYPE}
${SOURCES_GFXCore}
${HEADERS_GFXCore}
)
target_link_libraries(UGFXCore UCore)
set_target_properties(UGFXCore PROPERTIES COMPILE_FLAGS -DUGFXCORE_BUILD)

#GFXDirectX ---------------------------------------------------------------------------
add_library(UGFXDirectX ${UGFXDIRECTX_BUILD_TYPE}
${SOURCES_GFXDirectX}
${HEADERS_GFXDirectX}
)
target_link_libraries(UGFXDirectX UGFXCore UCore IMGUI d3d11.lib d3d12.lib dxgi.lib d3dcompiler.lib
debug ${TPPATH_DEBUG}/DirectXTK12.lib optimized ${TPPATH_RELEASE}/DirectXTK12.lib
)
set_target_properties(UGFXDirectX PROPERTIES COMPILE_FLAGS -DUGFXDIRECTX_BUILD)


#GFXVulkan ----------------------------------------------------------------------------
add_library(UGFXVulkan ${UGFXVULKAN_BUILD_TYPE}
${SOURCES_GFXVulkan}
${HEADERS_GFXVulkan}
)
target_link_libraries(UGFXVulkan  UGFXCore UCore
${CMAKE_SOURCE_DIR}/ThirdParty/vulkan/lib/vulkan-1.lib )

set_target_properties(UGFXVulkan PROPERTIES COMPILE_FLAGS -DUGFXVULKAN_BUILD)

#DXTest--------------------------------------------------------------------------
add_executable(UDXTest
${SOURCES_DXTest}
${HEADERS_DXTest})
target_link_libraries(UDXTest UCore UGFXDirectX)
AdjustTargetDebugWorkingDir(UDXTest)

#USceneRenderer ----------------------------------------------------------------------
add_library(USceneRenderer ${USCENERENDERER_BUILD_TYPE}
${SOURCES_SceneRenderer}
${HEADERS_SceneRenderer}
)

target_link_libraries(USceneRenderer UGFXDirectX)
set_target_properties(USceneRenderer PROPERTIES COMPILE_FLAGS -DUSCENERENDERER_BUILD)

#A*Test----------------------------------------------------------------------------------------------------------------------------------------
file(GLOB_RECURSE HEADERS_AStarTest "AStarTest/*.h")
file(GLOB_RECURSE SOURCES_AStarTest "AStarTest/*.cpp")

add_executable(AStarTest ${HEADERS_AStarTest} ${SOURCES_AStarTest})
target_include_directories(AStarTest PUBLIC "${QT5Dir}/include")

target_link_libraries(AStarTest UCore
debug ${QT5Dir}/lib/qt5cored.lib optimized ${QT5Dir}/lib/qt5core.lib
debug ${QT5Dir}/lib/qt5guid.lib optimized ${QT5Dir}/lib/qt5gui.lib
debug ${QT5Dir}/lib/qt5widgetsd.lib optimized ${QT5Dir}/lib/qt5widgets.lib
)
AdjustTargetDebugWorkingDir(AStarTest)



