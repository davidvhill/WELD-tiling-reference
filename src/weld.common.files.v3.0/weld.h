#ifndef WELD_H
#define WELD_H

#include <mfhdf.h>	/* Use hdf data types later */

/* The following are defined as variables instead of macros because sometimes they 
   are used in HDF SDsetattr function. But they have to be static because the header
   file is included in multiple files.
*/ 


// static uint32 uint32_0 = 0;
// static int16 int16_minus32768 = -32768;
// static int16 int16_minus2000 = -2000;
// static int16 int16_32767 = 32767;
// static uint8 uint8_255 = 255;
// static uint8 uint8_0 = 0;
// static uint16 uint16_65535 = 65535;
// static uint16 uint16_0 = 0;
// static int16 int16_0 = 0;

extern uint32 uint32_0        ;
extern int16  int16_minus32768 ;
extern int16  int16_minus2000  ;
extern int16  int16_32767      ;
extern uint8  uint8_255        ;
extern uint8  uint8_0          ;
extern uint16 uint16_65535    ;
extern uint16 uint16_0        ;
extern int16  int16_0          ;

#ifndef PI
#define PI 3.14159265358979323846 
#endif

/*
#define NDVIC   0.283
#define VI53C 	0.717


#define NDVIC   0.3897281282
#define VI53C 	0.6102718718

#define NDVIC    0.4387548683
#define VI53C 	0.5612451317
*/

#define NDVIC   0.283
#define VI53C 	0.717


typedef enum{false, true} bool;


/*MAX_STR_LEN is deleted. Just use MAX_LINE_LEN*/
#define MAX_LINE_LEN 5000
#define MAX_NAME_LEN 500
#define COMPRESS_LEVEL 2               /*HDF internal compression: 1 least, and 9 max*/

#define NMINI 7


/*Tile dimension: the number of rows/column in the square tile
 *#define TILEDIM 5000
 * */


/*World minitile dimension
 * #define WORLD_TILEDIM 5295
 */
/*****************************************************************
 *  REDEFINE FOR WORLD
 *#define TILEDIM WORLD_TILEDIM
 *****************************************************************/
#define WORLD_TILEDIM 5295
#define TILEDIM 5000


#define SRF_L1TI_FLAG 1
#define SRF_GEOM_FLAG 1
#define SRF_NBARSZ_FLAG 1

#define REF_PIXSIZE 30
#define THM_PIXSIZE 60
#define WELD_PIXSIZE 30

/* The spacing of the triangles in simplicial */
#define SIMPLICIAL_SPACING 15



#define NTILE_H_CONUS 33  /*horizontally*/
#define NTILE_V_CONUS 22  /*vertically*/ 

#define NTILE_H_ALASKA 17  /*horizontally*/
#define NTILE_V_ALASKA 14  /*vertically*/ 

/* Some thresholds in compositing UTM acquisitions*/
#define HIGH_NDVI  5000
#define NONVEG_NDVI 900  /*When a pixel has ndvi and band 7 less than those thresholds, it is nonveg*/ 
#define NONVEG_B7   480

typedef struct {
	/* 12 file at most; geometry is optional. Do not open a fil
	if a filename has a zero length.
	*/
	char b1fname[MAX_NAME_LEN];
	char b2fname[MAX_NAME_LEN];
	char b3fname[MAX_NAME_LEN];
	char b4fname[MAX_NAME_LEN];
	char b5fname[MAX_NAME_LEN];
	char b61fname[MAX_NAME_LEN];
	char b62fname[MAX_NAME_LEN];
	char b7fname[MAX_NAME_LEN];
	char satfname[MAX_NAME_LEN];
	char dtcldfname[MAX_NAME_LEN];
	char accafname[MAX_NAME_LEN];
	char geomfname[MAX_NAME_LEN];
  
	int16 *b1;
	int16 *b2;
	int16 *b3;
	int16 *b4;
	int16 *b5;
	int16 *b61;
	int16 *b62;
	int16 *b7;
	uint8 *sat;
	uint8 *dtcld;
	uint8 *acca;
	int16 *vz;
	int16 *va;
	int16 *sz;
	int16 *sa;
  
	float64 ulx, uly;             /*the center of the upper-left pixel in the input acquisition*/
	float64 xmin_utm, ymax_utm;   /*the upper-left corner of the upper-left pixel in the input acquition. 1/2 pixel difference*/ 
	int16 utm_zone;
	float32 pixel_size; 
	int16 nrow_ref, ncol_ref; 
	int16 nrow_thm, ncol_thm; 
	int16 doy;
	float32 center_sz;
	float32 center_sa;
} acq_info_t;


typedef struct {
	char fname[MAX_NAME_LEN];
	int tile_h;   /* Need tile ID to add the corner coordinates */
	int tile_v;
	int nrow;
	int ncol;
	char geometry_flag;      /* 1: output geometry; 0: do not output geometry */
	char l1t_index_flag;     /* 1: write an l1t_index SDS; 0: do not */
	float32 pixel_size;
	int n_l1t;

	int32 sd_id;
	int32 sds_id_b1;
	int32 sds_id_b2;
	int32 sds_id_b3;
	int32 sds_id_b4;
	int32 sds_id_b5;
	int32 sds_id_b61;
	int32 sds_id_b62;
	int32 sds_id_b7;
	int32 sds_id_ndvi;
	int32 sds_id_doy;
	int32 sds_id_sat;
	int32 sds_id_dtcld;
	int32 sds_id_acca;
	int32 sds_id_nobs;
	int32 sds_id_vz;
	int32 sds_id_va;
	int32 sds_id_sz;
	int32 sds_id_sa;
	int32 sds_id_l1t_index;
  
	int16 *b1;
	int16 *b2;
	int16 *b3;
	int16 *b4;
	int16 *b5;
	int16 *b61;
	int16 *b62;
	int16 *b7;
	int16 *ndvi;
	int16 *doy;
	uint8 *sat;
	uint8 *dtcld;
	uint8 *acca;
	uint8 *nobs;
	int16 *vz;
	int16 *va;
	int16 *sz;
	int16 *sa;
	uint8 *l1t_index;
	char  *l1t_metadata;
} tile_info_t;

#define ALLDOYMAX   27
/* 	In WELD TOA composites there are 18 SDS at most; if the 4 geometry SDS are
    	not desired, there will 14.
*/
#define NSDS 24
/* NSDS+1 */
#define NSDS_MAX 25



/*  Sinusoidal coordinate system*/
typedef struct
{
    double radius;
    double centmer;

    double sinus_ULX;           /*   The upper-left corner X, Y of the upper-left pixel*/
    double sinus_ULY;           /*   The upper-left corner X, Y of the upper-left pixel*/
    double modis_tile_size;     /* = 1111950.519667;*/
} sinus_proj_para_t;

/*  Albers coordinate system: projection parameters and UL corner coordinates*/
typedef struct
{
    double first_lat;      /*   First standard parallel*/
    double second_lat;     /*   Second standard parallel*/
    double central_lon;    /*   Central meridian*/
    double origin_lat;      /*   Latitude of projection origin*/

    double Albers_ULX;     /*   The upper-left corner X, Y of the upper-left pixel*/
    double Albers_ULY;
} albers_proj_para_t;

#endif
