################################################################################
#                                                                              #
# Copyright (c) 2011-2017, University of Delaware                              #
# All rights reserved.                                                         #
#                                                                              #
# Redistribution and use in source and binary forms, with or without           #
# modification, are permitted provided that the following conditions           #
# are met:                                                                     #
#                                                                              #
# 1. Redistributions of source code must retain the above copyright            #
# notice, this list of conditions and the following disclaimer.                #
#                                                                              #
# 2. Redistributions in binary form must reproduce the above copyright         #
# notice, this list of conditions and the following disclaimer in the          #
# documentation and/or other materials provided with the distribution.         #
#                                                                              #
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS          #
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT            #
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS            #
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE               #
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,         #
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,         #
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;             #
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER             #
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT           #
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN            #
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE              #
# POSSIBILITY OF SUCH DAMAGE.                                                  #
#                                                                              #
################################################################################

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -T ${ELDF_EDARTS_CACHEMAN}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${ELDF_EDARTS_CACHEMAN} ${CMAKE_BINARY_DIR}/eDARTS/src/softcache/cachemanager.o")


# Given that I am not able to use two compilers with cmake. I will
# use custom command and custom target to manually build this
if(DEBUG)
set(EXTERNAL_FLAGS -g -I${EPIPHANY_HOME}/tools/host/include -I${CMAKE_SOURCE_DIR}/eDARTS/include/runtime/host -L${PTHREAD_LIB} -lpthread ${E_HAL_LIB} ${E_LOADER_LIB})
set(EXTERNAL_FLAGS_API -g -I${EPIPHANY_HOME}/tools/host/include -I${CMAKE_SOURCE_DIR}/eDARTS/include/runtime -I${CMAKE_SOURCE_DIR}/eDARTS/include/runtime/host)
else()
set(EXTERNAL_FLAGS -I${EPIPHANY_HOME}/tools/host/include -I${CMAKE_SOURCE_DIR}/eDARTS/include/runtime/host -L${PTHREAD_LIB} -lpthread ${E_HAL_LIB} ${E_LOADER_LIB})
set(EXTERNAL_FLAGS_API -I${EPIPHANY_HOME}/tools/host/include -I${CMAKE_SOURCE_DIR}/eDARTS/include/runtime -I${CMAKE_SOURCE_DIR}/eDARTS/include/runtime/host)
endif()

# For the host we need to compile manually without cmake
add_custom_command(
   OUTPUT darts_print_server.o
   COMMAND gcc -c ${CMAKE_CURRENT_SOURCE_DIR}/darts_print_server.c ${EXTERNAL_FLAGS} -o darts_print_server.o
   DEPENDS darts_print_server.c
   COMMENT "CROSS-COMPILING HOST FILE darts_print_server "
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   VERBATIM
)

add_custom_target( darts_print_server
                   COMMAND ar cr ${CMAKE_CURRENT_BINARY_DIR}/libdarts_print_server.a ${CMAKE_CURRENT_BINARY_DIR}/darts_print_server.o
                    DEPENDS darts_print_server.o darts_print_server.c)

add_custom_command(
   OUTPUT darts_api.o
   COMMAND gcc -c ${CMAKE_CURRENT_SOURCE_DIR}/darts_api.c ${EXTERNAL_FLAGS_API} -o darts_api.o
   DEPENDS darts_api.c
   COMMENT "CROSS-COMPILING HOST FILE darts_api"
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   VERBATIM
)

add_custom_target( darts_api
                   COMMAND ar cr ${CMAKE_CURRENT_BINARY_DIR}/libdarts_api.a ${CMAKE_CURRENT_BINARY_DIR}/darts_api.o
                   DEPENDS darts_api.o darts_api.c)
