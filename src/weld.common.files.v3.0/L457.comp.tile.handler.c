/*  The input and output functions used by the compositing code*/

#include "L457.comp.tile.handler.h"


/********************************************************************************
	Open the tile in DFACC_READ mode and read the various SDS into corresponding
	buffers.

	Return 0 on success, and non-zero on failure.
*/
int l457_tile_open(l457_tile_info_t *tile)
{
	/*int32 n_datasets, n_file_attrs;*/
	int32 attr_index, sds_index;
	int32 n_values;
	char attr_name[MAX_NAME_LEN];
	char sds_name[MAX_NC_NAME];
	int32 dim_sizes[2];
	int32 rank, data_type, n_attrs;
	int32 start[2], edge[2];
	int status = 0; 
	char message[MSGLEN];
	sds_index=FAIL;
	if ((status = l457_tile_allocate(tile)) != 0) {
		Error("Error in allocate_tile", "l457_tile_open", 0);
		return status;
	}

	if ((tile->sd_id = SDstart(tile->fname, DFACC_READ)) == FAIL) {
		status = 2;
		sprintf(message, "Cannot open %s for DFACC_WRITE", tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
   
	/*  b01*/
	strcpy(sds_name, B01_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_b01 =  SDselect(tile->sd_id, sds_index);
   
	/* b02*/
	strcpy(sds_name, B02_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_b02 =  SDselect(tile->sd_id, sds_index);
   
	/* b03*/
	strcpy(sds_name, B03_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_b03 =  SDselect(tile->sd_id, sds_index);
   
	/*  b04*/
	strcpy(sds_name, B04_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_b04 =  SDselect(tile->sd_id, sds_index);
   
	/*  b05*/
	strcpy(sds_name, B05_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 5;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_b05 =  SDselect(tile->sd_id, sds_index);
   
	/*  b61*/
	strcpy(sds_name, B61_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_b61 =  SDselect(tile->sd_id, sds_index);
   
	/*  b62*/
	strcpy(sds_name, B62_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_b62 =  SDselect(tile->sd_id, sds_index);
   
	/*  b07*/
	strcpy(sds_name, B07_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_b07 =  SDselect(tile->sd_id, sds_index);
   
	/*  ndvi*/
	strcpy(sds_name, NDVI_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_ndvi =  SDselect(tile->sd_id, sds_index);
   
	/* doy*/
	strcpy(sds_name, DOY_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_doy =  SDselect(tile->sd_id, sds_index);
   
	/*  saturation*/
	strcpy(sds_name, SAT_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_sat = SDselect(tile->sd_id, sds_index);
   
	/*  dtcld*/
	strcpy(sds_name, DTCLD_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_dtcld = SDselect(tile->sd_id, sds_index);
   
	/* acca*/
	strcpy(sds_name, ACCA_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_acca = SDselect(tile->sd_id, sds_index);
   
	/* nobs*/
	strcpy(sds_name, NOBS_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_nobs =  SDselect(tile->sd_id, sds_index);

	/* cpath*/
	strcpy(sds_name, CPATH_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_cpath =  SDselect(tile->sd_id, sds_index);

	/* senrs */
	strcpy(sds_name, SENRS_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_senrs = SDselect(tile->sd_id, sds_index);

	/* GEOMETRY */
	if(tile->geometry_flag==1){
		/*  vz*/
		strcpy(sds_name, VZ_SDS_NAME);
		if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
			status = 2;
			sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
			Error(message, "l457_tile_open", status);
			goto CLEANUP;
		}
		tile->sds_id_vz = SDselect(tile->sd_id, sds_index);

		/*  va*/
		strcpy(sds_name, VA_SDS_NAME);
		if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
			status = 2;
			sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
			Error(message, "l457_tile_open", status);
			goto CLEANUP;
		}
		tile->sds_id_va= SDselect(tile->sd_id, sds_index);

		/*  sz*/
		strcpy(sds_name, SZ_SDS_NAME);
		if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
			status = 2;
			sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
			Error(message, "l457_tile_open", status);
			goto CLEANUP;
		}
		tile->sds_id_sz = SDselect(tile->sd_id, sds_index);

		/*  sa*/
		strcpy(sds_name, SA_SDS_NAME);
		if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
			status = 2;
			sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
			Error(message, "l457_tile_open", status);
			goto CLEANUP;
		}
		tile->sds_id_sa= SDselect(tile->sd_id, sds_index);
	}

	/* 1lt index */
	if(tile->l1t_index_flag==1){

		strcpy(sds_name, L1T_INDEX_SDS_NAME);
		if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
			status = 2;
			sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
			Error(message, "l457_tile_open", status);
			goto CLEANUP;
		}
		tile->sds_id_l1t_index = SDselect(tile->sd_id, sds_index);

		strcpy(sds_name, L1T_COL_SDS_NAME);
		if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
			status = 2;
			sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
			Error(message, "l457_tile_open", status);
			goto CLEANUP;
		}
		tile->sds_id_l1t_col = SDselect(tile->sd_id, sds_index);

		strcpy(sds_name, L1T_ROW_SDS_NAME);
		if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
			status = 2;
			sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
			Error(message, "l457_tile_open", status);
			goto CLEANUP;
		}
		tile->sds_id_l1t_row = SDselect(tile->sd_id, sds_index);

	}

	/*  Dimension of the tile */
	status = SDgetinfo(tile->sds_id_b01, sds_name, &rank, dim_sizes, &data_type, &n_attrs);
	start[0] = 0; edge[0] = dim_sizes[0];
	start[1] = 0; edge[1] = dim_sizes[1];
	if (dim_sizes[0] != tile->nrow || dim_sizes[1] != tile->ncol) {
		status = 2;
		sprintf(message, "The tile dimension of %s are not nrowi = %d, ncol = %d", tile->fname, tile->nrow, tile->ncol);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
   
	if (( SDreaddata(tile->sds_id_b01,	 start, NULL, edge, tile->b01)    == FAIL) ||
	    ( SDreaddata(tile->sds_id_b02,	 start, NULL, edge, tile->b02)    == FAIL) ||
	    ( SDreaddata(tile->sds_id_b03,	 start, NULL, edge, tile->b03)    == FAIL) ||
	    ( SDreaddata(tile->sds_id_b04,	 start, NULL, edge, tile->b04)    == FAIL) ||
	    ( SDreaddata(tile->sds_id_b05,	 start, NULL, edge, tile->b05)    == FAIL) ||
	    ( SDreaddata(tile->sds_id_b61,	 start, NULL, edge, tile->b61)    == FAIL) ||
	    ( SDreaddata(tile->sds_id_b62,	 start, NULL, edge, tile->b62)    == FAIL) ||
	    ( SDreaddata(tile->sds_id_b07,	 start, NULL, edge, tile->b07)    == FAIL) ||
	    ( SDreaddata(tile->sds_id_ndvi,  start, NULL, edge, tile->ndvi)   == FAIL) ||
	    ( SDreaddata(tile->sds_id_doy,	 start, NULL, edge, tile->doy)    == FAIL) ||
	    ( SDreaddata(tile->sds_id_sat,	 start, NULL, edge, tile->sat)    == FAIL) ||
	    ( SDreaddata(tile->sds_id_dtcld, start, NULL, edge, tile->dtcld)  == FAIL) ||
	    ( SDreaddata(tile->sds_id_acca,  start, NULL, edge, tile->acca)   == FAIL) ||
	    ( SDreaddata(tile->sds_id_nobs,  start, NULL, edge, tile->nobs)   == FAIL) ||
	    ( SDreaddata(tile->sds_id_cpath, start, NULL, edge, tile->cpath)  == FAIL) ||
	    ( SDreaddata(tile->sds_id_senrs, start, NULL, edge, tile->senrs)   == FAIL) ){
		status = 2;
		sprintf(message, "Cannot read bands and qa from %s", tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}

	if(tile->geometry_flag==1){
		if((SDreaddata(tile->sds_id_vz,   start, NULL, edge, tile->vz)    == FAIL) ||
		   (SDreaddata(tile->sds_id_va,   start, NULL, edge, tile->va)    == FAIL) ||
		   (SDreaddata(tile->sds_id_sz,   start, NULL, edge, tile->sz)    == FAIL) ||
		   (SDreaddata(tile->sds_id_sa,   start, NULL, edge, tile->sa)    == FAIL)){
			status = 2;
			sprintf(message, "Cannot read geometry from %s", tile->fname);
			Error(message, "l457_tile_open", status);
			goto CLEANUP;
		}
	}
	if(tile->l1t_index_flag==1){
		if((SDreaddata(tile->sds_id_l1t_index, start, NULL, edge, tile->l1t_index) == FAIL) ||
		   (SDreaddata(tile->sds_id_l1t_col, start, NULL, edge, tile->l1t_col) == FAIL) ||
		   (SDreaddata(tile->sds_id_l1t_row, start, NULL, edge, tile->l1t_row) == FAIL) ){
			status = 2;
			sprintf(message, "Cannot read l1t_index from %s", tile->fname);
			Error(message, "l457_tile_open", status);
			goto CLEANUP;
		}

		/*L1T_INFO*/
		attr_index = SDfindattr(tile->sd_id, L1T_METADATA);
		if (attr_index == FAIL) {
			status = 2;
			sprintf(message, "No attribute %s in %s", L1T_METADATA, tile->fname);
			Error(message, "l457_tile_open", status);
			goto CLEANUP;
		}
		
		if (SDattrinfo(tile->sd_id, attr_index, attr_name, &data_type, &n_values) == FAIL) {
			status = 2;
			sprintf(message, "SDattrinfo failed for %s", tile->fname);
			Error(message, "l457_tile_open", status);
			goto CLEANUP;
		}


		if ((tile->l1t_metadata = (char*)calloc(n_values+1, sizeof(char))) == NULL) {
			status = 4;
			Error("Cannot allocate memory", "l457_tile_open", status);
                        goto CLEANUP;
		}

		if (SDreadattr(tile->sd_id, attr_index, tile->l1t_metadata) == FAIL) {
			status = 2;
			sprintf(message, "SDreadattr failed for %s", tile->fname);
			Error(message, "l457_tile_open", status);
                        goto CLEANUP;
		}

		tile->l1t_metadata[n_values] = '\0';

		/*INP_PNTRS*/
		attr_index = SDfindattr(tile->sd_id, INP_POINETR);
		if (attr_index == FAIL) {
			status = 2;
			sprintf(message, "No attribute %s in %s", INP_POINETR, tile->fname);
			Error(message, "l457_tile_open", status);
			goto CLEANUP;
		}

		if (SDattrinfo(tile->sd_id, attr_index, attr_name, &data_type, &n_values) == FAIL) {
			status = 2;
			sprintf(message, "SDattrinfo failed for %s", tile->fname);
			Error(message, "l457_tile_open", status);
			goto CLEANUP;
		}


		if ((tile->inp_pointers = (char*)calloc(n_values+1, sizeof(char))) == NULL) {
			status = 4;
			Error("Cannot allocate memory", "l457_tile_open", status);
                        goto CLEANUP;
		}

		if (SDreadattr(tile->sd_id, attr_index, tile->inp_pointers) == FAIL) {
			status = 2;
			sprintf(message, "SDreadattr failed for %s", tile->fname);
			Error(message, "l457_tile_open", status);
                        goto CLEANUP;
		}
		tile->inp_pointers[n_values] = '\0';

	}

	CLEANUP:
	return status;
}

/*******************************************************************************
NAME:     l457_tile_add_cornes()

PURPOSE:  Store coordinates of appropriate projection in the hdf

Args:
         tile  -  pointer to the tile structure
         region-  region where tile belongs
*******************************************************************************/

int  l457_tile_add_cornes(l457_tile_info_t *tile, char * region){
	int status=0;
	sinus_proj_para_t  sinus_para;
	albers_proj_para_t albrs_para;
	char message[MSGLEN];
	if(setup_sinus_proj_para(&sinus_para, region)==0){
		if(world_add_corner_coordinates(tile->sd_id, tile->tile_h, tile->tile_v, tile->pixel_size)!=0){
			status = 2;
			sprintf(message, "Problem in l457_tile_add_cornes() for %s", tile->fname);
			Error(message, "l457_tile_add_cornes", status);
		}
	}else{
		setup_weld_v1point6_proj_para(&albrs_para, region);
		if(add_corner_coordinates_2_0(tile->sd_id, tile->tile_h, tile->tile_v,
							          tile->pixel_size, albrs_para)!=0){
			status = 2;
			sprintf(message, "Problem in l457_tile_add_cornes() for %s", tile->fname);
			Error(message, "l457_tile_add_cornes", status);
		}
	}
	return status;
}

/*******************************************************************************
NAME:     l457_tile_add_meta()

PURPOSE:  Store metadata for the tile

Args:
         tile  -  pointer to the tile structure
CALLS:
*******************************************************************************/

int  l457_tile_add_meta(l457_tile_info_t *tile){

	int status=0;
	int k_tile, length1, length2, i;
	int num_attr=20;



	/* counts */
	int32  Number_Valid_Obs;               	/*The number of non-fill pixels in the til*/
	int32  Number_Valid_Noncloudy_Obs; 		/*The number of non-fill and non-cloudy (ACCA SDS = 0 && DT SDS != 1) pixels in the tile*/
	uint16 Count_L1T;						/*Count of the number of unique L1T images present in the product*/

	char *Sensor_List ;				/* List of Landsat sensors present in the tile */
	char *Number_Valid_Sensor_Obs;	/*The number of non-fill pixels in the tile for each sensor indexed  by Sensor_List*/
	int32 N_L4_obs, N_L5_obs, N_L7_obs;		/*The number of obs from sensors*/

	/*JDOYs */
	long  l_Mean_JDOY;					  	/*Mean Julian Day of Year of non-fill pixels in the tile*/
	int16 Mean_JDOY, Min_JDOY, Max_JDOY; 	/*Minimum Julian Day of Year of non-fill pixels in the tile*/
	        								/*Maximum Julian Day of Year of non-fill pixels in the tile*/

	/*percentages*/
	float64  Percent_ACCA_Cloudy;				/*Percentage of non-fill pixels in the tile that were flagged as ACCA*/
	float64  Percent_DT_Cloudy;				/*Percentage of non-fill pixels in the tile that were flagged as DT cloudy*/
	float64  Percent_Saturated;				/*Percentage of non-fill pixels in the tile that were flagged as saturated */
											/*in any band (Saturation_Flag SDS != 0) */
	int32 N_ACCA_1, N_DTCL_1, N_sat_obs;	/*The number of cloudy by ACCA, DT and sat*/

	/*averages*/
	float64 Mean_B1, Mean_B2, Mean_B3, Mean_B4, Mean_B5, Mean_B6, Mean_B7, Mean_NDVI;
	float64 Mean_Solar_Zenith;		        /*Mean solar zenith angle computed from all non-fill pixels in the tile*/
	/*Mean values of spectral bands and NDVI  computed from all non-fill and non-cloudy*/
	float64 factor;

	char * attr_names[num_attr];
	char * attr_values[num_attr];

	factor=0.0001;
	l_Mean_JDOY=0;
	Mean_JDOY=0;
	Min_JDOY=366;
	Max_JDOY=1;

	Number_Valid_Obs=Number_Valid_Noncloudy_Obs=Count_L1T=0;
	N_ACCA_1=N_DTCL_1= N_L4_obs=N_L5_obs=N_L7_obs=N_sat_obs=0;
	Mean_NDVI=-1.0;
	Mean_B1=Mean_B2=Mean_B3=Mean_B4=Mean_B5=Mean_B6=Mean_B7=Mean_Solar_Zenith=0.0;
	Percent_ACCA_Cloudy=Percent_Saturated=Percent_DT_Cloudy=0;

	for(k_tile=0;k_tile<(TILEDIM*TILEDIM); ++k_tile){
		if(tile->b03[k_tile]==int16_minus32768)continue;
		/*count valids */
		Number_Valid_Obs++;
		switch(tile->senrs[k_tile]){
			case 4:
				N_L4_obs++;
				break;
			case 5:
				N_L5_obs++;
				break;
			case 7:
				N_L7_obs++;
				break;
			default :
			     ;
		}
		/* Count_L1T is a max (l1t_index)+1 */
		if(Count_L1T<tile->l1t_index[k_tile]){
			Count_L1T=tile->l1t_index[k_tile];
		}

		/* jdoy */
		l_Mean_JDOY+=tile->doy[k_tile];
		if(Min_JDOY>tile->doy[k_tile]){
			Min_JDOY=tile->doy[k_tile];
		}
		if(Max_JDOY<tile->doy[k_tile]){
			Max_JDOY=tile->doy[k_tile];
		}

		/*count clouds*/
		if(tile->acca[k_tile]==1){
			N_ACCA_1++;
		}
		if(tile->dtcld[k_tile]==1){
			N_DTCL_1++;
		}
		/*count saturated*/

		if(tile->sat[k_tile]!=0){
			N_sat_obs++;
		}
		/*screen clouds*/
		if(tile->acca[k_tile]==0 && tile->dtcld[k_tile]!=1){
			Number_Valid_Noncloudy_Obs++;
			/*sumate*/
			Mean_B1+=	(factor*(float64)(tile->b01[k_tile]));
			Mean_B2+=	(factor*(float64)(tile->b02[k_tile]));
			Mean_B3+=	(factor*(float64)(tile->b03[k_tile]));
			Mean_B4+=	(factor*(float64)(tile->b04[k_tile]));
			Mean_B5+=	(factor*(float64)(tile->b05[k_tile]));
			Mean_B6+=	(0.01*(float64)(tile->b61[k_tile]));
			Mean_B7+=	(factor*(float64)(tile->b07[k_tile]));
			if(tile->geometry_flag){
				Mean_Solar_Zenith+=	(0.01*(float64)(tile->sz[k_tile]));
			}
		}

	}
	if(Number_Valid_Obs>0){
		if(Number_Valid_Noncloudy_Obs>0){
			Mean_B1=	Mean_B1/(float64)(Number_Valid_Noncloudy_Obs);
			Mean_B2=	Mean_B2/(float64)(Number_Valid_Noncloudy_Obs);
			Mean_B3=	Mean_B3/(float64)(Number_Valid_Noncloudy_Obs);
			Mean_B4=	Mean_B4/(float64)(Number_Valid_Noncloudy_Obs);
			Mean_B5=	Mean_B5/(float64)(Number_Valid_Noncloudy_Obs);
			Mean_B6=	Mean_B6/(float64)(Number_Valid_Noncloudy_Obs);
			Mean_B7=	Mean_B7/(float64)(Number_Valid_Noncloudy_Obs);
			Mean_Solar_Zenith=Mean_Solar_Zenith/(float64)(Number_Valid_Noncloudy_Obs);
		}

		if((Mean_B4+Mean_B3)!=0.0){
			Mean_NDVI=	(Mean_B4-Mean_B3)/(Mean_B4+Mean_B3);
		}

		if(N_sat_obs>0){
			Percent_Saturated=	(100.0*(float64)(N_sat_obs))/(float64)(Number_Valid_Obs);
		}
		if(N_DTCL_1>0){
			Percent_DT_Cloudy=	(100.0*(float64)(N_DTCL_1))/(float64)(Number_Valid_Obs);
		}
		if(N_ACCA_1>0){
			Percent_ACCA_Cloudy=(100.0*(float64)(N_ACCA_1))/(float64)(Number_Valid_Obs);
		}
		Mean_JDOY=(int16)(l_Mean_JDOY/(long)(Number_Valid_Obs));
		Count_L1T++;/*compensate for based 0*/;
	}

	for(i=0;i<num_attr;i++){
		attr_names[i]=(char*)calloc(160, sizeof(char));
		attr_values[i]=(char*)calloc(160, sizeof(char));
	}

	i=0;
	sprintf(attr_names[i], "%s", "Mean_B1");
	sprintf(attr_values[i], "%lf", Mean_B1);
	++i;
	sprintf(attr_names[i], "%s", "Mean_B2");
	sprintf(attr_values[i], "%lf", Mean_B2);
	++i;
	sprintf(attr_names[i], "%s", "Mean_B3");
	sprintf(attr_values[i], "%lf", Mean_B3);
	++i;
	sprintf(attr_names[i], "%s", "Mean_B4");
	sprintf(attr_values[i], "%lf", Mean_B4);
	++i;
	sprintf(attr_names[i], "%s", "Mean_B5");
	sprintf(attr_values[i], "%lf", Mean_B5);
	++i;
	sprintf(attr_names[i], "%s", "Mean_B6");
	sprintf(attr_values[i], "%lf", Mean_B6);
	++i;
	sprintf(attr_names[i], "%s", "Mean_B7");
	sprintf(attr_values[i], "%lf", Mean_B7);
	++i;
	sprintf(attr_names[i], "%s", "Mean_NDVI");
	sprintf(attr_values[i], "%lf", Mean_NDVI);
	++i;
	sprintf(attr_names[i], "%s", "Mean_Solar_Zenith");
	sprintf(attr_values[i], "%lf", Mean_Solar_Zenith);
	++i;

	/*set averages
	if ((SDsetattr(tile->sd_id,   "Mean_B1",   DFNT_FLOAT64, 1, (VOIDP)&Mean_B1) == FAIL)   ||
		(SDsetattr(tile->sd_id,   "Mean_B2",   DFNT_FLOAT64, 1, (VOIDP)&Mean_B2) == FAIL)   ||
		(SDsetattr(tile->sd_id,   "Mean_B3",   DFNT_FLOAT64, 1, (VOIDP)&Mean_B3) == FAIL)   ||
		(SDsetattr(tile->sd_id,   "Mean_B4",   DFNT_FLOAT64, 1, (VOIDP)&Mean_B4) == FAIL)   ||
		(SDsetattr(tile->sd_id,   "Mean_B5",   DFNT_FLOAT64, 1, (VOIDP)&Mean_B5) == FAIL)   ||
		(SDsetattr(tile->sd_id,   "Mean_B6",   DFNT_FLOAT64, 1, (VOIDP)&Mean_B6) == FAIL)   ||
		(SDsetattr(tile->sd_id,   "Mean_B7",   DFNT_FLOAT64, 1, (VOIDP)&Mean_B7) == FAIL)   ||
		(SDsetattr(tile->sd_id,   "Mean_NDVI", DFNT_FLOAT64, 1, (VOIDP)&Mean_NDVI) == FAIL) ||
		(SDsetattr(tile->sd_id,   "Mean_Solar_Zenith", DFNT_FLOAT64, 1, (VOIDP)&Mean_Solar_Zenith) == FAIL)) {
		status = 1;
		Error("SDsetattr failed to add averages", "l457_tile_add_meta", status);
		return status;
	}*/

	sprintf(attr_names[i], "%s", "Percent_Saturated");
	sprintf(attr_values[i], "%lf", Percent_Saturated);
	++i;
	sprintf(attr_names[i], "%s", "Percent_DT_Cloudy");
	sprintf(attr_values[i], "%lf", Percent_DT_Cloudy);
	++i;
	sprintf(attr_names[i], "%s", "Percent_ACCA_Cloudy");
	sprintf(attr_values[i], "%lf", Percent_ACCA_Cloudy);
	++i;



	/*set Percentages
	if ((SDsetattr(tile->sd_id,   "Percent_Saturated",   DFNT_UINT16, 1, (VOIDP)&Percent_Saturated) == FAIL) ||
		(SDsetattr(tile->sd_id,   "Percent_DT_Cloudy",   DFNT_UINT16, 1, (VOIDP)&Percent_DT_Cloudy) == FAIL) ||
		(SDsetattr(tile->sd_id,   "Percent_ACCA_Cloudy", DFNT_UINT16, 1, (VOIDP)&Percent_ACCA_Cloudy) == FAIL)) {
		status = 1;
		Error("SDsetattr failed to add percentages", "l457_tile_add_meta", status);
		return status;
	}*/

	sprintf(attr_names[i], "%s", "Mean_JDOY");
	sprintf(attr_values[i], "%d", Mean_JDOY);
	++i;
	sprintf(attr_names[i], "%s", "Min_JDOY");
	sprintf(attr_values[i], "%d", Min_JDOY);
	++i;
	sprintf(attr_names[i], "%s", "Max_JDOY");
	sprintf(attr_values[i], "%d", Max_JDOY);
	++i;

	/* DOYS
	if ((SDsetattr(tile->sd_id,   "Mean_JDOY",  DFNT_INT16, 1, (VOIDP)&Mean_JDOY) == FAIL) ||
		(SDsetattr(tile->sd_id,   "Min_JDOY",   DFNT_INT16, 1, (VOIDP)&Min_JDOY ) == FAIL) ||
		(SDsetattr(tile->sd_id,   "Max_JDOY",   DFNT_INT16, 1, (VOIDP)&Max_JDOY ) == FAIL)) {
		status = 1;
		Error("SDsetattr failed to add JDOYs", "l457_tile_add_meta", status);
		return status;
	}*/

	sprintf(attr_names[i], "%s", "Number_Valid_Obs");
	sprintf(attr_values[i], "%d", Number_Valid_Obs);
	++i;
	sprintf(attr_names[i], "%s", "Number_Valid_Noncloudy_Obs");
	sprintf(attr_values[i], "%d", Number_Valid_Noncloudy_Obs);
	++i;
	sprintf(attr_names[i], "%s", "Count_L1T");
	sprintf(attr_values[i], "%d", Count_L1T);
	++i;


	/*counts

	if ((SDsetattr(tile->sd_id,   "Number_Valid_Obs",  DFNT_INT32, 1, (VOIDP)&Number_Valid_Obs) == FAIL) ||
		(SDsetattr(tile->sd_id,   "Number_Valid_Noncloudy_Obs",   DFNT_INT32, 1, (VOIDP)&Number_Valid_Noncloudy_Obs ) == FAIL) ||
		(SDsetattr(tile->sd_id,   "Count_L1T",   DFNT_UINT16, 1, (VOIDP)&(Count_L1T) ) == FAIL)) {
		status = 1;
		Error("SDsetattr failed to add JDOYs", "l457_tile_add_meta", status);
		return status;
	} */

	Sensor_List=NULL;
	Number_Valid_Sensor_Obs=NULL;
	if(((Sensor_List = (char*)calloc(16, sizeof(char))) == NULL)||
	   ((Number_Valid_Sensor_Obs = (char*)calloc(160, sizeof(char))) == NULL)){
		status = 1;
		Error("failed to allocate memory for Sensor_Lists and counts", "l457_tile_add_meta", status);
		return status;
	}

	if (N_L4_obs>0 && N_L5_obs>0){
		sprintf(Sensor_List, "%d|%d",4,5);
		sprintf(Number_Valid_Sensor_Obs, "%d|%d ", N_L4_obs, N_L5_obs);
	}else if(N_L5_obs>0 && N_L7_obs>0){
		sprintf(Sensor_List, "%d|%d",5,7);
		sprintf(Number_Valid_Sensor_Obs, "%d|%d", N_L5_obs, N_L7_obs);
	}else if(N_L4_obs>0){
		sprintf(Sensor_List, "%d",4);
		sprintf(Number_Valid_Sensor_Obs, "%d", N_L4_obs);
	}else if(N_L5_obs>0){
		sprintf(Sensor_List, "%d",5);
		sprintf(Number_Valid_Sensor_Obs, "%d", N_L5_obs);
	}else if(N_L7_obs>0){
		sprintf(Sensor_List, "%d",7);
		sprintf(Number_Valid_Sensor_Obs, "%d", N_L7_obs);
	}
	length1=strlen(Sensor_List);
	length2=strlen(Number_Valid_Sensor_Obs);

	sprintf(attr_names[i], "%s", "Sensor_List");
	sprintf(attr_values[i], "\"%s\"", Sensor_List);
	++i;
	sprintf(attr_names[i], "%s", "Number_Valid_Sensor_Obs");
	sprintf(attr_values[i], "\"%s\"", Number_Valid_Sensor_Obs);

	/*for(i=0;i<num_attr;i++){
		printf("%s = %s \n"  , attr_names[i], attr_values[i]);
	}*/

	if(write_ArchiveMetadata(tile->sd_id, attr_names, attr_values, num_attr)!=0){
		status = 1;
		Error("SDsetattr failed to add Sensor_Lists and counts", "l457_tile_add_meta", status);
		free(Sensor_List);
		free(Number_Valid_Sensor_Obs);
		for(i=0;i>num_attr;i++){
			free(attr_names[i]);
			free(attr_values[i]);
		}
		return status;
	}


	/*

	printf("~~~ %s ; %d \n", Number_Valid_Sensor_Obs, length2);
	if ((SDsetattr(tile->sd_id,   "Number_Valid_Sensor_Obs",  DFNT_CHAR8, length2, (VOIDP)Number_Valid_Sensor_Obs ) == FAIL) ||
		(SDsetattr(tile->sd_id,   "Sensor_List",  DFNT_CHAR8, length1, (VOIDP)Sensor_List ) == FAIL)) {
		status = 1;
		Error("SDsetattr failed to add Sensor_Lists and counts", "l457_tile_add_meta", status);
		free(Sensor_List);
		free(Number_Valid_Sensor_Obs);
		return status;
	}*/

	free(Sensor_List);
	free(Number_Valid_Sensor_Obs);
	for(i=0;i>num_attr;i++){
		free(attr_names[i]);
		free(attr_values[i]);
	}

	return status;
}


/*******************************************************************************
NAME:     write_ArchiveMetadata()

PURPOSE:  Store metadata for the tile as an annotation in one global attriburte

Args:
         tile  -  pointer to the tile structure
CALLS: l457_tile_add_meta
*******************************************************************************/

 int write_ArchiveMetadata( int32 sd_id, char **attr_names, char **attr_values, int num_attr){
	 	int status=0;
	 	int i, length;
	 	char *metabuf;
	 	char *metaCOPY;
	 	metabuf=metaCOPY=NULL;
		if((metabuf  = (char*)calloc(MYHDF_MAX_NATTR_VAL, sizeof(char)))== NULL||
		   (metaCOPY = (char*)calloc(MYHDF_MAX_NATTR_VAL, sizeof(char)))== NULL){
			status=1;
			Error("SDsetattr failed to allocate memory for metadata", "write_ArchiveMetadata", status);
			return status;
		}

		sprintf(metabuf, "\n "
				"GROUP                  = ARCHIVEDMETADATA\n"
				"  GROUPTYPE            = MASTERGROUP\n");

		for(i=0; i<num_attr; ++i){
			strcpy(metaCOPY, metabuf);
			sprintf(metabuf, "%s      OBJECT                = %s\n"
					 	 	 "      NUM_VAL              = 1\n"
							 "      VALUE                = %s\n"
							 "    END_OBJECT             = %s\n",
							 metaCOPY, attr_names[i], attr_values[i],  attr_names[i]);
		}
		strcpy(metaCOPY, metabuf);

		sprintf(metabuf,"%s\n"
						"END_GROUP              = ARCHIVEDMETADATA\n"
						"\n"
						"END\n",metaCOPY);
		/*printf("########################## %s ##########################\n",metabuf);*/
		length=strlen(metabuf);
		if ((SDsetattr(sd_id,   "ArchiveMetadata.0",  DFNT_CHAR8, length, (VOIDP)metabuf ) == FAIL)) {
				status = 1;
				Error("SDsetattr failed to add Sensor_Lists and counts", "write_ArchiveMetadata", status);
				return status;
			}

		free(metabuf);
		free(metaCOPY);

		return status;
}

/********************************************************************************
	Create a WELD composite tile and set the projection information and  SDS attributes.
	
	Return 0 if success, and non-zero on failure.

*********************************************************************************/
int l457_tile_create(l457_tile_info_t *tile, sds_info_t *all_sds, char *region, int * tot_sds)
{
	int32 dim_sizes[2];
	int status = 0;
	int32 rank;
	int irow, icol;
	long k;
	char message[MSGLEN];
	char tmpstr[MSGLEN];

	int32 comp_type;   /*Compression type flag*/
	comp_info c_info;  /*Compression structure*/

	char dim_names[2][100];
	int rank0, rank1;

	Myhdf_attr_t attr;
	double dval[100];   /* double type attribute values*/

	if ((status = l457_tile_allocate(tile)) != 0) {
		Error("Error in allocate_tile", "l457_tile_create", 0);
		return status;
	}
	if ((tile->sd_id = SDstart(tile->fname, DFACC_CREATE)) == FAIL) {
		status = 2;
		sprintf(message, "Cannot create hdf file %s", tile->fname);
		Error(message, "l457_tile_create", status);
		goto CLEANUP;
	}

	rank = 2;
	dim_sizes[0] = tile->nrow;
	dim_sizes[1] = tile->ncol;
	if ((tile->sds_id_b01	= SDcreate(tile->sd_id,   B01_SDS_NAME,	DFNT_INT16,  rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_b02	= SDcreate(tile->sd_id,   B02_SDS_NAME,	DFNT_INT16,  rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_b03	= SDcreate(tile->sd_id,   B03_SDS_NAME,	DFNT_INT16,  rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_b04	= SDcreate(tile->sd_id,   B04_SDS_NAME,	DFNT_INT16,  rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_b05	= SDcreate(tile->sd_id,   B05_SDS_NAME,	DFNT_INT16,  rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_b61   = SDcreate(tile->sd_id,   B61_SDS_NAME, DFNT_INT16,  rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_b62   = SDcreate(tile->sd_id,   B62_SDS_NAME, DFNT_INT16,  rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_b07	= SDcreate(tile->sd_id,   B07_SDS_NAME,	DFNT_INT16,  rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_ndvi  = SDcreate(tile->sd_id,  NDVI_SDS_NAME, DFNT_INT16,  rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_doy   = SDcreate(tile->sd_id,   DOY_SDS_NAME,	DFNT_INT16,  rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_sat   = SDcreate(tile->sd_id,   SAT_SDS_NAME, DFNT_UINT8,  rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_dtcld = SDcreate(tile->sd_id, DTCLD_SDS_NAME,	DFNT_UINT8,  rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_acca  = SDcreate(tile->sd_id,  ACCA_SDS_NAME,	DFNT_UINT8,  rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_nobs  = SDcreate(tile->sd_id,  NOBS_SDS_NAME,	DFNT_UINT16, rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_cpath = SDcreate(tile->sd_id, CPATH_SDS_NAME,	DFNT_UINT8,  rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_senrs = SDcreate(tile->sd_id, SENRS_SDS_NAME,	DFNT_UINT8,  rank, dim_sizes)) == FAIL) {
		status = 2;
		sprintf(message, "Cannot create SDS in %s", tile->fname);
		Error(message, "l457_tile_create", status);
		goto CLEANUP;
	}
	if (tile->geometry_flag == 1) {
		if ((tile->sds_id_vz	= SDcreate(tile->sd_id, VZ_SDS_NAME,  DFNT_INT16, rank, dim_sizes)) == FAIL ||
		    (tile->sds_id_va	= SDcreate(tile->sd_id, VA_SDS_NAME,  DFNT_INT16, rank, dim_sizes)) == FAIL ||
		    (tile->sds_id_sz	= SDcreate(tile->sd_id, SZ_SDS_NAME,  DFNT_INT16, rank, dim_sizes)) == FAIL ||
		    (tile->sds_id_sa	= SDcreate(tile->sd_id, SA_SDS_NAME,  DFNT_INT16, rank, dim_sizes)) == FAIL) {
			status = 2;
			sprintf(message, "Cannot create SDS in %s", tile->fname);
			Error(message, "l457_tile_create", status);
			goto CLEANUP;
		}
	}
	if (tile->l1t_index_flag == 1) {
		if ((tile->sds_id_l1t_index = SDcreate(tile->sd_id, L1T_INDEX_SDS_NAME, DFNT_UINT16, rank, dim_sizes)) == FAIL ||
		    (tile->sds_id_l1t_col = SDcreate(tile->sd_id, L1T_COL_SDS_NAME, DFNT_UINT16, rank, dim_sizes)) == FAIL ||
		    (tile->sds_id_l1t_row = SDcreate(tile->sd_id, L1T_ROW_SDS_NAME, DFNT_UINT16, rank, dim_sizes)) == FAIL ){
			status = 2;
			sprintf(message, "Cannot create SDS in %s", tile->fname);
			Error(message, "l457_tile_create", status);
			goto CLEANUP;
		}
	}

	/* Initialize to fill value */
	for (irow = 0; irow < tile->nrow; irow++) {
		for (icol = 0; icol < tile->ncol; icol++) {
		        k = irow * tile->ncol + icol;
		        tile->b01[k]	= int16_minus32768;
		        tile->b02[k]	= int16_minus32768;
		        tile->b03[k]	= int16_minus32768;
		        tile->b04[k]	= int16_minus32768;
		        tile->b05[k]	= int16_minus32768;
		        tile->b61[k] 	= int16_minus32768;
		        tile->b62[k] 	= int16_minus32768;
		        tile->b07[k]	= int16_minus32768;
		        tile->ndvi[k]  	= int16_minus32768;
		        tile->doy[k]   	= 0;
		        tile->sat[k]   	= uint16_0;
		        tile->dtcld[k] 	= uint8_255;   /*255*/
		        tile->acca[k]  	= uint8_255;
		        tile->nobs[k]  	= uint16_0;
		        tile->cpath[k]  	= uint8_0;
		        tile->senrs[k]  	= uint8_255;
		}
	}
	if (tile->geometry_flag == 1) {
		for (irow = 0; irow < tile->nrow; irow++) {
			for (icol = 0; icol < tile->ncol; icol++) {
			        k = irow * tile->ncol + icol;
				tile->vz[k]	= int16_minus32768;
				tile->va[k]	= int16_minus32768;
				tile->sz[k]	= int16_minus32768;
				tile->sa[k]	= int16_minus32768;
			}
		}
	}
	if (tile->l1t_index_flag == 1) {
		for (irow = 0; irow < tile->nrow; irow++) {
			for (icol = 0; icol < tile->ncol; icol++) {
			        k = irow * tile->ncol + icol;
				tile->l1t_index[k] = uint16_65535;
				tile->l1t_col[k] = uint16_65535;
				tile->l1t_row[k] = uint16_65535;
			}
		}
	}

	/*  Set the fill value attribute since LDOPE tools read an attribute "_FillValue" */
	if (SDsetattr(tile->sds_id_b01,  "_FillValue", DFNT_INT16,  1, (VOIDP)&int16_minus32768) == FAIL ||
	    SDsetattr(tile->sds_id_b02,  "_FillValue", DFNT_INT16,  1, (VOIDP)&int16_minus32768) == FAIL ||
	    SDsetattr(tile->sds_id_b03,  "_FillValue", DFNT_INT16,  1, (VOIDP)&int16_minus32768) == FAIL ||
	    SDsetattr(tile->sds_id_b04,  "_FillValue", DFNT_INT16,  1, (VOIDP)&int16_minus32768) == FAIL ||
	    SDsetattr(tile->sds_id_b05,  "_FillValue", DFNT_INT16,  1, (VOIDP)&int16_minus32768) == FAIL ||
	    SDsetattr(tile->sds_id_b61,  "_FillValue", DFNT_INT16,  1, (VOIDP)&int16_minus32768) == FAIL ||
	    SDsetattr(tile->sds_id_b62,  "_FillValue", DFNT_INT16,  1, (VOIDP)&int16_minus32768) == FAIL ||
	    SDsetattr(tile->sds_id_b07,  "_FillValue", DFNT_INT16,  1, (VOIDP)&int16_minus32768) == FAIL ||
	    SDsetattr(tile->sds_id_sat,  "_FillValue", DFNT_UINT8,  1, (VOIDP)&uint8_0		   ) == FAIL ||
	    SDsetattr(tile->sds_id_ndvi, "_FillValue", DFNT_INT16,  1, (VOIDP)&int16_minus32768) == FAIL ||
	    SDsetattr(tile->sds_id_doy , "_FillValue", DFNT_INT16,  1, (VOIDP)&int16_0         ) == FAIL ||
	    SDsetattr(tile->sds_id_dtcld,"_FillValue", DFNT_UINT8,  1, (VOIDP)&uint8_255	   ) == FAIL ||
	    SDsetattr(tile->sds_id_acca, "_FillValue", DFNT_UINT8,  1, (VOIDP)&uint8_255	   ) == FAIL ||
	    SDsetattr(tile->sds_id_nobs, "_FillValue", DFNT_UINT16, 1, (VOIDP)&uint16_0		   ) == FAIL ||
	    SDsetattr(tile->sds_id_cpath,"_FillValue", DFNT_UINT8,  1, (VOIDP)&uint8_0		   ) == FAIL ||
	    SDsetattr(tile->sds_id_senrs,"_FillValue", DFNT_UINT16, 1, (VOIDP)&uint8_255	   ) == FAIL ){
		status = 2;
		sprintf(message, "SDsetattr _FillValue failed for %s", tile->fname);
		Error(message, "l457_tile_create", status);
		goto CLEANUP;
	}
	if (tile->geometry_flag == 1) {
		if (SDsetattr(tile->sds_id_vz,   "_FillValue", DFNT_INT16, 1, (VOIDP)&int16_minus32768) == FAIL ||
		    SDsetattr(tile->sds_id_va,   "_FillValue", DFNT_INT16, 1, (VOIDP)&int16_minus32768) == FAIL ||
		    SDsetattr(tile->sds_id_sz,   "_FillValue", DFNT_INT16, 1, (VOIDP)&int16_minus32768) == FAIL ||
		    SDsetattr(tile->sds_id_sa,   "_FillValue", DFNT_INT16, 1, (VOIDP)&int16_minus32768) == FAIL) {
			status = 2;
			sprintf(message, "SDsetattr _FillValue failed for %s", tile->fname);
			Error(message, "l457_tile_create", status);
			goto CLEANUP;
		}
	}
	if (tile->l1t_index_flag == 1) {
		if ((SDsetattr(tile->sds_id_l1t_index, "_FillValue", DFNT_UINT16, 1, (VOIDP)&uint16_65535) == FAIL) ||
			(SDsetattr(tile->sds_id_l1t_col,   "_FillValue", DFNT_UINT16, 1, (VOIDP)&uint16_65535) == FAIL) ||
			(SDsetattr(tile->sds_id_l1t_row,   "_FillValue", DFNT_UINT16, 1, (VOIDP)&uint16_65535) == FAIL) ){
			status = 2;
			sprintf(message, "SDsetattr _FillValue failed for %s", tile->fname);
			Error(message, "l457_tile_create", status);
			goto CLEANUP;
		}
	}

	/* Set dimname */  
	strcpy(dim_names[0], "YDim_Grid");
	strcpy(dim_names[1], "XDim_Grid");
	rank0 = 0;
	rank1 = 1;
	if ((PutSDSDimInfo(tile->sds_id_b01,   dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_b01,   dim_names[1], rank1) == 1) ||
	    (PutSDSDimInfo(tile->sds_id_b02,   dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_b02,   dim_names[1], rank1) == 1) ||
	    (PutSDSDimInfo(tile->sds_id_b03,   dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_b03,   dim_names[1], rank1) == 1) ||
	    (PutSDSDimInfo(tile->sds_id_b04,   dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_b04,   dim_names[1], rank1) == 1) ||
	    (PutSDSDimInfo(tile->sds_id_b05,   dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_b05,   dim_names[1], rank1) == 1) ||
	    (PutSDSDimInfo(tile->sds_id_b61,   dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_b61,   dim_names[1], rank1) == 1) ||
	    (PutSDSDimInfo(tile->sds_id_b62,   dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_b62,   dim_names[1], rank1) == 1) ||
	    (PutSDSDimInfo(tile->sds_id_b07,   dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_b07,   dim_names[1], rank1) == 1) ||
	    (PutSDSDimInfo(tile->sds_id_ndvi,  dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_ndvi,  dim_names[1], rank1) == 1) ||
	    (PutSDSDimInfo(tile->sds_id_doy,   dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_doy,   dim_names[1], rank1) == 1) ||
	    (PutSDSDimInfo(tile->sds_id_sat,   dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_sat,   dim_names[1], rank1) == 1) ||
	    (PutSDSDimInfo(tile->sds_id_dtcld, dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_dtcld, dim_names[1], rank1) == 1) ||
	    (PutSDSDimInfo(tile->sds_id_acca,  dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_acca,  dim_names[1], rank1) == 1) ||
	    (PutSDSDimInfo(tile->sds_id_nobs,  dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_nobs,  dim_names[1], rank1) == 1) ||
	    (PutSDSDimInfo(tile->sds_id_cpath, dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_cpath, dim_names[1], rank1) == 1) ||
	    (PutSDSDimInfo(tile->sds_id_senrs, dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_senrs, dim_names[1], rank1) == 1) ){
		status = 2;
		Error("PutSDSDimInfo() failed", "l457_tile_create", status);
		goto CLEANUP;
	} 
	if (tile->geometry_flag == 1) {
		if ((PutSDSDimInfo(tile->sds_id_vz,   dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_vz,	dim_names[1], rank1) == 1) ||
		    (PutSDSDimInfo(tile->sds_id_va,   dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_va,	dim_names[1], rank1) == 1) ||
		    (PutSDSDimInfo(tile->sds_id_sz,   dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_sz,	dim_names[1], rank1) == 1) ||
		    (PutSDSDimInfo(tile->sds_id_sa,   dim_names[0], rank0) == 1 || PutSDSDimInfo(tile->sds_id_sa,	dim_names[1], rank1) == 1) ) {
			status = 2;
			Error("PutSDSDimInfo() failed", "l457_tile_create", status);
			goto CLEANUP;
		}
	}
	if (tile->l1t_index_flag == 1) {
		if ((PutSDSDimInfo(tile->sds_id_l1t_index,  dim_names[0], rank0) == 1  ||
			 PutSDSDimInfo(tile->sds_id_l1t_index,	dim_names[1], rank1) == 1) ||
			(PutSDSDimInfo(tile->sds_id_l1t_col,    dim_names[0], rank0) == 1  ||
			 PutSDSDimInfo(tile->sds_id_l1t_col,	dim_names[1], rank1) == 1) ||
			(PutSDSDimInfo(tile->sds_id_l1t_row,    dim_names[0], rank0) == 1  ||
			 PutSDSDimInfo(tile->sds_id_l1t_row,	dim_names[1], rank1) == 1) ){
			status = 2;
			Error("PutSDSDimInfo() failed", "l457_tile_create", 0);
			goto CLEANUP;
		}
	}


	/*	Setup the SDS attributes */
	if ((status = setup_sds_info(all_sds, tile->geometry_flag, tile->l1t_index_flag, tot_sds)) != 0) {
		Error("Error in setup_sds_info()", "l457_tile_create", 0);
		goto CLEANUP;
	}

	/*  SDS attribute */
	if (set_sds_attribute(tile->sds_id_b01,	     B01_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
	    set_sds_attribute(tile->sds_id_b02,	     B02_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
	    set_sds_attribute(tile->sds_id_b03,	     B03_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
	    set_sds_attribute(tile->sds_id_b04,	     B04_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
	    set_sds_attribute(tile->sds_id_b05,	     B05_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
	    set_sds_attribute(tile->sds_id_b61,      B61_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
	    set_sds_attribute(tile->sds_id_b62,      B62_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
	    set_sds_attribute(tile->sds_id_b07,	     B07_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
	    set_sds_attribute(tile->sds_id_ndvi,    NDVI_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
	    set_sds_attribute(tile->sds_id_doy,      DOY_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
	    set_sds_attribute(tile->sds_id_sat,      SAT_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
	    set_sds_attribute(tile->sds_id_dtcld,  DTCLD_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
	    set_sds_attribute(tile->sds_id_acca,    ACCA_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
	    set_sds_attribute(tile->sds_id_nobs,    NOBS_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
	    set_sds_attribute(tile->sds_id_cpath,    CPATH_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
	    set_sds_attribute(tile->sds_id_senrs,    SENRS_SDS_NAME, all_sds, NSDS_MAX) == 1 ){
		status = 2;
		Error("Error in set_sds_attribute", "l457_tile_create", 0);
		goto CLEANUP;
	}
	if (tile->geometry_flag == 1) {
		if (set_sds_attribute(tile->sds_id_vz,	VZ_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
		    set_sds_attribute(tile->sds_id_va,	VA_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
		    set_sds_attribute(tile->sds_id_sz,	SZ_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
		    set_sds_attribute(tile->sds_id_sa,	SA_SDS_NAME, all_sds, NSDS_MAX) == 1) {
			status = 2;
			Error("Error in set_sds_attribute", "l457_tile_create", 0);
			goto CLEANUP;
		}
	}
	if (tile->l1t_index_flag == 1) {
		if (set_sds_attribute(tile->sds_id_l1t_index,	L1T_INDEX_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
		    set_sds_attribute(tile->sds_id_l1t_col,	  L1T_COL_SDS_NAME, all_sds, NSDS_MAX) == 1 ||
		    set_sds_attribute(tile->sds_id_l1t_row,	  L1T_ROW_SDS_NAME, all_sds, NSDS_MAX) == 1) {
			status = 2;
			Error("Error in set_sds_attribute", "l457_tile_create", 0);
			goto CLEANUP;
		}
	}




	/*****************  begin: set valid range ********************************/
	/* ALL spectral bands valid range*/

	attr.type = DFNT_INT16;
	attr.nval = 2;
	attr.name = VALID_RANGE;
	dval[0] = -32767;
	dval[1] =  32767;
	if (!PutAttrDouble(tile->sds_id_b01,  &attr, dval) ||
	    !PutAttrDouble(tile->sds_id_b02,  &attr, dval) ||
	    !PutAttrDouble(tile->sds_id_b03,  &attr, dval) ||
	    !PutAttrDouble(tile->sds_id_b04,  &attr, dval) ||
	    !PutAttrDouble(tile->sds_id_b05,  &attr, dval) ||
	    !PutAttrDouble(tile->sds_id_b61,  &attr, dval) ||
	    !PutAttrDouble(tile->sds_id_b62,  &attr, dval) ||
	    !PutAttrDouble(tile->sds_id_b07,  &attr, dval) ){
		status = 2;
		Error("Error in PutAttrDouble", "l457_tile_create", 0);
		goto CLEANUP;
	}

	/* NDVI */
	attr.type = DFNT_INT16;
	attr.nval = 2;
	attr.name = VALID_RANGE;
	dval[0] = -10000;
	dval[1] =  10000;
	if (!PutAttrDouble(tile->sds_id_ndvi,  &attr, dval)) {
		status = 2;
		Error("Error in PutAttrDouble", "l457_tile_create", 0);
		goto CLEANUP;
	}

	/* doy*/
	attr.type = DFNT_INT16;
	attr.nval = 2;
	attr.name = VALID_RANGE;
	dval[0] = 1;
	dval[1] = 366;
	if (!PutAttrDouble(tile->sds_id_doy,  &attr, dval)) {
		status = 2;
		Error("Error in PutAttrDouble", "l457_tile_create", 0);
		goto CLEANUP;
	}

	/* saturation*/
	attr.type = DFNT_UINT8;
	attr.nval = 2;
	attr.name = VALID_RANGE;
	dval[0] = 0;
	dval[1] = 255;
	if (!PutAttrDouble(tile->sds_id_sat,  &attr, dval)) {
		status = 2;
		Error("Error in PutAttrDouble", "l457_tile_create", 0);
		goto CLEANUP;
	}
	  
	/* dtcld*/
	attr.type = DFNT_UINT8;
	attr.nval = 2;
	attr.name = VALID_RANGE;
	dval[0] = 0;
	dval[1] = 200;
	if (!PutAttrDouble(tile->sds_id_dtcld,  &attr, dval)) {
		status = 2;
		Error("Error in PutAttrDouble", "l457_tile_create", 0);
		goto CLEANUP;
	}

	/* acca*/
	attr.type = DFNT_UINT8;
	attr.nval = 2;
	attr.name = VALID_RANGE;
	dval[0] = 0;
	dval[1] = 1;
	if (!PutAttrDouble(tile->sds_id_acca,  &attr, dval)) {
		status = 2;
		Error("Error in PutAttrDouble", "l457_tile_create", 0);
		goto CLEANUP;
	}

	/* nobs*/
	attr.type = DFNT_UINT16;
	attr.nval = 2;
	attr.name = VALID_RANGE;
	dval[0] = 0;
	dval[1] = 65534;
	if (!PutAttrDouble(tile->sds_id_nobs,  &attr, dval)) {
		status = 2;
		Error("Error in PutAttrDouble", "l457_tile_create", 0);
		goto CLEANUP;
	}

	/* cpath */
	attr.type = DFNT_UINT16;
	attr.nval = 2;
	attr.name = VALID_RANGE;
	dval[0] = 0;
	dval[1] = 15;
	if (!PutAttrDouble(tile->sds_id_cpath,  &attr, dval)) {
		status = 2;
		Error("Error in PutAttrDouble", "l457_tile_create", 0);
		goto CLEANUP;
	}

	/* senrs */
	attr.type = DFNT_UINT16;
	attr.nval = 2;
	attr.name = VALID_RANGE;
	dval[0] = 0;
	dval[1] = 254;
	if (!PutAttrDouble(tile->sds_id_senrs,  &attr, dval)) {
		status = 2;
		Error("Error in PutAttrDouble", "l457_tile_create", 0);
		goto CLEANUP;
	}

	if (tile->geometry_flag == 1) {
		/* vz */
		attr.type = DFNT_INT16;
		attr.nval = 2;
		attr.name = VALID_RANGE;
		dval[0] = 0;
		dval[1] = 9000;
		if (!PutAttrDouble(tile->sds_id_vz,  &attr, dval)) {
			status = 2;
			Error("Error in PutAttrDouble", "l457_tile_create", 0);
			goto CLEANUP;
		}
	
		/* va*/
		attr.type = DFNT_INT16;
		attr.nval = 2;
		attr.name = VALID_RANGE;
		dval[0] = -18000;
		dval[1] =  18000;
		if (!PutAttrDouble(tile->sds_id_va,  &attr, dval)) {
			status = 2;
			Error("Error in PutAttrDouble", "l457_tile_create", 0);
			goto CLEANUP;
		}
	
		/* sz*/
		attr.type = DFNT_INT16;
		attr.nval = 2;
		attr.name = VALID_RANGE;
		dval[0] = 0;
		dval[1] = 9000;
		if (!PutAttrDouble(tile->sds_id_sz,  &attr, dval)) {
			status = 2;
			Error("Error in PutAttrDouble", "l457_tile_create", 0);
			goto CLEANUP;
		}
	
		/* sa*/
		attr.type = DFNT_INT16;
		attr.nval = 2;
		attr.name = VALID_RANGE;
		dval[0] = -18000;
		dval[1] =  18000;
		if (!PutAttrDouble(tile->sds_id_sa,  &attr, dval)) {
			status = 2;
			Error("Error in PutAttrDouble", "l457_tile_create", 0);
			goto CLEANUP;
		}
	}

	if (tile->l1t_index_flag == 1) {
		attr.type = DFNT_UINT16;
		attr.nval = 2;
		attr.name = VALID_RANGE;
		dval[0] = 0;
		dval[1] = 65534;
		if (!PutAttrDouble(tile->sds_id_l1t_index,  &attr, dval)) {
			status = 2;
			Error("Error in PutAttrDouble", "l457_tile_create", 0);
			goto CLEANUP;
		}

		/* for L1T row and column*/
		dval[0] = 0;
		/*should be no more than 9000 but 10000 is ok*/
		dval[1] = 10000;
		if (!PutAttrDouble(tile->sds_id_l1t_col,  &attr, dval)) {
			status = 2;
			Error("Error in PutAttrDouble", "l457_tile_create", 0);
			goto CLEANUP;
		}

		if (!PutAttrDouble(tile->sds_id_l1t_row,  &attr, dval)) {
			status = 2;
			Error("Error in PutAttrDouble", "l457_tile_create", 0);
			goto CLEANUP;
		}

	}

	/*****************  end: set valid range ********************************/
	
 
	comp_type = COMP_CODE_DEFLATE;
	c_info.deflate.level = COMPRESS_LEVEL;	 /*Level 9 would be too slow */
	if (SDsetcompress(tile->sds_id_b01,	  comp_type, &c_info) == FAIL ||
	    SDsetcompress(tile->sds_id_b02,	  comp_type, &c_info) == FAIL ||
	    SDsetcompress(tile->sds_id_b03,	  comp_type, &c_info) == FAIL ||
	    SDsetcompress(tile->sds_id_b04,	  comp_type, &c_info) == FAIL ||
	    SDsetcompress(tile->sds_id_b05,	  comp_type, &c_info) == FAIL ||
	    SDsetcompress(tile->sds_id_b61,	  comp_type, &c_info) == FAIL ||
	    SDsetcompress(tile->sds_id_b62,	  comp_type, &c_info) == FAIL ||
	    SDsetcompress(tile->sds_id_b07,	  comp_type, &c_info) == FAIL ||
	    SDsetcompress(tile->sds_id_ndvi,  comp_type, &c_info) == FAIL ||
	    SDsetcompress(tile->sds_id_doy,	  comp_type, &c_info) == FAIL ||
	    SDsetcompress(tile->sds_id_sat,	  comp_type, &c_info) == FAIL ||
	    SDsetcompress(tile->sds_id_dtcld, comp_type, &c_info) == FAIL ||
	    SDsetcompress(tile->sds_id_acca,  comp_type, &c_info) == FAIL ||
	    SDsetcompress(tile->sds_id_nobs,  comp_type, &c_info) == FAIL ||
	    SDsetcompress(tile->sds_id_cpath, comp_type, &c_info) == FAIL ||
	    SDsetcompress(tile->sds_id_senrs,  comp_type, &c_info) == FAIL ){
		status = 2;
		Error("SDsetcompress failed", "l457_tile_create", status);
		goto CLEANUP;
	}
	if (tile->geometry_flag == 1) {
		if (SDsetcompress(tile->sds_id_vz,	 comp_type, &c_info) == FAIL ||
		    SDsetcompress(tile->sds_id_va,	 comp_type, &c_info) == FAIL ||
		    SDsetcompress(tile->sds_id_sz,	 comp_type, &c_info) == FAIL ||
		    SDsetcompress(tile->sds_id_sa,	 comp_type, &c_info) == FAIL) {
			status = 2;
			Error("SDsetcompress failed", "l457_tile_create", status);
			goto CLEANUP;
		}
	}
	if (tile->l1t_index_flag == 1) {
		if (SDsetcompress(tile->sds_id_l1t_index, comp_type, &c_info) == FAIL ||
			SDsetcompress(tile->sds_id_l1t_col  , comp_type, &c_info) == FAIL ||
			SDsetcompress(tile->sds_id_l1t_row  , comp_type, &c_info) == FAIL ){
			status = 2;
			Error("SDsetcompress failed", "l457_tile_create", status);
			goto CLEANUP;
		}
	}

	strcpy(tmpstr, PRODUCT_VERSION);
	tmpstr[strlen(PRODUCT_VERSION)]='\0';
    if (SDsetattr(tile->sd_id, PRODUCT_VERSION_AN, DFNT_CHAR8, strlen(tmpstr), (VOIDP)tmpstr) == FAIL) {
		status = 2;
		Error("SDsetattr failed to add PRODUCT_VERSION and counts", "l457_tile_create", status);
		goto CLEANUP;
    }
    strcpy(tmpstr, PGE_VERSION_NRT);
    tmpstr[strlen(PGE_VERSION_NRT)]='\0';
    if (SDsetattr(tile->sd_id, PGE_VERSION_AN, DFNT_CHAR8, strlen(tmpstr), (VOIDP)tmpstr) == FAIL) {
    	status = 2;
    	Error("SDsetattr failed to add PGE_VERSION_NRT and counts", "l457_tile_create", status);
    	goto CLEANUP;
    }


	/* Add corner coordinates */
	tile->pixel_size = REF_PIXSIZE;
	if (l457_tile_add_cornes(tile, region) != 0) {
		status = 2;
		Error("Failed in l457_tile_add_cornes", "l457_tile_create", 0);
		goto CLEANUP;
	}

	CLEANUP:
		if (status!=0){
			l457_tile_close(tile);
		}
	return status;
}

/********************************************************************************
	Allocate memory for a tile.
	Return 0 on success and non-zero on failure.
*********************************************************************************/
int l457_tile_allocate(l457_tile_info_t *tile)
{
	int status = 0;

	tile->sd_id	= FAIL;
	tile->sds_id_b01	= FAIL;
	tile->sds_id_b02	= FAIL;
	tile->sds_id_b03	= FAIL;
	tile->sds_id_b04	= FAIL;
	tile->sds_id_b05	= FAIL;
	tile->sds_id_b61= FAIL;
	tile->sds_id_b62= FAIL;
	tile->sds_id_b07	= FAIL;
	tile->sds_id_ndvi  = FAIL;
	tile->sds_id_doy   = FAIL;
	tile->sds_id_sat   = FAIL;
	tile->sds_id_dtcld = FAIL;
	tile->sds_id_acca  = FAIL;
	tile->sds_id_nobs  = FAIL;
	tile->sds_id_cpath = FAIL;
	tile->sds_id_senrs = FAIL;

	tile->sds_id_vz	= FAIL;
	tile->sds_id_va	= FAIL;
	tile->sds_id_sz	= FAIL;
	tile->sds_id_sa	= FAIL;
	tile->sds_id_l1t_index = FAIL;
	tile->sds_id_l1t_col = FAIL;
	tile->sds_id_l1t_row = FAIL;


	tile->b01   = NULL;
	tile->b02   = NULL;
	tile->b03   = NULL;
	tile->b04   = NULL;
	tile->b05   = NULL;
	tile->b61   = NULL;
	tile->b62   = NULL;
	tile->b07   = NULL;
	tile->ndvi  = NULL;
	tile->doy   = NULL;
	tile->sat   = NULL;
	tile->dtcld = NULL;
	tile->acca  = NULL;
	tile->nobs  = NULL;
	tile->cpath = NULL;
	tile->senrs = NULL;
	tile->vz    = NULL;
	tile->va    = NULL;
	tile->sz    = NULL;
	tile->sa    = NULL;
	tile->l1t_index = NULL;
	tile->l1t_col = NULL;
	tile->l1t_row = NULL;
	tile->l1t_metadata = NULL;
	tile->inp_pointers = NULL;

	tile->as_pointers=0;

	if (( (tile->b01   = (int16*)calloc(tile->nrow*tile->ncol, sizeof(int16))) == NULL) ||
	    ( (tile->b02   = (int16*)calloc(tile->nrow*tile->ncol, sizeof(int16))) == NULL) ||
	    ( (tile->b03   = (int16*)calloc(tile->nrow*tile->ncol, sizeof(int16))) == NULL) ||
	    ( (tile->b04   = (int16*)calloc(tile->nrow*tile->ncol, sizeof(int16))) == NULL) ||
	    ( (tile->b05   = (int16*)calloc(tile->nrow*tile->ncol, sizeof(int16))) == NULL) ||
	    ( (tile->b61   = (int16*)calloc(tile->nrow*tile->ncol, sizeof(int16))) == NULL) ||
	    ( (tile->b62   = (int16*)calloc(tile->nrow*tile->ncol, sizeof(int16))) == NULL) ||
	    ( (tile->b07   = (int16*)calloc(tile->nrow*tile->ncol, sizeof(int16))) == NULL) ||
	    ( (tile->ndvi  = (int16*)calloc(tile->nrow*tile->ncol, sizeof(int16))) == NULL) ||
	    ( (tile->doy   = (int16*)calloc(tile->nrow*tile->ncol, sizeof(int16))) == NULL) ||
	    ( (tile->sat   = (uint8*)calloc(tile->nrow*tile->ncol, sizeof(uint8))) == NULL) ||
	    ( (tile->dtcld = (uint8*)calloc(tile->nrow*tile->ncol, sizeof(uint8))) == NULL) ||
	    ( (tile->acca  = (uint8*)calloc(tile->nrow*tile->ncol, sizeof(uint8))) == NULL) ||
	    ( (tile->nobs  = (uint16*)calloc(tile->nrow*tile->ncol,sizeof(uint16)))== NULL) ||
	    ( (tile->cpath = (uint8*)calloc(tile->nrow*tile->ncol, sizeof(uint8))) == NULL) ||
	    ( (tile->senrs = (uint8*)calloc(tile->nrow*tile->ncol, sizeof(uint8))) == NULL) ){
		status = 4;
		Error("Cannot allocate memory", "allocate_tile", status);
		goto CLEANUP;
			
	}

	if(tile->geometry_flag==1){
		if( ( (tile->vz   = (int16*)calloc(tile->nrow*tile->ncol, sizeof(int16))) == NULL) ||
		    ( (tile->va   = (int16*)calloc(tile->nrow*tile->ncol, sizeof(int16))) == NULL) ||
		    ( (tile->sz   = (int16*)calloc(tile->nrow*tile->ncol, sizeof(int16))) == NULL) ||
		    ( (tile->sa   = (int16*)calloc(tile->nrow*tile->ncol, sizeof(int16))) == NULL)){
			status = 4;
			Error("Cannot allocate memory", "allocate_tile", status);
			goto CLEANUP;
		}
	}

	if(tile->l1t_index_flag==1){
		  if( (tile->l1t_index = (uint16*)calloc(tile->nrow*tile->ncol, sizeof(uint16))) == NULL ||
			  (tile->l1t_col   = (uint16*)calloc(tile->nrow*tile->ncol, sizeof(uint16))) == NULL ||
			  (tile->l1t_row   = (uint16*)calloc(tile->nrow*tile->ncol, sizeof(uint16))) == NULL ){
				status = 4;
				Error("Cannot allocate memory", "allocate_tile", status);
				goto CLEANUP;
		  }
	}

	/*	Note: memory for TM and ETM l1t_metadata string will be allocated in function open_tile
		because amount of needed memory is not known yet 
	*/

	CLEANUP:
	if(status!=0){
		if(tile->b01    != NULL)free(tile->b01    );
		if(tile->b02    != NULL)free(tile->b02    );
		if(tile->b03    != NULL)free(tile->b03    );
		if(tile->b04    != NULL)free(tile->b04    );
		if(tile->b05    != NULL)free(tile->b05    );
		if(tile->b61    != NULL)free(tile->b61    );
		if(tile->b62    != NULL)free(tile->b62    );
		if(tile->b07    != NULL)free(tile->b07    );
		if(tile->ndvi   != NULL)free(tile->ndvi   );
		if(tile->doy    != NULL)free(tile->doy    );
		if(tile->sat    != NULL)free(tile->sat    );
		if(tile->dtcld  != NULL)free(tile->dtcld  );
		if(tile->acca   != NULL)free(tile->acca   );
		if(tile->nobs   != NULL)free(tile->nobs   );
		if(tile->cpath  != NULL)free(tile->cpath  );
		if(tile->senrs  != NULL)free(tile->senrs  );
		if(tile->vz     != NULL)free(tile->vz     );
		if(tile->va     != NULL)free(tile->va     );
		if(tile->sz     != NULL)free(tile->sz     );
		if(tile->sa     != NULL)free(tile->sa     );
		if(tile->l1t_index        != NULL) free(tile->l1t_index);
		if(tile->l1t_col          != NULL) free(tile->l1t_col);
		if(tile->l1t_row          != NULL) free(tile->l1t_row);
		if(tile->l1t_metadata  != NULL) free(tile->l1t_metadata);
		if(tile->inp_pointers  != NULL) free(tile->inp_pointers);


		tile->b01    = NULL;
		tile->b02    = NULL;
		tile->b03    = NULL;
		tile->b04    = NULL;
		tile->b05    = NULL;
		tile->b61   = NULL;
		tile->b62   = NULL;
		tile->b07    = NULL;
		tile->ndvi  = NULL;
		tile->doy   = NULL;
		tile->sat   = NULL;
		tile->dtcld = NULL;
		tile->acca  = NULL;
		tile->nobs  = NULL;
		tile->cpath = NULL;
		tile->senrs = NULL;
		tile->vz    = NULL;
		tile->va    = NULL;
		tile->sz    = NULL;
		tile->sa    = NULL;
		tile->l1t_index = NULL;
		tile->l1t_col   = NULL;
		tile->l1t_row   = NULL;
		tile->l1t_metadata = NULL;
		tile->inp_pointers = NULL;
	}
	return status;
}

/***************************************************************************************
	Update the pixel in the  given tile 
***************************************************************************************/

void l457_tile_update(l457_tile_info_t *tile,
		      int k_tile, 
		      l457_obs_t *obs,
		      int  acq_index,
		      int  acq_col,
		      int  acq_row)
{
	tile->b01[k_tile]    = obs->b01;
	tile->b02[k_tile]    = obs->b02;
	tile->b03[k_tile]    = obs->b03;
	tile->b04[k_tile]    = obs->b04;
	tile->b05[k_tile]    = obs->b05;
	tile->b61[k_tile]    = obs->b61;
	tile->b62[k_tile]    = obs->b62;
	tile->b07[k_tile]    = obs->b07;
	tile->ndvi[k_tile]   = obs->ndvi;
	tile->doy[k_tile]    = obs->doy;
	tile->sat[k_tile]    = obs->sat;
	tile->dtcld[k_tile]  = obs->dtcld;
	tile->acca[k_tile]   = obs->acca;
	//tile->cpath[k_tile]  = obs->qa_comp;// no need to do it now
	tile->senrs[k_tile]  = obs->lsnsr_i;

	if (tile->geometry_flag == 1) { 
		tile->vz[k_tile] = obs->vz;
		tile->va[k_tile] = obs->va;
		tile->sz[k_tile] = obs->sz;
		tile->sa[k_tile] = obs->sa;
	}

	if (tile->l1t_index_flag == 1){
		tile->l1t_index[k_tile] = (uint16)(acq_index);
		tile->l1t_col[k_tile] = (uint16)(acq_col);
		tile->l1t_row[k_tile] = (uint16)(acq_row);
	}
}

/*********************************************************************************
  	Write all the SDS to the composite tile. The tile will be closed by another 
	function close_comp_tile(l457_tile_info_t *) which is explicitly created as a
	function as it can also be close the existing old composite.

	Return 0 on success and non-zero on failure.
*********************************************************************************/
int l457_tile_write(l457_tile_info_t *tile)
{
	int32 start[2], edge[2];
	char status = 0;

	start[0] = 0; edge[0] = tile->nrow;
	start[1] = 0; edge[1] = tile->ncol;

	/*  b01*/
	if (SDwritedata(tile->sds_id_b01, start, NULL, edge, tile->b01) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "write_comp_tile", status);
		goto CLEANUP;
	}
   
	/*  b02*/
	if (SDwritedata(tile->sds_id_b02, start, NULL, edge, tile->b02) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "write_comp_tile", status);
		goto CLEANUP;
	}
   
	/* b03*/
	if (SDwritedata(tile->sds_id_b03, start, NULL, edge, tile->b03) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "write_comp_tile", status);
		goto CLEANUP;
	}
   
	/* b04*/
	if (SDwritedata(tile->sds_id_b04, start, NULL, edge, tile->b04) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "write_comp_tile", status);
		goto CLEANUP;
	}
   
	/*  b05*/
	if (SDwritedata(tile->sds_id_b05, start, NULL, edge, tile->b05) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "write_comp_tile", status);
		goto CLEANUP;
	}
   
	/* b61*/
	if (SDwritedata(tile->sds_id_b61, start, NULL, edge, tile->b61) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "write_comp_tile", status);
		goto CLEANUP;
	}
   
	/*  b62*/
	if (SDwritedata(tile->sds_id_b62, start, NULL, edge, tile->b62) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "write_comp_tile", status);
		goto CLEANUP;
	}
   
	/* b07*/
	if (SDwritedata(tile->sds_id_b07, start, NULL, edge, tile->b07) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "write_comp_tile", status);
		goto CLEANUP;
	}
   
	/*  ndvi*/
	if (SDwritedata(tile->sds_id_ndvi, start, NULL, edge, tile->ndvi) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "write_comp_tile", status);
		goto CLEANUP;
	}
   
	/*  doy*/
	if (SDwritedata(tile->sds_id_doy, start, NULL, edge, tile->doy) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "write_comp_tile", status);
		goto CLEANUP;
	}
   
	/* saturation*/
	if (SDwritedata(tile->sds_id_sat, start, NULL, edge, tile->sat) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "write_comp_tile", status);
		goto CLEANUP;
	}
   
	/* dtcld*/
	if (SDwritedata(tile->sds_id_dtcld, start, NULL, edge, tile->dtcld) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "write_comp_tile", status);
		goto CLEANUP;
	}
   
	/* acca*/
	if (SDwritedata(tile->sds_id_acca, start, NULL, edge, tile->acca) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "write_comp_tile", status);
		goto CLEANUP;
	}
   
	/* nobs*/
	if (SDwritedata(tile->sds_id_nobs, start, NULL, edge, tile->nobs) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "write_comp_tile", status);
		goto CLEANUP;
	}

	/* cpath */
	if (SDwritedata(tile->sds_id_cpath, start, NULL, edge, tile->cpath) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "write_comp_tile", status);
		goto CLEANUP;
	}

	/* senrs */
	if (SDwritedata(tile->sds_id_senrs, start, NULL, edge, tile->senrs) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "write_comp_tile", status);
		goto CLEANUP;
	}

	if (tile->geometry_flag == 1) {
		/*  vz*/
		if (SDwritedata(tile->sds_id_vz, start, NULL, edge, tile->vz) == FAIL) {
			Error("SDwritedata failed", "write_comp_tile", status);
			status = 2;
			goto CLEANUP;
		}
	   
		/*  va*/
		if (SDwritedata(tile->sds_id_va, start, NULL, edge, tile->va) == FAIL) {
			status = 2;
			Error("SDwritedata failed", "write_comp_tile", status);
			goto CLEANUP;
		}
	   
		/*  sz*/
		if (SDwritedata(tile->sds_id_sz, start, NULL, edge, tile->sz) == FAIL) {
			status = 2;
			Error("SDwritedata failed", "write_comp_tile", status);
			goto CLEANUP;
		}
	   
		/*  sa*/
		if (SDwritedata(tile->sds_id_sa, start, NULL, edge, tile->sa) == FAIL) {
			status = 2;
			Error("SDwritedata failed", "write_comp_tile", status);
			goto CLEANUP;
		}
	}

	if (tile->l1t_index_flag == 1) {
		if ((SDwritedata(tile->sds_id_l1t_index, start, NULL, edge, tile->l1t_index) == FAIL) ||
			(SDwritedata(tile->sds_id_l1t_col  , start, NULL, edge, tile->l1t_col) == FAIL) ||
			(SDwritedata(tile->sds_id_l1t_row  , start, NULL, edge, tile->l1t_row) == FAIL) ){
			status = 2;
			Error("SDwritedata failed", "write_comp_tile", status);
			goto CLEANUP;
		}
	}


	if (l457_tile_add_meta(tile)!=0) {
		status = 2;
		Error("SDwritedata failed to add meta data", "write_comp_tile", status);
		goto CLEANUP;
	}

	CLEANUP:
	return status;
}

/*************************************************************************
	Close the composite HDF and free the dynamically allocated memory.
*/
int l457_tile_close(l457_tile_info_t *tile)
{
	int status = 0;
	int32 stCheck = 0;

	/*b01*/
	if (tile->sds_id_b01 != FAIL) {
		if (SDendaccess(tile->sds_id_b01) == FAIL) {
			status = 2;
                	Error("SDendaccess failed", "l457_tile_close", status);
                	goto CLEANUP;
		}
		tile->sds_id_b01 = FAIL;
	}
	if (tile->b01	!= NULL) {free(tile->b01); tile->b01	= NULL;}

	/*b02*/
	if (tile->sds_id_b02 != FAIL) {
		if (SDendaccess(tile->sds_id_b02) == FAIL) {
			status = 2;
                	Error("SDendaccess failed", "l457_tile_close", status);
                	goto CLEANUP;
		}
		tile->sds_id_b02 = FAIL;
	}
	if (tile->b02	!= NULL) {free(tile->b02); tile->b02	= NULL;}

	/*b03*/
	if (tile->sds_id_b03 != FAIL) {
		if (SDendaccess(tile->sds_id_b03) == FAIL) {
			status = 2;
                	Error("SDendaccess failed", "l457_tile_close", status);
                	goto CLEANUP;
		}
		tile->sds_id_b03 = FAIL;
	}
	if (tile->b03	!= NULL) {free(tile->b03); tile->b03	= NULL;}


	/*b04*/
	if (tile->sds_id_b04 != FAIL) {
		if (SDendaccess(tile->sds_id_b04) == FAIL) {
			status = 2;
                	Error("SDendaccess failed", "l457_tile_close", status);
                	goto CLEANUP;
		}
		tile->sds_id_b04 = FAIL;
	}
	if (tile->b04	!= NULL) {free(tile->b04); tile->b04	= NULL;}

	/*b05*/
	if (tile->sds_id_b05 != FAIL) {
		if (SDendaccess(tile->sds_id_b05) == FAIL) {
			status = 2;
                	Error("SDendaccess failed", "l457_tile_close", status);
                	goto CLEANUP;
		}
		tile->sds_id_b05 = FAIL;
	}
	if (tile->b05	!= NULL) {free(tile->b05); tile->b05	= NULL;}


	/*b61*/
	if (tile->sds_id_b61 != FAIL) { 
		if (SDendaccess(tile->sds_id_b61) == FAIL) { 
			status = 2;
                	Error("SDendaccess failed", "l457_tile_close", status);
                	goto CLEANUP;
		}
		tile->sds_id_b61 = FAIL; 
	}
	if (tile->b61   != NULL) {free(tile->b61  ); tile->b61   = NULL;} 

	/*b62*/
	if (tile->sds_id_b62 != FAIL) {
		if (SDendaccess(tile->sds_id_b62) == FAIL) {
			status = 2;
                	Error("SDendaccess failed", "l457_tile_close", status);
                	goto CLEANUP;
		}
		tile->sds_id_b62 = FAIL;
	}
	if (tile->b62   != NULL) {free(tile->b62  ); tile->b62   = NULL;} 

	/*b07*/
	if (tile->sds_id_b07 != FAIL) {
		if (SDendaccess(tile->sds_id_b07) == FAIL) {
			status = 2;
			Error("Error in SDendaccess", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sds_id_b07 = FAIL;
	}
	if (tile->b07	!= NULL) {free(tile->b07); tile->b07	= NULL;}

	/*ndvi*/
	if (tile->sds_id_ndvi != FAIL) { 
		if (SDendaccess(tile->sds_id_ndvi) == FAIL) { 
			status = 2;
			Error("Error in SDendaccess", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sds_id_ndvi = FAIL; 
	}
	if (tile->ndvi  != NULL) {free(tile->ndvi ); tile->ndvi  = NULL;} 

	/*doy*/
	if (tile->sds_id_doy != FAIL) {
		if (SDendaccess(tile->sds_id_doy) == FAIL) {
			status = 2;
			Error("Error in SDendaccess", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sds_id_doy = FAIL;
	}
	if (tile->doy   != NULL) {free(tile->doy  ); tile->doy   = NULL;} 

	/*sat*/
	if (tile->sds_id_sat != FAIL) {
		if (SDendaccess(tile->sds_id_sat) == FAIL) {
			status = 2;
			Error("Error in SDendaccess", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sds_id_sat = FAIL;
	}
	if (tile->sat   != NULL) {free(tile->sat  ); tile->sat   = NULL;} 

	/*dtcld*/
	if (tile->sds_id_dtcld != FAIL) {
		if (SDendaccess(tile->sds_id_dtcld) == FAIL) {
			status = 2;
			Error("Error in SDendaccess", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sds_id_dtcld = FAIL;
	}
	if (tile->dtcld != NULL) {free(tile->dtcld); tile->dtcld = NULL;} 

	/*acca*/
	if (tile->sds_id_acca != FAIL) {
		if (SDendaccess(tile->sds_id_acca) == FAIL) {
			status = 2;
			Error("Error in SDendaccess", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sds_id_acca = FAIL;
	}
	if (tile->acca  != NULL) {free(tile->acca ); tile->acca  = NULL;} 

	/*nobs*/
	if (tile->sds_id_nobs != FAIL) {
		if (SDendaccess(tile->sds_id_nobs) == FAIL) {
			status = 2;
			Error("Error in SDendaccess", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sds_id_nobs = FAIL;
	}
	if (tile->nobs  != NULL) {free(tile->nobs ); tile->nobs  = NULL;} 

	/*cpath*/
	if (tile->sds_id_cpath != FAIL) {
		if (SDendaccess(tile->sds_id_cpath) == FAIL) {
			status = 2;
			Error("Error in SDendaccess", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sds_id_cpath = FAIL;
	}
	if (tile->cpath  != NULL) {free(tile->cpath ); tile->cpath  = NULL;}

	/*senrs*/
	if (tile->sds_id_senrs != FAIL) {
		if (SDendaccess(tile->sds_id_senrs) == FAIL) {
			status = 2;
			Error("Error in SDendaccess", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sds_id_senrs = FAIL;
	}
	if (tile->senrs  != NULL) {free(tile->senrs ); tile->senrs  = NULL;}


	/*  If geometry SDS is not desired in the output tile, the four sds_id are bound to be FAIL */
	if (tile->sds_id_vz != FAIL) {
		if (SDendaccess(tile->sds_id_vz) == FAIL) {
			status = 2;
			Error("Error in SDendaccess", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sds_id_vz = FAIL;
	}
	if (tile->vz	!= NULL) {free(tile->vz); tile->vz	= NULL;} 

	/*va*/
	if (tile->sds_id_va != FAIL) {
		if (SDendaccess(tile->sds_id_va) == FAIL) {
			status = 2;
			Error("Error in SDendaccess", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sds_id_va = FAIL;
	}
	if (tile->va	!= NULL) {free(tile->va); tile->va	= NULL;} 

	/*sz*/
	if (tile->sds_id_sz != FAIL) {
		if (SDendaccess(tile->sds_id_sz) == FAIL) {
			status = 2;
			Error("Error in SDendaccess", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sds_id_sz = FAIL; 
	}
	if (tile->sz	!= NULL) {free(tile->sz); tile->sz	= NULL;} 

	/*sa*/
	if (tile->sds_id_sa != FAIL) {
		if (SDendaccess(tile->sds_id_sa) == FAIL) {
			status = 2;
			Error("Error in SDendaccess", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sds_id_sa = FAIL;
	}
	if (tile->sa	!= NULL) {free(tile->sa); tile->sa	= NULL;} 


	/* l1t_index */
	if (tile->sds_id_l1t_index != FAIL) {
		if (SDendaccess(tile->sds_id_l1t_index) == FAIL) {
			status = 2;
			Error("Error in SDendaccess", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sds_id_l1t_index = FAIL;
	}
	if (tile->l1t_index	!= NULL) {free(tile->l1t_index); tile->l1t_index = NULL;}

	/* l1t_col */
	if (tile->sds_id_l1t_col != FAIL) {
		if (SDendaccess(tile->sds_id_l1t_col) == FAIL) {
			status = 2;
			Error("Error in SDendaccess", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sds_id_l1t_col = FAIL;
	}
	if (tile->l1t_col	!= NULL) {free(tile->l1t_col); tile->l1t_col = NULL;}

	/* l1t_row */
	if (tile->sds_id_l1t_row != FAIL) {
		if (SDendaccess(tile->sds_id_l1t_row) == FAIL) {
			status = 2;
			Error("Error in SDendaccess", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sds_id_l1t_row = FAIL;
	}
	if (tile->l1t_row	!= NULL) {free(tile->l1t_row); tile->l1t_row = NULL;}


	if (tile->l1t_metadata	!= NULL) {
		free(tile->l1t_metadata);
		tile->l1t_metadata	= NULL;
	}

	if (tile->inp_pointers	!= NULL) {
		free(tile->inp_pointers);
		tile->inp_pointers	= NULL;
	}

	/* the file */
	if (tile->sd_id != FAIL) { 
		stCheck=SDend(tile->sd_id);
		if (stCheck == FAIL) {
			status = 2;
			Error("Error in SDend", "l457_tile_close", status);
			goto CLEANUP;
		}
		tile->sd_id = FAIL; 
	}
	CLEANUP:
	return status;
}






















/*********************************************************************************
* Name:	l457_pointer_tile_open()
* Purpose: Open the tile in DFACC_READ mode and
*  read the various L1T pointer related sds's
* 	into corresponding buffers.
*
* Args:
*	  @tile		- l457_tile_info_t tile structure to hold the data
*
* Calls: main();
*********************************************************************************/

int l457_pointer_tile_open(l457_tile_info_t *tile)
{
	/*int32 n_datasets, n_file_attrs;*/
	int32 attr_index, sds_index;
	int32 n_values;
	char attr_name[MAX_NAME_LEN];
	char sds_name[MAX_NC_NAME];
	int32 dim_sizes[2];
	int32 rank, data_type, n_attrs;
	int32 start[2], edge[2];
	int status = 0;
	char message[MSGLEN];
	sds_index=FAIL;
	if ((status = l457_pointer_tile_allocate(tile)) != 0) {
		Error("Error in allocate_tile", "l457_pointer_tile_open", 0);
		return status;
	}

	if ((tile->sd_id = SDstart(tile->fname, DFACC_READ)) == FAIL) {
		status = 2;
		sprintf(message, "Cannot open %s for DFACC_WRITE", tile->fname);
		Error(message, "l457_pointer_tile_open", status);
		goto CLEANUP;
	}


	/*  b61*/
	strcpy(sds_name, B61_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_b61 =  SDselect(tile->sd_id, sds_index);

	/*  b62*/
	strcpy(sds_name, B62_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_b62 =  SDselect(tile->sd_id, sds_index);

	/*  saturation*/
	strcpy(sds_name, SAT_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_sat = SDselect(tile->sd_id, sds_index);

	/* nobs*/
	strcpy(sds_name, NOBS_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_pointer_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_nobs =  SDselect(tile->sd_id, sds_index);

	/* cpath*/
	strcpy(sds_name, CPATH_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_pointer_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_cpath =  SDselect(tile->sd_id, sds_index);

	strcpy(sds_name, L1T_INDEX_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_pointer_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_l1t_index = SDselect(tile->sd_id, sds_index);

	strcpy(sds_name, L1T_COL_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_pointer_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_l1t_col = SDselect(tile->sd_id, sds_index);

	strcpy(sds_name, L1T_ROW_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(tile->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, tile->fname);
		Error(message, "l457_pointer_tile_open", status);
		goto CLEANUP;
	}
	tile->sds_id_l1t_row = SDselect(tile->sd_id, sds_index);



	/*  Dimension of the tile */
	status = SDgetinfo(tile->sds_id_l1t_index, sds_name, &rank, dim_sizes, &data_type, &n_attrs);
	start[0] = 0; edge[0] = dim_sizes[0];
	start[1] = 0; edge[1] = dim_sizes[1];
	if (dim_sizes[0] != tile->nrow || dim_sizes[1] != tile->ncol) {
		status = 2;
		sprintf(message, "The tile dimension of %s are not nrowi = %d, ncol = %d", tile->fname, tile->nrow, tile->ncol);
		Error(message, "l457_pointer_tile_open", status);
		goto CLEANUP;
	}

	if((SDreaddata(tile->sds_id_b61,      start, NULL, edge, tile->b61)      == FAIL) ||
	   (SDreaddata(tile->sds_id_b62,      start, NULL, edge, tile->b62)      == FAIL) ||
	   (SDreaddata(tile->sds_id_sat,      start, NULL, edge, tile->sat)      == FAIL) ||
	   (SDreaddata(tile->sds_id_nobs,      start, NULL, edge, tile->nobs)      == FAIL) ||
	   (SDreaddata(tile->sds_id_cpath,     start, NULL, edge, tile->cpath)     == FAIL) ||
	   (SDreaddata(tile->sds_id_l1t_index, start, NULL, edge, tile->l1t_index) == FAIL) ||
	   (SDreaddata(tile->sds_id_l1t_col,   start, NULL, edge, tile->l1t_col)   == FAIL) ||
	   (SDreaddata(tile->sds_id_l1t_row,   start, NULL, edge, tile->l1t_row)   == FAIL) ){
		status = 2;
		sprintf(message, "Cannot read l1t_index from %s", tile->fname);
		Error(message, "l457_pointer_tile_open", status);
		goto CLEANUP;
	}

	/*L1T_INFO*/
	attr_index = SDfindattr(tile->sd_id, L1T_METADATA);
	if (attr_index == FAIL) {
		status = 2;
		sprintf(message, "No attribute %s in %s", L1T_METADATA, tile->fname);
		Error(message, "l457_pointer_tile_open", status);
		goto CLEANUP;
	}

	if (SDattrinfo(tile->sd_id, attr_index, attr_name, &data_type, &n_values) == FAIL) {
		status = 2;
		sprintf(message, "SDattrinfo failed for %s", tile->fname);
		Error(message, "l457_pointer_tile_open", status);
		goto CLEANUP;
	}


	if ((tile->l1t_metadata = (char*)calloc(n_values+1, sizeof(char))) == NULL) {
		status = 4;
		Error("Cannot allocate memory", "l457_pointer_tile_open", status);
					goto CLEANUP;
	}

	if (SDreadattr(tile->sd_id, attr_index, tile->l1t_metadata) == FAIL) {
		status = 2;
		sprintf(message, "SDreadattr failed for %s", tile->fname);
		Error(message, "l457_pointer_tile_open", status);
					goto CLEANUP;
	}

	tile->l1t_metadata[n_values] = '\0';

	/*INP_PNTRS*/
	attr_index = SDfindattr(tile->sd_id, INP_POINETR);
	if (attr_index == FAIL) {
		status = 2;
		sprintf(message, "No attribute %s in %s", INP_POINETR, tile->fname);
		Error(message, "l457_pointer_tile_open", status);
		goto CLEANUP;
	}

	if (SDattrinfo(tile->sd_id, attr_index, attr_name, &data_type, &n_values) == FAIL) {
		status = 2;
		sprintf(message, "SDattrinfo failed for %s", tile->fname);
		Error(message, "l457_pointer_tile_open", status);
		goto CLEANUP;
	}


	if ((tile->inp_pointers = (char*)calloc(n_values+1, sizeof(char))) == NULL) {
		status = 4;
		Error("Cannot allocate memory", "l457_pointer_tile_open", status);
					goto CLEANUP;
	}

	if (SDreadattr(tile->sd_id, attr_index, tile->inp_pointers) == FAIL) {
		status = 2;
		sprintf(message, "SDreadattr failed for %s", tile->fname);
		Error(message, "l457_pointer_tile_open", status);
					goto CLEANUP;
	}
	tile->inp_pointers[n_values] = '\0';


	CLEANUP:
	return status;
}



/*********************************************************************************
* Name:	l457_pointer_tile_create()
* Purpose: Create a WELD composite tile and set the projection information and  SDS attributes.
*
* Args:
*	  @tile		- l457_tile_info_t tile structure to hold the data
*     @all_sds  - sds_descriptor array
*     @region   - region of the data tile
*     @tot_sds   - total sds
*
* Calls: main();
*********************************************************************************/
int l457_pointer_tile_create(l457_tile_info_t *tile, sds_info_t *all_sds, char *region, int * tot_sds)
{
	int32 dim_sizes[2];
	int status = 0;
	int32 rank;
	int irow, icol;
	long k;
	char message[MSGLEN];
	char tmpstr[MSGLEN];

	int32 comp_type;   /*Compression type flag*/
	comp_info c_info;  /*Compression structure*/

	char dim_names[2][100];
	int rank0, rank1;

	Myhdf_attr_t attr;
	double dval[100];   /* double type attribute values*/

	if ((status = l457_tile_allocate(tile)) != 0) {
		Error("Error in allocate_tile", "l457_pointer_tile_create", 0);
		return status;
	}
	tile->as_pointers=1; // after previous step this is 0
	if ((tile->sd_id = SDstart(tile->fname, DFACC_CREATE)) == FAIL) {
		status = 2;
		sprintf(message, "Cannot create hdf file %s", tile->fname);
		Error(message, "l457_pointer_tile_create", status);
		goto CLEANUP;
	}

	rank = 2;
	dim_sizes[0] = tile->nrow;
	dim_sizes[1] = tile->ncol;

  if (	(tile->sds_id_b61   	= SDcreate(tile->sd_id,   B61_SDS_NAME, 	DFNT_INT16,  rank, dim_sizes)) == FAIL ||
		(tile->sds_id_b62   	= SDcreate(tile->sd_id,   B62_SDS_NAME, 	DFNT_INT16,  rank, dim_sizes)) == FAIL ||
		(tile->sds_id_sat   	= SDcreate(tile->sd_id,   SAT_SDS_NAME, 	DFNT_UINT8,  rank, dim_sizes)) == FAIL ||
		(tile->sds_id_nobs      = SDcreate(tile->sd_id, NOBS_SDS_NAME,	    DFNT_UINT16, rank, dim_sizes)) == FAIL ||
		(tile->sds_id_cpath     = SDcreate(tile->sd_id, CPATH_SDS_NAME,	    DFNT_UINT8,  rank, dim_sizes)) == FAIL ||
		(tile->sds_id_l1t_index = SDcreate(tile->sd_id, L1T_INDEX_SDS_NAME, DFNT_UINT16, rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_l1t_col   = SDcreate(tile->sd_id, L1T_COL_SDS_NAME,   DFNT_UINT16, rank, dim_sizes)) == FAIL ||
	    (tile->sds_id_l1t_row   = SDcreate(tile->sd_id, L1T_ROW_SDS_NAME,   DFNT_UINT16, rank, dim_sizes)) == FAIL ){
		status = 2;
		sprintf(message, "Cannot create SDS in %s", tile->fname);
		Error(message, "l457_pointer_tile_create", status);
		goto CLEANUP;
	}

	/* Initialize to fill value */
	for (irow = 0; irow < tile->nrow; irow++) {
		for (icol = 0; icol < tile->ncol; icol++) {
		        k = irow * tile->ncol + icol;
		        tile->b61[k] 	= int16_minus32768;
		        tile->b62[k] 	= int16_minus32768;
		        tile->sat[k]   	= uint8_0;
		        tile->nobs[k]  	= uint16_0;
		        tile->cpath[k]  = uint8_0;
				tile->l1t_index[k] = uint16_65535;
				tile->l1t_col[k] = uint16_65535;
				tile->l1t_row[k] = uint16_65535;

		}
	}



	if ((SDsetattr(tile->sds_id_b61,  	   "_FillValue", DFNT_INT16,  1, (VOIDP)&int16_minus32768) == FAIL) ||
		(SDsetattr(tile->sds_id_b62,  	   "_FillValue", DFNT_INT16,  1, (VOIDP)&int16_minus32768) == FAIL) ||
		(SDsetattr(tile->sds_id_sat,  	   "_FillValue", DFNT_UINT8, 1, (VOIDP)&uint8_0		 ) == FAIL) ||
		(SDsetattr(tile->sds_id_nobs,      "_FillValue", DFNT_UINT16, 1, (VOIDP)&uint16_0	 ) == FAIL) ||
		(SDsetattr(tile->sds_id_cpath,     "_FillValue", DFNT_UINT8, 1, (VOIDP)&uint8_0	 ) == FAIL)	||
		(SDsetattr(tile->sds_id_l1t_index, "_FillValue", DFNT_UINT16, 1, (VOIDP)&uint16_65535) == FAIL) ||
		(SDsetattr(tile->sds_id_l1t_col,   "_FillValue", DFNT_UINT16, 1, (VOIDP)&uint16_65535) == FAIL) ||
		(SDsetattr(tile->sds_id_l1t_row,   "_FillValue", DFNT_UINT16, 1, (VOIDP)&uint16_65535) == FAIL) ){
		status = 2;
		sprintf(message, "SDsetattr _FillValue failed for %s", tile->fname);
		Error(message, "l457_pointer_tile_create", status);
		goto CLEANUP;
	}

	/* Set dimname */
	strcpy(dim_names[0], "YDim_Grid");
	strcpy(dim_names[1], "XDim_Grid");
	rank0 = 0;
	rank1 = 1;

	if ((PutSDSDimInfo(tile->sds_id_b61,   dim_names[0], rank0) == 1  ||
		 PutSDSDimInfo(tile->sds_id_b61,   dim_names[1], rank1) == 1) ||
		(PutSDSDimInfo(tile->sds_id_b62,   dim_names[0], rank0) == 1  ||
		 PutSDSDimInfo(tile->sds_id_b62,   dim_names[1], rank1) == 1) ||
		(PutSDSDimInfo(tile->sds_id_sat,   dim_names[0], rank0) == 1  ||
		 PutSDSDimInfo(tile->sds_id_sat,   dim_names[1], rank1) == 1) ||
		(PutSDSDimInfo(tile->sds_id_nobs,  dim_names[0], rank0) == 1  ||
		 PutSDSDimInfo(tile->sds_id_nobs,  dim_names[1], rank1) == 1) ||
		(PutSDSDimInfo(tile->sds_id_cpath, dim_names[0], rank0) == 1  ||
	     PutSDSDimInfo(tile->sds_id_cpath, dim_names[1], rank1) == 1) ||
		(PutSDSDimInfo(tile->sds_id_l1t_index,  dim_names[0], rank0) == 1  ||
		 PutSDSDimInfo(tile->sds_id_l1t_index,	dim_names[1], rank1) == 1) ||
		(PutSDSDimInfo(tile->sds_id_l1t_col,    dim_names[0], rank0) == 1  ||
		 PutSDSDimInfo(tile->sds_id_l1t_col,	dim_names[1], rank1) == 1) ||
		(PutSDSDimInfo(tile->sds_id_l1t_row,    dim_names[0], rank0) == 1  ||
		 PutSDSDimInfo(tile->sds_id_l1t_row,	dim_names[1], rank1) == 1) ){
		status = 2;
		Error("PutSDSDimInfo() failed", "l457_pointer_tile_create", 0);
		goto CLEANUP;
	}



	/*	Setup the SDS attributes */

	if ((status = setup_pointer_sds_info(all_sds, tot_sds)) != 0) {
		Error("Error in setup_pointer_sds_info()", "l457_pointer_tile_create", 0);
		goto CLEANUP;
	}

	/*  SDS attribute */
		if (set_sds_attribute(tile->sds_id_b61,       B61_SDS_NAME,      all_sds, NSDS_MAX) == 1 ||
			set_sds_attribute(tile->sds_id_b62,       B62_SDS_NAME,      all_sds, NSDS_MAX) == 1 ||
			set_sds_attribute(tile->sds_id_sat,       SAT_SDS_NAME,      all_sds, NSDS_MAX) == 1 ||
			set_sds_attribute(tile->sds_id_nobs,      NOBS_SDS_NAME,     all_sds, NSDS_MAX) == 1 ||
			set_sds_attribute(tile->sds_id_cpath,     CPATH_SDS_NAME,    all_sds, NSDS_MAX) == 1 ||
			set_sds_attribute(tile->sds_id_l1t_index, L1T_INDEX_SDS_NAME,all_sds, NSDS_MAX) == 1 ||
		    set_sds_attribute(tile->sds_id_l1t_col,	  L1T_COL_SDS_NAME,  all_sds, NSDS_MAX) == 1 ||
		    set_sds_attribute(tile->sds_id_l1t_row,	  L1T_ROW_SDS_NAME,  all_sds, NSDS_MAX) == 1) {
			status = 2;
			Error("Error in set_sds_attribute", "l457_pointer_tile_create", 0);
			goto CLEANUP;
		}




	/*****************  begin: set valid range ********************************/
	/* ALL spectral bands valid range*/


	attr.type = DFNT_INT16;
	attr.nval = 2;
	attr.name = VALID_RANGE;
	dval[0] = -32767;
	dval[1] =  32767;
	if (!PutAttrDouble(tile->sds_id_b61,  &attr, dval) ||
		!PutAttrDouble(tile->sds_id_b62,  &attr, dval) ){
		status = 2;
		Error("Error in PutAttrDouble", "l457_tile_create", 0);
		goto CLEANUP;
	}
	/* saturation*/
	attr.type = DFNT_UINT8;
	attr.nval = 2;
	attr.name = VALID_RANGE;
	dval[0] = 0;
	dval[1] = 255;
	if (!PutAttrDouble(tile->sds_id_sat,  &attr, dval)) {
		status = 2;
		Error("Error in PutAttrDouble", "l457_tile_create", 0);
		goto CLEANUP;
	}

	/* nobs*/
	attr.type = DFNT_UINT16;
	attr.nval = 2;
	attr.name = VALID_RANGE;
	dval[0] = 0;
	dval[1] = 65534;
	if (!PutAttrDouble(tile->sds_id_nobs,  &attr, dval)) {
		status = 2;
		Error("Error in PutAttrDouble", "l457_tile_create", 0);
		goto CLEANUP;
	}

	/* cpath */
	attr.type = DFNT_UINT16;
	attr.nval = 2;
	attr.name = VALID_RANGE;
	dval[0] = 0;
	dval[1] = 15;
	if (!PutAttrDouble(tile->sds_id_cpath,  &attr, dval)) {
		status = 2;
		Error("Error in PutAttrDouble", "l457_tile_create", 0);
		goto CLEANUP;
	}

	attr.type = DFNT_UINT16;
	attr.nval = 2;
	attr.name = VALID_RANGE;
	dval[0] = 0;
	dval[1] = 65534;
	if (!PutAttrDouble(tile->sds_id_l1t_index,  &attr, dval)) {
		status = 2;
		Error("Error in PutAttrDouble", "l457_pointer_tile_create", 0);
		goto CLEANUP;
	}

	/* for L1T row and column*/
	dval[0] = 0;
	/*should be no more than 9000 but 10000 is ok*/
	dval[1] = 10000;
	if (!PutAttrDouble(tile->sds_id_l1t_col,  &attr, dval)) {
		status = 2;
		Error("Error in PutAttrDouble", "l457_pointer_tile_create", 0);
		goto CLEANUP;
	}

	if (!PutAttrDouble(tile->sds_id_l1t_row,  &attr, dval)) {
		status = 2;
		Error("Error in PutAttrDouble", "l457_pointer_tile_create", 0);
		goto CLEANUP;
	}


	/*****************  end: set valid range ********************************/


	comp_type = COMP_CODE_DEFLATE;
	c_info.deflate.level = COMPRESS_LEVEL;	 /*Level 9 would be too slow */

		if (SDsetcompress(tile->sds_id_b61		, comp_type, &c_info) == FAIL ||
			SDsetcompress(tile->sds_id_b62		, comp_type, &c_info) == FAIL ||
			SDsetcompress(tile->sds_id_sat		, comp_type, &c_info) == FAIL ||
			SDsetcompress(tile->sds_id_nobs     , comp_type, &c_info) == FAIL ||
			SDsetcompress(tile->sds_id_cpath    , comp_type, &c_info) == FAIL ||
			SDsetcompress(tile->sds_id_l1t_index, comp_type, &c_info) == FAIL ||
			SDsetcompress(tile->sds_id_l1t_col  , comp_type, &c_info) == FAIL ||
			SDsetcompress(tile->sds_id_l1t_row  , comp_type, &c_info) == FAIL ){
			status = 2;
			Error("SDsetcompress failed", "l457_pointer_tile_create", status);
			goto CLEANUP;
		}

	strcpy(tmpstr, PRODUCT_VERSION);
	tmpstr[strlen(PRODUCT_VERSION)]='\0';
    if (SDsetattr(tile->sd_id, PRODUCT_VERSION_AN, DFNT_CHAR8, strlen(tmpstr), (VOIDP)tmpstr) == FAIL) {
		status = 2;
		Error("SDsetattr failed to add PRODUCT_VERSION and counts", "l457_pointer_tile_create", status);
		goto CLEANUP;
    }
    strcpy(tmpstr, PGE_VERSION_NRT);
    tmpstr[strlen(PGE_VERSION_NRT)]='\0';
    if (SDsetattr(tile->sd_id, PGE_VERSION_AN, DFNT_CHAR8, strlen(tmpstr), (VOIDP)tmpstr) == FAIL) {
    	status = 2;
    	Error("SDsetattr failed to add PGE_VERSION_NRT and counts", "l457_pointer_tile_create", status);
    	goto CLEANUP;
    }


	/* Add corner coordinates */
	tile->pixel_size = REF_PIXSIZE;
	if (l457_tile_add_cornes(tile, region) != 0) {
		status = 2;
		Error("Failed in l457_tile_add_cornes", "l457_pointer_tile_create", 0);
		goto CLEANUP;
	}

	CLEANUP:
		if (status!=0){
			l457_tile_close(tile);
		}
	return status;
}


/*********************************************************************************
* Name:	l457_pointer_tile_allocate()
* Purpose: Allocates resources for L1T pointers only.
*
* Args:
*	  @tile		- l457_tile_info_t tile structure to hold the data
*
* Calls: l457_pointer_tile_create();
*********************************************************************************/

int  l457_pointer_tile_allocate(l457_tile_info_t *tile){

	int status = 0;

	tile->sd_id	= FAIL;
	tile->sds_id_b01	= FAIL;
	tile->sds_id_b02	= FAIL;
	tile->sds_id_b03	= FAIL;
	tile->sds_id_b04	= FAIL;
	tile->sds_id_b05	= FAIL;
	tile->sds_id_b61= FAIL;
	tile->sds_id_b62= FAIL;
	tile->sds_id_b07	= FAIL;
	tile->sds_id_ndvi  = FAIL;
	tile->sds_id_doy   = FAIL;
	tile->sds_id_sat   = FAIL;
	tile->sds_id_dtcld = FAIL;
	tile->sds_id_acca  = FAIL;
	tile->sds_id_nobs  = FAIL;
	tile->sds_id_cpath = FAIL;
	tile->sds_id_senrs = FAIL;

	tile->sds_id_vz	= FAIL;
	tile->sds_id_va	= FAIL;
	tile->sds_id_sz	= FAIL;
	tile->sds_id_sa	= FAIL;
	tile->sds_id_l1t_index = FAIL;
	tile->sds_id_l1t_col = FAIL;
	tile->sds_id_l1t_row = FAIL;


	tile->b01   = NULL;
	tile->b02   = NULL;
	tile->b03   = NULL;
	tile->b04   = NULL;
	tile->b05   = NULL;
	tile->b61   = NULL;
	tile->b62   = NULL;
	tile->b07   = NULL;
	tile->ndvi  = NULL;
	tile->doy   = NULL;
	tile->sat   = NULL;
	tile->dtcld = NULL;
	tile->acca  = NULL;
	tile->nobs  = NULL;
	tile->cpath = NULL;
	tile->senrs = NULL;
	tile->vz    = NULL;
	tile->va    = NULL;
	tile->sz    = NULL;
	tile->sa    = NULL;
	tile->l1t_index = NULL;
	tile->l1t_col = NULL;
	tile->l1t_row = NULL;
	tile->l1t_metadata = NULL;
	tile->inp_pointers = NULL;

	tile->as_pointers=1;


	  if( ((tile->b61      = (int16*)calloc( tile->nrow*tile->ncol, sizeof(int16)))  == NULL) ||
		  ((tile->b62      = (int16*)calloc( tile->nrow*tile->ncol, sizeof(int16)))  == NULL) ||
		  ((tile->sat      = (uint8*)calloc( tile->nrow*tile->ncol, sizeof(uint8)))  == NULL) ||
		  (tile->nobs      = (uint16*)calloc(tile->nrow*tile->ncol, sizeof(uint16))) == NULL ||
		  (tile->cpath     = (uint8*)calloc( tile->nrow*tile->ncol, sizeof(uint8)))  == NULL ||
		  (tile->l1t_index = (uint16*)calloc(tile->nrow*tile->ncol, sizeof(uint16))) == NULL ||
		  (tile->l1t_col   = (uint16*)calloc(tile->nrow*tile->ncol, sizeof(uint16))) == NULL ||
		  (tile->l1t_row   = (uint16*)calloc(tile->nrow*tile->ncol, sizeof(uint16))) == NULL ){
			status = 4;
			Error("Cannot allocate memory", "allocate_tile", status);
			goto CLEANUP;
	  }

	/*	Note: memory for TM and ETM l1t_metadata string will be allocated in function open_tile
		because amount of needed memory is not known yet
	*/

	CLEANUP:
	if(status!=0){

		if(tile->b61   != NULL)free(tile->b61   );
		if(tile->b62   != NULL)free(tile->b62  );
		if(tile->sat   != NULL)free(tile->sat   );
		if(tile->nobs   != NULL)free(tile->nobs   );
		if(tile->cpath  != NULL)free(tile->cpath  );
		if(tile->l1t_index        != NULL) free(tile->l1t_index);
		if(tile->l1t_col          != NULL) free(tile->l1t_col);
		if(tile->l1t_row          != NULL) free(tile->l1t_row);

		tile->b01    = NULL;
		tile->b02    = NULL;
		tile->b03    = NULL;
		tile->b04    = NULL;
		tile->b05    = NULL;
		tile->b61   = NULL;
		tile->b62   = NULL;
		tile->b07    = NULL;
		tile->ndvi  = NULL;
		tile->doy   = NULL;
		tile->sat   = NULL;
		tile->dtcld = NULL;
		tile->acca  = NULL;
		tile->nobs  = NULL;
		tile->cpath = NULL;
		tile->senrs = NULL;
		tile->vz    = NULL;
		tile->va    = NULL;
		tile->sz    = NULL;
		tile->sa    = NULL;
		tile->l1t_index = NULL;
		tile->l1t_col   = NULL;
		tile->l1t_row   = NULL;
		tile->l1t_metadata = NULL;
		tile->inp_pointers = NULL;
	}
	return status;
}




/*********************************************************************************
* Name:	l457_pointer_tile_write()
* Purpose: writes L1T pointers only + meta data;
*
* Args:
*	  @tile		- l457_tile_info_t tile structure to hold the data
*
* Calls: l457_pointer_tile_create();
*********************************************************************************/

int l457_pointer_tile_write(l457_tile_info_t *tile){
	int32 start[2], edge[2];
	char status = 0;

	start[0] = 0; edge[0] = tile->nrow;
	start[1] = 0; edge[1] = tile->ncol;

	/* b61*/
	if (SDwritedata(tile->sds_id_b61, start, NULL, edge, tile->b61) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "l457_pointer_tile_write", status);
		goto CLEANUP;
	}

	/*  b62*/
	if (SDwritedata(tile->sds_id_b62, start, NULL, edge, tile->b62) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "l457_pointer_tile_write", status);
		goto CLEANUP;
	}

	/* saturation*/
	if (SDwritedata(tile->sds_id_sat, start, NULL, edge, tile->sat) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "l457_pointer_tile_write", status);
		goto CLEANUP;
	}

	/* nobs*/
	if (SDwritedata(tile->sds_id_nobs, start, NULL, edge, tile->nobs) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "l457_pointer_tile_write", status);
		goto CLEANUP;
	}

	/* cpath */
	if (SDwritedata(tile->sds_id_cpath, start, NULL, edge, tile->cpath) == FAIL) {
		status = 2;
		Error("SDwritedata failed", "l457_pointer_tile_write", status);
		goto CLEANUP;
	}

	if ((SDwritedata(tile->sds_id_l1t_index, start, NULL, edge, tile->l1t_index) == FAIL) ||
		(SDwritedata(tile->sds_id_l1t_col  , start, NULL, edge, tile->l1t_col) == FAIL) ||
		(SDwritedata(tile->sds_id_l1t_row  , start, NULL, edge, tile->l1t_row) == FAIL) ){
		status = 2;
		Error("SDwritedata failed", "l457_pointer_tile_write", status);
		goto CLEANUP;
	}


	if (l457_tile_add_meta(tile)!=0) {
		status = 2;
		Error("SDwritedata failed to add meta data", "l457_pointer_tile_write", status);
		goto CLEANUP;
	}

	CLEANUP:
	return status;
}








/*********************************************************************************
	Open a bookkeeping alldoy tile in DFACC_WRITE mode.
	The alldoy tile is used in removing the same-day within-path UMT data redundancy.
 
	Return 0 if success, and non-zero on failure.
*********************************************************************************/
int alldoy_tile_open(alldoy_t *alldoy)
{
	int32 sds_index;
	char  sds_name[MAX_NC_NAME];
	int32 start[2], edge[2];
	int32 k, obs;
	char  message[MSGLEN];
	int   status = 0;

	alldoy->sd_id = FAIL;
	for(obs=0;obs<ALLDOYMAX;obs++){
		alldoy->sds_id_alldoy[obs] = FAIL;
	}
	alldoy->alldoy = NULL;


	if ((alldoy->alldoy = (uint16*)calloc((ALLDOYMAX * alldoy->nrow * alldoy->ncol), sizeof(uint16))) == NULL) {
		status = 4;
		Error("Cannot allocate memory", "open_alldoy_tile", status);
		goto CLEANUP;
	}

	if ((alldoy->sd_id = SDstart(alldoy->fname, DFACC_WRITE)) == FAIL) {
		status = 2;
		sprintf(message, "Cannot open bookkeeping file %s", alldoy->fname);
		Error(message, "open_alldoy_tile", status);
		goto CLEANUP;
	}
			
	start[0] = 0; edge[0] = alldoy->nrow;
	start[1] = 0; edge[1] = alldoy->ncol;
	
	for(obs=0;obs<ALLDOYMAX;obs++){
		sprintf(sds_name, "%s_%d", ALLDOY_27, obs);
		if ((sds_index = SDnametoindex(alldoy->sd_id, sds_name)) == FAIL) {
			status = 2;
			sprintf(message, "No SDS %s in %s", sds_name, alldoy->fname);
			Error(message, "open_alldoy_tile", status);
			goto CLEANUP;
		}
		alldoy->sds_id_alldoy[obs] = SDselect(alldoy->sd_id, sds_index);
		k=obs*(int32)(alldoy->ncol)*(int32)(alldoy->nrow);
		if (SDreaddata(alldoy->sds_id_alldoy[obs], start, NULL, edge, &alldoy->alldoy[k]) == FAIL) {
			status = 2;
			sprintf(message, "Cannot read SDS %s from %s", sds_name, alldoy->fname);
			Error(message, "open_alldoy_tile", status);
			goto CLEANUP;
		}
	}

	CLEANUP:
	if (status != 0) 
		alldoy_tile_close(alldoy);

	return status; 
}

/*********************************************************************************
	Create a bookkeeping alldoy tile.
	The alldoy tile is used in removing the same-day within-path UMT data redundancy.
 
	Return 0 if success, and non-zero on failure.
*********************************************************************************/
int alldoy_tile_create(alldoy_t *alldoy)
{
	char  sds_name[MAX_NC_NAME];
	int32 dim_sizes[2];
	int32 rank;
	int32 irow, icol, k, obs;
	char  message[MSGLEN];
	int   status = 0;
	int32 comp_type;   /*Compression flag*/
	comp_info c_info;  /*Compression structure*/

	alldoy->sd_id = FAIL;
	for(obs=0;obs<ALLDOYMAX;obs++){
		alldoy->sds_id_alldoy[obs] = FAIL;
	}
	alldoy->alldoy = NULL;


	if ((alldoy->alldoy = (uint16*)calloc((ALLDOYMAX * alldoy->nrow * alldoy->ncol), sizeof(uint16))) == NULL) {
		status = 4;
		Error("Cannot allocate memory", "open_alldoy_tile", status);
		goto CLEANUP;
	}

	if ((alldoy->sd_id = SDstart(alldoy->fname, DFACC_CREATE)) == FAIL) {
		status = 2;
		sprintf(message, "Cannot create hdf file %s", alldoy->fname);
		Error(message, "open_alldoy_tile", status);
		goto CLEANUP;
	}

	rank = 2;
	dim_sizes[0] = alldoy->nrow;
	dim_sizes[1] = alldoy->ncol;
	comp_type = COMP_CODE_DEFLATE;
	c_info.deflate.level = COMPRESS_LEVEL;	 /*Level 9 would be too slow */

	for(k=0;k<ALLDOYMAX;k++){
		sprintf(sds_name, "%s_%d", ALLDOY_27, k);
		if ((alldoy->sds_id_alldoy[k] = SDcreate(alldoy->sd_id, sds_name, DFNT_UINT16, rank, dim_sizes)) == FAIL) {
			status = 2;
			sprintf(message, "Cannot create SDS %s in %s", sds_name, alldoy->fname);
			Error(message, "open_alldoy_tile", status);
			goto CLEANUP;
		}
		SDsetcompress(alldoy->sds_id_alldoy[k],	 comp_type, &c_info);
		SDsetattr(alldoy->sds_id_alldoy[k], "_FillValue", DFNT_UINT32, 1, (VOIDP)&int16_0);
	}

	/*Initialize to fillval */
	for (obs = 0;obs<ALLDOYMAX;obs++){
		for (irow = 0; irow < alldoy->nrow; irow++) {
			for (icol = 0; icol < alldoy->ncol; icol++) {
				k = (obs*(int32)(alldoy->ncol)*(int32)(alldoy->nrow))+(irow * alldoy->ncol) + icol;
				alldoy->alldoy[k] = uint16_0;
			}
		}
	}
	CLEANUP:
	if (status != 0) 
		alldoy_tile_close(alldoy);
	return status; 
}


/***************************************************************************************
	Update the list of DOYs at a pixel in the tile. 

	Note: this function is designed to work in conjunction with function isRedundant()
	to remove the same-day within-orbit overlap in generating the monthly (or shorter period)
	composites, so that the NOBS SDS in the composite is correct without double counting. 

	Potential bugs:
	The considered observations include those from the adjacent paths and so 
	there are no more than 4 observations in a month. But for high latitude,
	because of the extensive sideral overlap with the adjacent paths, there might
	be more than 4 observations in a month. As a result, these two functions might fail 
	to remove the within-path data redundancy, but the composite result is correct in any case.  
	To correct the problem for higher latitude, it would need  to use an additional array;
	fix this later by trial and error to find out how many arrays are needed. 

	Return 0 on success, return 1 if this function does not work because 
	there are more than 4 observation in the given period.

***************************************************************************************/
int alldoy_tile_update(int16 doy,
			alldoy_t *alldoy, 
			int k_tile)
{
	int status;
	int32 obs,k;

	status = 6;/* asssuming failure */

	for(obs=0;obs<ALLDOYMAX;obs++){
		k = (obs*(int32)(alldoy->ncol)*(int32)(alldoy->nrow))+(int32)(k_tile);
		if(alldoy->alldoy[k]==0){
			alldoy->alldoy[k]=doy;
			status = 0; /*success*/
			break;
		}
	}

	if(status !=0){
		Error("There are more than 27 observation in the given period; use additional arrays to track DOYs",
			  "update_alldoy_tile", status);
	}
	return status;
}

/*********************************************************************************
	Write the bookkeeping alldoy tile. The alldoy tile will be closed later using 
	another function.

	Return 0 on success and non-zero on failure.
*********************************************************************************/
int alldoy_tile_write(alldoy_t *alldoy)
{
	int32 obs, k;
	char status = 0;
	char message[MSGLEN];
	int32 start[2], edge[2];

	start[0] = 0; edge[0] = alldoy->nrow;
	start[1] = 0; edge[1] = alldoy->ncol;

	for(obs=0;obs<ALLDOYMAX;obs++){
		k=obs*(int32)(alldoy->ncol)*(int32)(alldoy->nrow);
		if (SDwritedata(alldoy->sds_id_alldoy[obs], start, NULL, edge, &alldoy->alldoy[k]) == FAIL) {
			status = 2;
			sprintf(message, "SDwritedata failed for %s", alldoy->fname);
			Error(message, "write_alldoy_tile", status);
			goto CLEANUP;
		}
	}
	CLEANUP:
	return status;
}



/*********************************************************************************
	Close the bookkeeping alldoy tile and free the dynamically allocated memory.
	Return 0 on success and 1 on failure.
*********************************************************************************/
int alldoy_tile_close(alldoy_t *alldoy)
{
	int   status = 0;
	char  message[MSGLEN];
	int32 obs;

	for(obs=0;obs<ALLDOYMAX;obs++){
		if (alldoy->sds_id_alldoy[obs] != FAIL) {
			if (SDendaccess(alldoy->sds_id_alldoy[obs]) == FAIL) {
				status = 2;
				sprintf(message, "Error in SDendaccess() to %s", alldoy->fname);
				Error(message, "close_alldoy_tile", status);
				goto CLEANUP;
			}
		}

	}
	if (alldoy->alldoy != NULL) {free(alldoy->alldoy); alldoy->alldoy = NULL; }
   
	if (alldoy->sd_id != FAIL) {
		if (SDend(alldoy->sd_id) == FAIL) {
			status = 2;
			sprintf("Error in SDend() for %s", alldoy->fname);
			Error(message, "close_alldoy_tile", status);
			goto CLEANUP;
		}
	}

	CLEANUP:
	return status;
}




/***************************************************************************************/
/*
	To check if the input DOY of the pixel is already in the recorded list of DOYs for the
	given pixel of the tile.

	Return 1 if the observation is redundant and the pixel will be ignored.
	Return 0 otherwise.
*/
/***************************************************************************************/

int isRedundant( int16 doy,
		 alldoy_t *alldoy,
		 int k_tile)
{
	char found = 0;
	int32 obs,k;
	for(obs=0;obs<ALLDOYMAX;obs++){
		k = (obs*(int32)(alldoy->ncol)*(int32)(alldoy->nrow))+(int32)(k_tile);
		if(alldoy->alldoy[k]==doy){
			found = 1;
			break;
		}
	}

	return found;
}


/***********************************************************************************************************************
	load the array of strings all_l1tinfo by parsing the given tile's l1t_metadata
	character string, and also update all_nacq and derive new_nacq.

	Return 0 on success and non-zero on failure.

***********************************************************************************************************************/
int l457_tile_load_l1tinfo(char *l1t_metadata, char ***all_l1tinfo, int *all_nacq, int *new_nacq)
{
	int LEN = 200;
	char line[LEN];
	char *line_begin, *line_end, *pos;
	int n;
	int status;
	int iline;
	char **ptr;
	int i;

	/*  Example l1t_metadata from ncdump.

  	:L1T_METADATA = "BEGIN_L1T_INFO\n",
    	"Totally 8 acquisitions with the following band 1 names and scene center solar geometry:\n",
    	"0 L70450282008070LGN00 center_sz=29.640991 center_sa=134.639969\n",
    	"1 L70450282008072LGN00 center_sz=31.998859 center_sa=136.539413\n",
    	"2 L70450292008070LGN00 center_sz=28.876621 center_sa=132.151764\n",
    	"3 L70450292008072LGN00 center_sz=31.188808 center_sa=134.285782\n",
    	"4 L70460282008071LGN00 center_sz=30.535648 center_sa=135.205811\n",
    	"5 L70460292008071LGN00 center_sz=29.760048 center_sa=132.803543\n",
    	"6 L70470282008072LGN00 center_sz=31.646263 center_sa=136.190369\n",
    	"7 L70470292008072LGN00 center_sz=30.844398 center_sa=133.900421\n",
    	"END_L1T_INFO\n",
	*/

	status = 0;
	iline = 0;
	line_begin = l1t_metadata;
	while ((line_end = strchr(line_begin, '\n')) != NULL) {	/* Each acquisition's L1T information ends with '\n' in l1t_metadata */
		n = line_end - line_begin;
		if (n >= LEN) {
			status = 3;
			Error("LEN is too small; set a greater value", "l457_tile_load_l1tinfo", status);
			goto CLEANUP;
		}
		strncpy(line, line_begin, n); 
		line[n] = '\0';

		if (iline == 0)   		/* first line: "BEGIN_L1T_INFO" */
			;
		else if (strcmp(line, "END_L1T_INFO") == 0) 
			break; 			/* last line anyway */
		else if (iline == 1) {		/* second line */
			sscanf(line, "Totally %d", new_nacq); 	
			
			if ((ptr = (char**)calloc(*all_nacq+*new_nacq, sizeof(char*))) == NULL) {
				status = 4;
				Error("Cannot allocate memory", "update_l1tinfo", status);
				goto CLEANUP;
			}
			if (*all_l1tinfo != NULL) {
				for (i=0; i < *all_nacq; i++) 
					ptr[i] = (*all_l1tinfo)[i];

				free(*all_l1tinfo);
			}

			*all_l1tinfo = ptr;
		} 
		else {
			/* Skip the beginning number on each line before copying */
			pos = strchr(line, 'L');
			if (((*all_l1tinfo)[*all_nacq] = mystrdup(pos)) == NULL) {
				status = 4;
				Error("Cannot allocate memory in mystrdup", "l457_tile_load_l1tinfo", status);
				goto CLEANUP;
			}

			(*all_nacq)++;
		}
		
		iline++;
		line_begin = line_end+1;
	}

	CLEANUP:

	return status;
}


/***********************************************************************************************************************
	load the array of strings all_inp_pointers by parsing the given tile's inp_pointers
	character string, and also update all_inpntrs and derive new_inpntrs.

	Return 0 on success and non-zero on failure.

***********************************************************************************************************************/
int l457_tile_load_inp_pointers(char *inp_pointers, char ***all_inp_pointers, int *all_inpntrs, int *new_inpntrs)
{
	int LEN = 200;
	char line[LEN];
	char *line_begin, *line_end, *pos;
	int n;
	int status;
	int iline;
	char **ptr;
	int i;

	/*  Example l1t_metadata from ncdump.

  	:INP_POINTERS = "BEGIN_INP_POINTERS\n",
    	"Totally 8 acquisitions with the following band 1 names and scene center solar geometry:\n",
    	"0 L7045028_02820080706_B10.hdf center_sz=29.640991 center_sa=134.639969\n",
    	"1 L7045028_02820080722_B10.hdf center_sz=31.998859 center_sa=136.539413\n",
    	"2 L7045029_02920080706_B10.hdf center_sz=28.876621 center_sa=132.151764\n",
    	"3 L7045029_02920080722_B10.hdf center_sz=31.188808 center_sa=134.285782\n",
    	"4 L7046028_02820080713_B10.hdf center_sz=30.535648 center_sa=135.205811\n",
    	"5 L7046029_02920080713_B10.hdf center_sz=29.760048 center_sa=132.803543\n",
    	"6 L7047028_02820080720_B10.hdf center_sz=31.646263 center_sa=136.190369\n",
    	"7 L7047029_02920080720_B10.hdf center_sz=30.844398 center_sa=133.900421\n",
    	"END_INP_POINTERS\n",
	*/

	status = 0;
	iline = 0;
	line_begin = inp_pointers;
	while ((line_end = strchr(line_begin, '\n')) != NULL) {	/* Each acquisition's L1T information ends with '\n' in l1t_metadata */
		n = line_end - line_begin;
		if (n >= LEN) {
			status = 3;
			Error("LEN is too small; set a greater value", "update_l1tinfo", status);
			goto CLEANUP;
		}
		strncpy(line, line_begin, n);
		line[n] = '\0';

		if (iline == 0)   		/* first line: "BEGIN_L1T_INFO" */
			;
		else if (strcmp(line, "END_INPUT_POINTER") == 0)
			break; 			/* last line anyway */
		else if (iline == 1) {		/* second line */
			sscanf(line, "Totally %d", new_inpntrs);

			if ((ptr = (char**)calloc(*all_inpntrs+*new_inpntrs, sizeof(char*))) == NULL) {
				status = 4;
				Error("Cannot allocate memory", "l457_tile_load_inp_pointers", status);
				goto CLEANUP;
			}
			if (*all_inp_pointers != NULL) {
				for (i=0; i < *all_inpntrs; i++)
					ptr[i] = (*all_inp_pointers)[i];

				free(*all_inp_pointers);
			}

			*all_inp_pointers = ptr;
		}
		else {
			/* Skip the beginning number on each line before copying */
			pos = strchr(line, 'L');
			if (((*all_inp_pointers)[*all_inpntrs] = mystrdup(pos)) == NULL) {
				status = 4;
				Error("Cannot allocate memory in mystrdup", "l457_tile_load_inp_pointers", status);
				goto CLEANUP;
			}

			(*all_inpntrs)++;
		}

		iline++;
		line_begin = line_end+1;
	}

	CLEANUP:

	return status;
}



/****************************************************************
	Write the band 1 filenames and scene center geometry of the acquisitions
	that contain valid data for this tile.

****************************************************************/
int l457_tile_write_l1tinfo(l457_tile_info_t *tile, char **l1tinfo, char **inp_pointers, int nacq)
{
	char *l1tmetabuf;
	char *cbuf;
	char *l1tmetaCOPY;
	int ic;
	int iacq;
	int index;
	int status;

	ic = 0;
	status = 0;
	l1tmetabuf=cbuf=l1tmetaCOPY=NULL;

	if((l1tmetabuf  = (char*)calloc(MYHDF_MAX_NATTR_VAL, sizeof(char)))== NULL||
	   (cbuf	    = (char*)calloc(MYHDF_MAX_NATTR_VAL, sizeof(char)))== NULL||
	   (l1tmetaCOPY = (char*)calloc(MYHDF_MAX_NATTR_VAL, sizeof(char)))== NULL){
    	status = 4;
    	Error("Error cannot allocate buffers", "write_l1tinfo", status);
    	goto CLEANUP;
	}

	/***********************************************************
	 * FIRST WRITE L1T info
	 ***********************************************************/


	sprintf(cbuf, "BEGIN_L1T_INFO\n"
		      "Totally %d acquisitions with the following L1T names and scene center solar geometry:\n", nacq);

 	if (!AppendMeta(l1tmetabuf, &ic, cbuf)) {
        	status = 4;
		Error("Error in AppendMeta", "l457_tile_write_l1tinfo", status);
        	goto CLEANUP;
	}
	index = 0;
	for (iacq = 0; iacq < nacq; iacq++) {
		sprintf(cbuf, "%d %s\n", iacq, l1tinfo[iacq]);
		if (!AppendMeta(l1tmetabuf, &ic, cbuf)) {
			status = 4;
			Error("Error in AppendMeta", "l457_tile_write_l1tinfo", status);
			goto CLEANUP;
		}
	}
	sprintf(cbuf, "END_L1T_INFO\n");
 	if (!AppendMeta(l1tmetabuf, &ic, cbuf)) {
        	status = 4;
		Error("Error in AppendMeta", "l457_tile_write_l1tinfo", status);
        	goto CLEANUP;
	}
 	strcpy(l1tmetaCOPY, l1tmetabuf);
	if (SDsetattr(tile->sd_id, L1T_METADATA, DFNT_CHAR8, strlen(l1tmetaCOPY), (VOIDP)l1tmetaCOPY) == FAIL) {
		status = 2;
		Error("SDsetattr failed", "l457_tile_write_l1tinfo", status);
		goto CLEANUP;
	}


	/***********************************************************
	 * SECOND WRITE INP_POINTERS
	 ***********************************************************/

	/* reset buffers */
	if(l1tmetabuf!=NULL)free(l1tmetabuf);
	if(cbuf!=NULL)free(cbuf);
	if(l1tmetaCOPY!=NULL)free(l1tmetaCOPY);
	ic = 0;
	l1tmetabuf=cbuf=l1tmetaCOPY=NULL;
	if((l1tmetabuf  = (char*)calloc(MYHDF_MAX_NATTR_VAL, sizeof(char)))== NULL||
		(cbuf	    = (char*)calloc(MYHDF_MAX_NATTR_VAL, sizeof(char)))== NULL||
		(l1tmetaCOPY = (char*)calloc(MYHDF_MAX_NATTR_VAL, sizeof(char)))== NULL){
	    status = 4;
	    Error("Error cannot allocate buffers", "write_l1tinfo", status);
	    goto CLEANUP;
	}

	sprintf(cbuf, "BEGIN_INPUT_POINTER\n"
		      "Totally %d acquisitions with the following TOA file names and scene center solar geometry:\n", nacq);

 	if (!AppendMeta(l1tmetabuf, &ic, cbuf)) {
        	status = 4;
		Error("Error in AppendMeta", "l457_tile_write_l1tinfo", status);
        	goto CLEANUP;
	}
	index = 0;
	for (iacq = 0; iacq < nacq; iacq++) {
		sprintf(cbuf, "%d %s\n", iacq, inp_pointers[iacq]);
		if (!AppendMeta(l1tmetabuf, &ic, cbuf)) {
			status = 4;
			Error("Error in AppendMeta", "l457_tile_write_l1tinfo", status);
			goto CLEANUP;
		}
	}
	sprintf(cbuf, "END_INPUT_POINTER\n");
 	if (!AppendMeta(l1tmetabuf, &ic, cbuf)) {
        	status = 4;
		Error("Error in AppendMeta", "l457_tile_write_l1tinfo", status);
        	goto CLEANUP;
	}
 	strcpy(l1tmetaCOPY, l1tmetabuf);
	if (SDsetattr(tile->sd_id, INP_POINETR, DFNT_CHAR8, strlen(l1tmetaCOPY), (VOIDP)l1tmetaCOPY) == FAIL) {
		status = 2;
		Error("SDsetattr failed", "l457_tile_write_l1tinfo", status);
		goto CLEANUP;
	}


	
	CLEANUP:
	if(l1tmetabuf!=NULL)free(l1tmetabuf);
	if(cbuf!=NULL)free(cbuf);
	if(l1tmetaCOPY!=NULL)free(l1tmetaCOPY);
	return status;
}

/****************************************************************
	Write the band 1 filenames and scene center geometry of the acquisitions
	that contain valid data for this tile.
	alternative to the above
****************************************************************/

int  l457_tile_write_l1tinfo_alt(l457_tile_info_t *tile, char **l1tinfo, char **inp_pointers, char *use_flags, int nacq)
{
	char *l1tmetabuf;
	char *cbuf;
	char *l1tmetaCOPY;
	int ic, acq_cnt;
	int iacq;
	int status;

	ic = 0;
	acq_cnt=0;
	status = 0;
	l1tmetabuf=cbuf=l1tmetaCOPY=NULL;

	if((l1tmetabuf  = (char*)calloc(MYHDF_MAX_NATTR_VAL, sizeof(char)))== NULL||
	   (cbuf	    = (char*)calloc(MYHDF_MAX_NATTR_VAL, sizeof(char)))== NULL||
	   (l1tmetaCOPY = (char*)calloc(MYHDF_MAX_NATTR_VAL, sizeof(char)))== NULL){
    	status = 4;
    	Error("Error cannot allocate buffers", "write_l1tinfo", status);
    	goto CLEANUP;
	}

	/***********************************************************
	 * FIRST WRITE L1T info
	 ***********************************************************/

	acq_cnt=0;
	for(iacq = 0; iacq < nacq; iacq++){
		if(use_flags[iacq]==1)acq_cnt++;
	}

	sprintf(cbuf, "BEGIN_L1T_INFO\n"
		      "Totally %d acquisitions with the following L1T names and scene center solar geometry:\n", acq_cnt);


	if (!AppendMeta(l1tmetabuf, &ic, cbuf)) {
        	status = 4;
		Error("Error in AppendMeta", "l457_tile_write_l1tinfo", status);
        	goto CLEANUP;
	}
	acq_cnt=0;
	for (iacq = 0; iacq < nacq; iacq++) {
		if(use_flags[iacq]==0)continue;
		sprintf(cbuf, "%d %s\n", acq_cnt, l1tinfo[iacq]);
		if (!AppendMeta(l1tmetabuf, &ic, cbuf)) {
			status = 4;
			Error("Error in AppendMeta", "l457_tile_write_l1tinfo", status);
			goto CLEANUP;
		}
		acq_cnt++;
	}
	sprintf(cbuf, "END_L1T_INFO\n");
 	if (!AppendMeta(l1tmetabuf, &ic, cbuf)) {
        	status = 4;
		Error("Error in AppendMeta", "l457_tile_write_l1tinfo", status);
        	goto CLEANUP;
	}
 	strcpy(l1tmetaCOPY, l1tmetabuf);
	if (SDsetattr(tile->sd_id, L1T_METADATA, DFNT_CHAR8, strlen(l1tmetaCOPY), (VOIDP)l1tmetaCOPY) == FAIL) {
		status = 2;
		Error("SDsetattr failed", "l457_tile_write_l1tinfo", status);
		goto CLEANUP;
	}


	/***********************************************************
	 * SECOND WRITE INP_POINTERS
	 ***********************************************************/

	/* reset buffers */

	ic = 0;

	sprintf(cbuf, "BEGIN_INPUT_POINTER\n"
		      "Totally %d acquisitions with the following TOA file names and scene center solar geometry:\n", acq_cnt);

 	if (!AppendMeta(l1tmetabuf, &ic, cbuf)) {
        	status = 4;
		Error("Error in AppendMeta", "l457_tile_write_l1tinfo", status);
        	goto CLEANUP;
	}
	acq_cnt=0;
	for (iacq = 0; iacq < nacq; iacq++) {
		if(use_flags[iacq]==0)continue;
		sprintf(cbuf, "%d %s\n", acq_cnt, inp_pointers[iacq]);
		if (!AppendMeta(l1tmetabuf, &ic, cbuf)) {
			status = 4;
			Error("Error in AppendMeta", "l457_tile_write_l1tinfo", status);
			goto CLEANUP;
		}
		acq_cnt++;
	}
	sprintf(cbuf, "END_INPUT_POINTER\n");
 	if (!AppendMeta(l1tmetabuf, &ic, cbuf)) {
        	status = 4;
		Error("Error in AppendMeta", "l457_tile_write_l1tinfo", status);
        	goto CLEANUP;
	}
 	strcpy(l1tmetaCOPY, l1tmetabuf);
	if (SDsetattr(tile->sd_id, INP_POINETR, DFNT_CHAR8, strlen(l1tmetaCOPY), (VOIDP)l1tmetaCOPY) == FAIL) {
		status = 2;
		Error("SDsetattr failed", "l457_tile_write_l1tinfo", status);
		goto CLEANUP;
	}


	CLEANUP:
	if(l1tmetabuf!=NULL)free(l1tmetabuf);
	if(cbuf!=NULL)free(cbuf);
	if(l1tmetaCOPY!=NULL)free(l1tmetaCOPY);
	return status;
}



/*********************************************************************************************************************
	Update the l1t_index SDS of the composite regarding the requirements that
	1) the acquisitions that do not have valid data on this tile are not considered in l1tinfo
	2) the l1tinfo strings and input pointers should be sorted alphabetically.

	Side effect: l1tinfo and this_acq_is_used are re-ordered.

	Return 0 on success and non-zero on failure.

*********************************************************************************************************************/
int l457_tile_update_comp_l1t_index(l457_tile_info_t *tile,
				char **l1tinfo,
				char **inp_pointers,
				char *this_acq_is_used,
			 	int nacq,
				int index_fill)
{	
	/* A look-up table to cross-walk the old l1t_index to the new l1t_index.
	   When the new index is generated for an acquisition, any un-used acquisitions before it in
	   the sorted l1t_info are not considered.

	   The LUT is an array, indexed by the old l1t_index value and the value in the array
	   is the new l1t_index value.
	*/
	int *lut;	

	/* For the strings in the aphabetically sorted l1tinfo array, need to know their original 
	   index in l1tinfo before sorting.
	*/
	int *old_order;

	int iacq;
	int i, j;
	char *tempstr;
	int temp, sum;
	int status;

	int idx_old;
	int old; 		/* Must be uint8 because of the fill value 255 in the original index */ 
	int irow, icol, k;

	old_order = NULL;
	tempstr= NULL;
	lut = NULL;

	/*	The original order of l1tinfo strings before sorting */
	if ((old_order = (int*)calloc(nacq, sizeof(int))) == NULL   ||
		(tempstr = (char*)calloc(MAX_NAME_LEN, sizeof(char))) == NULL  ) {
		status = 4;
		Error("Cannot allocate memory", "update_comp_l1t_index", status);
		goto CLEANUP; 
	}
	for (i = 0; i < nacq; i++) 
		old_order[i] = i;
	
	/* A bubble sort of the l1tinfo strings is good enough since the number of strings is small */ 
	for (i = 0; i < nacq-1; i++) {
		for (j = i+1; j < nacq; j++) {
			if (strcmp(l1tinfo[i],  l1tinfo[j]) > 0) {
				strcpy(tempstr, l1tinfo[i]);
				strcpy(l1tinfo[i], l1tinfo[j]);
				strcpy(l1tinfo[j], tempstr);

				strcpy(tempstr, inp_pointers[i]);
				strcpy(inp_pointers[i], inp_pointers[j]);
				strcpy(inp_pointers[j], tempstr);

				temp = this_acq_is_used[i];
				this_acq_is_used[i] = this_acq_is_used[j];
				this_acq_is_used[j] = temp;

				temp = old_order[i];
				old_order[i] = old_order[j];
				old_order[j] = temp;
			}
		} 
	}

	/* LUT */
	if ((lut = (int*)calloc(nacq, sizeof(int))) == NULL) {
		status = 4;
		Error("Cannot allocate memory", "update_comp_l1t_index", status);
		goto CLEANUP; 
	}

	for (idx_old = 0; idx_old < nacq; idx_old++) {
		for (iacq = 0; iacq < nacq; iacq++) {
			if (old_order[iacq] == idx_old) {
				sum = 0;
				for (i = 0; i < iacq; i++)
					sum += this_acq_is_used[i];
				lut[idx_old] = sum;
				break;
			}
		}
	}

	for (irow = 0; irow < TILEDIM; irow++) {
		for (icol = 0; icol < TILEDIM; icol++) {
			k = irow*TILEDIM + icol;
			old = tile->l1t_index[k];
			if (old != index_fill){ 		/* not fill */
				tile->l1t_index[k] = lut[old];
			}
		}
	}

	CLEANUP:
	
	if (tempstr != NULL) free(tempstr);
	if (old_order != NULL) free(old_order);
	if (lut       != NULL) free(lut);

	return status;
}




/***************************************************************************************
	Copy the content of the old composite tile to that of the new composite tile.
***************************************************************************************/
void copy_from_old_comp(l457_tile_info_t *new, l457_tile_info_t *old)
{
	int irow, icol, k;

	for (irow = 0; irow < new->nrow; irow++) {
		for (icol = 0; icol < new->ncol; icol++) {
			k = irow * new->ncol + icol;
			new->b01[k]  = old->b01[k];
			new->b02[k]  = old->b02[k];
			new->b03[k]  = old->b03[k];
			new->b04[k]  = old->b04[k];
			new->b05[k]  = old->b05[k];
			new->b61[k]  = old->b61[k];
			new->b62[k]  = old->b62[k];
			new->b07[k]  = old->b07[k];
			new->ndvi[k] = old->ndvi[k];
			new->doy[k]  = old->doy[k];
			new->sat[k]  = old->sat[k];
			new->dtcld[k]= old->dtcld[k];
			new->acca[k] = old->acca[k];
			new->nobs[k] = old->nobs[k];
			new->cpath[k]= old->cpath[k];
			new->senrs[k]= old->senrs[k];
			if (new->geometry_flag == 1) {
				new->vz[k] = old->vz[k];
				new->va[k] = old->va[k];
				new->sz[k] = old->sz[k];
				new->sa[k] = old->sa[k];
			}
			if (new->l1t_index_flag == 1){
				new->l1t_index[k] = old->l1t_index[k];
				new->l1t_col[k] = old->l1t_col[k];
				new->l1t_row[k] = old->l1t_row[k];
			}
		}
	}
	if (new->l1t_index_flag == 1){
		new->l1t_metadata=old->l1t_metadata;
		old->l1t_metadata=NULL;/*to prevent freeing l1t_metadata when closing the old tile*/
	}
}

/***************************************************************************************
	Copy the content of the old alldoy tile to that of the new alldoy tile.
***************************************************************************************/
void copy_from_old_alldoy(alldoy_t *new, alldoy_t *old)
{
	int32 irow, icol, k, obs;
	for(obs=0;obs<ALLDOYMAX;obs++){
		for (irow = 0; irow < new->nrow; irow++) {
			for (icol = 0; icol < new->ncol; icol++) {
				k = (obs*(int32)(new->ncol)*(int32)(new->nrow))+(irow * new->ncol) + icol;
				new->alldoy[k] = old->alldoy[k];
			}
		}
	}
}

/********************************************************************************
	Find the minimum and maximum nonfill DOY of a tile. This DOY range is used
	in naming the composite. The minimum DOY is intended to represent a day that
	is earlier than maximum DOY. This temporal order is not a problem for the
	non-winter and the non-annual composite. But for the winter and annual composite
	when DOYmin and DOYmax are in different years, the temporal order represented
	by the two numbers is confusing and should be reversed (DO THIS later).


	Return 0 on success and 1 on failure.
*********************************************************************************/

void doy_range(l457_tile_info_t * tile, int16 *doymin, int16 *doymax)
{
	int16 min, max;
	int irow, icol, k;

	min = 366;
	max = 0;

	for (irow = 0; irow < tile->nrow; irow++) {
		for (icol = 0; icol < tile->ncol; icol++) {
			k = irow * tile->ncol + icol;
			if (tile->doy[k] != 0) {	/* nonfill */
				if (tile->doy[k] > max)
					max =  tile->doy[k];

				if (tile->doy[k] < min)
					min =  tile->doy[k];
			}
		}
	}

	*doymin = min;
	*doymax = max;

	return;
}

