set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -T ${ELDF_EDARTS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${ELDF_EDARTS} ${CMAKE_BINARY_DIR}/eDARTS/src/softcache/cachemanager.o")


# Given that I am not able to use two compilers with cmake. I will
# use custom command and custom target to manually build this
if(DEBUG)
    set(EXTERNAL_FLAGS -I${EPIPHANY_HOME}/tools/host/include -I${CMAKE_SOURCE_DIR}/eDARTS/include -I${CMAKE_SOURCE_DIR}/eDARTS/include/runtime/host -L${CMAKE_BINARY_DIR}/eDARTS/src/runtime/host -ldarts_api -ldarts_print_server -L${PTHREAD_LIB} -lpthread ${E_LOADER_LIB} ${E_HAL_LIB} -g)
else()
    set(EXTERNAL_FLAGS -I${EPIPHANY_HOME}/tools/host/include -I${CMAKE_SOURCE_DIR}/eDARTS/include -I${CMAKE_SOURCE_DIR}/eDARTS/include/runtime/host -L${CMAKE_BINARY_DIR}/eDARTS/src/runtime/host -ldarts_api -ldarts_print_server -L${PTHREAD_LIB} -lpthread ${E_LOADER_LIB} ${E_HAL_LIB})
endif()

# For the host we need to compile manually without cmake
add_custom_command(
   OUTPUT darts_api_test.out
   COMMAND gcc basic_signal_test/main.c ${EXTERNAL_FLAGS} -o darts_api_test.out
   DEPENDS basic_signal_test/main.c
   COMMENT "CROSS-COMPILING HOST FILE darts_api_test "
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
   VERBATIM
)

add_custom_target( darts_api_test
                   COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/darts_api_test.out ${CMAKE_CURRENT_BINARY_DIR}/darts_api_test
                    DEPENDS darts_api_test.out darts_api basic_signal_test/main.c)

# For e_darts_mailbox_test.elf
add_executable(e_darts_mailbox_test.elf basic_signal_test/e_darts_mailbox_test.c)
target_link_libraries(e_darts_mailbox_test.elf edarts_runtime_device edarts_codeletModel edarts_assets edarts_multithreading ${EPIPHANY_LIBS})

add_dependencies(e_darts_mailbox_test.elf darts_api_test darts_api edarts_cachemanager)
