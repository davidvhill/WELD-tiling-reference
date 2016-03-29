/*  Called by the toa code to read HDF or write a binary array to an HDF.
	this code is derived as a next generation of TOA processing
	based on

*/

#include <mfhdf.h>
#include "weld.h"
#include "L457.toa.acq.handler.h"

/*********************************************************************************
* Name:	acq_create()
* Purpose: create one hdf for an acq
*
* Args:
*	  @fname_mtl- fname of mtl file
*	  @outdir 	- path for output
*	  @acq		- acquisition to be created
*	  @geom_flag- weather or not alocate space for geometry
*
* Calls: main()
*********************************************************************************/

int acq_create(char* fname_mtl,char * outdir, l457_acq_info_t * acq, uint8 geom_flag){
	int status=0;
	int mtl_len=0;
	char * pos;
	FILE *fmtl = NULL;
	char message[MSGLEN];
	int32 rank;
	int32 dim_sizes[2];
	uint8 fillv_0;
	char acq_date[256];
    int32 comp_type;   /*Compression flag*/
    comp_info c_info;  /*Compression structure*/
    float64 ulx,uly;
    comp_type = COMP_CODE_DEFLATE;
    c_info.deflate.level = COMPRESS_LEVEL;     /*Level 9 would be too slow */
	fillv_0=0;
	acq->isTM=1;
	if(acq_init_attr(fname_mtl, acq, &mtl_len)){
		status = 2;
		sprintf(message, "Cannot initialize ACQ from %s ", fname_mtl);
		Error(message, "acq_create", status);
		return status;
	}

	acq_set_blank(acq);
	if(strstr(acq->input_pointer,L7_KEY) == acq->input_pointer)acq->isTM=0;
	/*now lets allocate memory and room for mtl file in one string */
	if((acq->b01=(int16*)calloc(acq->ncol_ref * acq->nrow_ref, sizeof(int16))) == NULL ||
	   (acq->b02=(int16*)calloc(acq->ncol_ref * acq->nrow_ref, sizeof(int16))) == NULL ||
	   (acq->b03=(int16*)calloc(acq->ncol_ref * acq->nrow_ref, sizeof(int16))) == NULL ||
	   (acq->b04=(int16*)calloc(acq->ncol_ref * acq->nrow_ref, sizeof(int16))) == NULL ||
	   (acq->b05=(int16*)calloc(acq->ncol_ref * acq->nrow_ref, sizeof(int16))) == NULL ||
	   (acq->b61=(int16*)calloc(acq->ncol_thm * acq->nrow_thm, sizeof(int16))) == NULL ||
	   (acq->b62=(int16*)calloc(acq->ncol_thm * acq->nrow_thm, sizeof(int16))) == NULL ||
	   (acq->b07=(int16*)calloc(acq->ncol_ref * acq->nrow_ref, sizeof(int16))) == NULL ||
	   (acq->sat=(uint8*)calloc(acq->ncol_ref * acq->nrow_ref, sizeof(uint8))) == NULL){
		status=2;
		sprintf(message, "Cannot allocate memory for bands " );
		Error(message, "acq_create", status);
		goto CLEANUP;
	}
	if(geom_flag){
		if((acq->sa=(int16*)calloc(acq->ncol_ref * acq->nrow_ref, sizeof(int16))) == NULL ||
		   (acq->sz=(int16*)calloc(acq->ncol_ref * acq->nrow_ref, sizeof(int16))) == NULL ||
		   (acq->va=(int16*)calloc(acq->ncol_ref * acq->nrow_ref, sizeof(int16))) == NULL ||
		   (acq->vz=(int16*)calloc(acq->ncol_ref * acq->nrow_ref, sizeof(int16))) == NULL ){
			status=2;
			sprintf(message, "Cannot allocate memory for bands " );
			Error(message, "acq_create", status);
			goto CLEANUP;
		}
	}
	if ((fmtl = fopen(fname_mtl, "r")) == NULL) {
		status = 2;
		sprintf(message, "Cannot open %s for read", fname_mtl);
		Error(message, "acq_create", status);
		return status;
	}

	pos=strrchr(acq->acq_hdf,'.');

	sprintf(acq->acq_hdf, "%s/%s", outdir, acq->input_pointer);
	pos=strrchr(acq->acq_hdf,'_');
	printf("%s \n", acq->acq_hdf);
	sprintf(pos, "%s.%s%s%c", ACQ_KEY, TOA_VERSION, ".hdf", '\0');

	/*now create all the bands*/
    if ((acq->sd_id = SDstart(acq->acq_hdf, DFACC_CREATE)) == FAIL) {
        status = 5;
        sprintf(message, "Cannot create %s", acq->acq_hdf);
        Error(message, "acq_create", status);
        goto CLEANUP;
    }
	rank = 2;
	dim_sizes[0] = acq->nrow_ref;
	dim_sizes[1] = acq->ncol_ref;
	if ((acq->sds_b01	= SDcreate(acq->sd_id,   B01_SDS_NAME,	DFNT_INT16, rank, dim_sizes)) == FAIL ||
	    (acq->sds_b02	= SDcreate(acq->sd_id,   B02_SDS_NAME,	DFNT_INT16, rank, dim_sizes)) == FAIL ||
	    (acq->sds_b03	= SDcreate(acq->sd_id,   B03_SDS_NAME,	DFNT_INT16, rank, dim_sizes)) == FAIL ||
	    (acq->sds_b04	= SDcreate(acq->sd_id,   B04_SDS_NAME,	DFNT_INT16, rank, dim_sizes)) == FAIL ||
	    (acq->sds_b05	= SDcreate(acq->sd_id,   B05_SDS_NAME,	DFNT_INT16, rank, dim_sizes)) == FAIL ||
	    (acq->sds_b61   = SDcreate(acq->sd_id,   B61_SDS_NAME,  DFNT_INT16, rank, dim_sizes)) == FAIL ||
	    (acq->sds_b62   = SDcreate(acq->sd_id,   B62_SDS_NAME,  DFNT_INT16, rank, dim_sizes)) == FAIL ||
	    (acq->sds_b07	= SDcreate(acq->sd_id,   B07_SDS_NAME,	DFNT_INT16, rank, dim_sizes)) == FAIL ||
	    (acq->sds_sat   = SDcreate(acq->sd_id,   SAT_SDS_NAME,  DFNT_UINT8, rank, dim_sizes)) == FAIL) {
		status = 2;
		sprintf(message, "Cannot create SDS in %s", acq->acq_hdf);
		Error(message, "acq_create", status);
		goto CLEANUP;
	}

	/*************************************************************************************
	* SHOULD NOT BE USED AT THIS POINT (DAVID INSISTED)
	* if(geom_flag){
	*	if ((acq->sds_sz	= SDcreate(acq->sd_id,   SZ_SDS_NAME,	DFNT_INT16, rank, dim_sizes)) == FAIL ||
	*	    (acq->sds_sa	= SDcreate(acq->sd_id,   SA_SDS_NAME,	DFNT_INT16, rank, dim_sizes)) == FAIL ||
	*	    (acq->sds_vz	= SDcreate(acq->sd_id,   VZ_SDS_NAME,	DFNT_INT16, rank, dim_sizes)) == FAIL ||
	*	    (acq->sds_va	= SDcreate(acq->sd_id,   VA_SDS_NAME,	DFNT_INT16, rank, dim_sizes)) == FAIL ){
	*		status=2;
	*		sprintf(message, "Cannot create SDS in %s", acq->acq_hdf);
	*		Error(message, "acq_create", status);
	*		goto CLEANUP;
	*	}
	*}
	**************************************************************************************/

	/* set fill */

	if ((SDsetattr(acq->sds_b01,   "_FillValue",DFNT_INT16, 1, (VOIDP)&int16_minus32768)) == FAIL ||
	    (SDsetattr(acq->sds_b02,   "_FillValue",DFNT_INT16, 1, (VOIDP)&int16_minus32768)) == FAIL ||
	    (SDsetattr(acq->sds_b03,   "_FillValue",DFNT_INT16, 1, (VOIDP)&int16_minus32768)) == FAIL ||
	    (SDsetattr(acq->sds_b04,   "_FillValue",DFNT_INT16, 1, (VOIDP)&int16_minus32768)) == FAIL ||
	    (SDsetattr(acq->sds_b05,   "_FillValue",DFNT_INT16, 1, (VOIDP)&int16_minus32768)) == FAIL ||
	    (SDsetattr(acq->sds_b61,   "_FillValue",DFNT_INT16, 1, (VOIDP)&int16_minus32768)) == FAIL ||
	    (SDsetattr(acq->sds_b62,   "_FillValue",DFNT_INT16, 1, (VOIDP)&int16_minus32768)) == FAIL ||
	    (SDsetattr(acq->sds_b07,   "_FillValue",DFNT_INT16, 1, (VOIDP)&int16_minus32768)) == FAIL ||
	    (SDsetattr(acq->sds_sat,   "_FillValue",DFNT_UINT8, 1, (VOIDP)&fillv_0)) == FAIL) {
		status = 2;
		sprintf(message, "SDsetattr failed for %s", acq->acq_hdf);
		Error(message, "acq_create", status);
		goto CLEANUP;
	}
	/*************************************************************************************
	* SHOULD NOT BE USED AT THIS POINT (DAVID INSISTED)
	*	if(geom_flag){
	*		if ((SDsetattr(acq->sds_sz,   "_FillValue",	DFNT_INT16, 1, (VOIDP)&int16_minus32768)) == FAIL ||
	*			(SDsetattr(acq->sds_sa,   "_FillValue",	DFNT_INT16, 1, (VOIDP)&int16_minus32768)) == FAIL ||
	*			(SDsetattr(acq->sds_vz,   "_FillValue",	DFNT_INT16, 1, (VOIDP)&int16_minus32768)) == FAIL ||
	*			(SDsetattr(acq->sds_va,   "_FillValue",	DFNT_INT16, 1, (VOIDP)&int16_minus32768)) == FAIL ){
	*			status=2;
	*			sprintf(message, "SDsetattr failed for %s", acq->acq_hdf);
	*			Error(message, "acq_create", status);
	*			goto CLEANUP;
	*		}
	*	}
	**************************************************************************************/
	/* set compress only when writing */
	if ((SDsetcompress(acq->sds_b01,   comp_type, &c_info)) == FAIL ||
	    (SDsetcompress(acq->sds_b02,   comp_type, &c_info)) == FAIL ||
	    (SDsetcompress(acq->sds_b03,   comp_type, &c_info)) == FAIL ||
	    (SDsetcompress(acq->sds_b04,   comp_type, &c_info)) == FAIL ||
	    (SDsetcompress(acq->sds_b05,   comp_type, &c_info)) == FAIL ||
	    (SDsetcompress(acq->sds_b61,   comp_type, &c_info)) == FAIL ||
	    (SDsetcompress(acq->sds_b62,   comp_type, &c_info)) == FAIL ||
	    (SDsetcompress(acq->sds_b07,   comp_type, &c_info)) == FAIL ||
	    (SDsetcompress(acq->sds_sat,   comp_type, &c_info)) == FAIL) {
		status = 2;
		sprintf(message, "SDsetcompress failed for %s", acq->acq_hdf);
		Error(message, "acq_create", status);
		goto CLEANUP;
	}
	/*************************************************************************************
	* SHOULD NOT BE USED AT THIS POINT (DAVID INSISTED)
	*	if(geom_flag){
	*		if ((SDsetcompress(acq->sds_sz,comp_type, &c_info)) == FAIL ||
	*			(SDsetcompress(acq->sds_sa,comp_type, &c_info)) == FAIL ||
	*			(SDsetcompress(acq->sds_vz,comp_type, &c_info)) == FAIL ||
	*			(SDsetcompress(acq->sds_va,comp_type, &c_info)) == FAIL ){
	*			status=2;
	*			sprintf(message, "SDsetcompress failed for %s", acq->acq_hdf);
	*			Error(message, "acq_create", status);
	*			goto CLEANUP;
	*		}
	*	}
	***************************************************************************************/

	/*now put all the attributes into the acquisition */
    if (SDsetattr(acq->sd_id, "PRODUCT_TYPE", DFNT_CHAR8, strlen(acq->product_type), (VOIDP)(acq->product_type)) == FAIL) {
        fprintf(stderr, "SDsetattr failed\n");
        status = 5;
        goto CLEANUP;
    }
    sprintf(acq_date,"%d-%d-%d",acq->year,acq->month,acq->day);
    if (SDsetattr(acq->sd_id, INPUT_ACQUISITION_DATE, DFNT_CHAR8, strlen(acq_date), (VOIDP)acq_date) == FAIL) {
        fprintf(stderr, "SDsetattr failed\n");
        status = 5;
        goto CLEANUP;
    }

    if (SDsetattr(acq->sd_id, "WRS_Path", DFNT_INT16, 1, (VOIDP)&acq->pth) == FAIL) {
        fprintf(stderr, "SDsetattr failed\n");
        status = 5;
        goto CLEANUP;
    }

    if (SDsetattr(acq->sd_id, "WRS_Row", DFNT_INT16, 1, (VOIDP)&acq->row) == FAIL) {
        fprintf(stderr, "SDsetattr failed\n");
        status = 5;
        goto CLEANUP;
    }
    ulx=(float64)(acq->ulx);
    if (SDsetattr(acq->sd_id, INPUT_PRODUCT_UL_CORNER_MAPX, DFNT_FLOAT64, 1, (VOIDP)&ulx) == FAIL) {
        fprintf(stderr, "SDsetattr failed\n");
        status = 5;
        goto CLEANUP;
    }
    uly=(float64)(acq->uly);
    if (SDsetattr(acq->sd_id, INPUT_PRODUCT_UL_CORNER_MAPY, DFNT_FLOAT64, 1, (VOIDP)&uly) == FAIL) {
        fprintf(stderr, "SDsetattr failed\n");
        status = 5;
        goto CLEANUP;
    }

    if (SDsetattr(acq->sd_id, INPUT_CENTER_SA, DFNT_FLOAT32, 1, (VOIDP)&acq->center_sa) == FAIL) {
        fprintf(stderr, "SDsetattr failed\n");
        status = 5;
        goto CLEANUP;
    }

    if (SDsetattr(acq->sd_id, INPUT_CENTER_SZ, DFNT_FLOAT32, 1, (VOIDP)&acq->center_sz) == FAIL) {
        fprintf(stderr, "SDsetattr failed\n");
        status = 5;
        goto CLEANUP;
    }

    if (SDsetattr(acq->sd_id, INPUT_GRID_CELL_SIZE, DFNT_FLOAT32, 1, (VOIDP)&acq->pixel_size) == FAIL) {
        fprintf(stderr, "SDsetattr failed\n");
        status = 5;
        goto CLEANUP;
    }

    if (SDsetattr(acq->sd_id, INPUT_UTM_ZONE_NUMBER, DFNT_INT16, 1, (VOIDP)&acq->utm_zone) == FAIL) {
        fprintf(stderr, "SDsetattr failed\n");
        status = 5;
        goto CLEANUP;
    }

    if (SDsetattr(acq->sd_id, "INPUT_POINTER", DFNT_CHAR8, strlen(acq->input_pointer), (VOIDP)(acq->input_pointer)) == FAIL) {
        fprintf(stderr, "SDsetattr failed\n");
        status = 5;
        goto CLEANUP;
    }


	CLEANUP:
	if(status!=0){
		acq_close(acq);
	}

	fclose(fmtl);
	return status;
}


