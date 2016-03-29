/*
 * weld.comp.util.c
 *
 *  Created on: Nov 11, 2014
 *      Author: valeriy.kovalskyy
 */

#include "weld.comp.util.h"



/*********************************************************************************
* Name:	asign_tileHV()
* Purpose: parse the tile id from tile_id string
*
* Args:
*	  @tile_id		- tile_id string
*	  @tile_H 		- horizontal tile position
*	  @tile_V 		- Verticall tile position
*	  @region  		-region or code of tiling system
*	  @message      -messaga to pass in case of error
*
* Calls: main();
*********************************************************************************/
int asign_tileHV(char *tile_id, int* tileH, int* tileV, char *region, char *message){

	int status=0;
	char * chpos;
	int modis_tile_h, modis_tile_v, mini_tile_h, mini_tile_v;
	/* tile ID */
    /* Example tile ID:  hh03vv05.h4v6 */


	if (strcmp(region, "WORLD") == 0) {

		chpos = strstr(tile_id, "hh");
		if (chpos == NULL) {
			sprintf(message, "tile id %s does not contain string patter hh", tile_id);
			status=1;
			Error(message, "main", status);
			return status;
		}
		modis_tile_h = atoi(chpos+2);

		chpos = strstr(tile_id, "vv");
		if (chpos == NULL) {
			sprintf(message, "tile id %s does not contain string patter vv", tile_id);
			status=1;
			Error(message, "main", status);
			return status;
		}
		modis_tile_v = atoi(chpos+2);

		chpos = strstr(chpos, "h");
		if (chpos == NULL) {
			sprintf(message, "tile id %s does not contain mini-tile string patter h", tile_id);
			status=1;
			Error(message, "main", status);
			return status;
		}
		mini_tile_h  = atoi(chpos+1);

		chpos = strstr(chpos, "v");
		if (chpos == NULL) {
			sprintf(message, "tile id %s does not contain mini-tile string patter v", tile_id);
			status=1;
			Error(message, "main", status);
			return status;
		}
		mini_tile_v  = atoi(chpos+1);

		*tileH = modis_tile_h * NMINI + mini_tile_h;
		*tileV = modis_tile_v * NMINI + mini_tile_v;
	}else{
		chpos = strstr(tile_id, "h");
		if (chpos == NULL) {
			sprintf(message, "tile id %s does not contain tile string patter h", tile_id);
			status=1;
			Error(message, "main", status);
			return status;
		}
		mini_tile_h  = atoi(chpos+1);

		chpos = strstr(chpos, "v");
		if (chpos == NULL) {
			sprintf(message, "tile id %s does not contain tile string patter v", tile_id);
			status=1;
			Error(message, "main", status);
			return status;
		}
		mini_tile_v  = atoi(chpos+1);

		*tileH = mini_tile_h;
		*tileV = mini_tile_v;

	}

	return status;
}


/*********************************************************************************/
/*
	For a given landsat tile derive each pixel's UTM coordinates of the given zone
	by triangulation interpolation.


 Parameters::
	  int	   utm_zone:
	  float32   pixel_size:   30.0 meters in this project.
	  int	   tiledim_nrow: tile dimension in number of rows,  (5000, defined in the code header file)
	  int	   tiledim_ncol: tile dimension in number of columns, (5000, defined in the code header file)
	  //int	   first_row_albers: the row ID in the Albers system for the first row of the tile
	  //int	   first_col_albers: the column ID in the Albers system for the first column of the tile
	  int	   ih
	  int	   iv
	  int	   spacing:  the spacing of the grid in number of pixels for triangulation, defined in the code header file.
	  double	*utmx, *utmy:  the UTM X and Y for each pixel on the tile.



	APRIL 8, 2010:
	Note: this is changed after world processing experience. No long use first_row_albers
		and first_col_albers as parameters.
 */
