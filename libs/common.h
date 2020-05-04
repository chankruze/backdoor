#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif

#define SERVER_IP ("192.168.43.31")
#define SERVER_PORT (50005)
#define CONN_AMNT (5)

/**
 * The bzero() function erases the data in the n bytes of the memory
   starting at the location pointed to by s, by writing zeros (bytes
   containing '\0') to that area.
**/
// port bzero in linux for windows (for ease of use)
#ifndef bzero
#define bzero(p, size) (void)memset((p), 0, (size))
#endif