/*********************************************************************************
* Name:	acq_init_attr()
* Purpose: create one hdf for an acq
*
* Args:
*	  @fname_mtl- fname of mtl file
*	  @acq		- acquisition to be created
*	  @mtl_len  - length of mtl file
* Calls: acq_create()
*********************************************************************************/
int acq_init_attr(char* fname_mtl,l457_acq_info_t * acq, int* mtl_len){
	int status=0;
	FILE *fmtl = NULL;
	char message[MSGLEN];
    char s[MAX_LINE_LEN];
    char *ss;
    int tmpInt1=0;
    int tmpInt2=0;
    int tmpInt3=0;

	uint8 nitems=15;
	uint8 Iitem=0;
	if ((fmtl = fopen(fname_mtl, "r")) == NULL) {
		status = 2;
		sprintf(message, "Cannot open %s for read", fname_mtl);
		Error(message, "acq_create", status);
		return status;
	}

	Iitem=0;

	while (fscanf(fmtl, "%s", s) == 1) {
		if(strcmp(s,"END") == 0 ){
			break;
		}
		if(strcmp(s,"DATA_TYPE") == 0 ){
					fscanf(fmtl, "%s", s);	  /*skip '=' */
					fscanf(fmtl, "%s", s);    /*in quotes*/
					strcpy(acq->product_type, (&s[1]));//++ to remove first"
					ss = strchr(acq->product_type, '\"');
					*ss = '\0';//2 is to remove last "
					Iitem++;
		}else if(strcmp(s,"FILE_NAME_BAND_1") == 0 ){
					fscanf(fmtl, "%s", s);	  /*skip '=' */
					fscanf(fmtl, "%s", s);    /*in quotes*/
					strcpy( acq->input_pointer, ((&s[1])));//++ to remove first"
					ss = strchr(acq->input_pointer, '\"');
					*ss = '\0';//2 is to remove last "
					Iitem++;
		}else if(strcmp(s,"REFLECTIVE_LINES") == 0 ){
			fscanf(fmtl, "%s", s);	  /*skip '=' */
			fscanf(fmtl, "%d", &acq->nrow_ref);
			Iitem++;
		}else if(strcmp(s,"REFLECTIVE_SAMPLES") == 0 ){
			fscanf(fmtl, "%s", s);	  /*skip '=' */
			fscanf(fmtl, "%d", &acq->ncol_ref);
			Iitem++;
		}else
		if(strcmp(s,"THERMAL_LINES") == 0 ){
			fscanf(fmtl, "%s", s);	  /*skip '=' */
			fscanf(fmtl, "%d", &acq->nrow_thm);
			Iitem++;
		}else
		if(strcmp(s,"THERMAL_SAMPLES" ) == 0){
			fscanf(fmtl, "%s", s);	  /*skip '=' */
			fscanf(fmtl, "%d", &acq->ncol_thm);
			Iitem++;
		}else
		if(strcmp(s,"CORNER_UL_PROJECTION_X_PRODUCT") == 0 ){
			fscanf(fmtl, "%s", s);	  /*skip '=' */
			fscanf(fmtl, "%f", &acq->ulx);
			Iitem++;
		}else
		if(strcmp(s,"CORNER_UL_PROJECTION_Y_PRODUCT") == 0 ){
			fscanf(fmtl, "%s", s);	  /*skip '=' */
			fscanf(fmtl, "%f", &acq->uly);
			Iitem++;
		}else
		if(strcmp(s,"GRID_CELL_SIZE_REFLECTIVE") == 0 ){
			fscanf(fmtl, "%s", s);	  /*skip '=' */
			fscanf(fmtl, "%f", &acq->pixel_size);
			Iitem++;
		}else
		if(strcmp(s,"UTM_ZONE") == 0 ){
			fscanf(fmtl, "%s", s);	  /*skip '=' */
			fscanf(fmtl, "%d", &tmpInt1);
			acq->utm_zone=(int16)(tmpInt1);
			Iitem++;
		}else
		if(strcmp(s,"DATE_ACQUIRED") == 0 ){
			fscanf(fmtl, "%s", s);	  /*skip '=' */
			fscanf(fmtl, "%d-%d-%d", &tmpInt1, &tmpInt2, &tmpInt3 );
			acq->year=tmpInt1;
			acq->month=tmpInt2;
			acq->day=tmpInt3;
			Iitem++;
		}else
		if(strcmp(s,"WRS_PATH") == 0 ){
			fscanf(fmtl, "%s", s);	  /*skip '=' */
			fscanf(fmtl, "%d", &tmpInt1);
			acq->pth=tmpInt1;
			Iitem++;
		}else
		if(strcmp(s,"WRS_ROW") == 0 ){
			fscanf(fmtl, "%s", s);	  /*skip '=' */
			fscanf(fmtl, "%d", &tmpInt1);
			acq->row=tmpInt1;
			Iitem++;
		}else
		if(strcmp(s,"SUN_AZIMUTH") == 0 ){
			fscanf(fmtl, "%s", s);	  /*skip '=' */
			fscanf(fmtl, "%f", &acq->center_sa);
			Iitem++;
		}else
		if(strcmp(s,"SUN_ELEVATION") == 0 ){
			fscanf(fmtl, "%s", s);	  /*skip '=' */
			fscanf(fmtl, "%f", &acq->center_sz);
			acq->center_sz=90.0-acq->center_sz;
			Iitem++;
		}
	}
	acq->ymax_utm=acq->uly+(acq->pixel_size/2);
	acq->xmin_utm=acq->ulx-(acq->pixel_size/2);

	rewind(fmtl);
	*mtl_len=0;
	while (fgets(s, MAX_LINE_LEN-1, fmtl)) {
		*mtl_len+=(strlen(s)+1);
	}


    if (Iitem != nitems) {
    	sprintf(message, "could not find all the attributes in the mtl; nfound = %d ", Iitem);
    	Error(message, "init_acq_attr", status);
    	status = 1;
    }

	fclose(fmtl);
	return status;
}


