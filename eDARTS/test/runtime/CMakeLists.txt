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

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -T ${ELDF_EDARTS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${ELDF_EDARTS} ${CMAKE_BINARY_DIR}/eDARTS/src/softcache/cachemanager.o")


# Given that I am not able to use two compilers with cmake. I will
# use custom command and custom target to manually build this
if(DEBUG)
    set(EXTERNAL_FLAGS -I${EPIPHANY_HOME}/tools/host/include -I${CMAKE_SOURCE_DIR}/eDARTS/include ${E_LOADER_LIB} -I${CMAKE_SOURCE_DIR}/eDARTS/include/runtime/host -L${CMAKE_BINARY_DIR}/eDARTS/src/runtime/host -ldarts_api -ldarts_print_server -L${PTHREAD_LIB} -lpthread ${E_HAL_LIB} -g)
else()
    set(EXTERNAL_FLAGS -I${EPIPHANY_HOME}/tools/host/include -I${CMAKE_SOURCE_DIR}/eDARTS/include -I${CMAKE_SOURCE_DIR}/eDARTS/include/runtime/host ${E_LOADER_LIB} -L${CMAKE_BINARY_DIR}/eDARTS/src/runtime/host -ldarts_api -ldarts_print_server -L${PTHREAD_LIB} -lpthread ${E_HAL_LIB})
endif()

# For the host we need to compile manually without cmake
add_custom_command(
   OUTPUT darts_print_test.out
   COMMAND gcc e_darts_print/main.c ${EXTERNAL_FLAGS} -o darts_print_test.out
   DEPENDS e_darts_print/main.c darts_api
   COMMENT "CROSS-COMPILING HOST FILE darts_print_test "
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
   VERBATIM
)

add_custom_target( darts_print_test
                   COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/darts_print_test.out ${CMAKE_CURRENT_BINARY_DIR}/darts_print_test
                    DEPENDS darts_print_test.out darts_api darts_print_server e_darts_print/main.c)

# For e_darts_print_test.elf
add_executable(e_darts_print_test.elf e_darts_print/e_darts_print_test.c)
target_link_libraries(e_darts_print_test.elf edarts_runtime_device edarts_multithreading ${EPIPHANY_LIBS})

add_dependencies(e_darts_print_test.elf darts_print_test darts_print_server edarts_cachemanager)


# FOR e_darts_rt_test
# For the host we need to compile manually without cmake
add_custom_command(
   OUTPUT darts_rt_test.out
   COMMAND gcc e_darts_rt/main.c ${EXTERNAL_FLAGS} -o darts_rt_test.out
   DEPENDS e_darts_rt/main.c
   COMMENT "CROSS-COMPILING HOST FILE darts_rt_test "
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
   VERBATIM
)

add_custom_target( darts_rt_test
                   COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/darts_rt_test.out ${CMAKE_CURRENT_BINARY_DIR}/darts_rt_test
                    DEPENDS darts_rt_test.out e_darts_rt/main.c)

# For e_darts_rt_test.elf
add_executable(e_darts_rt_test.elf e_darts_rt/e_darts_rt_test.c)
target_link_libraries(e_darts_rt_test.elf edarts_runtime_device edarts_codeletModel edarts_assets edarts_multithreading ${EPIPHANY_LIBS})

add_dependencies(e_darts_rt_test.elf darts_rt_test edarts_cachemanager)
