/*Set month, day from day of year */

#include <stdlib.h>
#include <stdio.h>
int main (int argc, char *argv[])
{
  static char daytab[2][13] = {
       {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, 
       {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}  
  };
  int i;
  int leap;
  int year, month, doy, day;


  if ( argc != 3) {
    fprintf(stderr, "Usage: %s <year> <doy>\n", argv[0]);
    exit(1);
  }
  year = atoi(argv[1]);
  doy = atoi(argv[2]);

  leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;

  if (leap == 1) {
    if (doy > 366) {
      fprintf(stderr, "There are 366 days in %d\n", year);
      exit(1);
   }
  }
  else {
    if (doy > 365) {
      fprintf(stderr, "There are 365 days in %d\n", year);
      exit(1);
   }
  }


  for (i = 1; doy > daytab[leap][i]; i++)
      doy -= daytab[leap][i];
  month = i;
  day = doy;

  printf("%3d%3d\n",month, day);
  return 0;
}
