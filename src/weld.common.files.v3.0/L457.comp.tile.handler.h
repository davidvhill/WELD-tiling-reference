#ifndef L457_COMP_TILE_HANDLER_H
#define L457_COMP_TILE_HANDLER_H

#include <mfhdf.h>
#include "error.h"
#include "weld.projection.h"
#include "weld.h"
#include "weld.setup.sds.h"
#include "weld.hdf.eos.h"
//#include "select_obs.h"
#include "sds.name.def.h"
#include "weld.util.h"

/* The content of an observation; used to compare two observations in composition */
typedef struct {
	int16 b01;
	int16 b02;
	int16 b03;
	int16 b04;
	int16 b05;
	int16 b61;
	int16 b62;
	int16 b07;
	int16 ndvi;
	int16 doy;
	uint8 sat;
	uint8 dtcld;
	uint8 acca;
	int16 vz;
	int16 va;
	int16 sz;
	int16 sa;
	uint8 qa_comp;
	uint8 lsnsr_i;
} l457_obs_t;



typedef struct {
	char fname[MAX_NAME_LEN];
	int tile_h;   /* Need tile ID to add the corner coordinates */
	int tile_v;
	int nrow;
	int ncol;
	char geometry_flag;      /* 1: output geometry; 0: do not output geometry */
	char l1t_index_flag;     /* 1: write an l1t_index SDS; 0: do not */
	uint8 as_pointers;        /* 1: chopped version of product will be read/written; 0: normal version of product */
	float32 pixel_size;
	int n_l1t;

	int32 sd_id;
	int32 sds_id_b01;
	int32 sds_id_b02;
	int32 sds_id_b03;
	int32 sds_id_b04;
	int32 sds_id_b05;
	int32 sds_id_b61;
	int32 sds_id_b62;
	int32 sds_id_b07;
	int32 sds_id_ndvi;
	int32 sds_id_doy;
	int32 sds_id_sat;
	int32 sds_id_dtcld;
	int32 sds_id_acca;
	int32 sds_id_nobs;
	int32 sds_id_cpath;
	int32 sds_id_senrs;
	int32 sds_id_vz;
	int32 sds_id_va;
	int32 sds_id_sz;
	int32 sds_id_sa;
	int32 sds_id_l1t_index;
	int32 sds_id_l1t_col;
	int32 sds_id_l1t_row;

	int16  *b01;
	int16  *b02;
	int16  *b03;
	int16  *b04;
	int16  *b05;
	int16  *b61;
	int16  *b62;
	int16  *b07;
	int16  *ndvi;
	int16  *doy;
	uint8  *sat;
	uint8  *dtcld;
	uint8  *acca;
	uint16 *nobs;
	uint8  *cpath;  /*this is QA sds for new compositing algorithm */
	uint8  *senrs;  /* this is SDS showing Landsat sensor index
	 	 	 	 	* i.e. 7 means Landsat ETM+ */
	int16  *vz;
	int16  *va;
	int16  *sz;
	int16  *sa;
	uint16 *l1t_index;
	uint16 *l1t_col;
	uint16 *l1t_row;
	char   *l1t_metadata;
	char   *inp_pointers;

} l457_tile_info_t;


/*	Keep track of all the input DOY in the UTM data for each pixel
	on the tile to avoid double counting the same-day within-path overlap.
    	Used in compositing the UMT data to generate weekly or montly composite.
*/

typedef struct {
	char fname[MAX_NAME_LEN];
	int nrow;
	int ncol;
	int32 sd_id;
	int32 sds_id_alldoy[ALLDOYMAX];
	uint16 *alldoy;
} alldoy_t;







/* 	In WELD TOA composites there are 18 SDS at most; if the 4 geometry SDS are
    	not desired, there will 14. (check weld.h for NSDS)
*/
sds_info_t  all_comp_sds[NSDS_MAX];
int  l457_tile_open(l457_tile_info_t *tile);
int  l457_tile_add_cornes(l457_tile_info_t *tile, char * region);
int  l457_tile_add_meta(l457_tile_info_t *tile);
int  write_ArchiveMetadata( int32 sd_id, char **attr_names, char **attr_values, int num_attr);
int  l457_tile_create(l457_tile_info_t *tile, sds_info_t *all_sds, char* region, int *tot_sds);
int  l457_tile_allocate(l457_tile_info_t *tile);
void l457_tile_update(l457_tile_info_t *tile, int k_tile, l457_obs_t *obs,
		              int acq_index, int acq_col, int acq_row);
int l457_tile_write(l457_tile_info_t *tile);
int l457_tile_close(l457_tile_info_t *tile);

int l457_tile_load_l1tinfo(char *l1t_metadata, char ***all_l1tinfo, int *all_nacq, int *new_nacq);
int l457_tile_load_inp_pointers(char *inp_pointers, char ***all_inp_pointers,
		                        int *all_inpntrs, int *new_inpntrs);
int l457_tile_write_l1tinfo(l457_tile_info_t *tile, char **l1tinfo, char **inp_pointers, int nacq);
int l457_tile_write_l1tinfo_alt(l457_tile_info_t *tile, char **l1tinfo, char **inp_pointers, char *use_flags, int nacq);
int l457_tile_update_comp_l1t_index(l457_tile_info_t *tile,char **l1tinfo,	char **inp_pointers,
				                    char *this_acq_is_used,	int nacq, int index_fill);



int  l457_pointer_tile_open(l457_tile_info_t *tile);
int  l457_pointer_tile_create(l457_tile_info_t *tile, sds_info_t *all_sds, char* region, int *tot_sds);
int  l457_pointer_tile_allocate(l457_tile_info_t *tile);
int l457_pointer_tile_write(l457_tile_info_t *tile);







int  alldoy_tile_create(alldoy_t *alldoy);
int  alldoy_tile_open(alldoy_t *alldoy);
int  alldoy_tile_update(int16 doy,  alldoy_t *alldoy, int k_tile);
int  alldoy_tile_write(alldoy_t *alldoy);
int  alldoy_tile_close(alldoy_t *alldoy);
int  isRedundant(int16 doy, alldoy_t *alldoy, int k_tile);

void copy_from_old_comp(l457_tile_info_t *new, l457_tile_info_t *old);
void copy_from_old_alldoy(alldoy_t *new, alldoy_t *old);
void doy_range(l457_tile_info_t * tile, int16 *doymin, int16 *doymax);


#endif