/*********************************************************************************
* Name:	acq_close()
* Purpose: create one hdf for an acq
*
* Args:
*	  @acq		- acquisition to be closed
*
* Calls: main();
*********************************************************************************/
int acq_close(l457_acq_info_t * acq){
	int status=0;
	char message[MSGLEN];
	if(!strstr(acq->acq_hdf,ACQ_KEY)){
		status=1;
    	sprintf(message, "cannot close acq which has not been created yet");
    	Error(message, "close_acq", status);
		return status;
	}

	if(acq->b01 != NULL)free(acq->b01);
	if(acq->b02 != NULL)free(acq->b02);
	if(acq->b03 != NULL)free(acq->b03);
	if(acq->b04 != NULL)free(acq->b04);
	if(acq->b05 != NULL)free(acq->b05);
	if(acq->b61 != NULL)free(acq->b61);
	if(acq->b62 != NULL)free(acq->b62);
	if(acq->b07 != NULL)free(acq->b07);
	if(acq->sat != NULL)free(acq->sat);
	if(acq->mtl_cont != NULL)free(acq->mtl_cont);

	if(acq->acca  != NULL)free(acq->acca );
	if(acq->dtcld != NULL)free(acq->dtcld);

	if(acq->sz != NULL)free(acq->sz);
	if(acq->sa != NULL)free(acq->sa);
	if(acq->vz != NULL)free(acq->vz);
	if(acq->va != NULL)free(acq->va);

	if(acq->sds_b01!=FAIL)SDendaccess(acq->sds_b01);
	if(acq->sds_b02!=FAIL)SDendaccess(acq->sds_b02);
	if(acq->sds_b03!=FAIL)SDendaccess(acq->sds_b03);
	if(acq->sds_b04!=FAIL)SDendaccess(acq->sds_b04);
	if(acq->sds_b05!=FAIL)SDendaccess(acq->sds_b05);
	if(acq->sds_b61!=FAIL)SDendaccess(acq->sds_b61);
	if(acq->sds_b62!=FAIL)SDendaccess(acq->sds_b62);
	if(acq->sds_b07!=FAIL)SDendaccess(acq->sds_b07);
	if(acq->sds_sat!=FAIL)SDendaccess(acq->sds_sat);
	if(acq->sds_sz!=FAIL) SDendaccess(acq->sds_sz);
	if(acq->sds_sa!=FAIL) SDendaccess(acq->sds_sa);
	if(acq->sds_vz!=FAIL) SDendaccess(acq->sds_vz);
	if(acq->sds_va!=FAIL) SDendaccess(acq->sds_va);
	if(acq->sd_id !=FAIL) SDend(acq->sd_id);

	acq_set_blank(acq);

	return status;
}
/*********************************************************************************
* Name:	acq_set_blank()
* Purpose: set all pointers and hdf ids to initial position
*
* Args:
*	  @acq		- acquisition to be set blank
*
* Calls: main();
*********************************************************************************/
void acq_set_blank(l457_acq_info_t * acq){

	acq->b01   	 = NULL;
	acq->b02   	 = NULL;
	acq->b03   	 = NULL;
	acq->b04   	 = NULL;
	acq->b05   	 = NULL;
	acq->b61   	 = NULL;
	acq->b62   	 = NULL;
	acq->b07   	 = NULL;
	acq->sat   	 = NULL;
	acq->mtl_cont= NULL;
	acq->sz    	 = NULL;
	acq->sa   	 = NULL;
	acq->vz    	 = NULL;
	acq->va    	 = NULL;
	acq->acca  	 = NULL;
	acq->dtcld 	 = NULL;

	acq->sds_b01 = FAIL;
	acq->sds_b02 = FAIL;
	acq->sds_b03 = FAIL;
	acq->sds_b04 = FAIL;
	acq->sds_b05 = FAIL;
	acq->sds_b61 = FAIL;
	acq->sds_b62 = FAIL;
	acq->sds_b07 = FAIL;
	acq->sds_sat = FAIL;
	acq->sds_sz  = FAIL;
	acq->sds_sa  = FAIL;
	acq->sds_vz  = FAIL;
	acq->sds_va  = FAIL;
	acq->sd_id   = FAIL;

}