/*********************************************************************************/
void interpolate_utm_xy_albers(albers_proj_para_t albers_para,
			int utm_zone,
			float32 pixel_size,
			int tiledim_nrow,
			int tiledim_ncol,
			int tile_h,
			int tile_v,
			int spacing,
			double *utm_x,
			double *utm_y)
{

 clock_t t1, t2;

 simplicial_t lower_triang, upper_triang;

 void compute_simplicial_abc(double x1, double y1, double x2, double y2, double x3, double y3, simplicial_t *triang);
 bool isInTriang(simplicial_t triang, double x, double y, double *s1, double *s2, double *s3);

 int lastRegRow, lastRegCol;   /*The last row/col on the reguarly spaced grid*/
 int nRegBlock_h, nRegBlock_v, iblock_h, iblock_v;   /*blocks on the grid*/
 int base;			   /*temporary varible to save computation*/

 double x1, y1, x2, y2, x3, y3;	/*coordinates of the triangle in the Albers system*/
 double s1, s2, s3;				/*three variables to characterize a point in a triangle*/
 double ux1[2], uy1[2], ux2[2], uy2[2], ux3[2], uy3[2];   /*UTM coordinates of the vertices of the two triangles*/
 int temp, temp1, temp2, temp3;
 int nrow_process, ncol_process;

 int irow, icol;
 double ax, ay, ux, uy;

	int first_row_albers = tile_v * tiledim_nrow;
	int first_col_albers = tile_h * tiledim_ncol;

 //fprintf(stderr, "zone = %d, pixsize = %f, nrow = %d, ncol = %d, first_row = %d, first_col = %d, spacing = %d\n",
 //				 utm_zone, pixel_size, tiledim_nrow, tiledim_ncol, first_row_albers, first_col_albers, spacing);

 /*A regular grid with equal spacing both vertically and horizontally is set up, and each grid cell is divided into
   two triangles for triangulation. For the rows at the bottom or the columns on the right of the tile that are not
   covered by the regular grid, triangles of different sizes are used.

   In all cases, a square or rectangle of pixels are divided in NW-SE direction into two triangles.
   For each regular grid cell only two types of triangles are needed. For the rows and column that are not covered the
   regular grid, another different triangles are used in interpolation.

   This function is lengthly. The idea was rather that within the loops use "if" statement for every pixel to decide
   how the pixel should be treated, the "if" is taken out of the loops in the hopes of increasing the efficiency, but
   at the apparent price of the length of the code.
 */


 /***Set up the regularly spaced grid for interpolating. First, project the pixels that are regularly spaced every
	 "spacing" rows and columns in the Albers coordinate system to UTM, starting with the 0th row and 0th column.
	 And then if the last row (tiledim_nrow-1) or last column (tiledim_ncol-1) on the tile is not integer multiples
	 of the "spacing", project the last row or column too.  Make sure the lower-right pixel--the very last pixel
	 on the regular grid is projected too.
	 ***/

 t1 = clock();

 /*************************************************************************************************
	CALL GCTP TO CALCULATE THE UTM COORDINATES FOR THE PIXELS FROM WHICH THE UTM COORDINATES OF
	OTHER PIXELS WILL BE INTERPOLATED.
  *************************************************************************************************
 */

 /*Project the regular grid with columns and rows spaced "spacing" pixels.
   Keep in mind an easy mistake: Suppose there are 10 rows and 10 column and the grid lines are
   spaced 5 pixels, then the last row and last column are actually not on the regular grid!
  */
 for (irow = 0; irow < tiledim_nrow; irow+=spacing) {
   ay = albers_para.Albers_ULY - (irow + first_row_albers + 0.5) * pixel_size;   /* pixel center */
   base = irow * tiledim_ncol;
   for (icol = 0; icol < tiledim_ncol; icol+=spacing) {
	 ax = albers_para.Albers_ULX + (icol + first_col_albers + 0.5) * pixel_size;  /* pixel center */
	 albers2utm(albers_para, ax, ay, utm_zone, &ux, &uy);
	 utm_x[base + icol] = ux;
	 utm_y[base + icol] = uy;
   }
 }

 /*Project the last row regularly every "spacing" pixels horizontally if the last row is not on the regular grid*/
 lastRegRow = (tiledim_nrow-1) - (tiledim_nrow-1) % spacing;  /*Last row on the regularly spaced grid*/
 if (tiledim_nrow-1 > lastRegRow) {
   base = (tiledim_nrow-1) * tiledim_ncol;
   ay = albers_para.Albers_ULY - ((tiledim_nrow-1) + first_row_albers + 0.5) * pixel_size;   /* pixel center */
   for (icol = 0; icol < tiledim_ncol; icol+=spacing) {
	 ax = albers_para.Albers_ULX + (icol + first_col_albers + 0.5) * pixel_size;  /* pixel center */
	 albers2utm(albers_para, ax, ay, utm_zone, &ux, &uy);
	 utm_x[base + icol] = ux;
	 utm_y[base + icol] = uy;
   }
 }

 /*Project the last col regularly every "spacing" pixels vertically if the last column is not on the regular grid*/
 lastRegCol = (tiledim_ncol-1) - (tiledim_ncol-1) % spacing;  /*Last row on the regularly spaced grid*/
 if (tiledim_ncol-1  > lastRegCol) {
   ax = albers_para.Albers_ULX + ((tiledim_ncol-1) + first_col_albers + 0.5) * pixel_size;  /* pixel center */
   for (irow = 0; irow < tiledim_nrow; irow+=spacing) {
	 ay = albers_para.Albers_ULY - (irow + first_row_albers + 0.5) * pixel_size;   /* pixel center */
	 albers2utm(albers_para, ax, ay, utm_zone, &ux, &uy);
	 utm_x[(irow+1) * tiledim_ncol -1] = ux;
	 utm_y[(irow+1) * tiledim_ncol -1] = uy;
   }
 }

 /*Make sure that the single, lower-right corner  pixel on the tile is projected too*/
 if (tiledim_nrow-1 > lastRegRow && tiledim_ncol-1  > lastRegCol) {
   ax = albers_para.Albers_ULX + ((tiledim_ncol-1) + first_col_albers + 0.5) * pixel_size;   /* pixel center */
   ay = albers_para.Albers_ULY - ((tiledim_nrow-1) + first_row_albers + 0.5) * pixel_size;   /* pixel center */
   albers2utm(albers_para, ax, ay, utm_zone, &ux, &uy);
   utm_x[tiledim_nrow * tiledim_ncol -1] = ux;
   utm_y[tiledim_nrow * tiledim_ncol -1] = uy;
 }


 t2 = clock();
 //fprintf(stdout, "Time to set up the grid : %d (seconds)\n", (int)((t2-t1)/CLOCKS_PER_SEC));


 /*No more GCTP from now on. Triangulation begins*/
 t1 = clock();
 /*************************************************************************************************
	HANDLE THE PIXELS ON THE REGULAR GRID
  *************************************************************************************************
 */

 /*The number of blocks enclosed horizontally or vertically by the lines or column
   on the regularly spaced grid. BE CAREFUL, IT IS NOT tiledim_ncol/spacing.
  */
 nRegBlock_h = (tiledim_ncol-1)/spacing;
 nRegBlock_v = (tiledim_nrow-1)/spacing;

 /*Divide each grid cell in the NW-SE direction into two triangles and
   compute the three parameters "abc"  for the resulting lower and upper triangle comprising a cell on
   the regular grid.  Only need do it once since all the lower triangles and all the uppper
   triangles in the grid have the same topology. Just use the relative row and column number as
   the proxy coordinates in Albers system, but no need to use Albers coordinates*/

 /*Lower triangle with a single top vertice*/
 /*The relative row/col numbers are used for the coordinates in the Albers , instead of Albers coordinates*/
 /*top vertex*/
 x1 = 0;  y1 = 0;

 /*lower-left vertex*/
 x2 = x1; y2 = spacing;

 /*lower-right vertex*/
 x3 = spacing; y3 = y2;

 compute_simplicial_abc(x1, y1, x2, y2, x3, y3, &lower_triang);

 /*Upper triangle with a single bottom vertice.
   upper-left vertex is the same as x1, y1, and bottom vertex is the same as x3,y3*/
 /*upper-right vertex*/
 x2 = x3; y2 = y1;

 compute_simplicial_abc(x1, y1, x2, y2, x3, y3, &upper_triang);

 /*The pixles on the regularly spaced blocks in the  grid*/
 for (iblock_h = 0; iblock_h < nRegBlock_h; iblock_h++) {
   for (iblock_v = 0; iblock_v < nRegBlock_v; iblock_v++) {

	  /*****UTM coordinates for the lower triangle*****/
	  /*The top vertice of the lower triangle*/
	  temp1 = (iblock_v*spacing)*tiledim_ncol + iblock_h*spacing;
	  ux1[0] = utm_x[temp1];  uy1[0] = utm_y[temp1];

	  /*lower-left vertice of the lower triangle*/
	  temp2 = temp1 + spacing * tiledim_ncol;
	  ux2[0] = utm_x[temp2];  uy2[0] = utm_y[temp2];

	  /*lower-right vertice of the lower triangle*/
	  temp3 = temp2 + spacing;
	  ux3[0] = utm_x[temp3];  uy3[0] = utm_y[temp3];

	  /*****UTM coordinates for the upper triangle*****/
	  /*upper-left vertice of the upper triangle*/
	  ux1[1] = ux1[0];  uy1[1] = uy1[0];

	  /*upper-right vertice of the upper triangle*/
	  temp2 = temp1 + spacing;
	  ux2[1] = utm_x[temp2];  uy2[1] = utm_y[temp2];

	  /*Bottom vertice of the upper triangle*/
	  ux3[1] = ux3[0];  uy3[1] = uy3[0];

	  if (iblock_v == nRegBlock_v-1 && lastRegRow == tiledim_nrow-1)
		 /*If all the rows are perfectly covered the regular grid, interpolate the last row of the tile too.*/
		 nrow_process = spacing+1;
	  else
		 /*Otherwise, the last row will be processed later*/
		 nrow_process = spacing;

	  if (iblock_h == nRegBlock_h-1 && lastRegCol == tiledim_ncol-1)
		 /*If all the columns are perfectly covered the regular grid, interpolate the last column of the tile too.*/
		 ncol_process = spacing+1;
	  else
		 /*Otherwise, the last column will be processed later*/
		 ncol_process = spacing;

	  for (irow = 0; irow < nrow_process; irow++) {
		 base =  (iblock_v*spacing + irow) * tiledim_ncol + iblock_h*spacing;
		 for (icol = 0; icol < ncol_process; icol++) {
		   if (isInTriang(lower_triang, icol, irow, &s1, &s2, &s3)) {
			  ux = s1 * ux1[0] + s2 * ux2[0] + s3 * ux3[0];
			  uy = s1 * uy1[0] + s2 * uy2[0] + s3 * uy3[0];
		   }
		   else if (isInTriang(upper_triang, icol, irow, &s1, &s2, &s3)) {
			  ux = s1 * ux1[1] + s2 * ux2[1] + s3 * ux3[1];
			  uy = s1 * uy1[1] + s2 * uy2[1] + s3 * uy3[1];
		   }
		   else {
			 fprintf(stderr, "Something is wrong. nRegBlock_h = %d, iblock_h = %d, irow = %d, icol = %d.\n",
							  nRegBlock_h, iblock_h, irow, icol);
			 exit(1);
		   }
		   utm_x[base+icol] = ux;
		   utm_y[base+icol] = uy;
		 }
	  }
   }
 }

 /*************************************************************************************************
	HANDLE THE PIXELS IN THE ROWS FOLLOWING THE REGULAR GRID
  *************************************************************************************************
 */
 if (tiledim_nrow-1 > lastRegRow) {
	/*Lower triangle. The relative coordinates in the Albers system*/
	x1 = 0;   y1 = 0;							   /*top vertice*/
	x2 = x1;  y2 = (tiledim_nrow-1) % spacing;	  /*lower-left vertice*/
	x3 = spacing; y3 = y2;						  /*lower-right vertice*/
	compute_simplicial_abc(x1, y1, x2, y2, x3, y3, &lower_triang);

	/*Upper triangle.*/
	/*x1,y1 is upper-left vertice and stay the same, and x3,y3 is the bottom vertice and stays the same*/
	x2 = x3;  y2 = y1;							 /*upper-right triangle*/
	compute_simplicial_abc(x1, y1, x2, y2, x3, y3, &upper_triang);

	for (iblock_h = 0; iblock_h < nRegBlock_h; iblock_h++) {

	  /*****UTM coordinates for the lower triangle*****/
	  /*top vertice of the lower triangle*/
	  temp = (nRegBlock_v*spacing)*tiledim_ncol + iblock_h*spacing;
	  ux1[0] = utm_x[temp];  uy1[0] = utm_y[temp];

	  /*lower-left vertice of the lower triangle*/
	  temp = (tiledim_nrow-1)*tiledim_ncol + iblock_h*spacing;
	  ux2[0] = utm_x[temp];  uy2[0] = utm_y[temp];

	  /*lower-right vertice of the lower triangle*/
	  temp = (tiledim_nrow-1)*tiledim_ncol + (iblock_h+1)*spacing;
	  ux3[0] = utm_x[temp];  uy3[0] = utm_y[temp];

	  /*UTM coordinates for the upper triangle*/
	  /*upper-left vertice of the upper triangle*/
	  ux1[1] = ux1[0];  uy1[1] = uy1[0];

	  /*upper-right vertice of the upper triangle*/
	  temp = (nRegBlock_v*spacing)*tiledim_ncol + (iblock_h+1)*spacing;
	  ux2[1] = utm_x[temp];  uy2[1] = utm_y[temp];

	  /*bottom vertice of the upper triangle*/
	  ux3[1] = ux3[0];  uy3[1] = uy3[0];


	  /*If the last column in on the regular grid, interpolate it as well*/
	  if (iblock_h == nRegBlock_h-1 && lastRegCol == tiledim_ncol-1)
		  ncol_process = spacing+1;
	  else
		  ncol_process = spacing;

	  for (irow = 0; irow <= (tiledim_nrow-1)%spacing; irow++) {	/*Note the equal sign to include the last row*/
		 base = (nRegBlock_v*spacing + irow) * tiledim_ncol + iblock_h*spacing;
		 for (icol = 0; icol < ncol_process; icol++) {
		   if (isInTriang(lower_triang, icol, irow, &s1, &s2, &s3)) {
			  ux = s1 * ux1[0] + s2 * ux2[0] + s3 * ux3[0];
			  uy = s1 * uy1[0] + s2 * uy2[0] + s3 * uy3[0];
		   }
		   else if (isInTriang(upper_triang, icol, irow, &s1, &s2, &s3)) {
			  ux = s1 * ux1[1] + s2 * ux2[1] + s3 * ux3[1];
			  uy = s1 * uy1[1] + s2 * uy2[1] + s3 * uy3[1];
		   }
		   else {
			 fprintf(stderr, "Something is wrong. After lastRegRow:\n");
			 fprintf(stderr, "nRegBlock_h = %d, iblock_h = %d, irow = %d, icol = %d.\n", nRegBlock_h, iblock_h, irow, icol);
			 fprintf(stderr, "exit...\n");
			 exit(1);
		   }
		   utm_x[base+icol] = ux;
		   utm_y[base+icol] = uy;

		 }
	  }
	}
 }

 /*************************************************************************************************
	HANDLE THE PIXELS IN THE COLUMNS FOLLOWING THE REGULAR GRID
  *************************************************************************************************
 */
 /*Columns beyond the regularly spaced columns.*/
 if (tiledim_ncol-1  > lastRegCol) {
	/*Lower triangle*/
	x1 = 0;   y1 = 0;
	x2 = x1;  y2 = spacing;
	x3 = (tiledim_ncol-1) % spacing; y3 = y2;
	compute_simplicial_abc(x1, y1, x2, y2, x3, y3, &lower_triang);

	/*Upper triangle*/
	x2 = x3;  y2 = y1;
	compute_simplicial_abc(x1, y1, x2, y2, x3, y3, &upper_triang);

	for (iblock_v = 0; iblock_v < nRegBlock_v; iblock_v++) {
	  /*UTM coordinates for the lower triangle*/
	  temp = (iblock_v*spacing)*tiledim_ncol + nRegBlock_h*spacing;
	  ux1[0] = utm_x[temp];  uy1[0] = utm_y[temp];

	  temp = (iblock_v+1)*spacing*tiledim_ncol + nRegBlock_h*spacing;
	  ux2[0] = utm_x[temp];  uy2[0] = utm_y[temp];

	  temp = (iblock_v+1)*spacing*tiledim_ncol + (tiledim_ncol-1);
	  ux3[0] = utm_x[temp];  uy3[0] = utm_y[temp];

	  /*UTM coordinates for the upper triangle*/
	  ux1[1] = ux1[0];  uy1[1] = uy1[0];
	  temp = (iblock_v*spacing)*tiledim_ncol + (tiledim_ncol-1);
	  ux2[1] = utm_x[temp]; uy2[1] = utm_y[temp];
	  ux3[1] = ux3[0];  uy3[1] = uy3[0];

	  /*If the last row is on the regular grid, process it as well*/
	  if (iblock_v == nRegBlock_v-1 && lastRegRow == tiledim_nrow-1)
		  nrow_process = spacing+1;
	  else
		  nrow_process = spacing;

	  for (irow = 0; irow < nrow_process; irow++) {
		 base = (iblock_v*spacing + irow) * tiledim_ncol + nRegBlock_h*spacing;
		 for (icol = 0; icol <= (tiledim_ncol-1)%spacing; icol++) {  /*Note the equal sign to include the last column*/
			  if (isInTriang(lower_triang, icol, irow, &s1, &s2, &s3)) {
				 ux = s1 * ux1[0] + s2 * ux2[0] + s3 * ux3[0];
				 uy = s1 * uy1[0] + s2 * uy2[0] + s3 * uy3[0];
			  }
			  else if (isInTriang(upper_triang, icol, irow, &s1, &s2, &s3)) {
				 ux = s1 * ux1[1] + s2 * ux2[1] + s3 * ux3[1];
				 uy = s1 * uy1[1] + s2 * uy2[1] + s3 * uy3[1];
			  }
			  else {
				fprintf(stderr, "Something is wrong. After lastRegCol:\n");
				fprintf(stderr, "nRegBlock_v = %d, iblock_v = %d, irow = %d, icol = %d.\n", nRegBlock_v, iblock_v, irow, icol);
				exit(1);
			  }
			  utm_x[base+icol] = ux;
			  utm_y[base+icol] = uy;
		 }
	  }
	}
 }

 /*************************************************************************************************
	HANDLE THE GROUPS OF PIXELS AT THE LOWER-RIGHT CORNER OF THE TILE THAT ARE NOT COVERED BY THE REGULAR GRID.
  *************************************************************************************************
 */
 /*The very lower-right block shorter than "spacing" pixels.*/
 if (tiledim_ncol-1  > lastRegCol && tiledim_nrow-1 > lastRegRow) {
	/*Lower triangle*/
	x1 = 0;   y1 = 0;
	x2 = x1;  y2 = (tiledim_nrow-1) % spacing;
	x3 = (tiledim_ncol-1) % spacing; y3 = y2;
	compute_simplicial_abc(x1, y1, x2, y2, x3, y3, &lower_triang);

	/*Upper triangle*/
	x2 = x3;  y2 = y1;
	compute_simplicial_abc(x1, y1, x2, y2, x3, y3, &upper_triang);

	/*UTM coordinates for the lower triangle*/
	temp = (nRegBlock_v*spacing)*tiledim_ncol + nRegBlock_h*spacing;
	ux1[0] = utm_x[temp];  uy1[0] = utm_y[temp];

	temp = (tiledim_nrow-1)* tiledim_ncol + nRegBlock_h*spacing;
	ux2[0] = utm_x[temp];  uy2[0] = utm_y[temp];

	temp = tiledim_nrow*tiledim_ncol-1;
	ux3[0] = utm_x[temp];  uy3[0] = utm_y[temp];

	/*UTM coordinates for the upper triangle*/
	ux1[1] = ux1[0];  uy1[1] = uy1[0];
	temp = (iblock_v*spacing)*tiledim_ncol + (tiledim_ncol-1);
	ux2[1] = utm_x[temp];  uy2[1] = utm_y[temp];
	ux3[1] = ux3[0];  uy3[1] = uy3[0];

	for (irow = 0; irow <= (tiledim_nrow-1) % spacing; irow++) {	   /*Note the equal sign*/
	  base = (nRegBlock_v*spacing + irow)*tiledim_ncol + nRegBlock_h*spacing;
	  for (icol = 0; icol <= (tiledim_ncol-1) % spacing; icol++) {

		if (isInTriang(lower_triang, icol, irow,  &s1, &s2, &s3)) {
		   ux = s1 * ux1[0] + s2 * ux2[0] + s3 * ux3[0];
		   uy = s1 * uy1[0] + s2 * uy2[0] + s3 * uy3[0];
		}
		else if (isInTriang(upper_triang, icol, irow, &s1, &s2, &s3)) {
		   ux = s1 * ux1[1] + s2 * ux2[1] + s3 * ux3[1];
		   uy = s1 * uy1[1] + s2 * uy2[1] + s3 * uy3[1];
		}
		else {
		  fprintf(stderr, "Something is wrong. lower-right corner. irow = %d, icol = %d, exit...\n", irow, icol);
		  exit(1);
		}
		utm_x[base+icol] = ux;
		utm_y[base+icol] = uy;
	  }
	}
 }

 t2 = clock();
 //fprintf(stdout, "Time to do the interpolation : %.1lf (seconds)\n", (double) ((t2-t1)/CLOCKS_PER_SEC));
}





