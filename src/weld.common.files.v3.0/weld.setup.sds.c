/*  

    Junchang Ju 01/20/2010

    refactored by Val Kovalskyy july 2013

*/

#include "weld.setup.sds.h"


int set_blank_qa_sds_info(sds_info_t *all_sds){
	int isds=0;
	for(isds=0; isds<NSDS_MAX; ++isds)
		all_sds[isds].qa=NULL;

	return 0;
}

int set_free_qa_sds_info(sds_info_t *all_sds){
	int isds=0;
	for(isds=0; isds<NSDS_MAX; ++isds){
		if(all_sds[isds].qa!=NULL){
			free(all_sds[isds].qa);
			all_sds[isds].qa=NULL;
		}
	}
	return 0;
}


/*********************************************************************************
    Set the some attributes for the field. The type of attributes depend on
    the field_name.

    return 0 on success and 1 on failure
*/
int setup_sds_info(sds_info_t *all_sds, char geometry_flag, char l1t_index_flag, int * tot_sds)
{
	set_blank_qa_sds_info(all_sds);
	int nchar = 1500;   /* A smaller number had caused segmentation fault*/
	int status;
	int isds;
	isds = 0;
	/*  b1*/
	strcpy(all_sds[isds].name, B01_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "reflectance, unitless");
	all_sds[isds].scale_factor = 0.0001;
	
	/*  b2*/ 
	isds++;
	strcpy(all_sds[isds].name, B02_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "reflectance, unitless");
	all_sds[isds].scale_factor = 0.0001;

	/*  b3*/ 
	isds++;
	strcpy(all_sds[isds].name, B03_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "reflectance, unitless");
	all_sds[isds].scale_factor = 0.0001;

	/*  b4*/ 
	isds++;
	strcpy(all_sds[isds].name, B04_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "reflectance, unitless");
	all_sds[isds].scale_factor = 0.0001;

	/*  b5*/
	isds++;
	strcpy(all_sds[isds].name, B05_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "reflectance, unitless");
	all_sds[isds].scale_factor = 0.0001;

	/*  b61*/
	isds++;
	strcpy(all_sds[isds].name, B61_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "Degrees Celsius");
	all_sds[isds].scale_factor = 0.01;

	/*  b62*/
	isds++;
	strcpy(all_sds[isds].name, B62_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "Degrees Celsius");
	all_sds[isds].scale_factor = 0.01;

	/*  b7*/
	isds++;
	strcpy(all_sds[isds].name, B07_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "reflectance, unitless");
	all_sds[isds].scale_factor = 0.0001;

	/*  ndvi*/
	isds++;
	strcpy(all_sds[isds].name, NDVI_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 0.0001;

	/*  doy*/
	isds++;
	strcpy(all_sds[isds].name, DOY_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;

	/* sat*/
	isds++;
	strcpy(all_sds[isds].name, SAT_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT8");
	all_sds[isds].data_type = DFNT_UINT8;
	strcpy(all_sds[isds].units, "bit field");
	all_sds[isds].scale_factor = 1;
	if ((all_sds[isds].qa = (char*)calloc(nchar, sizeof(char))) == NULL) {
		status = 4; 
		Error("Cannot allocate memory", "setup_sds_info", status); 
		return status;
	} 
	strcpy(all_sds[isds].qa,   "\n\tBits are numbered from right to left (bit 0 = LSB, bit 7 = MSB):\n"
		                "\tBit      Description\n"
		                "\t0        band 1  (1 = saturated, 0 = not saturated)\n"
		                "\t1        band 2  (1 = saturated, 0 = not saturated)\n"
		                "\t2        band 3  (1 = saturated, 0 = not saturated)\n"
		                "\t3        band 4  (1 = saturated, 0 = not saturated)\n"
		                "\t4        band 5  (1 = saturated, 0 = not saturated)\n"
		                "\t5        band 61 (1 = saturated, 0 = not saturated)\n"
		                "\t6        band 62 (1 = saturated, 0 = not saturated)\n"
		                "\t7        band 7  (1 = saturated, 0 = not saturated)\n");

	/* dtcld*/
	isds++;
	strcpy(all_sds[isds].name, DTCLD_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT8");
	all_sds[isds].data_type = DFNT_UINT8;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;
	if ((all_sds[isds].qa = (char*)calloc(nchar, sizeof(char))) == NULL) {
		status = 4; 
		Error("Cannot allocate memory", "setup_sds_info", status); 
		return status;
	} 
	strcpy(all_sds[isds].qa,   "\n"
		                "\tValue        Description\n"
		                "\t0           not cloudy\n"
		                "\t1           cloudy\n"
		                "\t2           not cloudy but adjacent to a cloudy pixel\n"
		                "\t200         could not be classified reliably\n");

	/* acca*/ 
	isds++;
	strcpy(all_sds[isds].name, ACCA_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT8");
	all_sds[isds].data_type = DFNT_UINT8;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;
	if ((all_sds[isds].qa = (char*)calloc(nchar, sizeof(char))) == NULL) {
		status = 1;
		Error("Cannot allocate memory", "setup_sds_info", status); 
		return status;
	} 
	strcpy(all_sds[isds].qa,   "\n"
		                "\tValue        Description\n"
		                "\t0           not cloudy\n"
		                "\t1           cloudy\n");


	/* nobs*/ 
	isds++;
	strcpy(all_sds[isds].name, NOBS_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT16");
	all_sds[isds].data_type = DFNT_UINT16;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;


	/* cpath*/
	isds++;
	strcpy(all_sds[isds].name, CPATH_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT8");
	all_sds[isds].data_type = DFNT_UINT8;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;

	/* senrs */
	isds++;
	strcpy(all_sds[isds].name, SENRS_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT8");
	all_sds[isds].data_type = DFNT_UINT8;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;
	

	/* Geometry SDS if wanted */
	if (geometry_flag == 1) {
		*tot_sds=18;
		/* vz */
		isds++;
		strcpy(all_sds[isds].name, VZ_SDS_NAME);
		strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
		all_sds[isds].data_type = DFNT_INT16;
		strcpy(all_sds[isds].units, "degrees");
		all_sds[isds].scale_factor = 0.01;
	
		/* va */
		isds++;
		strcpy(all_sds[isds].name, VA_SDS_NAME);
		strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
		all_sds[isds].data_type = DFNT_INT16;
		strcpy(all_sds[isds].units, "degrees");
		all_sds[isds].scale_factor = 0.01;
		
		/* sz*/
		isds++;
		strcpy(all_sds[isds].name, SZ_SDS_NAME);
		strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
		all_sds[isds].data_type = DFNT_INT16;
		strcpy(all_sds[isds].units, "degrees");
		all_sds[isds].scale_factor = 0.01;
	
		/* sa*/
		isds++;
		strcpy(all_sds[isds].name, SA_SDS_NAME);
		strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
		all_sds[isds].data_type = DFNT_INT16;
		strcpy(all_sds[isds].units, "degrees");
		all_sds[isds].scale_factor = 0.01;
	}

	/* L1T_index */
	if (l1t_index_flag == 1) {
		isds++;
		strcpy(all_sds[isds].name, L1T_INDEX_SDS_NAME);
		strcpy(all_sds[isds].data_type_name, "DFNT_UINT16");
		all_sds[isds].data_type = DFNT_UINT16;
		strcpy(all_sds[isds].units, "unitless");
		all_sds[isds].scale_factor = 1;

		isds++;
		strcpy(all_sds[isds].name, L1T_COL_SDS_NAME);
		strcpy(all_sds[isds].data_type_name, "DFNT_UINT16");
		all_sds[isds].data_type = DFNT_UINT16;
		strcpy(all_sds[isds].units, "unitless");
		all_sds[isds].scale_factor = 1;

		isds++;
		strcpy(all_sds[isds].name, L1T_ROW_SDS_NAME);
		strcpy(all_sds[isds].data_type_name, "DFNT_UINT16");
		all_sds[isds].data_type = DFNT_UINT16;
		strcpy(all_sds[isds].units, "unitless");
		all_sds[isds].scale_factor = 1;
	}
	*tot_sds=(isds+1);
	return 0;
}


/*********************************************************************************
    Set the some attributes for the field. The type of attributes depend on
    the field_name.

    return 0 on success and 1 on failure
*/
int setup_pointer_sds_info(sds_info_t *all_sds, int * tot_sds)
{
	set_blank_qa_sds_info(all_sds);
	int nchar = 1500;   /* A smaller number had caused segmentation fault*/
	int status;
	int isds;
	status=0;
	isds = 0;

	/*  b61*/
	strcpy(all_sds[isds].name, B61_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "Degrees Celsius");
	all_sds[isds].scale_factor = 0.01;
	isds++;
	/*  b62*/
	strcpy(all_sds[isds].name, B62_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "Degrees Celsius");
	all_sds[isds].scale_factor = 0.01;
	isds++;
	/* sat*/
	strcpy(all_sds[isds].name, SAT_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT8");
	all_sds[isds].data_type = DFNT_UINT8;
	strcpy(all_sds[isds].units, "bit field");
	all_sds[isds].scale_factor = 1;
	if ((all_sds[isds].qa = (char*)calloc(nchar, sizeof(char))) == NULL) {
		status = 4;
		Error("Cannot allocate memory", "setup_sds_info", status);
		return status;
	}
	strcpy(all_sds[isds].qa,   "\n\tBits are numbered from right to left (bit 0 = LSB, bit 7 = MSB):\n"
		                "\tBit      Description\n"
		                "\t0        band 1  (1 = saturated, 0 = not saturated)\n"
		                "\t1        band 2  (1 = saturated, 0 = not saturated)\n"
		                "\t2        band 3  (1 = saturated, 0 = not saturated)\n"
		                "\t3        band 4  (1 = saturated, 0 = not saturated)\n"
		                "\t4        band 5  (1 = saturated, 0 = not saturated)\n"
		                "\t5        band 61 (1 = saturated, 0 = not saturated)\n"
		                "\t6        band 62 (1 = saturated, 0 = not saturated)\n"
		                "\t7        band 7  (1 = saturated, 0 = not saturated)\n");

	isds++;

	/* nobs*/
	strcpy(all_sds[isds].name, NOBS_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT16");
	all_sds[isds].data_type = DFNT_UINT16;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;
	isds++;

	/* cpath*/
	strcpy(all_sds[isds].name, CPATH_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT8");
	all_sds[isds].data_type = DFNT_UINT8;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;
	isds++;

	strcpy(all_sds[isds].name, L1T_INDEX_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT16");
	all_sds[isds].data_type = DFNT_UINT16;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;
	isds++;

	strcpy(all_sds[isds].name, L1T_COL_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT16");
	all_sds[isds].data_type = DFNT_UINT16;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;
	isds++;

	strcpy(all_sds[isds].name, L1T_ROW_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT16");
	all_sds[isds].data_type = DFNT_UINT16;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;
	isds++;

	*tot_sds=(isds);
	return status;
}



/***************************************************************************
    Set the "scaling factor" and "units" attributes for the given SDS.
*/

int set_sds_attribute(int32 sds_id, char *sds_name, sds_info_t *all_sds, int nsds_max)
{
	int isds; 
	char found = 0;
	char status;
	char message[MSGLEN];

	for (isds = 0; isds < nsds_max; isds++) {
		if (strcmp(sds_name, all_sds[isds].name) == 0) {
			found = 1;
			break;
		}
	}

	if (found == 0) {
		status = 1;
		sprintf(message, "The given sds name %s is not supported", sds_name);
		Error(message, "set_sds_attribute", status);
		return status;
	}

	/*  units*/
	if (SDsetattr(sds_id, "units", DFNT_CHAR8, strlen(all_sds[isds].units), all_sds[isds].units) == FAIL) {
		status = 1;
		sprintf(message, "SDsetattr() failed for  %s\n", all_sds[isds].name);
		Error(message, "set_sds_attribute", status);
		return status;
	}
	/*  scale_factor*/
	if (SDsetattr(sds_id, "scale_factor", DFNT_FLOAT64, 1, &all_sds[isds].scale_factor) == FAIL) {
		status = 1;
		sprintf(message, "SDsetattr() failed for  %s\n", all_sds[isds].name);
		Error(message, "set_sds_attribute", status);
		return status;
	}

	/* Additionally, for the sturation and cloud masks, need to state what the numbers in the mask represent*/
	if (strcmp(sds_name, SAT_SDS_NAME) == 0) {
		if (SDsetattr(sds_id, "band saturation mask", DFNT_CHAR8, strlen(all_sds[isds].qa), all_sds[isds].qa) == FAIL) {
			status = 1;
			sprintf(message, "SDsetattr() failed for  %s\n", all_sds[isds].name);
			Error(message, "set_sds_attribute", status);
			return status;
		}
	}
	else if (strcmp(sds_name, DTCLD_SDS_NAME) == 0) {
		if (SDsetattr(sds_id, "Classification tree cloud mask", DFNT_CHAR8, strlen(all_sds[isds].qa), all_sds[isds].qa) == FAIL) {
			status = 1;
			sprintf(message, "SDsetattr() failed for  %s\n", all_sds[isds].name);
			Error(message, "set_sds_attribute", status);
			return status;
		}
	}
	else if (strcmp(sds_name, ACCA_SDS_NAME) == 0) {  
		if (SDsetattr(sds_id, "ACCA cloud mask", DFNT_CHAR8, strlen(all_sds[isds].qa), all_sds[isds].qa) == FAIL) {
			status = 1;
			sprintf(message, "SDsetattr() failed for  %s\n", all_sds[isds].name);
			Error(message, "set_sds_attribute", status);
			return status;
		}
	}

	return 0;
}



/*********************************************************************************
    Set the some attributes for the field. The type of attributes depend on
    the field_name.

    return 0 on success and 1 on failure
*/
int  setup_srf_sds_info(sds_info_t *all_sds, char geometry_flag, char nbar_sz_flag, char l1t_index_flag, int * tot_sds)
{

	set_blank_qa_sds_info(all_sds);
	int nchar = 1500;   /* A smaller number had caused segmentation fault*/
	int status;
	int isds;
	isds = 0;
	/*  b1*/
	strcpy(all_sds[isds].name, B01_SRF_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "reflectance, unitless");
	all_sds[isds].scale_factor = 0.0001;

	/*  b2*/
	isds++;
	strcpy(all_sds[isds].name, B02_SRF_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "reflectance, unitless");
	all_sds[isds].scale_factor = 0.0001;

	/*  b3*/
	isds++;
	strcpy(all_sds[isds].name, B03_SRF_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "reflectance, unitless");
	all_sds[isds].scale_factor = 0.0001;

	/*  b4*/
	isds++;
	strcpy(all_sds[isds].name, B04_SRF_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "reflectance, unitless");
	all_sds[isds].scale_factor = 0.0001;

	/*  b5*/
	isds++;
	strcpy(all_sds[isds].name, B05_SRF_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "reflectance, unitless");
	all_sds[isds].scale_factor = 0.0001;


	/*  b61*/
	isds++;
	strcpy(all_sds[isds].name, B61_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "Degrees Celsius");
	all_sds[isds].scale_factor = 0.01;

	/*  b62*/
	isds++;
	strcpy(all_sds[isds].name, B62_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "Degrees Celsius");
	all_sds[isds].scale_factor = 0.01;

	/*  b7*/
	isds++;
	strcpy(all_sds[isds].name, B07_SRF_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "reflectance, unitless");
	all_sds[isds].scale_factor = 0.0001;

	/*  ndvi*/
	isds++;
	strcpy(all_sds[isds].name, NDVI_SRF_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 0.0001;

	/*  doy*/
	isds++;
	strcpy(all_sds[isds].name, DOY_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
	all_sds[isds].data_type = DFNT_INT16;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;

	/* sat*/
	isds++;
	strcpy(all_sds[isds].name, SAT_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT8");
	all_sds[isds].data_type = DFNT_UINT8;
	strcpy(all_sds[isds].units, "bit field");
	all_sds[isds].scale_factor = 1;
	if ((all_sds[isds].qa = (char*)calloc(nchar, sizeof(char))) == NULL) {
		status = 4;
		Error("Cannot allocate memory", "setup_sds_info", status);
		return status;
	}
	strcpy(all_sds[isds].qa,   "\n\tBits are numbered from right to left (bit 0 = LSB, bit 7 = MSB):\n"
		                "\tBit      Description\n"
		                "\t0        band 1  (1 = saturated, 0 = not saturated)\n"
		                "\t1        band 2  (1 = saturated, 0 = not saturated)\n"
		                "\t2        band 3  (1 = saturated, 0 = not saturated)\n"
		                "\t3        band 4  (1 = saturated, 0 = not saturated)\n"
		                "\t4        band 5  (1 = saturated, 0 = not saturated)\n"
		                "\t5        band 61 (1 = saturated, 0 = not saturated)\n"
		                "\t6        band 62 (1 = saturated, 0 = not saturated)\n"
		                "\t7        band 7  (1 = saturated, 0 = not saturated)\n");

	/* dtcld*/
	isds++;
	strcpy(all_sds[isds].name, DTCLD_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT8");
	all_sds[isds].data_type = DFNT_UINT8;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;
	if ((all_sds[isds].qa = (char*)calloc(nchar, sizeof(char))) == NULL) {
		status = 4;
		Error("Cannot allocate memory", "setup_sds_info", status);
		return status;
	}
	strcpy(all_sds[isds].qa,   "\n"
		                "\tValue        Description\n"
		                "\t0           not cloudy\n"
		                "\t1           cloudy\n"
		                "\t2           not cloudy but adjacent to a cloudy pixel\n"
		                "\t200         could not be classified reliably\n");

	/* acca*/
	isds++;
	strcpy(all_sds[isds].name, ACCA_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT8");
	all_sds[isds].data_type = DFNT_UINT8;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;
	if ((all_sds[isds].qa = (char*)calloc(nchar, sizeof(char))) == NULL) {
		status = 1;
		Error("Cannot allocate memory", "setup_sds_info", status);
		return status;
	}
	strcpy(all_sds[isds].qa,   "\n"
		                "\tValue        Description\n"
		                "\t0           not cloudy\n"
		                "\t1           cloudy\n");


	/* nobs*/
	isds++;
	strcpy(all_sds[isds].name, NOBS_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT16");
	all_sds[isds].data_type = DFNT_UINT16;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;


	/* cpath*/
	isds++;
	strcpy(all_sds[isds].name, CPATH_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT8");
	all_sds[isds].data_type = DFNT_UINT8;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;

	/* senrs */
	isds++;
	strcpy(all_sds[isds].name, SENRS_SDS_NAME);
	strcpy(all_sds[isds].data_type_name, "DFNT_UINT8");
	all_sds[isds].data_type = DFNT_UINT8;
	strcpy(all_sds[isds].units, "unitless");
	all_sds[isds].scale_factor = 1;


	/* Geometry SDS if wanted */
	if (geometry_flag == 1) {
		*tot_sds=18;
		/* vz */
		isds++;
		strcpy(all_sds[isds].name, VZ_SDS_NAME);
		strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
		all_sds[isds].data_type = DFNT_INT16;
		strcpy(all_sds[isds].units, "degrees");
		all_sds[isds].scale_factor = 0.01;

		/* va */
		isds++;
		strcpy(all_sds[isds].name, VA_SDS_NAME);
		strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
		all_sds[isds].data_type = DFNT_INT16;
		strcpy(all_sds[isds].units, "degrees");
		all_sds[isds].scale_factor = 0.01;

		/* sz*/
		isds++;
		strcpy(all_sds[isds].name, SZ_SDS_NAME);
		strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
		all_sds[isds].data_type = DFNT_INT16;
		strcpy(all_sds[isds].units, "degrees");
		all_sds[isds].scale_factor = 0.01;

		/* sa*/
		isds++;
		strcpy(all_sds[isds].name, SA_SDS_NAME);
		strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
		all_sds[isds].data_type = DFNT_INT16;
		strcpy(all_sds[isds].units, "degrees");
		all_sds[isds].scale_factor = 0.01;
	}
	if ( nbar_sz_flag == 1) {
		/* sz*/
		isds++;
		strcpy(all_sds[isds].name, NBAR_SZ_SDS_NAME);
		strcpy(all_sds[isds].data_type_name, "DFNT_INT16");
		all_sds[isds].data_type = DFNT_INT16;
		strcpy(all_sds[isds].units, "degrees");
		all_sds[isds].scale_factor = 0.01;
	}

	/* L1T_index */
	if (l1t_index_flag == 1) {
		isds++;
		strcpy(all_sds[isds].name, L1T_INDEX_SDS_NAME);
		strcpy(all_sds[isds].data_type_name, "DFNT_UINT16");
		all_sds[isds].data_type = DFNT_UINT16;
		strcpy(all_sds[isds].units, "unitless");
		all_sds[isds].scale_factor = 1;

		isds++;
		strcpy(all_sds[isds].name, L1T_COL_SDS_NAME);
		strcpy(all_sds[isds].data_type_name, "DFNT_UINT16");
		all_sds[isds].data_type = DFNT_UINT16;
		strcpy(all_sds[isds].units, "unitless");
		all_sds[isds].scale_factor = 1;

		isds++;
		strcpy(all_sds[isds].name, L1T_ROW_SDS_NAME);
		strcpy(all_sds[isds].data_type_name, "DFNT_UINT16");
		all_sds[isds].data_type = DFNT_UINT16;
		strcpy(all_sds[isds].units, "unitless");
		all_sds[isds].scale_factor = 1;
	}

	*tot_sds=(isds+1);
	return 0;
}



