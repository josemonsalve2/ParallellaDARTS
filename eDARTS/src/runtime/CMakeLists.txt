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
add_subdirectory(host)
add_subdirectory(device)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -T ${ELDF_EDARTS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${ELDF_EDARTS} ${CMAKE_BINARY_DIR}/eDARTS/src/softcache/cachemanager.o")


# Given that I am not able to use two compilers with cmake. I will
# use custom command and custom target to manually build this
if(DEBUG)
    set(EXTERNAL_FLAGS -I${EPIPHANY_HOME}/tools/host/include -I${CMAKE_SOURCE_DIR}/eDARTS/include ${E_LOADER_LIB} -I${CMAKE_SOURCE_DIR}/eDARTS/include/runtime/host -L${CMAKE_BINARY_DIR}/eDARTS/src/runtime/host -ldarts_print_server -ldarts_api -L${PTHREAD_LIB} -lpthread ${E_HAL_LIB} -g)
else()
    set(EXTERNAL_FLAGS -I${EPIPHANY_HOME}/tools/host/include -I${CMAKE_SOURCE_DIR}/eDARTS/include -I${CMAKE_SOURCE_DIR}/eDARTS/include/runtime/host ${E_LOADER_LIB} -L${CMAKE_BINARY_DIR}/eDARTS/src/runtime/host -ldarts_print_server -ldarts_api -L${PTHREAD_LIB} -lpthread ${E_HAL_LIB})
endif()

# if for block comment for now because not in use
if(FALSE)
# For the host we need to compile manually without cmake
add_custom_command(
   OUTPUT rt_main.out
   COMMAND gcc rt_main.c ${EXTERNAL_FLAGS} -o rt_main.out
   DEPENDS rt_main.c
   COMMENT "CROSS-COMPILING HOST FILE darts_print_test "
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
   VERBATIM
)

add_custom_target( rt_main
                   COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/rt_main.out ${CMAKE_CURRENT_BINARY_DIR}/rt_main
		   DEPENDS rt_main.out rt_main rt_main.c darts_print_server darts_api)

# For e_darts_rt_init.elf
add_executable(e_darts_rt_init.elf e_DARTS_init.c)
target_link_libraries(e_darts_rt_init.elf edarts_runtime_device edarts_codeletModel edarts_assets edarts_multithreading ${EPIPHANY_LIBS})

add_dependencies(e_darts_rt_init.elf rt_main darts_print_server edarts_cachemanager)
endif()