/**************************************************************************************************
    For a given landsat tile derive each pixel's UTM coordinates of the given zone
    by triangulation interpolation.

 Parameters::
      int       utm_zone:
      float32   pixel_size:   30.0 meters in this project.
      int       tiledim_nrow: Landsat tile dimension in number of rows,  (5295 defined in the code header file)
      int       tiledim_ncol: Landsat tile dimension in number of columns, (5295 defined in the code header file)
      int       itile_h:      the horizontal landsat tile  ID.
      int       itile_v:      the vertical landsat tile  ID.
      int       spacing:      the spacing of the grid in number of pixels for triangulation, defined in the code header file.
      double    *utmx:        the UTM X for each pixel on the tile.
      double    *utmy:        the UTM Y for each pixel on the tile.

    Why are utmx utmy not double in the actual function?  April 6, 2010. Probably to save memory; but later
    need go back to double.


    Note: Since a MODIS tile is 1111950.519667 meters and we are nesting 7x7 Landsat tiles each of
          5295x5295 30m pixels, there will be a 0.519667 meter gap at the right and bottom edges of
          each MODIS tile. These gaps are ignored and the nesting of the Landsat tiles within the adjacent
          MODIS tiles starts from the UL corner of the MODIS tile again.
          So when compute the UL corner coordinate of each MODIS tile we need to use the MODIS tile size,
          not the Landsat tile size and pixel size.

 **************************************************************************************************
 */
