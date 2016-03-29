#include "stdlib.h"
#include "string.h"
#include "weld.h"

uint32 uint32_0 = (uint32)0;
int16  int16_minus32768 = -32768;
int16  int16_minus2000 = -2000;
int16  int16_32767 = 32767;
uint8  uint8_255 = 255;
uint8  uint8_0 = 0;
uint16 uint16_65535 = 65535;
uint16 uint16_0 = 0;
int16  int16_0 = 0;


/*******************************************************************
 	Make a duplicate of s.
	Source: K&R 89, p143.
*/
char *mystrdup(char *s) 
{
	char *p = NULL;
	
	p = (char*)malloc(strlen(s)+1);  	/* +1 for '\0' */
	if (p != NULL) 
		strcpy(p,s);
	return p;
}


/********************************************************************
 	Get the day of the year for the given year, month, and day.
	Kernighan & Ritchie 89, p111.
********************************************************************/
int day_of_year(int year, int month, int day)
{
	static char daytab[2][13] = {
				      {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
				      {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
				    };
	int i;
	int leap;
	int doy = 0;

	leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
	for (i = 0; i < month; i++)
      		doy += daytab[leap][i];
	doy += day;
	return doy;
}



/****************************************************************
    	Save the double number as int16. Check for overflow and underflow.
	If overflow/underflow, store the max/min allowed (i.e., nonfill) int16 number.
****************************************************************/
int16 asInt16(double x)
{
    int16 res;

    if (x >= int16_32767)
        res = int16_32767;
    else if (x <= int16_minus32768+1)
        res = int16_minus32768+1;
    else
        res = (int16)(x+0.5);

    return res;
}