/*********************************************************************************
* Name:	acq_write()
* Purpose: create one hdf for an acq
*
* Args:
*	  @acq		- acquisition to be created
*	  @geom_flag- weather or not to write geometry
* Calls: main()
*********************************************************************************/

int acq_write(l457_acq_info_t * acq, uint8 geom_flag){
    int status = 0;
    char message[MSGLEN];
    int32 out_start[2], out_edge[2];      /* start and edge for writing 2d SDS */

    out_start[0] = 0;    out_edge[0] = acq->nrow_ref;
    out_start[1] = 0;    out_edge[1] = acq->ncol_ref;
    /* 457 Landsats */
	if ((SDwritedata(acq->sds_b01,	out_start, NULL, out_edge,acq->b01 )) == FAIL ||
	    (SDwritedata(acq->sds_b02,	out_start, NULL, out_edge,acq->b02 )) == FAIL ||
	    (SDwritedata(acq->sds_b03,	out_start, NULL, out_edge,acq->b03 )) == FAIL ||
	    (SDwritedata(acq->sds_b04,	out_start, NULL, out_edge,acq->b04 )) == FAIL ||
	    (SDwritedata(acq->sds_b05,	out_start, NULL, out_edge,acq->b05 )) == FAIL ||
	    (SDwritedata(acq->sds_b61,  out_start, NULL, out_edge,acq->b61 )) == FAIL ||
	    (SDwritedata(acq->sds_b07,	out_start, NULL, out_edge,acq->b07 )) == FAIL ||
	    (SDwritedata(acq->sds_sat,  out_start, NULL, out_edge,acq->sat )) == FAIL) {
		status = 2;
		sprintf(message, "Cannot write to  %s", acq->acq_hdf);
		Error(message, "write_acq", status);
		goto CLEANUP;
	}
	if(!acq->isTM){/* Landsat 7 only */
		if((SDwritedata(acq->sds_b62,  out_start, NULL, out_edge,acq->b62 )) == FAIL){
			status = 2;
			sprintf(message, "Cannot write to  %s", acq->acq_hdf);
			Error(message, "write_acq", status);
			goto CLEANUP;
		}
	}

    if (SDsetattr(acq->sd_id, PRODUCT_VERSION_AN, DFNT_CHAR8, strlen(PRODUCT_VERSION), (VOIDP)PRODUCT_VERSION) == FAIL) {
		status = 2;
		sprintf(message, "Cannot SET ATTIBUTE to  %s", acq->acq_hdf);
		Error(message, "write_acq", status);
        goto CLEANUP;
    }

    if (SDsetattr(acq->sd_id, PGE_VERSION_AN, DFNT_CHAR8, strlen(PGE_VERSION_TOA), (VOIDP)PGE_VERSION_TOA) == FAIL) {
    	status = 2;
		sprintf(message, "Cannot SET ATTIBUTE to  %s", acq->acq_hdf);
		Error(message, "write_acq", status);

        goto CLEANUP;
    }


	/*************************************************************************************
	* SHOULD NOT BE USED AT THIS POINT (DAVID INSISTED)
	*	if(geom_flag){
	*		if ((SDwritedata(acq->sds_sz, out_start, NULL, out_edge,acq->sz)) == FAIL ||
	*			(SDwritedata(acq->sds_sa, out_start, NULL, out_edge,acq->sa)) == FAIL ||
	*			(SDwritedata(acq->sds_vz, out_start, NULL, out_edge,acq->vz)) == FAIL ||
	*			(SDwritedata(acq->sds_va, out_start, NULL, out_edge,acq->va)) == FAIL ){
	*			status=2;
	*			sprintf(message, "Cannot write to  %s", acq->acq_hdf);
	*			Error(message, "write_acq", status);
	*			goto CLEANUP;
	*		}
	*	}
	**************************************************************************************/
	 CLEANUP:
	return status;
}


/*********************************************************************************
* Name:	acq_grab()
* Purpose: get a TOA acq into memory
*
* Args:
*	  @acq		- acquisition to be created
*	  @geom_flag- weather or not to allocate 4 geometry bands
*	  @cldm_flag- weather or not to allocate 2 cloud masks bands
*
* Calls: main()
* NOTE: has to call close after
*********************************************************************************/