void interpolate_utm_xy_to_sinusoidal(int utm_zone,
                        float32 pixel_size,
                        int tiledim_nrow,
                        int tiledim_ncol,
                        int itile_h,
                        int itile_v,
                        int spacing,
                        float64 *utm_x,
                        float64 *utm_y)
{

 clock_t t1, t2;

 simplicial_t lower_triang, upper_triang;



 int lastRegRow, lastRegCol;   /*The last row/col on the reguarly spaced grid*/
 int nRegBlock_h, nRegBlock_v, iblock_h, iblock_v;   /*blocks on the grid*/
 int base;               /*temporary varible to save computation*/

 double x1, y1, x2, y2, x3, y3;    /*coordinates of the triangle in the sinusoidal system*/
 double s1, s2, s3;                /*three variables to characterize a point in a triangle*/
 double ux1[2], uy1[2], ux2[2], uy2[2], ux3[2], uy3[2];   /*UTM coordinates of the vertices of the two triangles*/
 int temp, temp1, temp2, temp3;
 int nrow_process, ncol_process;

 int irow, icol;
 double ax, ay;     /* In the CONUS code, they are for Albers X, Y. Here used to represent Sinusoidal X Y*/
 double ux, uy;

 //fprintf(stderr, "zone = %d, pixsize = %f, nrow = %d, ncol = %d, first_row = %d, first_col = %d, spacing = %d\n",
 //                 utm_zone, pixel_size, tiledim_nrow, tiledim_ncol, first_row_sinus, first_col_sinus, spacing);

 /*A regular grid with equal spacing both vertically and horizontally is set up, and each grid cell is divided into
   two triangles for triangulation. For the rows at the bottom or the columns on the right of the tile that are not
   covered by the regular grid, triangles of different sizes are used.

   In all cases, a square or rectangle of pixels are divided in NW-SE direction into two triangles.
   For each regular grid cell only two types of triangles are needed. For the rows and column that are not covered the
   regular grid, another different triangles are used in interpolation.

   This function is lengthly. The idea was rather that within the loops use "if" statement for every pixel to decide
   how the pixel should be treated, the "if" is taken out of the loops in the hopes of increasing the efficiency, but
   at the apparent price of the length of the code.
 */


 /***Set up the regularly spaced grid for interpolating. First, project the pixels that are regularly spaced every
     "spacing" rows and columns in the Sinusoidal coordinate system to UTM, starting with the 0th row and 0th column.
     And then if the last row (tiledim_nrow-1) or last column (tiledim_ncol-1) on the tile is not integer multiples
     of the "spacing", project the last row or column too.  Make sure the lower-right pixel--the very last pixel
     on the regular grid is projected too.
     ***/


    int modis_tile_h, modis_tile_v;     /*  The modis tile ID,  0-35, and 0-17 */
    int mini_tile_h, mini_tile_v;       /*  The mini tiles nested within a modis tile,  0-6, 0-6*/
    modis_tile_h = itile_h / NMINI;
    modis_tile_v = itile_v / NMINI;
    mini_tile_h  = itile_h % NMINI;
    mini_tile_v  = itile_v % NMINI;

 t1 = clock();

 /*************************************************************************************************
    CALL GCTP TO CALCULATE THE UTM COORDINATES FOR THE PIXELS FROM WHICH THE UTM COORDINATES OF
    OTHER PIXELS WILL BE INTERPOLATED.
  *************************************************************************************************
 */

 /*Project the regular grid with columns and rows spaced "spacing" pixels.
   Keep in mind an easy mistake: Suppose there are 10 rows and 10 column and the grid lines are
   spaced 5 pixels, then the last row and last column are actually not on the regular grid!
  */
 for (irow = 0; irow < tiledim_nrow; irow+=spacing) {
   ay = sinus_global.sinus_ULY - (double)(modis_tile_v * sinus_global.modis_tile_size) -
                               ((double)(mini_tile_v * tiledim_nrow + irow) + 0.5) * pixel_size;   /* pixel center */
   base = irow * tiledim_ncol;
   for (icol = 0; icol < tiledim_ncol; icol+=spacing) {
     ax = sinus_global.sinus_ULX + (double)(modis_tile_h * sinus_global.modis_tile_size) +
                                 ((double)(mini_tile_h * tiledim_ncol + icol) + 0.5) * pixel_size;  /* pixel center */
     sinus2utm(sinus_global, ax, ay, utm_zone, &ux, &uy);
			/********************************
			 * this is debugging

			if((irow==15 && icol==15)||
			   (irow==30 && icol==30)||
			   (irow==45 && icol==45) ){
				printf(" ^^^^^^^^^^^^^^^^^^^^  \n");
				printf("sinx=%lf, siny=%lf  \n", ax, ay);
				printf("utmx=%lf, utmy=%lf  \n", ux, uy);
			}*********************************/
     utm_x[base + icol] = ux;
     utm_y[base + icol] = uy;
   }
 }

 /*Project the last row regularly every "spacing" pixels horizontally if the last row is not on the regular grid*/
 lastRegRow = (tiledim_nrow-1) - (tiledim_nrow-1) % spacing;  /*Last row on the regularly spaced grid*/
 if (tiledim_nrow-1 > lastRegRow) {
   base = (tiledim_nrow-1) * tiledim_ncol;
   ay = sinus_global.sinus_ULY - (double)(modis_tile_v * sinus_global.modis_tile_size) -
                               ((double)(mini_tile_v * tiledim_nrow + (tiledim_nrow-1)) + 0.5) * pixel_size;   /* pixel center */
   for (icol = 0; icol < tiledim_ncol; icol+=spacing) {
     ax = sinus_global.sinus_ULX + (double)(modis_tile_h * sinus_global.modis_tile_size) +
                                 ((double)(mini_tile_h * tiledim_ncol + icol) + 0.5) * pixel_size;  /* pixel center */
     sinus2utm(sinus_global, ax, ay, utm_zone, &ux, &uy);
     utm_x[base + icol] = ux;
     utm_y[base + icol] = uy;
   }
 }

 /*Project the last col regularly every "spacing" pixels vertically if the last column is not on the regular grid*/
 lastRegCol = (tiledim_ncol-1) - (tiledim_ncol-1) % spacing;  /*Last row on the regularly spaced grid*/
 if (tiledim_ncol-1  > lastRegCol) {
   ax = sinus_global.sinus_ULX + (double)(modis_tile_h * sinus_global.modis_tile_size) +
                               ((double)(mini_tile_h * tiledim_ncol + (tiledim_ncol-1)) + 0.5) * pixel_size;  /* pixel center */
   for (irow = 0; irow < tiledim_nrow; irow+=spacing) {
     ay = sinus_global.sinus_ULY - (double)(modis_tile_v * sinus_global.modis_tile_size) -
                                 ((double)(mini_tile_v * tiledim_nrow + irow) + 0.5) * pixel_size;   /* pixel center */
     sinus2utm(sinus_global, ax, ay, utm_zone, &ux, &uy);
     utm_x[(irow+1) * tiledim_ncol -1] = ux;
     utm_y[(irow+1) * tiledim_ncol -1] = uy;
   }
 }

 /*Make sure that the single, lower-right corner  pixel on the tile is projected too*/
 if (tiledim_nrow-1 > lastRegRow && tiledim_ncol-1  > lastRegCol) {
   ax = sinus_global.sinus_ULX + (double)(modis_tile_h * sinus_global.modis_tile_size) +
                               ((double)(mini_tile_h * tiledim_ncol + (tiledim_ncol-1)) + 0.5) * pixel_size;  /* pixel center */
   ay = sinus_global.sinus_ULY - (double)(modis_tile_v * sinus_global.modis_tile_size) -
                               ((double)(mini_tile_v * tiledim_nrow + (tiledim_nrow-1)) + 0.5) * pixel_size;   /* pixel center */
   sinus2utm(sinus_global, ax, ay, utm_zone, &ux, &uy);
   utm_x[tiledim_nrow * tiledim_ncol -1] = ux;
   utm_y[tiledim_nrow * tiledim_ncol -1] = uy;
 }


 t2 = clock();
 /*fprintf(stderr, "Time to set up the grid : %d (seconds)\n", (int)((t2-t1)/CLOCKS_PER_SEC));*/


 /*No more GCTP from now on. Triangulation begins*/
 t1 = clock();
 /*************************************************************************************************
    HANDLE THE PIXELS ON THE REGULAR GRID
  *************************************************************************************************
 */

 /*The number of blocks enclosed horizontally or vertically by the lines or column
   on the regularly spaced grid. BE CAREFUL, IT IS NOT tiledim_ncol/spacing.
  */
 nRegBlock_h = (tiledim_ncol-1)/spacing;
 nRegBlock_v = (tiledim_nrow-1)/spacing;

 /*Divide each grid cell in the NW-SE direction into two triangles and
   compute the three parameters "abc"  for the resulting lower and upper triangle comprising a cell on
   the regular grid.  Only need do it once since all the lower triangles and all the uppper
   triangles in the grid have the same topology. Just use the relative row and column number as
   the proxy coordinates in Sinusoidal system, but no need to use sinusoidal coordinates*/

 /*Lower triangle with a single top vertice*/
 /*The relative row/col numbers are used for the coordinates in the sinusoidal , instead of actual coordinates*/
 /*top vertex*/
 x1 = 0;  y1 = 0;

 /*lower-left vertex*/
 x2 = x1; y2 = spacing;

 /*lower-right vertex*/
 x3 = spacing; y3 = y2;

 compute_simplicial_abc(x1, y1, x2, y2, x3, y3, &lower_triang);

 /*Upper triangle with a single bottom vertice.
   upper-left vertex is the same as x1, y1, and bottom vertex is the same as x3,y3*/
 /*upper-right vertex*/
 x2 = x3; y2 = y1;

 compute_simplicial_abc(x1, y1, x2, y2, x3, y3, &upper_triang);

 /*The pixles on the regularly spaced blocks in the  grid*/
 for (iblock_h = 0; iblock_h < nRegBlock_h; iblock_h++) {
   for (iblock_v = 0; iblock_v < nRegBlock_v; iblock_v++) {

      /*****UTM coordinates for the lower triangle*****/
      /*The top vertice of the lower triangle*/
      temp1 = (iblock_v*spacing)*tiledim_ncol + iblock_h*spacing;
      ux1[0] = utm_x[temp1];  uy1[0] = utm_y[temp1];

      /*lower-left vertice of the lower triangle*/
      temp2 = temp1 + spacing * tiledim_ncol;
      ux2[0] = utm_x[temp2];  uy2[0] = utm_y[temp2];

      /*lower-right vertice of the lower triangle*/
      temp3 = temp2 + spacing;
      ux3[0] = utm_x[temp3];  uy3[0] = utm_y[temp3];

      /*****UTM coordinates for the upper triangle*****/
      /*upper-left vertice of the upper triangle*/
      ux1[1] = ux1[0];  uy1[1] = uy1[0];

      /*upper-right vertice of the upper triangle*/
      temp2 = temp1 + spacing;
      ux2[1] = utm_x[temp2];  uy2[1] = utm_y[temp2];

      /*Bottom vertice of the upper triangle*/
      ux3[1] = ux3[0];  uy3[1] = uy3[0];

      if (iblock_v == nRegBlock_v-1 && lastRegRow == tiledim_nrow-1)
         /*If all the rows are perfectly covered the regular grid, interpolate the last row of the tile too.*/
         nrow_process = spacing+1;
      else
         /*Otherwise, the last row will be processed later*/
         nrow_process = spacing;

      if (iblock_h == nRegBlock_h-1 && lastRegCol == tiledim_ncol-1)
         /*If all the columns are perfectly covered the regular grid, interpolate the last column of the tile too.*/
         ncol_process = spacing+1;
      else
         /*Otherwise, the last column will be processed later*/
         ncol_process = spacing;

      for (irow = 0; irow < nrow_process; irow++) {
         base =  (iblock_v*spacing + irow) * tiledim_ncol + iblock_h*spacing;
         for (icol = 0; icol < ncol_process; icol++) {
           if (isInTriang(lower_triang, icol, irow, &s1, &s2, &s3)) {
              ux = s1 * ux1[0] + s2 * ux2[0] + s3 * ux3[0];
              uy = s1 * uy1[0] + s2 * uy2[0] + s3 * uy3[0];
           }
           else if (isInTriang(upper_triang, icol, irow, &s1, &s2, &s3)) {
              ux = s1 * ux1[1] + s2 * ux2[1] + s3 * ux3[1];
              uy = s1 * uy1[1] + s2 * uy2[1] + s3 * uy3[1];
           }
           else {
             fprintf(stderr, "Something is wrong. nRegBlock_h = %d, iblock_h = %d, irow = %d, icol = %d.\n",
                              nRegBlock_h, iblock_h, irow, icol);
             exit(1);
           }
           utm_x[base+icol] = ux;
           utm_y[base+icol] = uy;
         }
      }
   }
 }

 /*************************************************************************************************
    HANDLE THE PIXELS IN THE ROWS FOLLOWING THE REGULAR GRID
  *************************************************************************************************
 */
 if (tiledim_nrow-1 > lastRegRow) {
    /*Lower triangle. The relative coordinates in the Sinusoidal system*/
    x1 = 0;   y1 = 0;                               /*top vertice*/
    x2 = x1;  y2 = (tiledim_nrow-1) % spacing;      /*lower-left vertice*/
    x3 = spacing; y3 = y2;                          /*lower-right vertice*/
    compute_simplicial_abc(x1, y1, x2, y2, x3, y3, &lower_triang);

    /*Upper triangle.*/
    /*x1,y1 is upper-left vertice and stay the same, and x3,y3 is the bottom vertice and stays the same*/
    x2 = x3;  y2 = y1;                             /*upper-right triangle*/
    compute_simplicial_abc(x1, y1, x2, y2, x3, y3, &upper_triang);

    for (iblock_h = 0; iblock_h < nRegBlock_h; iblock_h++) {

      /*****UTM coordinates for the lower triangle*****/
      /*top vertice of the lower triangle*/
      temp = (nRegBlock_v*spacing)*tiledim_ncol + iblock_h*spacing;
      ux1[0] = utm_x[temp];  uy1[0] = utm_y[temp];

      /*lower-left vertice of the lower triangle*/
      temp = (tiledim_nrow-1)*tiledim_ncol + iblock_h*spacing;
      ux2[0] = utm_x[temp];  uy2[0] = utm_y[temp];

      /*lower-right vertice of the lower triangle*/
      temp = (tiledim_nrow-1)*tiledim_ncol + (iblock_h+1)*spacing;
      ux3[0] = utm_x[temp];  uy3[0] = utm_y[temp];

      /*UTM coordinates for the upper triangle*/
      /*upper-left vertice of the upper triangle*/
      ux1[1] = ux1[0];  uy1[1] = uy1[0];

      /*upper-right vertice of the upper triangle*/
      temp = (nRegBlock_v*spacing)*tiledim_ncol + (iblock_h+1)*spacing;
      ux2[1] = utm_x[temp];  uy2[1] = utm_y[temp];

      /*bottom vertice of the upper triangle*/
      ux3[1] = ux3[0];  uy3[1] = uy3[0];


      /*If the last column in on the regular grid, interpolate it as well*/
      if (iblock_h == nRegBlock_h-1 && lastRegCol == tiledim_ncol-1)
          ncol_process = spacing+1;
      else
          ncol_process = spacing;

      for (irow = 0; irow <= (tiledim_nrow-1)%spacing; irow++) {    /*Note the equal sign to include the last row*/
         base = (nRegBlock_v*spacing + irow) * tiledim_ncol + iblock_h*spacing;
         for (icol = 0; icol < ncol_process; icol++) {
           if (isInTriang(lower_triang, icol, irow, &s1, &s2, &s3)) {
              ux = s1 * ux1[0] + s2 * ux2[0] + s3 * ux3[0];
              uy = s1 * uy1[0] + s2 * uy2[0] + s3 * uy3[0];
           }
           else if (isInTriang(upper_triang, icol, irow, &s1, &s2, &s3)) {
              ux = s1 * ux1[1] + s2 * ux2[1] + s3 * ux3[1];
              uy = s1 * uy1[1] + s2 * uy2[1] + s3 * uy3[1];
           }
           else {
             fprintf(stderr, "Something is wrong. After lastRegRow:\n");
             fprintf(stderr, "nRegBlock_h = %d, iblock_h = %d, irow = %d, icol = %d.\n", nRegBlock_h, iblock_h, irow, icol);
             fprintf(stderr, "exit...\n");
             exit(1);
           }
           utm_x[base+icol] = ux;
           utm_y[base+icol] = uy;

         }
      }
    }
 }

 /*************************************************************************************************
    HANDLE THE PIXELS IN THE COLUMNS FOLLOWING THE REGULAR GRID
  *************************************************************************************************
 */
 /*Columns beyond the regularly spaced columns.*/
 if (tiledim_ncol-1  > lastRegCol) {
    /*Lower triangle*/
    x1 = 0;   y1 = 0;
    x2 = x1;  y2 = spacing;
    x3 = (tiledim_ncol-1) % spacing; y3 = y2;
    compute_simplicial_abc(x1, y1, x2, y2, x3, y3, &lower_triang);

    /*Upper triangle*/
    x2 = x3;  y2 = y1;
    compute_simplicial_abc(x1, y1, x2, y2, x3, y3, &upper_triang);

    for (iblock_v = 0; iblock_v < nRegBlock_v; iblock_v++) {
      /*UTM coordinates for the lower triangle*/
      temp = (iblock_v*spacing)*tiledim_ncol + nRegBlock_h*spacing;
      ux1[0] = utm_x[temp];  uy1[0] = utm_y[temp];

      temp = (iblock_v+1)*spacing*tiledim_ncol + nRegBlock_h*spacing;
      ux2[0] = utm_x[temp];  uy2[0] = utm_y[temp];

      temp = (iblock_v+1)*spacing*tiledim_ncol + (tiledim_ncol-1);
      ux3[0] = utm_x[temp];  uy3[0] = utm_y[temp];

      /*UTM coordinates for the upper triangle*/
      ux1[1] = ux1[0];  uy1[1] = uy1[0];
      temp = (iblock_v*spacing)*tiledim_ncol + (tiledim_ncol-1);
      ux2[1] = utm_x[temp]; uy2[1] = utm_y[temp];
      ux3[1] = ux3[0];  uy3[1] = uy3[0];

      /*If the last row is on the regular grid, process it as well*/
      if (iblock_v == nRegBlock_v-1 && lastRegRow == tiledim_nrow-1)
          nrow_process = spacing+1;
      else
          nrow_process = spacing;

      for (irow = 0; irow < nrow_process; irow++) {
         base = (iblock_v*spacing + irow) * tiledim_ncol + nRegBlock_h*spacing;
         for (icol = 0; icol <= (tiledim_ncol-1)%spacing; icol++) {  /*Note the equal sign to include the last column*/
              if (isInTriang(lower_triang, icol, irow, &s1, &s2, &s3)) {
                 ux = s1 * ux1[0] + s2 * ux2[0] + s3 * ux3[0];
                 uy = s1 * uy1[0] + s2 * uy2[0] + s3 * uy3[0];
              }
              else if (isInTriang(upper_triang, icol, irow, &s1, &s2, &s3)) {
                 ux = s1 * ux1[1] + s2 * ux2[1] + s3 * ux3[1];
                 uy = s1 * uy1[1] + s2 * uy2[1] + s3 * uy3[1];
              }
              else {
                fprintf(stderr, "Something is wrong. After lastRegCol:\n");
                fprintf(stderr, "nRegBlock_v = %d, iblock_v = %d, irow = %d, icol = %d.\n", nRegBlock_v, iblock_v, irow, icol);
                exit(1);
              }
              utm_x[base+icol] = ux;
              utm_y[base+icol] = uy;
         }
      }
    }
 }

 /*************************************************************************************************
    HANDLE THE GROUPS OF PIXELS AT THE LOWER-RIGHT CORNER OF THE TILE THAT ARE NOT COVERED BY THE REGULAR GRID.
  *************************************************************************************************
 */
 /*The very lower-right block shorter than "spacing" pixels.*/
 if (tiledim_ncol-1  > lastRegCol && tiledim_nrow-1 > lastRegRow) {
    /*Lower triangle*/
    x1 = 0;   y1 = 0;
    x2 = x1;  y2 = (tiledim_nrow-1) % spacing;
    x3 = (tiledim_ncol-1) % spacing; y3 = y2;
    compute_simplicial_abc(x1, y1, x2, y2, x3, y3, &lower_triang);

    /*Upper triangle*/
    x2 = x3;  y2 = y1;
    compute_simplicial_abc(x1, y1, x2, y2, x3, y3, &upper_triang);

    /*UTM coordinates for the lower triangle*/
    temp = (nRegBlock_v*spacing)*tiledim_ncol + nRegBlock_h*spacing;
    ux1[0] = utm_x[temp];  uy1[0] = utm_y[temp];

    temp = (tiledim_nrow-1)* tiledim_ncol + nRegBlock_h*spacing;
    ux2[0] = utm_x[temp];  uy2[0] = utm_y[temp];

    temp = tiledim_nrow*tiledim_ncol-1;
    ux3[0] = utm_x[temp];  uy3[0] = utm_y[temp];

    /*UTM coordinates for the upper triangle*/
    ux1[1] = ux1[0];  uy1[1] = uy1[0];
    temp = (iblock_v*spacing)*tiledim_ncol + (tiledim_ncol-1);
    ux2[1] = utm_x[temp];  uy2[1] = utm_y[temp];
    ux3[1] = ux3[0];  uy3[1] = uy3[0];

    for (irow = 0; irow <= (tiledim_nrow-1) % spacing; irow++) {       /*Note the equal sign*/
      base = (nRegBlock_v*spacing + irow)*tiledim_ncol + nRegBlock_h*spacing;
      for (icol = 0; icol <= (tiledim_ncol-1) % spacing; icol++) {

        if (isInTriang(lower_triang, icol, irow,  &s1, &s2, &s3)) {
           ux = s1 * ux1[0] + s2 * ux2[0] + s3 * ux3[0];
           uy = s1 * uy1[0] + s2 * uy2[0] + s3 * uy3[0];
        }
        else if (isInTriang(upper_triang, icol, irow, &s1, &s2, &s3)) {
           ux = s1 * ux1[1] + s2 * ux2[1] + s3 * ux3[1];
           uy = s1 * uy1[1] + s2 * uy2[1] + s3 * uy3[1];
        }
        else {
          fprintf(stderr, "Something is wrong. lower-right corner. irow = %d, icol = %d, exit...\n", irow, icol);
          exit(1);
        }
        utm_x[base+icol] = ux;
        utm_y[base+icol] = uy;
      }
    }
 }

 t2 = clock();
 /*fprintf(stderr, "Time to do the interpolation : %.1lf (seconds)\n", (double) ((t2-t1)/CLOCKS_PER_SEC));*/
}


