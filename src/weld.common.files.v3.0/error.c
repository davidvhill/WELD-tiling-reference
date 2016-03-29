/*
!C****************************************************************************

!File: error.c
  
!Description: Function for handling errors.

 ! Design Notes:
   1. See 'error.h' for information on the 'ERROR' macro 
      that automatically populates the source code file name, line number and 
      exit flag.  

!END****************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "error.h"

void _Error_(const char *message, const char *module, 
           const char *source, long line, int status)
/* 
!C******************************************************************************

!Description: 'Error' writes an error message to 'stderr' and optionally 
  exit's the program with a 'EXIT_FAILURE' status.
 
!Input Parameters:
 message        error message
 module         calling module name
 source         source code file name containing the calling module
 line           line number in the source code file
 log_code       a predefined code that roughly indicates the type of error 

!Output Parameters: (none)
 (returns)      (void)

!END****************************************************************************
*/
{
    /*  A status code is printed to stderr only inside a terminal function where
        the error originates. The status code is not printed in the caller 
        and this situation is indicated by status being 0.
    */
    if (status == 0)
        fprintf(stderr, "\t%s, in function %s, [%s: line %ld]\n", message, module, source, line);
    else
        fprintf(stderr, "%d\t%s, in function %s, [%s: line %ld]\n", status, message, module, source, line);

    return;
}
