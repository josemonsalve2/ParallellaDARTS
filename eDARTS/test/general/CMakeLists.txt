set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -T ${ELDF_EDARTS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${ELDF_EDARTS} ${CMAKE_BINARY_DIR}/eDARTS/src/softcache/cachemanager.o")


# Given that I am not able to use two compilers with cmake. I will
# use custom command and custom target to manually build this
if(DEBUG)
    set(EXTERNAL_FLAGS -I${EPIPHANY_HOME}/tools/host/include -I${CMAKE_SOURCE_DIR}/eDARTS/include -I${CMAKE_SOURCE_DIR}/eDARTS/include/runtime/host -L${CMAKE_BINARY_DIR}/eDARTS/src/runtime/host -ldarts_api -ldarts_print_server -L${PTHREAD_LIB} -lpthread ${E_LOADER_LIB} ${E_HAL_LIB} -g)
else()
    set(EXTERNAL_FLAGS -I${EPIPHANY_HOME}/tools/host/include -I${CMAKE_SOURCE_DIR}/eDARTS/include -I${CMAKE_SOURCE_DIR}/eDARTS/include/runtime/host -L${CMAKE_BINARY_DIR}/eDARTS/src/runtime/host -ldarts_api -ldarts_print_server -L${PTHREAD_LIB} -lpthread ${E_LOADER_LIB} ${E_HAL_LIB})
endif()


# --------- For e_darts_fib_test.elf ---------
add_executable(e_darts_fib_test.elf fib/e_darts_fib_test.c)
target_link_libraries(e_darts_fib_test.elf edarts_runtime_device edarts_codeletModel edarts_assets edarts_multithreading ${EPIPHANY_LIBS})
add_dependencies(e_darts_fib_test.elf darts_fib_test edarts_runtime_device darts_api edarts_cachemanager)

add_custom_target( darts_fib_test
                   COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/darts_fib_test.out ${CMAKE_CURRENT_BINARY_DIR}/darts_fib_test
                    DEPENDS darts_fib_test.out darts_api fib/main.c)

add_custom_command(
   OUTPUT darts_fib_test.out
   COMMAND gcc fib/main.c ${EXTERNAL_FLAGS} -o darts_fib_test.out
   DEPENDS fib/main.c
   COMMENT "CROSS-COMPILING HOST FILE darts_fib_test.out"
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
   VERBATIM
)
                    
# --------- For e_darts_vector_add_test.elf ---------
add_executable(e_darts_vector_add_test.elf vector_add/e_darts_vector_add_test.c)
target_link_libraries(e_darts_vector_add_test.elf edarts_runtime_device edarts_codeletModel edarts_assets edarts_multithreading ${EPIPHANY_LIBS})
add_dependencies(e_darts_vector_add_test.elf darts_vector_add_test darts_api edarts_cachemanager)

add_custom_target( darts_vector_add_test
                  COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/darts_vector_add_test.out ${CMAKE_CURRENT_BINARY_DIR}/darts_vector_add_test
                  DEPENDS darts_vector_add_test.out darts_api vector_add/main.c)

add_custom_command(
   OUTPUT darts_vector_add_test.out
   COMMAND gcc vector_add/main.c ${EXTERNAL_FLAGS} -o darts_vector_add_test.out
   DEPENDS vector_add/main.c
   COMMENT "CROSS-COMPILING HOST FILE darts_vector_add_test.out"
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
   VERBATIM
)

# --------- For e_darts_vector_dotProduct_test.elf ---------
add_executable(e_darts_vector_dotProduct_test.elf vector_dotProduct/e_darts_vector_dotProduct_test.c)
target_link_libraries(e_darts_vector_dotProduct_test.elf edarts_runtime_device edarts_codeletModel edarts_assets edarts_multithreading ${EPIPHANY_LIBS})
add_dependencies(e_darts_vector_dotProduct_test.elf darts_vector_dotProduct_test darts_api edarts_cachemanager)

add_custom_target( darts_vector_dotProduct_test
                  COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/darts_vector_dotProduct_test.out ${CMAKE_CURRENT_BINARY_DIR}/darts_vector_dotProduct_test
                  DEPENDS darts_vector_dotProduct_test.out darts_api vector_dotProduct/main.c)

add_custom_command(
   OUTPUT darts_vector_dotProduct_test.out
   COMMAND gcc vector_dotProduct/main.c ${EXTERNAL_FLAGS} -o darts_vector_dotProduct_test.out
   DEPENDS vector_dotProduct/main.c
   COMMENT "CROSS-COMPILING HOST FILE darts_vector_dotProduct_test.out"
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
   VERBATIM
)

# --------- For e_darts_matrix_multiply_test.elf ---------
add_executable(e_darts_matrix_multiply_test.elf matrix_multiply/e_darts_matrix_multiply_test.c)
target_link_libraries(e_darts_matrix_multiply_test.elf edarts_runtime_device edarts_codeletModel edarts_assets edarts_multithreading ${EPIPHANY_LIBS})
add_dependencies(e_darts_matrix_multiply_test.elf darts_matrix_multiply_test edarts_runtime_device darts_api edarts_cachemanager)

add_custom_target( darts_matrix_multiply_test
                  COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/darts_matrix_multiply_test.out ${CMAKE_CURRENT_BINARY_DIR}/darts_matrix_multiply_test
                  DEPENDS darts_matrix_multiply_test.out darts_api matrix_multiply/main.c)

add_custom_command(
   OUTPUT darts_matrix_multiply_test.out
   COMMAND gcc matrix_multiply/main.c ${EXTERNAL_FLAGS} -o darts_matrix_multiply_test.out
   DEPENDS matrix_multiply/main.c
   COMMENT "CROSS-COMPILING HOST FILE darts_matrix_multiply_test.out"
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
   VERBATIM
)