/*Compute the three numbers two sets of numbers a,b,c to characterize the triangle. A relative row/col numbers
  can be used as coordinates and no need to use actual coordinates since the row/col are related to
  sinusoidal coordinates linearly.  The a,b,c are returned in the pointer to the triangle.
*/
void compute_simplicial_abc(double x1, double y1, double x2, double y2, double x3, double y3, simplicial_t *triang)
{
 double t;

 t = x1*y2 + x2*y3 + x3*y1 - x3*y2 - x2*y1 - x1*y3;

 triang->a1 = (x3 - x2)/t;
 triang->b1 = (y2 - y3)/t;
 triang->c1 = (x2*y3 - x3*y2)/t;

 triang->a2 = (x1 - x3)/t;
 triang->b2 = (y3 - y1)/t;
 triang->c2 = (x3*y1 - x1*y3)/t;
}

/*Test if a point is in the given triangle.
  If the coefficients a,b,c's are calculated using the relative row/col numbers, then the point to be
  tested must be in the relative row/col numbers.
 */

bool isInTriang(simplicial_t triang, double x, double y, double *s1, double *s2, double *s3)
{
 if ((*s1 = triang.a1 * y + triang.b1 * x + triang.c1) < 0)
    return false;
 if ((*s2 = triang.a2 * y + triang.b2 * x + triang.c2) < 0)
    return false;
 if ((*s3 = (1 - (*s1 + *s2))) < 0)
    return false;

 return true;
}



