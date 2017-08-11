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

set( edarts_codeletModel_src
     codelet.c
     threadedProcedure.c)
set( edarts_codeletModel_inc
     ${CMAKE_SOURCE_DIR}/eDARTS/include/codeletModel/codelet.h
     ${CMAKE_SOURCE_DIR}/eDARTS/include/codeletModel/threadedProcedure.h
     ${CMAKE_SOURCE_DIR}/eDARTS/include/codeletModel/tpClosure.h
     )

add_library( edarts_codeletModel ${edarts_codeletModel_src} ${edarts_codeletModel_inc})

set_target_properties(edarts_codeletModel PROPERTIES LINKER_LANGUAGE C)
set_target_properties(edarts_codeletModel PROPERTIES PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/eDARTS/include/codeletModel/codelet.h ${CMAKE_SOURCE_DIR}/eDARTS/include/codeletModel/threadedProcedure.h ${CMAKE_SOURCE_DIR}/eDARTS/include/codeletModel/tpClosure.h")

install(TARGETS edarts_codeletModel
        EXPORT edartsLibraryDepends
        ARCHIVE DESTINATION edarts/lib/             COMPONENT bin
        PUBLIC_HEADER DESTINATION edarts/include/   COMPONENT dev)