#ifndef WELD_ACQ_L4547_HANDLER_H
#define WELD_ACQ_L4547_HANDLER_H

/*  Called by the toa code to read HDF or write a binary array to an HDF.   
   
    gcc -c read_write_hdf.c -I $HDFINC
 

 See toa.c for how to compile.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "error.h"
#include "sds.name.def.h"
#include "weld.util.h"

#ifndef MAX_NAME_LEN
#define MAX_NAME_LEN 500
#endif

#ifndef MAX_LINE_LEN
#define MAX_LINE_LEN 500
#endif


typedef struct {
	uint8 isTM;
	int32 sd_id;
	int32 sds_b01;
	int32 sds_b02;
	int32 sds_b03;
	int32 sds_b04;
	int32 sds_b05;
	int32 sds_b61;
	int32 sds_b62;
	int32 sds_b07;
	int32 sds_sat;
	int32 sds_sz;
	int32 sds_sa;
	int32 sds_vz;
	int32 sds_va;

	int16 *b01;
	int16 *b02;
	int16 *b03;
	int16 *b04;
	int16 *b05;
	int16 *b61;
	int16 *b62;
	int16 *b07;
	uint8 *sat;
	int16 *sz;
	int16 *sa;
	int16 *vz;
	int16 *va;
	uint8 *acca;
	uint8 *dtcld;

	float ulx, uly;             /*the center of the upper-left pixel in the input acquisition*/
	float xmin_utm, ymax_utm;   /*the upper-left corner of the upper-left pixel in the input acquition. 1/2 pixel difference*/
	int16 utm_zone;
	float pixel_size;
	int32 nrow_ref, ncol_ref;
	int32 nrow_thm, ncol_thm;
	int16 year;
	int16 month;
	int16 doy;
	int16 day;
	int16 pth;
	int16 row;
	float center_sz;
	float center_sa;
	char *mtl_cont;
	char input_pointer[MAX_NAME_LEN];
	char product_type[256];
	char acq_hdf[MAX_NAME_LEN];
	char geomhdf[MAX_NAME_LEN];
	char accahdf[MAX_NAME_LEN];
	char dtclhdf[MAX_NAME_LEN];
} l457_acq_info_t;

typedef struct {
	char acq_fname[MAX_NAME_LEN];
	char geomfname[MAX_NAME_LEN];
	char accafname[MAX_NAME_LEN];
	char dtclfname[MAX_NAME_LEN];
} l457acq_fname_t;


int  acq_create(char* fname_mtl,char * outdir, l457_acq_info_t * acq, uint8 geom_flag);
int  acq_init_attr(char* fname_mtl,l457_acq_info_t * acq, int* mtl_len);
int acq_load_geom(char *fname_geom, int nrow, int ncol, int16 *sz,
		          int16 *sa, int16 *vz, int16 *va);
int  acq_load_acca_dtcld(char *fname_acca, char *fname_dtcld,
                         int nrow, int ncol, uint8 *acca, uint8 *dtcld);
int acq_get_inp_pointer(char *fname_hdf, char *inp_pntr, char *attr_name);
int  acq_close(l457_acq_info_t * acq);
void acq_set_blank(l457_acq_info_t * acq);
int  acq_write(l457_acq_info_t * acq, uint8 geom_flag);
int  acq_grab(l457_acq_info_t * this_acq, uint8 geom_flag, uint8 cldm_flag);

int  get_l457acq_number(char *acq_list, int *nacq);
int  get_l457acq_fnames(char *acq_list, l457acq_fname_t *all_acq_fname, int nacq);

void sort_l457acq_fnames(l457acq_fname_t *all_acq_fname, int nacq);


#endif