/*********************************************************************************/
/*
    Read the UTM zone number given the acquisition's b1fname, but nothing else.
    It is very light and should be very fast  compared to read_acq().
    Close the HDF after reading.

    Return 0 on success and 1 on failure.
*/
/*********************************************************************************/
int read_utm_zone(int16 *zone,  char *fname_b1)
{
    int status = 0;
    int32 sd_id_b1 = FAIL;
    int32 attr_index;
    char message[256];

    if ((sd_id_b1 = SDstart(fname_b1, DFACC_READ)) == FAIL) {
        sprintf(message, "Cannot open %s for read", fname_b1);
        status = 1;
        Error(message, "read_utm_zone", status);
        goto CLEANUP;
    }

    attr_index = SDfindattr(sd_id_b1, INPUT_UTM_ZONE_NUMBER);
    if (attr_index == FAIL || SDreadattr(sd_id_b1, attr_index, zone) == FAIL) {
        status = 1;
        sprintf(message, "Cannot find/read the attribute %s in %s", INPUT_UTM_ZONE_NUMBER, fname_b1);
        Error(message, "read_utm_zone", status);
        goto CLEANUP;
    }

    CLEANUP:
    if (sd_id_b1 != FAIL)
        SDend(sd_id_b1);

    return status;
}


/****************************************************************************************
    Find out if a mini-tile straddles the edge of the projected earth.
    The idea: for every edge pixel of the minitile (i.e, first/last column and column)
    find its lat/lon from the sininusodal X,Y by inverting the projection equation.
    If the absolute value of the longitude of a pixel is equal to or greater than 180 degrees,
    the pixel is outide the earth and the minitile straddles the earth edge.
    If it straddles, furhter generates a mask for the inside pixels and in the mask 1 means
    inside the earth.

    We have known that roughtly on what minitiles there are L1T acquisition, so a lot of
    impossible minitiles have been eliminated and that makes this test realy easy; for example
    we don't have to worry about latitude being 90 deg.

    Return 1 if on edge, and 0 if no.

*/

