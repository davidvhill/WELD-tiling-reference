/*
 * weld.comp.util.h
 *
 *  Created on: Nov 11, 2014
 *      Author: valeriy.kovalskyy
 *
 *   Purpose:  service of compositing process
 *   functionality is not directly related to tiles or acquisitions
 *
 *
 */
#ifndef COMP_UTIL_H
#define COMP_UTIL_H

#include <mfhdf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "weld.projection.h"
#include "weld.h"
#include "L457.toa.acq.handler.h"
typedef struct
{
 double a1, b1, c1;
 double a2, b2, c2;
} simplicial_t;




bool is_valid_clear(int16 b03, uint8 sat, uint8 acca, uint dtcld);
bool is_valid_clear_rdt( int16 b03, uint8 sat, uint8 acca, uint dtcld);
bool is_valid_clear_snex( int16 b02, int16 b03, int16 b04, int16 b05,
						  uint8 sat, uint8 acca, uint dtcld);
/* **********************************************************************
 * Lets make sinus para global and there will be no need to change
 * tested Junchangs code
 * **********************************************************************/
sinus_proj_para_t  sinus_global; /*  original name sinus_para is causing conflocts*/
albers_proj_para_t albers_global;

bool isInTriang(simplicial_t triang, double x, double y, double *s1, double *s2, double *s3);
void compute_simplicial_abc(double x1, double y1, double x2, double y2, double x3, double y3, simplicial_t *triang);

int asign_tileHV(char *tile_id, int* tileH, int* tileV, char *region, char *message);
int read_utm_zone(int16 *zone,  char *fname_b1);
int isEdgeTile(int itile_h, int itile_v, char *inside_mask);
void gctp_utm_xy_sinusoidal(int itile_h, int itile_v, char *inside_mask, int utm_zone, float64 *utm_x, float64 *utm_y);

void interpolate_utm_xy_albers(albers_proj_para_t albers_para,
			int utm_zone,
			float32 pixel_size,
			int tiledim_nrow,
			int tiledim_ncol,
			int tile_h,
			int tile_v,
			int spacing,
			double *utm_x,
			double *utm_y);

void interpolate_utm_xy_to_sinusoidal(int utm_zone,
                        float32 pixel_size,
                        int tiledim_nrow,
                        int tiledim_ncol,
                        int itile_h,
                        int itile_v,
                        int spacing,
                        float64 *utm_x,
                        float64 *utm_y);


int match_l1t_to_srfacq(l457acq_fname_t *all_srfinfo_acq, char **all_l1tinfo_tile,
		                int nacq_from_toa_tile, char *srf_acq_list, uint8 in_list_option );

int tile_write_extra_sds(l457_tile_info_t* comp, char *sds_name, int32 sds_type, VOIDP sdscontent);

void set_cpath(l457_tile_info_t* comp, uint8* validCounts, uint8* waterCounts, uint8* soilCounts, uint8* snowCounts);

void rearrange_l457acq_fnames(l457acq_fname_t *all_acq_fname, int nacq, long* Nnofillpxs, long* Nvalidpxs, int16* zone_index);

#endif
