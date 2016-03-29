/*
!C****************************************************************************

!File: error.h
  
!Description: Header file for error.c - see error.c for more information. 

  
! Design Notes:
   1. These macros automatically generates the source file name, line number 
      and return status parameters for the 'Error' function.

!END****************************************************************************


Junchang Ju
December 15, 2010
borrowed from LEDAPS.

*/

#ifndef ERROR_H
#define ERROR_H

#define MSGLEN 256

/*  Name ERROR is also used by GCTP, so start to use Error instead.  April 26, 2011. */
#define Error(message, module, status) \
          _Error_((message), (module), (__FILE__), (long)(__LINE__), (status))

void _Error_(const char *message, const char *module, 
           const char *source, long line, int status);

#endif