int isEdgeTile(int itile_h, int itile_v, char *inside_mask)
{
    char is_edge = 0;
    double X, Y;
    double latitude, longitude;
    double epsilon  = pow(10,-8);
    int irow, icol, k;

    int modis_tile_h, modis_tile_v;     /*  The modis tile ID,  0-35, and 0-17 */
    int mini_tile_h, mini_tile_v;       /*  The mini tiles nested within a modis tile,  0-6, 0-6*/

    modis_tile_h = itile_h / NMINI;
    modis_tile_v = itile_v / NMINI;
    mini_tile_h  = itile_h % NMINI;
    mini_tile_v  = itile_v % NMINI;

    /* Remember to initialize to "true" */
    for (irow = 0; irow < TILEDIM; irow++) {
        for (icol = 0; icol < TILEDIM; icol++) {
            k = irow * TILEDIM + icol;
            //inside_mask[k] = 0;
            inside_mask[k] = 1;
        }
    }

    /*  For the edge test, we only need to test on the first row and last row of the mini_tile
        (or first column and last column)
    */
    is_edge = 0;
    for (irow = 0; irow < TILEDIM; irow+=(TILEDIM-1)) {
        if (is_edge == 1)
            break;

        Y = sinus_global.sinus_ULY - modis_tile_v * sinus_global.modis_tile_size -
                                    (mini_tile_v * TILEDIM + irow + 0.5) * REF_PIXSIZE;   /* pixel center */
        latitude = Y / sinus_global.radius;
        for (icol= 0; icol < TILEDIM; icol++) {
            X = sinus_global.sinus_ULX + modis_tile_h * sinus_global.modis_tile_size +
                                        (mini_tile_h * TILEDIM + icol + 0.5) * REF_PIXSIZE;  /* pixel center */
            longitude = X / (sinus_global.radius * cos(latitude));

            if (fabs(longitude) > PI-epsilon){
                is_edge = 1;
                break;
            }
        }
    }

    if (is_edge == 1) {
        for (irow = 0; irow < TILEDIM; irow++) {
            Y = sinus_global.sinus_ULY - modis_tile_v * sinus_global.modis_tile_size -
                                        (mini_tile_v * TILEDIM + irow + 0.5) * REF_PIXSIZE;   /* pixel center */
            latitude = Y / sinus_global.radius;
            for (icol = 0; icol < TILEDIM; icol++) {
                X = sinus_global.sinus_ULX + modis_tile_h * sinus_global.modis_tile_size +
                                            (mini_tile_h * TILEDIM + icol + 0.5) * REF_PIXSIZE;  /* pixel center */
                longitude = X / (sinus_global.radius * cos(latitude));

                k = irow * TILEDIM + icol;
                if (fabs(longitude) < PI-epsilon)
                    inside_mask[k] = 1;
                else
                    inside_mask[k] = 0;
            }
        }
    }

    return is_edge;
}

/**********************************************************************************************************************
    Call GCTP to compute the UTM X/Y for each pixel that is inisde the earth on the edge minitile.
*/
void gctp_utm_xy_sinusoidal(int itile_h, int itile_v, char *inside_mask, int utm_zone, float64 *utm_x, float64 *utm_y)
{
    double X, Y;
    int irow, icol, k;
    double ux, uy;

    int modis_tile_h, modis_tile_v;     /*  The modis tile ID,  0-35, and 0-17 */
    int mini_tile_h, mini_tile_v;       /*  The mini tiles nested within a modis tile,  0-6, 0-6*/

    modis_tile_h = itile_h / NMINI;
    modis_tile_v = itile_v / NMINI;
    mini_tile_h  = itile_h % NMINI;
    mini_tile_v  = itile_v % NMINI;

    for (irow = 0; irow < TILEDIM; irow++) {
        for (icol = 0; icol < TILEDIM; icol++) {
            k = irow * TILEDIM + icol;
            if (inside_mask[k] == 1) {
                Y = sinus_global.sinus_ULY - modis_tile_v * sinus_global.modis_tile_size -
                                            (mini_tile_v * TILEDIM + irow + 0.5) * REF_PIXSIZE;   /* pixel center */
                X = sinus_global.sinus_ULX + modis_tile_h * sinus_global.modis_tile_size +
                                            (mini_tile_h * TILEDIM + icol + 0.5) * REF_PIXSIZE;  /* pixel center */

                sinus2utm(sinus_global, X, Y, utm_zone, &ux, &uy);

                utm_x[k] = ux;
                utm_y[k] = uy;
            }
        }
    }
}


/*********************************************************************************
* Name:	match_l1t_to_srfacq()
* Purpose: parse the tile id from tile_id string
*
* Args:
*	  @all_srfinfo_acq		- list of LEDAPS processed SRF scenes (to be fillef)
*	  @all_l1tinfo_tile 	- list of UTM processed TOA scenes
*	  @nacq_from_toa_tile   - number of UTM scenes in the TOA tile
*	  @srf_acq_list         - name of the tile with ldps, acca, dtcld, geom in hte list
*	  @in_list_option	    - if 1 acca, dtcld only in the list
*	  							if 2  geom only
*	  							if 3 geoms and clouds
*
* Calls: main();
*********************************************************************************/
int match_l1t_to_srfacq(l457acq_fname_t *all_srfinfo_acq, char **all_l1tinfo_tile,
		                int nacq_from_toa_tile, char *srf_acq_list, uint8 in_list_option ){
	FILE *flist=NULL;
	int i, j, status;
	char *pos;
	uint8 found[nacq_from_toa_tile];
	char line[MAX_LINE_LEN];
	char acqID[128], message[MSGLEN];

	status=0;
	for(i=0;i<nacq_from_toa_tile;++i){
		found[i]=0;
	}
	if ((flist = fopen(srf_acq_list, "r")) == NULL) {
		status = 2;
		sprintf(message, "Cannot open %s for reading ", srf_acq_list);
		Error(message, "match_l1t_to_srfacq", status);
		return status;
	}
	while (fgets(line, MAX_LINE_LEN-1, flist)) {
		if (strlen(line) > 1 && line[0] != '\n'){
			pos=strstr(line,".hdf");
			if(pos)	pos[4]='\0';
			for(i=0;i<nacq_from_toa_tile;++i){
				if(found[i]==1)continue;
				pos=NULL;
				pos=strstr(all_l1tinfo_tile[i], ACQ_KEY);
				if(pos==NULL)continue;
				strncpy(acqID, all_l1tinfo_tile[i], (pos-all_l1tinfo_tile[i]));
				acqID[((pos-all_l1tinfo_tile[i]))]='\0';
				//printf("acqID=%s  and %s \n", acqID, line);
				if(strstr(line,acqID)){
					//printf("acqID=%s  and %s \n", acqID, line);
					found[i]=1;
					strcpy(all_srfinfo_acq[i].acq_fname, line);
					if(in_list_option==0)continue;
					for(j=0; j<3; ++j){
						if(fgets(line, MAX_LINE_LEN-1, flist)){
							pos=strstr(line,".hdf");
							if(pos)	pos[4]='\0';
							if( (strstr(line, GEOMKEY)!=NULL) && in_list_option>1){
								strcpy(all_srfinfo_acq[i].geomfname, line);
							}else if((strstr(line, ACCAKEY)!=NULL)  && (in_list_option%2)) {
								strcpy(all_srfinfo_acq[i].accafname, line);
							}else if((strstr(line, DTCLKEY)!=NULL)  && (in_list_option%2)) {
								strcpy(all_srfinfo_acq[i].dtclfname, line);
							}
						}else{
							status = 1;
							printf("nacq_from_toa_tile=%d AND j=%d \n", nacq_from_toa_tile, j);
							sprintf(message, "Mismatch between list file %s and L1T METADATA ", srf_acq_list);
							Error(message, "match_l1t_to_srfacq", status);
							return status;
						}
					}
					/*instead of break */
					i=nacq_from_toa_tile;
				}
			}
		}
	}
	j=0;
	for(i=0;i<nacq_from_toa_tile;++i){
		if(found[i]==1)++j;
	}
	fclose(flist);
	/*Check for possible mismatch*/
	if(j!=nacq_from_toa_tile){
		status = 2;
		printf("nacq_from_toa_tile=%d AND j=%d \n", nacq_from_toa_tile, j);
		sprintf(message, "Mismatch between %s and L1T METADATA ", srf_acq_list);
		Error(message, "match_l1t_to_srfacq", status);
		return status;
	}

	return status;
}




/* WRITE extra QA */
int tile_write_extra_sds(l457_tile_info_t* comp, char *sds_name, int32 sds_type, VOIDP sdscontent){
	int32 dim_sizes[2];
	int32 start[2];
	int rank;
	int32 sds_id;
	int32 comp_type;   /*Compression type flag*/
	comp_info c_info;  /*Compression structure*/
	comp_type = COMP_CODE_DEFLATE;
	rank = 2;
	dim_sizes[0] = comp->nrow;
	dim_sizes[1] = comp->ncol;
	start[0]=0;
	start[1]=0;
	sds_id	= SDcreate(comp->sd_id, sds_name,	sds_type,  rank, dim_sizes);
	SDsetcompress(sds_id,	  comp_type, &c_info);
	SDwritedata(sds_id, start, NULL, dim_sizes, sdscontent);
	SDendaccess(sds_id);
	return 0;
}


