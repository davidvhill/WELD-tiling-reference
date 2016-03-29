#ifndef WELD_SETUP_SDS_H
#define WELD_SETUP_SDS_H

#include <mfhdf.h>
#include "weld.h"
#include "error.h"
#include "sds.name.def.h"
/*  Some attribute information of an SDS in the tile hdf. */ 
typedef struct
{
    char name[MAX_NAME_LEN];
    char data_type_name[MAX_NAME_LEN];
    int32 data_type;

    char units[100];
    int32 valid_min, valid_max;
    float64 scale_factor;
    char *qa; 	/* the meaning of the values in the mask */
} sds_info_t;

int set_blank_qa_sds_info(sds_info_t *all_sds);
int set_free_qa_sds_info(sds_info_t *all_sds);
int setup_pointer_sds_info(sds_info_t *all_sds, int * tot_sds);
int setup_sds_info(sds_info_t *all_sds, char geometry_flag, char l1t_index_flag, int *tot_sds);
int set_sds_attribute(int32 sds_id, char *sds_name, sds_info_t *all_sds, int nsds_max);
int setup_srf_sds_info(sds_info_t *all_sds, char geometry_flag, char nbar_sz_flag, char l1t_index_flag, int * tot_sds);


#endif
