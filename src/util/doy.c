/* get the Julian day of the year for the given year, month, and day */
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
  int julian = 0; 
  int year, month, day;


  if ( argc != 4) {
    fprintf(stderr, "Usage: %s <year> <month> <day>\n", argv[0]);
    exit(1);
  }
  year = atoi(argv[1]);
  month = atoi(argv[2]);
  day = atoi(argv[3]);

  leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
  if (daytab[leap][month] < day) {
    fprintf(stderr, "There is no day %d in year %d month %d\n", day, year, month);
    exit(1);
  }
  
  for (i = 0; i < month; i++)
        julian += daytab[leap][i];
  julian += day;
  
  printf("%d\n",julian);
  return 0;
}