/*********************************************************************************
* Name:	set_cpath()
* Purpose: set compositing rule for each pixel
*
* Args:
*	  @comp		- band 3 value
*	  @validCounts		- Selfdescriptive
*	  @waterCounts		- Selfdescriptive
*	  @waterCounts 		- Selfdescriptive
*	  @snowCounts   	- Selfdescriptive
*
* Calls: main();
*********************************************************************************/
void set_cpath(l457_tile_info_t* comp, uint8* validCounts, uint8* waterCounts, uint8* soilCounts, uint8* snowCounts){
	long  irow, icol, base, k_tile;
	double wvratio;
	for (irow = 0; irow < TILEDIM; irow++) {
		base = irow * TILEDIM;
		for (icol = 0; icol < TILEDIM; icol++) {
			k_tile = base+icol;
			if (comp->nobs[k_tile]>0)  {
				if (validCounts[k_tile] == 0) {
					comp->cpath[k_tile]=1;   /* NO (ZERO) valid obs - MIN Blue*/
				}

				if (validCounts[k_tile] == 1){
					comp->cpath[k_tile]=2;   /* Only one valid obs keep it */
					if((snowCounts[k_tile]==1) || (waterCounts[k_tile]==1)){
						comp->cpath[k_tile]=1; /* for shadow snow and false cloud over cold bare ground */
					}
				}

				if (validCounts[k_tile]==2){
					comp->cpath[k_tile]=4; /* Max WNDVI&VI */
					if(waterCounts[k_tile]==2) {
						comp->cpath[k_tile]=3;	/* Min Blue */
					}
					if(waterCounts[k_tile]==1){
						if(soilCounts[k_tile]==0){
							comp->cpath[k_tile]=6; /* to be changed to 3 or 4 */

						}
						if(soilCounts[k_tile]==1){
							comp->cpath[k_tile]=7; /* to be changed to 3 or 4 */
						}
					}
				}

				if (validCounts[k_tile] > 2) {
					comp->cpath[k_tile]=4; /* Max WNDVI&VI */
					/*override*/
					wvratio=(double)(waterCounts[k_tile])/(double)(validCounts[k_tile]);
					if((wvratio>=0.50)) {
						comp->cpath[k_tile]=3;	/* Min Blue */
					}
				}
			}
		}
	}
}


/*********************************************************************************
* Name:	is_valid_clear()
* Purpose: check if the pixel is valid or
*
* Args:
*	  @b03		- band 3 value
*	  @b02		- band 2 value
*	  @b05		- band 5 value
*	  @sat 		- saturation value
*	  @acca   	- acca score
*	  @dtcld    - decision tree cloud score
*
* Calls: main();
*********************************************************************************/
bool is_valid_clear( int16 b03, uint8 sat, uint8 acca, uint dtcld){

	bool returnval=false;

	if(b03==int16_minus32768){
		return returnval;
	}
	if(((sat >> 0) & 1) ||
	   ((sat >> 2) & 1) ||
	   ((sat >> 3) & 1) ||
	   ((sat >> 4) & 1) ){
		return returnval;
	}
	/* STRICT screening */
	if((acca == 0) &&
	   ((dtcld == 0) || (dtcld == 2) || (dtcld == 200))) {
		returnval=true;
	}

	/* Relaxed screening
	if((acca == 0)) {
		returnval=true;
	}
	if(((dtcld == 0) || (dtcld == 2) || (dtcld == 200))) {
			returnval=true;
	}*/


	return returnval;
}

/*********************************************************************************
* Name:	is_valid_clear_rdt()
* Purpose: check if the pixel is valid or
*
* Args:
*	  @b03		- band 3 value
*	  @b02		- band 2 value
*	  @b05		- band 5 value
*	  @sat 		- saturation value
*	  @acca   	- acca score
*	  @dtcld    - decision tree cloud score
*
* Calls: main();
*********************************************************************************/
bool is_valid_clear_rdt( int16 b03, uint8 sat, uint8 acca, uint dtcld){

	bool returnval=false;

	if(b03==int16_minus32768){
		return returnval;
	}
	if(((sat >> 0) & 1) ||
	   ((sat >> 2) & 1) ||
	   ((sat >> 3) & 1) ||
	   ((sat >> 4) & 1) ){
		return returnval;
	}
	/* dtcld-relaxed screening */
	if((acca == 0) &&
	   ((dtcld == 0) || (dtcld == 200))) {
		returnval=true;
	}

	/* Relaxed screening
	if((acca == 0)) {
		returnval=true;
	}
	if(((dtcld == 0) || (dtcld == 2) || (dtcld == 200))) {
			returnval=true;
	}*/


	return returnval;
}


/*********************************************************************************
* Name:	is_valid_clear_snex()
* Purpose: check if the pixel is valid or
*
* Args:
*	  @b02		- band 2 value
*	  @b03		- band 3 value
 *	  @b04		- band 4 value
*	  @b05		- band 5 value
*	  @sat 		- saturation value
*	  @acca   	- acca score
*	  @dtcld    - decision tree cloud score
*
* Calls: main();
*********************************************************************************/
bool is_valid_clear_snex( int16 b02, int16 b03, int16 b04, int16 b05,
						  uint8 sat, uint8 acca, uint dtcld){

	bool returnval=false;

	if(b03==int16_minus32768){
		return returnval;
	}
	if(((sat >> 0) & 1) ||
	   ((sat >> 2) & 1) ||
	   ((sat >> 3) & 1) ||
	   ((sat >> 4) & 1) ){
		return returnval;
	}
	/* STRICT screening */
	if((acca == 0) &&
	   ((dtcld == 0) || (dtcld == 200))) {
		returnval=true;
	}

	if((acca == 0) && 							//acca clear
	   ((dtcld == 1) || (dtcld == 2)) &&  		//dt cloudy or edge
	   ((b02<=b03) && (b03<=b04) && (b04<=b05)) //soil
		) {
		returnval=true;
	}

	/* Relaxed screening
	if((acca == 0)) {
		returnval=true;
	}
	if(((dtcld == 0) || (dtcld == 2) || (dtcld == 200))) {
			returnval=true;
	}*/


	return returnval;
}


/*********************************************************************************
* Name:	rearrange_l457acq_fnames()
* Purpose: sorts the acquisitions from low to high clarity within the tile
*
* Args:
*	  @all_acq_fname- array od acq_filenames structures (MUST BE ALLOCATED ALREADY)
*	  @nacq 		- number of acq in all_acq_fname array
*
* Calls: main()
*********************************************************************************/

void rearrange_l457acq_fnames(l457acq_fname_t *all_acq_fname, int nacq,
		    long* Nnofillpxs, long* Nvalidpxs, int16* zone_index){
	l457acq_fname_t temp;
	int i,j;
	double iclr, jclr;
	long tmpNNF, tmpNVL;
	int16 tmpZI;
	/* A bubble sort of the yyyymmdd strings is good enough since the number of strings is small */

	for (i = 0; i < nacq-1; i++) {
		for (j = i+1; j < nacq; j++) {
			iclr=jclr=0.0;
			iclr=(double)(Nvalidpxs[i])/(double)(Nvalidpxs[i]);
			jclr=(double)(Nvalidpxs[j])/(double)(Nvalidpxs[j]);
			if(iclr>jclr) {
				/* SWAPPING */
				tmpNNF=Nnofillpxs[i];
				tmpNVL=Nvalidpxs[i];
				tmpZI=zone_index[i];

				Nnofillpxs[i]=Nnofillpxs[j];
				Nvalidpxs[i]=Nvalidpxs[j];
				zone_index[i]=zone_index[j];

				Nnofillpxs[j]=tmpNNF;
				Nvalidpxs[j]=tmpNVL;
				zone_index[j]=tmpZI;

				strcpy(temp.acq_fname, all_acq_fname[i].acq_fname);
				strcpy(all_acq_fname[i].acq_fname, all_acq_fname[j].acq_fname);
				strcpy(all_acq_fname[j].acq_fname, temp.acq_fname);

				printf(" Swap : %s \n with %s \n", all_acq_fname[i].acq_fname, all_acq_fname[j].acq_fname);

				strcpy(temp.accafname, all_acq_fname[i].accafname);
				strcpy(all_acq_fname[i].accafname, all_acq_fname[j].accafname);
				strcpy(all_acq_fname[j].accafname, temp.accafname);

				strcpy(temp.dtclfname, all_acq_fname[i].dtclfname);
				strcpy(all_acq_fname[i].dtclfname, all_acq_fname[j].dtclfname);
				strcpy(all_acq_fname[j].dtclfname, temp.dtclfname);

				strcpy(temp.geomfname, all_acq_fname[i].geomfname);
				strcpy(all_acq_fname[i].geomfname, all_acq_fname[j].geomfname);
				strcpy(all_acq_fname[j].geomfname, temp.geomfname);

			}
		}
	}

}

