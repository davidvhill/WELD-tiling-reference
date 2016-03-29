/*  
	The map coordinate system related functions used by various WELD modules. 
	They include the setup of the projection system, adding the corner 
	coordinates of a WELD tile, and map projection conversion.

	Functions for set up both v1.5 and v2.0 coordinate systems are included.

    	Junchang Ju 04/26/2011


*/

#ifndef WELD_PROJECTION_H
#define WELD_PROJECTION_H


#include <mfhdf.h>
#include <ctype.h>
#include "error.h"
#include "cproj.h"
#include "proj.h"
#include "weld.h"
#include "L457.comp.tile.handler.h"


int setup_weld_v1point5_proj_para(albers_proj_para_t *albers, char *region);
int setup_weld_v1point6_proj_para(albers_proj_para_t *albers, char *region);
int setup_sinus_proj_para(sinus_proj_para_t *sinus, char *region);

int add_corner_coordinate(tile_info_t *tile, albers_proj_para_t albers_para);
int add_corner_coordinates_2_0(int32 tile_sd_id, int tile_h, int tile_v,
		                float32 pixel_size, albers_proj_para_t albers_para );
int world_add_corner_coordinates(int32 tile_sd_id, int tile_h, int tile_v,
		                                                 float32 pixel_size);

void albers2latlon(albers_proj_para_t albers_para, double ax, double ay,
		                                              double* lon, double* lat);
void albers2sinusoidal(albers_proj_para_t albers_para, double ax, double ay,
														double *sx, double *sy);
void albers2utm(albers_proj_para_t albers_para, double ax, double ay,
		 	 	 	 	 	 	 	 	  int utm_zone, double* ux, double* uy);
void utm2albers(int utm_zone, double ux, double uy,
					   albers_proj_para_t albers_para,  double* ax, double* ay);
void latlon2albers(albers_proj_para_t albers_para, double lon, double lat,
														double *ax, double *ay);
void utm2sinus(int utm_zone, double ux, double uy, sinus_proj_para_t sinus_para,
														double* ax, double* ay);
void sinus2utm(sinus_proj_para_t sinus_para, double ax, double ay, int utm_zone,
														double* ux, double* uy);

#endif