int acq_grab(l457_acq_info_t * this_acq, uint8 geom_flag, uint8 cldm_flag){
	int status = 0;
	char message[MSGLEN];
	int32 sds_index;
	int32 attr_index;
	char sds_name[MAX_NC_NAME];
	int32 dim_sizes[2];
	int32 rank, data_type, n_attrs;
	int32 start[2], edge[2];
	int32 sd_id, sds_id;
	/*For parsing  the acquistion date attribute*/
	char8 acq_date[11];   /*YYYY-MM_DD, plus '\0'. attribute.*/
	int year, month, day;
	int ncol_ref, nrow_ref, nrow_thm, ncol_thm;
	/* Three global attributes */
	double ulx, uly;	 /*X, Y of the center of upper-left corner pixel. Attributes in the input HDF*/
	int16 utm_zone;	  /*An input attribute*/
	float32 pixel_size;  /*Also an input attribute*/
	float32 center_sz, center_sa;	/* scene center solar zenith and azimuth */

	if(this_acq->sd_id != FAIL){
		status = 2;
		sprintf(message, "Acquisition is not ready to be open %s", this_acq->acq_hdf);
		Error(message, "open_acq", status);
		goto CLEANUP;
    }


	acq_set_blank(this_acq);
	this_acq->isTM=0;/*DEFAULT INFORCED*/
	if(strstr(this_acq->acq_hdf, L7_KEY)== NULL){
		this_acq->isTM=1;
	}

	sds_index=0;
	/*b01*/
	sd_id=sds_id=FAIL;
	if ((sd_id = SDstart(this_acq->acq_hdf, DFACC_READ)) == FAIL) {
		status = 2;
		sprintf(message, "Cannot open %s for DFACC_READ", this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		goto CLEANUP;
	}else{
		this_acq->sd_id=sd_id;
		strcpy(sds_name, B01_SDS_NAME);
		if ((sds_index = SDnametoindex(this_acq->sd_id, sds_name)) == FAIL) {
			status = 2;
			sprintf(message, "No SDS %s in %s", sds_name, this_acq->acq_hdf);
			Error(message, "grab_acq", status);
			goto CLEANUP;
		}
		sds_id =  SDselect(sd_id, sds_index);
		if (SDgetinfo(sds_id, sds_name, &rank, dim_sizes, &data_type, &n_attrs) == FAIL) {
			status = 1;
			sprintf(message, "Error in SDgetinfo() for %s",  this_acq->acq_hdf);
			Error(message, "grab_acq", status);
			if (sds_id	!= FAIL) SDendaccess(sds_id);
			if (sd_id	!= FAIL) SDend(sd_id);
			goto CLEANUP;
		}
		/*this is simple enough */
		this_acq->nrow_ref = dim_sizes[0];
		this_acq->ncol_ref = dim_sizes[1];
		this_acq->nrow_thm = dim_sizes[0];
		this_acq->ncol_thm = dim_sizes[1];
		nrow_ref = dim_sizes[0];
		ncol_ref = dim_sizes[1];
		nrow_thm = dim_sizes[0];
		ncol_thm = dim_sizes[1];
		/* INIT all the BANDS */
		if ((this_acq->b01   = (int16*)calloc(nrow_ref*ncol_ref, sizeof(int16))) == NULL ||
			(this_acq->b02   = (int16*)calloc(nrow_ref*ncol_ref, sizeof(int16))) == NULL ||
			(this_acq->b03   = (int16*)calloc(nrow_ref*ncol_ref, sizeof(int16))) == NULL ||
			(this_acq->b04   = (int16*)calloc(nrow_ref*ncol_ref, sizeof(int16))) == NULL ||
			(this_acq->b05   = (int16*)calloc(nrow_ref*ncol_ref, sizeof(int16))) == NULL ||
			(this_acq->b61 	 = (int16*)calloc(nrow_thm*ncol_thm, sizeof(int16))) == NULL ||
			(this_acq->b62 	 = (int16*)calloc(nrow_thm*ncol_thm, sizeof(int16))) == NULL ||
			(this_acq->b07   = (int16*)calloc(nrow_ref*ncol_ref, sizeof(int16))) == NULL ||
			(this_acq->sat   = (uint8*)calloc(nrow_ref*ncol_ref, sizeof(uint8))) == NULL) {
			status = 4;
			Error("Cannot allocate memory", "grab_acq", status);
			if (sds_id	!= FAIL) SDendaccess(sds_id);
			if (sd_id	!= FAIL) SDend(sd_id);
			goto CLEANUP;
		}
		/*************************************************************************************
		* This will just allocate the memory for geometry
		**************************************************************************************/
		if(geom_flag){
			if((this_acq->sa=(int16*)calloc(ncol_ref * nrow_ref, sizeof(int16))) == NULL ||
			   (this_acq->sz=(int16*)calloc(ncol_ref * nrow_ref, sizeof(int16))) == NULL ||
			   (this_acq->va=(int16*)calloc(ncol_ref * nrow_ref, sizeof(int16))) == NULL ||
			   (this_acq->vz=(int16*)calloc(ncol_ref * nrow_ref, sizeof(int16))) == NULL ){
				status=2;
				sprintf(message, "Cannot allocate memory for bands " );
				Error(message, "grab_acq", status);
				goto CLEANUP;
			}
		}
		/*************************************************************************************
		* This will just allocate the memory for cloud masks
		**************************************************************************************/
		if(cldm_flag){
			if((this_acq->acca  = (uint8*)calloc(ncol_ref * nrow_ref, sizeof(uint8))) == NULL ||
			   (this_acq->dtcld = (uint8*)calloc(ncol_ref * nrow_ref, sizeof(uint8))) == NULL ){
				status=2;
				sprintf(message, "Cannot allocate memory for bands " );
				Error(message, "grab_acq", status);
				goto CLEANUP;
			}
		}


		start[0] = 0;  edge[0] = this_acq->nrow_ref;
		start[1] = 0;  edge[1] = this_acq->ncol_ref;
		if (SDreaddata(sds_id, start, NULL, edge, this_acq->b01) == FAIL) {
			status = 3;
			sprintf(message, "Cannot read from %s", this_acq->acq_hdf);
			Error(message, "grab_acq", status);
			if (sds_id	!= FAIL) SDendaccess(sds_id);
			if (sd_id	!= FAIL) SDend(sd_id);
			sd_id=sds_id=FAIL;
			goto CLEANUP;
		}
		/**************************************************************************************
		 * NOW all the other attributes since we know the data was readable
		 *************************************************************************************/
		/******Read the attributes*/
		attr_index = SDfindattr(sd_id, INPUT_UTM_ZONE_NUMBER);
		if (status !=0 || attr_index == FAIL || SDreadattr(sd_id, attr_index, &utm_zone) == FAIL) {
			status = 3;
			sprintf(message,  "Cannot find/read the attribute %s in %s", INPUT_UTM_ZONE_NUMBER, this_acq->acq_hdf);
			Error(message, "read_acq", status);
		}
		this_acq->utm_zone = utm_zone;

		attr_index = SDfindattr(sd_id, INPUT_PRODUCT_UL_CORNER_MAPX);
		if (status !=0 || attr_index == FAIL || SDreadattr(sd_id, attr_index, &ulx) == FAIL) {
			status = 3;
			sprintf(message, "Cannot find/read the attribute %s in %s", INPUT_PRODUCT_UL_CORNER_MAPX, this_acq->acq_hdf);
			Error(message, "read_acq", status);
		}
		this_acq->ulx = ulx;

		attr_index = SDfindattr(sd_id, INPUT_PRODUCT_UL_CORNER_MAPY);
		if (status !=0 || attr_index == FAIL  || SDreadattr(sd_id, attr_index, &uly) == FAIL) {
			status = 3;
			sprintf(message, "Cannot find/read the attribute %s in %s", INPUT_PRODUCT_UL_CORNER_MAPY, this_acq->acq_hdf);
			Error(message, "read_acq", status);
		}
		this_acq->uly = uly;

		attr_index = SDfindattr(sd_id, INPUT_GRID_CELL_SIZE);
		if (status !=0 || attr_index == FAIL || SDreadattr(sd_id, attr_index, &pixel_size) == FAIL) {
			status = 3;
			sprintf(message, "Cannot find/read the attribute %s in %s", INPUT_GRID_CELL_SIZE, this_acq->acq_hdf);
			Error(message, "read_acq", status);
		}
		if (status !=0 || pixel_size != REF_PIXSIZE) {
			status = 3;
			sprintf(message, "The pixel size is not %f in  %s", (float)REF_PIXSIZE, this_acq->acq_hdf);
			Error(message, "read_acq", status);
		}
		this_acq->pixel_size = pixel_size;

		/* Scene center solar zenith and azimuth */
		attr_index = SDfindattr(sd_id, INPUT_CENTER_SZ);
		if (status !=0 || attr_index == FAIL || SDreadattr(sd_id, attr_index, &center_sz) == FAIL) {
			status = 3;
			sprintf(message, "Cannot find/read the attribute %s in %s", INPUT_CENTER_SZ, this_acq->acq_hdf);
			Error(message, "read_acq", status);
		}
		this_acq->center_sz = center_sz;

		attr_index = SDfindattr(sd_id, INPUT_CENTER_SA);
		if (status !=0 || attr_index == FAIL || SDreadattr(sd_id, attr_index, &center_sa) == FAIL) {
			status = 3;
			sprintf(message, "Cannot find/read the attribute %s in %s", INPUT_CENTER_SA, this_acq->acq_hdf);
			Error(message, "read_acq", status);
		}
		this_acq->center_sa = center_sa;
		/*  The upper-left corner of the upper-left pixel */
		this_acq->ymax_utm = uly + pixel_size/2;
		this_acq->xmin_utm = ulx - pixel_size/2;

		/***Read the acquisition date attribute from b1*/
		strcpy(acq_date,"----------");/*to fix parsing process*/
		attr_index = SDfindattr(sd_id, INPUT_ACQUISITION_DATE);
		if (status !=0 || attr_index == FAIL || SDreadattr(sd_id, attr_index, acq_date) == FAIL) {
			status = 3;
			sprintf(message, "Cannot find/read the attribute %s in %s", INPUT_ACQUISITION_DATE, this_acq->acq_hdf);
			Error(message, "read_acq", status);
		}

		acq_date[10] = '\0';
		/*Parse year, month, doy and then derive doy*/
		sscanf(acq_date, "%d-%d-%d", &year, &month, &day);
		this_acq->doy = day_of_year(year, month, day);
		/*
		 *  printf("FROM acq_date %s comes  y=%d ; m=%d ; d=%d; doy=%d \n", acq_date, year, month,  day, this_acq->doy);
		 *  printf("acq_name isTM=%d : %s  \n", (int)(this_acq->isTM), this_acq->acq_hdf);
		 */
		this_acq->year=year;
		this_acq->month=month;
		this_acq->day=day;
		this_acq->sds_b01=sds_id;
	}
	/*b02*/
	strcpy(sds_name, B02_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(this_acq->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		goto CLEANUP;
	}
	sds_id =  SDselect(sd_id, sds_index);
	if (SDreaddata(sds_id, start, NULL, edge, this_acq->b02) == FAIL) {
		status = 3;
		sprintf(message, "Cannot read from %s", this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		if (sds_id	!= FAIL) SDendaccess(sds_id);
		if (sd_id	!= FAIL) SDend(sd_id);
		sd_id=sds_id=FAIL;
		goto CLEANUP;
	}
	this_acq->sds_b02=sds_id;
	/*b03*/
	strcpy(sds_name, B03_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(this_acq->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		goto CLEANUP;
	}
	sds_id =  SDselect(sd_id, sds_index);
	if (SDreaddata(sds_id, start, NULL, edge, this_acq->b03) == FAIL) {
		status = 3;
		sprintf(message, "Cannot read from %s", this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		if (sds_id	!= FAIL) SDendaccess(sds_id);
		if (sd_id	!= FAIL) SDend(sd_id);
		sd_id=sds_id=FAIL;
		goto CLEANUP;
	}
	this_acq->sds_b03=sds_id;
	/*b04*/
	strcpy(sds_name, B04_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(this_acq->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		goto CLEANUP;
	}
	sds_id =  SDselect(sd_id, sds_index);
	if (SDreaddata(sds_id, start, NULL, edge, this_acq->b04) == FAIL) {
		status = 3;
		sprintf(message, "Cannot read from %s", this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		if (sds_id	!= FAIL) SDendaccess(sds_id);
		if (sd_id	!= FAIL) SDend(sd_id);
		sd_id=sds_id=FAIL;
		goto CLEANUP;
	}
	this_acq->sds_b04=sds_id;
	/*b05*/
	strcpy(sds_name, B05_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(this_acq->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		goto CLEANUP;
	}
	sds_id =  SDselect(sd_id, sds_index);
	if (SDreaddata(sds_id, start, NULL, edge, this_acq->b05) == FAIL) {
		status = 3;
		sprintf(message, "Cannot read from %s", this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		if (sds_id	!= FAIL) SDendaccess(sds_id);
		if (sd_id	!= FAIL) SDend(sd_id);
		sd_id=sds_id=FAIL;
		goto CLEANUP;
	}
	this_acq->sds_b05=sds_id;

	/*b61*/
	strcpy(sds_name, B61_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(this_acq->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		goto CLEANUP;
	}
	sds_id =  SDselect(sd_id, sds_index);
	if (SDreaddata(sds_id, start, NULL, edge, this_acq->b61) == FAIL) {
		status = 3;
		sprintf(message, "Cannot read from %s", this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		if (sds_id	!= FAIL) SDendaccess(sds_id);
		if (sd_id	!= FAIL) SDend(sd_id);
		sd_id=sds_id=FAIL;
		goto CLEANUP;
	}
	this_acq->sds_b61=sds_id;
	/*b62*/
	strcpy(sds_name, B62_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(this_acq->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		goto CLEANUP;
	}
	sds_id =  SDselect(sd_id, sds_index);
	if (SDreaddata(sds_id, start, NULL, edge, this_acq->b62) == FAIL) {
		status = 3;
		sprintf(message, "Cannot read from %s", this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		if (sds_id	!= FAIL) SDendaccess(sds_id);
		if (sd_id	!= FAIL) SDend(sd_id);
		sd_id=sds_id=FAIL;
		goto CLEANUP;
	}
	this_acq->sds_b62=sds_id;
	/*b07*/
	strcpy(sds_name, B07_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(this_acq->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		goto CLEANUP;
	}
	sds_id =  SDselect(sd_id, sds_index);
	if (SDreaddata(sds_id, start, NULL, edge, this_acq->b07) == FAIL) {
		status = 3;
		sprintf(message, "Cannot read from %s", this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		if (sds_id	!= FAIL) SDendaccess(sds_id);
		if (sd_id	!= FAIL) SDend(sd_id);
		sd_id=sds_id=FAIL;
		goto CLEANUP;
	}
	this_acq->sds_b07=sds_id;

	/*sat*/
	strcpy(sds_name, SAT_SDS_NAME);
	if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(this_acq->sd_id, sds_name)) == FAIL) {
		status = 2;
		sprintf(message, "No SDS %s in %s", sds_name, this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		goto CLEANUP;
	}
	sds_id =  SDselect(sd_id, sds_index);
	if (SDreaddata(sds_id, start, NULL, edge, this_acq->sat) == FAIL) {
		status = 3;
		sprintf(message, "Cannot read from %s", this_acq->acq_hdf);
		Error(message, "grab_acq", status);
		if (sds_id	!= FAIL) SDendaccess(sds_id);
		if (sd_id	!= FAIL) SDend(sd_id);
		sd_id=sds_id=FAIL;
		goto CLEANUP;
	}
	this_acq->sds_sat=sds_id;
	this_acq->sd_id=sd_id;

	if(geom_flag){
		status=acq_load_geom(this_acq->geomhdf, this_acq->nrow_ref, this_acq->ncol_ref,
						 this_acq->sz, this_acq->sa, this_acq->vz, this_acq->va);
		if(status!=0){
			sprintf(message, "problem in load_geom %s", this_acq->acq_hdf);
			Error(message, "grab_acq", status);
		}
	}
	if(cldm_flag){
		status=acq_load_acca_dtcld(this_acq->accahdf, this_acq->dtclhdf,
								this_acq->nrow_ref, this_acq->ncol_ref,
				  	  	  	   this_acq->acca, this_acq->dtcld);
		if(status!=0){
			sprintf(message, "problem in load_acca_dtcld %s", this_acq->acq_hdf);
			Error(message, "grab_acq", status);
		}
	}

	/*************************************************************************************
	* SHOULD NOT BE USED AT THIS POINT (DAVID INSISTED)
	* load_geom is used instead
	*	if(geom_flag){
	*		SA
	*		strcpy(sds_name, SA_SDS_NAME);
	*		if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(this_acq->sd_id, sds_name)) == FAIL) {
	*			status = 2;
	*			sprintf(message, "No SDS %s in %s", sds_name, this_acq->acq_hdf);
	*			Error(message, "grab_acq", status);
	*			goto CLEANUP;
	*		}
	*		sds_id =  SDselect(sd_id, sds_index);
	*		if (SDreaddata(sds_id, start, NULL, edge, this_acq->sa) == FAIL) {
	*			status = 3;
	*			sprintf(message, "Cannot read from %s", this_acq->acq_hdf);
	*			Error(message, "grab_acq", status);
	*			if (sds_id	!= FAIL) SDendaccess(sds_id);
	*			if (sd_id	!= FAIL) SDend(sd_id);
	*			sd_id=sds_id=FAIL;
	*			goto CLEANUP;sa, this_acq->va, this_acq->vz);
		if(status!=0){
	*		}
	*		this_acq->sds_sa=sds_id;
	*		SZ
	*		strcpy(sds_name, SZ_SDS_NAME);
	*		if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(this_acq->sd_id, sds_name)) == FAIL) {
	*			status = 2;
	*			sprintf(message, "No SDS %s in %s", sds_name, this_acq->acq_hdf);
	*			Error(message, "grab_acq", status);
	*			goto CLEANUP;
	*		}
	*		sds_id =  SDselect(sd_id, sds_index);
	*		if (SDreaddata(sds_id, start, NULL, edge, this_acq->sz) == FAIL) {
	*			status = 3;
	*			sprintf(message, "Cannot read from %s", this_acq->acq_hdf);
	*			Error(message, "grab_acq", status);
	*			if (sds_id	!= FAIL) SDendaccess(sds_id);
	*			if (sd_id	!= FAIL) SDend(sd_id);
	*			sd_id=sds_id=FAIL;
	*			goto CLEANUP;
	*		}
	*		this_acq->sds_sz=sds_id;
	*		VA
	*		strcpy(sds_name, VA_SDS_NAME);
	*		if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(this_acq->sd_id, sds_name)) == FAIL) {
	*			status = 2;
	*			sprintf(message, "No SDS %s in %s", sds_name, this_acq->acq_hdf);
	*			Error(message, "grab_acq", status);
	*			goto CLEANUP;
	*		}
	*		sds_id =  SDselect(sd_id, sds_index);
	*		if (SDreaddata(sds_id, start, NULL, edge, this_acq->va) == FAIL) {
	*			status = 3;
	*			sprintf(message, "Cannot read from %s", this_acq->acq_hdf);
	*			Error(message, "grab_acq", status);
	*			if (sds_id	!= FAIL) SDendaccess(sds_id);
	*			if (sd_id	!= FAIL) SDend(sd_id);
	*			sd_id=sds_id=FAIL;
	*			goto CLEANUP;
	*		}
	*		this_acq->sds_va=sds_id;sa, this_acq->va, this_acq->vz);
		if(status!=0){
	*		VZ
	*		strcpy(sds_name, VZ_SDS_NAME);
	*		if (strlen(sds_name) > 0 && (sds_index = SDnametoindex(this_acq->sd_id, sds_name)) == FAIL) {
	*			status = 2;
	*			sprintf(message, "No SDS %s in %s", sds_name, this_acq->acq_hdf);
	*			Error(message, "grab_acq", status);
	*			goto CLEANUP;
	*		}
	*		sds_id =  SDselect(sd_id, sds_index);
	*		if (SDreaddata(sds_id, start, NULL, edge, this_acq->vz) == FAIL) {
	*			status = 3;
	*			sprintf(message, "Cannot read from %s", this_acq->acq_hdf);
	*			Error(message, "grab_acq", status);
	*			if (sds_id	!= FAIL) SDendaccess(sds_id);
	*			if (sd_id	!= FAIL) SDend(sd_id);
	*			sd_id=sds_id=FAIL;
	*			goto CLEANUP;
	*		}
	*		this_acq->sds_vz=sds_id;
	*	}
	**************************************************************************************/



	 CLEANUP:
	 if(status!=0){
		 acq_close(this_acq);
	 }else{
		/* just release all the hdf handlers */
		if(this_acq->sds_b01!=FAIL)SDendaccess(this_acq->sds_b01);
		if(this_acq->sds_b02!=FAIL)SDendaccess(this_acq->sds_b02);
		if(this_acq->sds_b03!=FAIL)SDendaccess(this_acq->sds_b03);
		if(this_acq->sds_b04!=FAIL)SDendaccess(this_acq->sds_b04);
		if(this_acq->sds_b05!=FAIL)SDendaccess(this_acq->sds_b05);
		if(this_acq->sds_b61!=FAIL)SDendaccess(this_acq->sds_b61);
		if(this_acq->sds_b62!=FAIL)SDendaccess(this_acq->sds_b62);
		if(this_acq->sds_b07!=FAIL)SDendaccess(this_acq->sds_b07);
		if(this_acq->sds_sat!=FAIL)SDendaccess(this_acq->sds_sat);
		if(this_acq->sds_sz!=FAIL) SDendaccess(this_acq->sds_sz);
		if(this_acq->sds_sa!=FAIL) SDendaccess(this_acq->sds_sa);
		if(this_acq->sds_vz!=FAIL) SDendaccess(this_acq->sds_vz);
		if(this_acq->sds_va!=FAIL) SDendaccess(this_acq->sds_va);
		if(this_acq->sd_id !=FAIL) SDend(this_acq->sd_id);
		this_acq->sds_b01=FAIL;
		this_acq->sds_b02=FAIL;
		this_acq->sds_b03=FAIL;
		this_acq->sds_b04=FAIL;
		this_acq->sds_b05=FAIL;
		this_acq->sds_b61=FAIL;
		this_acq->sds_b62=FAIL;
		this_acq->sds_b07=FAIL;
		this_acq->sds_sat=FAIL;
		this_acq->sds_sz=FAIL;
		this_acq->sds_sa=FAIL;
		this_acq->sds_vz=FAIL;
		this_acq->sds_va=FAIL;
		this_acq->sd_id =FAIL;
	 }

	return status;
}




/*********************************************************************************************
    Read the int16 solar and sensor geometry SDSs from the given geometry hdf file.
    nrow and ncol are passed in just to make sure that image dimension is right.

*/
int acq_load_geom(char *fname_geom,
                        int nrow,
                        int ncol,
                        int16 *sz,
                        int16 *sa,
                        int16 *vz,
                        int16 *va)
{
    int32 fid  = FAIL;
    int32 sds_id  = FAIL;
    intn status = 0;
    char SZ_sds_name_dup[MAX_NAME_LEN];
    char message[256];
    int32 sds_index;
    int32 dim_sizes[MAX_VAR_DIMS];
    int32 rank, data_type, n_attrs;
    int32 start[2], edge[2];




    /*  The passed in character string SZ_sds_name may be a string constant, and in
        in SDgetinfo if SZ_sds_name is used instead of SZ_sds_name_dup a segmentation
        fault will occur because SDgetinfo tries to write to SZ_sds_name. A problem
        bugged me for 5 hours.
    */
    strcpy(SZ_sds_name_dup, SZ_SDS_NAME);

    if ((fid = SDstart(fname_geom, DFACC_READ)) == FAIL) {
        status = 2;
        sprintf(message, "Cannot open %s for read", fname_geom);
        Error(message, "acq_load_geom", status);
        goto CLEANUP;
    }
    if ((sds_index = SDnametoindex(fid, SZ_SDS_NAME)) == FAIL) {
        status = 2;
        sprintf(message, "File %s does not have SDS %s", fname_geom, SZ_SDS_NAME);
        Error(message, "acq_load_geom", status);
        goto CLEANUP;
    }
    sds_id = SDselect(fid, sds_index);

    SDgetinfo(sds_id, SZ_sds_name_dup, &rank, dim_sizes, &data_type, &n_attrs);

    if (nrow != dim_sizes[0] || ncol != dim_sizes[1]) {
        status = 2;
        sprintf(message, "The geometry file %s has a dimension different from that of the TIFF file", fname_geom);
        Error(message, "acq_load_geom", status);
        goto CLEANUP;
    }
    start[0] = 0; edge[0] = dim_sizes[0];
    start[1] = 0; edge[1] = dim_sizes[1];
    if (SDreaddata(sds_id, start, NULL, edge, sz) == FAIL) {
        status = 2;
        Error("SDreaddata failed", "load_geom", status);
        goto CLEANUP;
    }
    if (sds_id != FAIL)  SDendaccess(sds_id);

    if ((sds_index = SDnametoindex(fid, SA_SDS_NAME)) == FAIL) {
        status = 2;
        sprintf(message, "File %s does not have SDS %s", fname_geom, SA_SDS_NAME);
        Error(message, "acq_load_geom", status);
        goto CLEANUP;
    }
    sds_id = SDselect(fid, sds_index);
    if (SDreaddata(sds_id, start, NULL, edge, sa) == FAIL) {
        status = 2;
        Error("SDreaddata failed", "acq_load_geom", status);
        goto CLEANUP;
    }
    if (sds_id != FAIL)  SDendaccess(sds_id);

    if ((sds_index = SDnametoindex(fid, VZ_SDS_NAME)) == FAIL) {
        status = 2;
        sprintf(message, "File %s does not have SDS %s", fname_geom,VZ_SDS_NAME);
        Error(message, "acq_load_geom", status);
        goto CLEANUP;
    }
    sds_id = SDselect(fid, sds_index);
    if (SDreaddata(sds_id, start, NULL, edge, vz) == FAIL) {
        status = 2;
        Error("SDreaddata failed", "acq_load_geom", status);
        goto CLEANUP;
    }
    if (sds_id != FAIL)  SDendaccess(sds_id);

    if ((sds_index = SDnametoindex(fid, VA_SDS_NAME)) == FAIL) {
        status = 2;
        sprintf(message, "File %s does not have SDS %s", fname_geom,VA_SDS_NAME);
        Error(message, "acq_load_geom", status);
        goto CLEANUP;
    }
    sds_id = SDselect(fid, sds_index);
    if (SDreaddata(sds_id, start, NULL, edge, va) == FAIL) {
        status = 2;
        Error("SDreaddata failed", "acq_load_geom", status);
        goto CLEANUP;
    }



    CLEANUP:
    if (sds_id != FAIL)  SDendaccess(sds_id);
    if (fid != FAIL)     SDend(fid);
    return status;
}

/*********************************************************************************************
  Obtain input pointer.
*/
int acq_get_inp_pointer(char *fname_hdf,
                        char *inp_pntr, char *attr_name)
{
    int32 fid  = FAIL;

    intn status = 0;
    char message[256];

    int32 attr_index, n_values;
    int32 data_type;


    if ((fid = SDstart(fname_hdf, DFACC_READ)) == FAIL) {
        status = 2;
        sprintf(message, "Cannot open %s for read", fname_hdf);
        Error(message, "acq_load_geom", status);
        goto CLEANUP;
    }


    /*INP_POINTER*/
	attr_index = SDfindattr(fid, "INPUT_POINTER");
	if (attr_index == FAIL) {
		status = 2;
		sprintf(message, "No attribute %s in %s", "INPUT_POINTER", fname_hdf);
		Error(message, "acq_get_inp_pointer", status);
		goto CLEANUP;
	}

	if (SDattrinfo(fid, attr_index, attr_name, &data_type, &n_values) == FAIL) {
		status = 2;
		sprintf(message, "SDattrinfo failed for %s", fname_hdf);
		Error(message, "acq_get_inp_pointer", status);
		goto CLEANUP;
	}

	if (SDreadattr(fid, attr_index, inp_pntr) == FAIL) {
		status = 2;
		sprintf(message, "SDreadattr failed for %s", fname_hdf);
		Error(message, "acq_get_inp_pointer", status);
		goto CLEANUP;
	}

    CLEANUP:

    if (fid != FAIL)     SDend(fid);
    return status;
}


/*********************************************************************************************
    Read the Cloud Masks SDS from the given hdf files.
    nrow and ncol are passed in just to make sure that image dimension is right.

*/
int acq_load_acca_dtcld(char *fname_acca, char *fname_dtcld,
                        int nrow, int ncol,
                        uint8 *acca, uint8 *dtcld)
{
    int32 fid  = FAIL;
    int32 sds_id  = FAIL;
    intn status = 0;
    char sds_name_dup[256];
    char message[256];

    int32 sds_index;
    int32 dim_sizes[MAX_VAR_DIMS];
    int32 rank, data_type, n_attrs;
    int32 start[2], edge[2];

    /****************************************************************
     * LOAD ACCA
     ***************************************************************/
    strcpy(sds_name_dup, ACCA_SDS_NAME);

    if ((fid = SDstart(fname_acca, DFACC_READ)) == FAIL) {
        status = 2;
        sprintf(message, "Cannot open %s for read", fname_acca);
        Error(message, "load_acca_dtcld", status);
        goto CLEANUP;
    }

    /**************************************************************************
     * if ((sds_index = SDnametoindex(fid, sds_name_dup)) == FAIL) {
     *    status = 2;
     *    sprintf(message, "File %s does not have SDS %s", fname_acca, sds_name_dup);
     *    Error(message, "load_acca_dtcld", status);
     *    goto CLEANUP;
     * }
     * ACCA sds has non standard name
     * **************************************************************************/
    sds_index=0;
    sds_id = SDselect(fid, sds_index);
    SDgetinfo(sds_id, sds_name_dup, &rank, dim_sizes, &data_type, &n_attrs);

    if (nrow != dim_sizes[0] || ncol != dim_sizes[1]) {
        status = 2;
        sprintf(message, "The ACCA file %s has a dimension different from that of the TIFF file", fname_acca);
        Error(message, "load_acca_dtcld", status);
        goto CLEANUP;
    }
    start[0] = 0; edge[0] = dim_sizes[0];
    start[1] = 0; edge[1] = dim_sizes[1];
    if (SDreaddata(sds_id, start, NULL, edge, acca) == FAIL) {
        status = 2;
        Error("SDreaddata failed", "load_acca_dtcld", status);
        goto CLEANUP;
    }
    if (sds_id != FAIL)  SDendaccess(sds_id);
    if (fid != FAIL)     SDend(fid);

    /****************************************************************
     * LOAD DTCLD
     ***************************************************************/
    strcpy(sds_name_dup, DTCLD_SDS_NAME);
    if ((fid = SDstart(fname_dtcld, DFACC_READ)) == FAIL) {
        status = 2;
        sprintf(message, "Cannot open %s for read", fname_dtcld);
        Error(message, "load_acca_dtcld", status);
        goto CLEANUP;
    }
    /****************************************************************************
    *if ((sds_index = SDnametoindex(fid, sds_name_dup)) == FAIL) {
    *    status = 2;
    *    sprintf(message, "File %s does not have SDS %s", fname_dtcld, sds_name_dup);
    *    Error(message, "load_acca_dtcld", status);
    *    goto CLEANUP;
    *}
    * DTCLD_SDS sds has non standard name
    * **************************************************************************/
    sds_index=0;
    sds_id = SDselect(fid, sds_index);

    SDgetinfo(sds_id, sds_name_dup, &rank, dim_sizes, &data_type, &n_attrs);

    if (nrow != dim_sizes[0] || ncol != dim_sizes[1]) {
        status = 2;
        sprintf(message, "The ACCA file %s has a dimension different from that of the TIFF file", fname_dtcld);
        Error(message, "load_acca_dtcld", status);
        goto CLEANUP;
    }
    start[0] = 0; edge[0] = dim_sizes[0];
    start[1] = 0; edge[1] = dim_sizes[1];
    if (SDreaddata(sds_id, start, NULL, edge, dtcld) == FAIL) {
        status = 2;
        Error("SDreaddata failed", "load_acca_dtcld", status);
        goto CLEANUP;
    }

    CLEANUP:
    if (sds_id != FAIL)  SDendaccess(sds_id);
    if (fid != FAIL)     SDend(fid);
    return status;
}


/*********************************************************************************
* Name:	get_l457acq_number()
* Purpose: get number of input acq
*
* Args:
*	  @acq_list     - fname with file lists
*	  @nacq 		- number to return
*
*
* Calls: main()
*********************************************************************************/

int get_l457acq_number(char *acq_list, int *nacq)
{
	int status = 0;
	char message[MSGLEN];
	char line[MAX_LINE_LEN];
	FILE *flist = NULL;
	if ((flist = fopen(acq_list, "r")) == NULL) {
		status = 2;
		sprintf(message, "Cannot open %s for read", acq_list);
		Error(message, "get_l8acq_number", status);
		return status;
	}
	*nacq = 0;
	while (fgets(line, MAX_LINE_LEN-1, flist)) {
		if (strlen(line) == 1 && line[0] == '\n')
			(*nacq)++;
	}
	fclose(flist);
	if(*nacq==0){
		status=1;
	}
	return status;
}

/*********************************************************************************
* Name:	get_l8acq_fnames()
* Purpose: get number of input acq
*
* Args:
*	  @acq_list     - fname with lists of files that make up acquisitions (4 per acq)
*	  @all_acq_fname- array od acq_filenames structures (MUST BE ALLOCATED ALREADY)
*	  @nacq 		- number of acq in all_acq_fname array
*
* Calls: main()
*********************************************************************************/
int get_l457acq_fnames(char *acq_list, l457acq_fname_t *all_acq_fname, int nacq){
	int status = 0;
	char *pos;
	char message[MSGLEN];
	char line[MAX_LINE_LEN];
	FILE *flist = NULL;
	int curAcq;

	if ((flist = fopen(acq_list, "r")) == NULL) {
		status = 2;
		sprintf(message, "Cannot open %s for read", acq_list);
		Error(message, "get_l457acq_fnames", status);
		return status;
	}
	curAcq=0;
	while (fgets(line, MAX_LINE_LEN-1, flist)) {
		if (strlen(line) > 1 && line[0] != '\n'){
			/*trip the path*/
			pos=strstr(line,".hdf");
			if(pos)	pos[4]='\0';
			if (      strstr(line,ACQ_KEY)){
				strcpy(all_acq_fname[curAcq].acq_fname, line);
			}else if (strstr(line,GEOMKEY)){
				strcpy(all_acq_fname[curAcq].geomfname, line);
			}else if (strstr(line,ACCAKEY)){
				strcpy(all_acq_fname[curAcq].accafname, line);
			}else if (strstr(line,DTCLKEY)){
				strcpy(all_acq_fname[curAcq].dtclfname, line);
			}
		}else{
			curAcq++;
			if(curAcq >= nacq){
				break;
			}
		}
	}
	fclose(flist);
	return status;
}
/*********************************************************************************
* Name:	sort_l457acq_fnames()
* Purpose: get names sorted by time
*
* Args:
*	  @all_acq_fname- array od acq_filenames structures (MUST BE ALLOCATED ALREADY)
*	  @nacq 		- number of acq in all_acq_fname array
*
* Calls: main()
*********************************************************************************/

void sort_l457acq_fnames(l457acq_fname_t *all_acq_fname, int nacq){
	l457acq_fname_t temp;
	int i,j;
	int int_dp, jnt_dp;
	char * i_dp, *j_dp;
	int_dp=jnt_dp=0;
	i_dp=NULL;
	j_dp=NULL;
	/* A bubble sort of the yyyymmdd strings is good enough since the number of strings is small */

	for (i = 0; i < nacq-1; i++) {
		for (j = i+1; j < nacq; j++) {
			i_dp=strrchr(all_acq_fname[i].acq_fname, '_');
			//i_dp=i_dp-8;//yyyymmdd
			i_dp=(i_dp-7-5);//yyyydoy  & STATIONID
			int_dp=atoi(i_dp);

			j_dp=strrchr(all_acq_fname[j].acq_fname, '_');
			//j_dp=j_dp-8;//yyyymmdd
			j_dp=(j_dp-7-5);//yyyydoy  & STATIONID
			jnt_dp=atoi(j_dp);
			/*if (strcmp(i_dp,  j_dp) > 0) {*/
			if(int_dp>jnt_dp) {
				/* SWAPPING */
				strcpy(temp.acq_fname, all_acq_fname[i].acq_fname);
				strcpy(all_acq_fname[i].acq_fname, all_acq_fname[j].acq_fname);
				strcpy(all_acq_fname[j].acq_fname, temp.acq_fname);


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
