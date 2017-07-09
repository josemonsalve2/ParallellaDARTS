/**
 * @file e_darts_print_server.h
 * @author Jose M Monsalve
 * @date 8 July 2017
 * @brief Listener printer server for the host to print from the device
 * @todo Add copyright
 * @todo add printing of variables
 *
 * This will spawn a PThread that will be looping over the printing buffer that 
 * is created on the device. After filling the print buffer, the device will 
 * set a flag to 1 when it wants to print something. The host will restart the
 * flag to 0 after printing the buffer
 */

/**
 * @brief wrapper for the pthred initialization
 * 
 * starts the printing server and comes back 
 *
 * @returns Error by pthreads library 
 */
int start_printing_server();


/**
 * @brief The actual printing function
 * 
 * This function is async.
 * 
 */

void printing_server();