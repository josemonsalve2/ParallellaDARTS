# Install script for directory: /home/parallella/svn/darts-parallella/DARTS/src/threading

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/home/parallella")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "RELEASE")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "bin")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/darts/lib" TYPE STATIC_LIBRARY FILES "/home/parallella/svn/darts-parallella/DARTS/build/src/threading/libcodelet.a")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "bin")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/darts/include" TYPE FILE FILES
    "/home/parallella/svn/darts-parallella/DARTS/include/threading/Codelet.h"
    "/home/parallella/svn/darts-parallella/DARTS/include/threading/codeletDefines.h"
    "/home/parallella/svn/darts-parallella/DARTS/include/threading/SyncSlot.h"
    "/home/parallella/svn/darts-parallella/DARTS/include/threading/ThreadedProcedure.h"
    "/home/parallella/svn/darts-parallella/DARTS/include/threading/doTP.h"
    "/home/parallella/svn/darts-parallella/DARTS/include/threading/doLoop.h"
    "/home/parallella/svn/darts-parallella/DARTS/include/threading/tpClosure.h"
    "/home/parallella/svn/darts-parallella/DARTS/include/threading/loopClosure.h"
    "/home/parallella/svn/darts-parallella/DARTS/include/threading/loop.h"
    "/home/parallella/svn/darts-parallella/DARTS/include/threading/nested.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")

