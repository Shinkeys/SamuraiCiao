# Install script for directory: /home/bob/Projects/SamuraiCiao/vendor/assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.4.3" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so.5.4.3"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so.5"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/bob/Projects/SamuraiCiao/build/vendor/assimp/bin/libassimp.so.5.4.3"
    "/home/bob/Projects/SamuraiCiao/build/vendor/assimp/bin/libassimp.so.5"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so.5.4.3"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so.5"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.4.3" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/bob/Projects/SamuraiCiao/build/vendor/assimp/bin/libassimp.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/anim.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/aabb.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/ai_assert.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/camera.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/color4.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/color4.inl"
    "/home/bob/Projects/SamuraiCiao/build/vendor/assimp/code/../include/assimp/config.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/ColladaMetaData.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/commonMetaData.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/defs.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/cfileio.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/light.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/material.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/material.inl"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/matrix3x3.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/matrix3x3.inl"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/matrix4x4.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/matrix4x4.inl"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/mesh.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/ObjMaterial.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/pbrmaterial.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/GltfMaterial.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/postprocess.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/quaternion.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/quaternion.inl"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/scene.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/metadata.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/texture.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/types.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/vector2.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/vector2.inl"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/vector3.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/vector3.inl"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/version.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/cimport.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/AssertHandler.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/importerdesc.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/Importer.hpp"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/DefaultLogger.hpp"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/ProgressHandler.hpp"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/IOStream.hpp"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/IOSystem.hpp"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/Logger.hpp"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/LogStream.hpp"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/NullLogger.hpp"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/cexport.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/Exporter.hpp"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/DefaultIOStream.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/DefaultIOSystem.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/ZipArchiveIOSystem.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/SceneCombiner.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/fast_atof.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/qnan.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/BaseImporter.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/Hash.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/MemoryIOWrapper.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/ParsingUtils.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/StreamReader.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/StreamWriter.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/StringComparison.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/StringUtils.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/SGSpatialSort.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/GenericProperty.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/SpatialSort.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/SkeletonMeshBuilder.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/SmallVector.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/SmoothingGroups.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/SmoothingGroups.inl"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/StandardShapes.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/RemoveComments.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/Subdivision.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/Vertex.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/LineSplitter.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/TinyFormatter.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/Profiler.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/LogAux.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/Bitmap.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/XMLTools.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/IOStreamBuffer.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/CreateAnimMesh.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/XmlParser.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/BlobIOSystem.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/MathFunctions.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/Exceptional.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/ByteSwapper.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/Base64.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/Compiler/poppack1.h"
    "/home/bob/Projects/SamuraiCiao/vendor/assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/bob/Projects/SamuraiCiao/build/vendor/assimp/code/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
