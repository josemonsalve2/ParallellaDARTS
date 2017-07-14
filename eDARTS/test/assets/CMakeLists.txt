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

# Given that I am not able to use two compilers with cmake. I will
# use custom command and custom target to manually build this
if(DEBUG)
    set(EXTERNAL_FLAGS -I${EPIPHANY_HOME}/tools/host/include -I${CMAKE_SOURCE_DIR}/eDARTS/include -I${CMAKE_SOURCE_DIR}/eDARTS/include/runtime/host -L${CMAKE_BINARY_DIR}/eDARTS/src/runtime/host -ldarts_print_server -L${PTHREAD_LIB} -lpthread ${E_LOADER_LIB} ${E_HAL_LIB} -g)
else()
    set(EXTERNAL_FLAGS -I${EPIPHANY_HOME}/tools/host/include -I${CMAKE_SOURCE_DIR}/eDARTS/include -I${CMAKE_SOURCE_DIR}/eDARTS/include/runtime/host -L${CMAKE_BINARY_DIR}/eDARTS/src/runtime/host -ldarts_print_server ${E_LOADER_LIB} ${E_HAL_LIB})
endif()

### FOR CODELET QUEUE TESTS ###
# For the host we need to compile manually without cmake
add_custom_command(
   OUTPUT codeletQueue_test.out
   COMMAND gcc codeletQueue/main.c ${EXTERNAL_FLAGS} -o codeletQueue_test.out
   DEPENDS codeletQueue/main.c
   COMMENT "CROSS-COMPILING HOST FILE codeletQueue_test "
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
   VERBATIM
)

add_custom_target( codeletQueue_test
                   COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/codeletQueue_test.out ${CMAKE_CURRENT_BINARY_DIR}/codeletQueue_test
                    DEPENDS codeletQueue_test.out darts_print_server codeletQueue/main.c)

# For e_codQueue.elf
add_executable(e_codQueue.elf codeletQueue/e_codQueue.c)
target_link_libraries(e_codQueue.elf edarts_assets ${EPIPHANY_LIBS})

add_dependencies(e_codQueue.elf codeletQueue_test darts_print_server edarts_cachemanager)

### FOR SYNC SLOTS TESTS ###
# For the host we need to compile manually without cmake
add_custom_command(
   OUTPUT syncSlot_test.out
   COMMAND gcc syncSlot/main.c ${EXTERNAL_FLAGS} -o syncSlot_test.out
   DEPENDS syncSlot/main.c
   COMMENT "CROSS-COMPILING HOST FILE syncSlot_test "
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
   VERBATIM
)

add_custom_target( syncSlot_test
                   COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/syncSlot_test.out ${CMAKE_CURRENT_BINARY_DIR}/syncSlot_test
                    DEPENDS syncSlot_test.out darts_print_server syncSlot/main.c)

# For e_codQueue.elf
add_executable(e_syncSlot_test.elf syncSlot/e_syncSlot_test.c)
target_link_libraries(e_syncSlot_test.elf edarts_codeletModel edarts_assets edarts_runtime_device edarts_multithreading ${EPIPHANY_LIBS})

add_dependencies(e_syncSlot_test.elf syncSlot_test darts_print_server edarts_cachemanager)

