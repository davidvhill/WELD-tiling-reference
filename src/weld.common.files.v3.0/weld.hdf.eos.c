/*********************************************************************************
    Convert the data to HDF-EOS format without using the HDF-EOS interface. The main
    reason of doing it this way is that the HDF-EOS interface is very limited in writing
    attributes for the individual SDS  and the SDS file.    It uses the basic HDF API
    as the HDF-EOS does and is more flexible.
    
    Adapted from LEDAPS space.c
*/


#include "weld.hdf.eos.h"

int world_PutSpaceDefHDF(char *tile_fname, int tile_h, int tile_v, sds_info_t *sds, int nsds)
{
    int32 sds_file_id;
    char struct_meta[MYHDF_MAX_NATTR_VAL];      /*Make sure it is long enough*/
    char cbuf[MYHDF_MAX_NATTR_VAL];
    /*char *hdfeos_version = "HDFEOS_V2.4";*/
    char *dim_names[2] = {"YDim", "XDim"};
    int ip, ic;
    int32 hdf_id;
    int32 vgroup_id[3];
    int32 sds_index, sds_id;
    char *grid_name = "WELD_GLOBE_GRID";
    char message[256];

    char status = 0;
    int32 xdimsize = TILEDIM;
    int32 ydimsize = TILEDIM;

    int k;


    //int32 projCode = 16;                        //Sinusoidal
    int32 sphereCode=12;                          //WGS 84
    //int32 zoneCode = -1;                        //For UTM only. will be ignored
    float64 projParameters[NPROJ_PARAM_HDFEOS];
    char cproj[] = "GCTP_SNSOID";

    float64 upleftpt[2], lowrightpt[2];
    float64 f_integral, f_fractional;
    sinus_proj_para_t  sinus_para;
    int isds;

    setup_sinus_proj_para(&sinus_para, "WORLD");

    /*  The projection parameter array is just like the GCTP one, but the latter has two additional elements. */
    for (k = 0; k < NPROJ_PARAM_HDFEOS; k++)
    projParameters[k] = 0.0;
    projParameters[0] = 6371007.181;                    //sphere
    /*
    projParameters[1] = 0;                    			//NA minor elipsoid axis b (standard for the datum)
    projParameters[2] = 0;                             //NA 1st standard paralel
    //projParameters[3] = albers_para.second_lat;      //NA 2nd standard paralel
    projParameters[4] = 0;                             //NA central meridian
    projParameters[5] = 0;                             //NA latitude of origin
	*/

    /*  Start to generate the character string for the global attribute:
        #define SPACE_STRUCT_METADATA ("StructMetadata.0")
    */

    ic = 0;
    sprintf(cbuf,
        "GROUP=SwathStructure\n"
        "END_GROUP=SwathStructure\n"
        "GROUP=GridStructure\n"
        "\tGROUP=GRID_1\n");

    if (!AppendMeta(struct_meta, &ic, cbuf)) {
        status = 1;
        Error("Error in AppendMeta() for (head)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    /* Put Grid information */
    upleftpt[0] = sinus_para.sinus_ULX + (tile_h/NMINI)*sinus_para.modis_tile_size + (tile_h%NMINI)* TILEDIM * WELD_PIXSIZE;
    upleftpt[1] = sinus_para.sinus_ULY - (tile_v/NMINI)*sinus_para.modis_tile_size - (tile_v%NMINI)* TILEDIM * WELD_PIXSIZE;
    lowrightpt[0] = upleftpt[0] + TILEDIM * WELD_PIXSIZE;
    lowrightpt[1] = upleftpt[1] - TILEDIM * WELD_PIXSIZE;

    sprintf(cbuf,
        "\t\tGridName=\"%s\"\n"
        "\t\tXDim=%d\n"
        "\t\tYDim=%d\n"
        "\t\tUpperLeftPointMtrs=(%.6f,%.6f)\n"
        "\t\tLowerRightMtrs=(%.6f,%.6f)\n"
        "\t\tProjection=%s\n",
        grid_name,
        xdimsize, ydimsize,
        upleftpt[0],   upleftpt[1],
        lowrightpt[0], lowrightpt[1],
        cproj);

    if (!AppendMeta(struct_meta, &ic, cbuf)) {
        status = 1;
        Error("Error in AppendMeta() for (grid information start)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    /*  GCTP projection parameters*/
    sprintf(cbuf, "\t\tProjParams=(");
    if (!AppendMeta(struct_meta, &ic, cbuf)) {
        status = 1;
        Error("Error in AppendMeta() for (grid projection parameters start)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    for (ip = 0; ip < NPROJ_PARAM_HDFEOS; ip++) {
        f_fractional = modf(projParameters[ip], &f_integral);
        if (fabs(f_fractional) == 0) {
            if (ip < (NPROJ_PARAM_HDFEOS - 1))
                sprintf(cbuf, "%.0lf,", projParameters[ip]);
            else
                sprintf(cbuf, "%.0lf)", projParameters[ip]);
        }
        else {
            if (ip < (NPROJ_PARAM_HDFEOS - 1))  /* The LEDAPS is a potential bug: ip < (NPROJ_PARAM_HDFEOS + 1)   */
                sprintf(cbuf, "%.6lf,", projParameters[ip]);
            else
                sprintf(cbuf, "%.6lf)", projParameters[ip]);
        }
        if (!AppendMeta(struct_meta, &ic, cbuf)) {
            status = 1;
            Error("Error in AppendMeta() for (individual grid projection parameter) ", "world_PutSpaceDefHDF", status);
            goto CLEANUP;
        }
    }
    sprintf(cbuf, "\n");

    if (!AppendMeta(struct_meta, &ic, cbuf)) {
        status = 1;
        Error("Error in AppendMeta() for (grid projection parameter end) %s", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    sprintf(cbuf,
        "\t\tSphereCode=%d\n"
        "\t\tGridOrigin=HDFE_GD_UL\n",
        sphereCode);
    if (!AppendMeta(struct_meta, &ic, cbuf)) {
        status = 1;
        Error("Error in AppendMeta() for (grid information end) ", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    /* Put SDS group */

    sprintf(cbuf,
        "\t\tGROUP=Dimension\n"
        "\t\tEND_GROUP=Dimension\n"
        "\t\tGROUP=DataField\n");

    if (!AppendMeta(struct_meta, &ic, cbuf)) {
        status = 1;
        Error("Error in AppendMeta() for (SDS group start)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    for (isds = 0; isds < nsds; isds++) {
        sprintf(cbuf,
            "\t\t\tOBJECT=DataField_%d\n"
            "\t\t\t\tDataFieldName=\"%s\"\n"
            "\t\t\t\tDataType=%s\n"
            "\t\t\t\tDimList=(\"%s\",\"%s\")\n"
            "\t\t\tEND_OBJECT=DataField_%d\n",
            isds, sds[isds].name, sds[isds].data_type_name, dim_names[0], dim_names[1], isds);
        if (!AppendMeta(struct_meta, &ic, cbuf)) {
            status = 1;
            Error("Error in AppendMeta() for (SDS group)", "world_PutSpaceDefHDF", status);
            goto CLEANUP;
        }
    }

    sprintf(cbuf,
        "\t\tEND_GROUP=DataField\n"
        "\t\tGROUP=MergedFields\n"
        "\t\tEND_GROUP=MergedFields\n");

    if (!AppendMeta(struct_meta, &ic, cbuf)) {
        status = 1;
        Error("Error in AppendMeta() for (SDS group end) ", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    /* Put trailer */

    sprintf(cbuf,
        "\tEND_GROUP=GRID_1\n"
        "END_GROUP=GridStructure\n"
        "GROUP=PointStructure\n"
        "END_GROUP=PointStructure\n"
        "END\n");

    if (!AppendMeta(struct_meta, &ic, cbuf)) {
        status = 1;
        Error("Error in AppendMeta() for (tail) ", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }


    /* Write the StructMetadata attribute.  Other global attributes have been set when the HDF is opened. */
    /* I didn't find DFACC_RDWR in hdf manual????*/
    if ((sds_file_id = SDstart(tile_fname, DFACC_RDWR)) == FAIL) {
        status = 1;
        sprintf(message, "Cannot open %s for DFACC_RDWR\n", tile_fname);
        Error(message, "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    if (SDsetattr(sds_file_id, SPACE_STRUCT_METADATA, DFNT_CHAR8, strlen(struct_meta), (VOIDP)struct_meta) == FAIL) {
        status = 1;
        Error("Error write global attributes", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }
    if (SDend(sds_file_id) == FAIL) {
        status = 1;
        sprintf(message, "Error in SDend() for %s", tile_fname);
        Error(message, "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }


    /* Setup the HDF Vgroup */

    if ((hdf_id = Hopen(tile_fname, DFACC_RDWR, 0)) == FAIL) {
        status = 1;
        sprintf(message, "Error in Hopen () for %s", tile_fname);
        Error(message, "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    /* Start the Vgroup access */

    if (Vstart(hdf_id) == FAIL) {
        status = 1;
        sprintf(message, "Error in Vstart () for %s", tile_fname);
        Error(message, "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    /* Create Root Vgroup for Grid */

    vgroup_id[0] = Vattach(hdf_id, -1, "w");
    if (vgroup_id[0] == FAIL) {
        status = 1;
        Error("Error in getting Grid Vgroup ID (Vattach)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    if (Vsetname(vgroup_id[0], grid_name) == FAIL) {
        status = 1;
        Error("setting Grid Vgroup name (Vsetname)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    if (Vsetclass(vgroup_id[0], "GRID") == FAIL) {
        status = 1;
        Error("Error in setting Grid Vgroup class (Vsetclass)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    /* Create Data Fields Vgroup */

    vgroup_id[1] = Vattach(hdf_id, -1, "w");
    if (vgroup_id[1] == FAIL) {
        status = 1;
        Error("Error in getting Data Fields Vgroup ID (Vattach)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }
    if (Vsetname(vgroup_id[1], "Data Fields") == FAIL) {
        status = 1;
        Error("setting Data Fields Vgroup name (Vsetname)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }
    if (Vsetclass(vgroup_id[1], "GRID Vgroup") == FAIL) {
        status = 1;
        Error("Error in setting Data Fields Vgroup class (Vsetclass)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }
    if (Vinsert(vgroup_id[0], vgroup_id[1]) == FAIL) {
        status = 1;
        Error("Error in inserting Data Fields Vgroup (Vinsert)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    /* Create Attributes Vgroup */

    vgroup_id[2] = Vattach(hdf_id, -1, "w");
    if (vgroup_id[2] == FAIL) {
        status = 1;
        Error("getting Attributes Vgroup ID (Vattach)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }
    if (Vsetname(vgroup_id[2], "Grid Attributes") == FAIL) {
        status = 1;
        Error("Error in setting Attributes Vgroup name (Vsetname)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }
    if (Vsetclass(vgroup_id[2], "GRID Vgroup") == FAIL) {
        status = 1;
        Error("Error in setting Attributes Vgroup class (Vsetclass)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }
    if (Vinsert(vgroup_id[0], vgroup_id[2]) == FAIL) {
        status = 1;
        Error("Error in inserting Attributes Vgroup (Vinsert)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    /* Attach SDSs to Data Fields Vgroup */

    sds_file_id = SDstart(tile_fname, DFACC_RDWR);
    if (sds_file_id == FAIL) {
        status = 1;
        Error("Error in opening output file for DFACC_RDWR(2)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }
    for (isds = 0; isds < nsds; isds++) {
        sds_index = SDnametoindex(sds_file_id, sds[isds].name);
        if (sds_index == FAIL) {
            status = 1;
            sprintf(message, "Error in getting SDS index (SDnametoindex) for %s", sds[isds].name);
            Error(message, "world_PutSpaceDefHDF", status);
            goto CLEANUP;
        }
        sds_id = SDselect(sds_file_id, sds_index);
        if (sds_id == FAIL) {
            status = 1;
            sprintf(message, "Error in getting SDS ID (SDselect) for %s", sds[isds].name);
            Error(message, "world_PutSpaceDefHDF", status);
            goto CLEANUP;
        }
        if (Vaddtagref(vgroup_id[1], DFTAG_NDG, SDidtoref(sds_id)) == FAIL) {
            status = 1;
            Error("Error in adding reference tag to SDS (Vaddtagref)", "world_PutSpaceDefHDF", status);
            goto CLEANUP;
        }
        if (SDendaccess(sds_id) == FAIL) {
            status = 1;
            sprintf(message, "Error in  ending access to SDS for %s", sds[isds].name);
            Error(message, "world_PutSpaceDefHDF", status);
            goto CLEANUP;
        }
    }
    if (SDend(sds_file_id) == FAIL) {
        status = 1;
        sprintf(message, "Error in ending SD access (2) for %s", tile_fname);
        Error(message, "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    /* Detach Vgroups */

    if (Vdetach(vgroup_id[0]) == FAIL) {
        status = 1;
        Error("Error in detaching from Grid Vgroup (Vdetach)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }
    if (Vdetach(vgroup_id[1]) == FAIL) {
        status = 1;
        Error("Error in detaching from Data Fields Vgroup (Vdetach)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }
    if (Vdetach(vgroup_id[2]) == FAIL) {
        status = 1;
        Error("Error in detaching from Attributes Vgroup (Vdetach)", "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    /* Close access */

    if (Vend(hdf_id) == FAIL) {
        status = 1;
        sprintf(message, "Error in end Vgroup access (Vend) for %s", tile_fname);
        Error(message, "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }
    if (Hclose(hdf_id) == FAIL) {
        status = 1;
        sprintf(message, "Error in end HDF access (Hclose) for %s", tile_fname);
        Error(message, "world_PutSpaceDefHDF", status);
        goto CLEANUP;
    }

    CLEANUP:
    return status;
}


int PutSpaceDefHDF(char *tile_fname, int tile_h, int tile_v, sds_info_t *sds, albers_proj_para_t albers_para, int nsds)
{
	int32 sds_file_id;
	char struct_meta[MYHDF_MAX_NATTR_VAL];      /*Make sure it is long enough*/
	char cbuf[MYHDF_MAX_NATTR_VAL];
	/*char *hdfeos_version = "HDFEOS_V2.4";*/
	char *dim_names[2] = {"YDim", "XDim"};
	int ip, ic;
	int32 hdf_id;
	int32 vgroup_id[3];
	int32 sds_index, sds_id;
	char *grid_name = "WELD_GRID";
	char message[MSGLEN];

	char status = 0;
	int32 xdimsize = TILEDIM;
	int32 ydimsize = TILEDIM;

	int k;
	//int32 projCode = 3;                         //Albers Conic Equal Area
	int32 sphereCode=12;                          //WGS 84
	//int32 zoneCode = -1;                        //For UTM only. will be ignored
	float64 projParameters[NPROJ_PARAM_HDFEOS];
	char cproj[] = "GCTP_ALBERS";

	float64 upleftpt[2], lowrightpt[2];
	float64 f_integral, f_fractional;

	int isds;
	

	/*  The projection parameter array is just like the GCTP one, but the latter has two additional elements. */
	for (k = 0; k < NPROJ_PARAM_HDFEOS; k++)
		projParameters[k] = 0.0;
	projParameters[0] = 0.0;                    //major elipsoid axis a (standard for the datum)
	projParameters[1] = 0.0;                    //minor elipsoid axis b (standard for the datum)
	projParameters[2] = albers_para.first_lat;       //1st standard paralel
	projParameters[3] = albers_para.second_lat;      //2nd standard paralel
	projParameters[4] = albers_para.central_lon;     //central meridian
	projParameters[5] = albers_para.origin_lat;      //latitude of origin

	/*  Start to generate the character string for the global attribute:
	    #define SPACE_STRUCT_METADATA ("StructMetadata.0")
	*/

	ic = 0;
	sprintf(cbuf,
	    "GROUP=SwathStructure\n"
	    "END_GROUP=SwathStructure\n"
	    "GROUP=GridStructure\n"
	    "\tGROUP=GRID_1\n");

	if (!AppendMeta(struct_meta, &ic, cbuf)) {
	    	status = 1;
		Error("Error in AppendMeta() for (head)", "PutSpaceDefHDF", 0);
		goto CLEANUP;
	}

	/* Put Grid information */
	upleftpt[0] = albers_para.Albers_ULX + tile_h * TILEDIM * WELD_PIXSIZE;
	upleftpt[1] = albers_para.Albers_ULY - tile_v * TILEDIM * WELD_PIXSIZE;
	lowrightpt[0] = upleftpt[0] + TILEDIM * WELD_PIXSIZE;
	lowrightpt[1] = upleftpt[1] - TILEDIM * WELD_PIXSIZE;

	sprintf(cbuf,
	    "\t\tGridName=\"%s\"\n"
	    "\t\tXDim=%d\n"
	    "\t\tYDim=%d\n"
	    "\t\tUpperLeftPointMtrs=(%.6f,%.6f)\n"
	    "\t\tLowerRightMtrs=(%.6f,%.6f)\n"
	    "\t\tProjection=%s\n",
	    grid_name,
	    xdimsize, ydimsize,
	    upleftpt[0],   upleftpt[1],
	    lowrightpt[0], lowrightpt[1],
	    cproj);

	if (!AppendMeta(struct_meta, &ic, cbuf)) {
		status = 1;
		Error("Error in AppendMeta() for (grid information start)", "PutSpaceDefHDF", 0);
		goto CLEANUP;
	}

	/*  GCTP projection parameters*/
	sprintf(cbuf, "\t\tProjParams=(");
	if (!AppendMeta(struct_meta, &ic, cbuf)) {
		status = 1;
		Error("Error in AppendMeta() for (grid projection parameters start)", "PutSpaceDefHDF", 0);
		goto CLEANUP;
	}

	for (ip = 0; ip < NPROJ_PARAM_HDFEOS; ip++) {
	    f_fractional = modf(projParameters[ip], &f_integral);
	    if (fabs(f_fractional) == 0) {
	        if (ip < (NPROJ_PARAM_HDFEOS - 1))
	            sprintf(cbuf, "%.0lf,", projParameters[ip]);
	        else
	            sprintf(cbuf, "%.0lf)", projParameters[ip]);
	    }
	    else {
	        if (ip < (NPROJ_PARAM_HDFEOS - 1))  /* The LEDAPS is a potential bug: ip < (NPROJ_PARAM_HDFEOS + 1)   */
	            sprintf(cbuf, "%.6lf,", projParameters[ip]);
	        else
	            sprintf(cbuf, "%.6lf)", projParameters[ip]);
	    }
	    if (!AppendMeta(struct_meta, &ic, cbuf)) {
	        status = 1;
	        Error("Error in AppendMeta() for (individual grid projection parameter)", "PutSpaceDefHDF", 0);
	        goto CLEANUP;
	    }
	}
	sprintf(cbuf, "\n");

	if (!AppendMeta(struct_meta, &ic, cbuf)) {
	    status = 1;
	    Error("Error in AppendMeta() for (grid projection parameter end)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}
	
	sprintf(cbuf,
	    "\t\tSphereCode=%d\n"
	    "\t\tGridOrigin=HDFE_GD_UL\n",
	    sphereCode);
	if (!AppendMeta(struct_meta, &ic, cbuf)) {
	    status = 1;
	    Error("Error in AppendMeta() for (grid information end)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}

	/* Put SDS group */

	sprintf(cbuf,
	    "\t\tGROUP=Dimension\n"
	    "\t\tEND_GROUP=Dimension\n"
	    "\t\tGROUP=DataField\n");

	if (!AppendMeta(struct_meta, &ic, cbuf)) {
	    status = 1;
	    Error("Error in AppendMeta() for (SDS group start) ", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}

	for (isds = 0; isds < nsds; isds++) {
	    sprintf(cbuf,
	        "\t\t\tOBJECT=DataField_%d\n"
	        "\t\t\t\tDataFieldName=\"%s\"\n"
	        "\t\t\t\tDataType=%s\n"
	        "\t\t\t\tDimList=(\"%s\",\"%s\")\n"
	        "\t\t\tEND_OBJECT=DataField_%d\n",
	        isds, sds[isds].name, sds[isds].data_type_name, dim_names[0], dim_names[1], isds);
	    if (!AppendMeta(struct_meta, &ic, cbuf)) {
	        status = 1;
	        Error("Error in AppendMeta() for (SDS group)", "PutSpaceDefHDF", 0);
	        goto CLEANUP;
	    }
	}

	sprintf(cbuf,
	    "\t\tEND_GROUP=DataField\n"
	    "\t\tGROUP=MergedFields\n"
	    "\t\tEND_GROUP=MergedFields\n");

	if (!AppendMeta(struct_meta, &ic, cbuf)) {
	    status = 1;
	    Error("Error in AppendMeta() for (SDS group end)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}

	/* Put trailer */

	sprintf(cbuf,
	    "\tEND_GROUP=GRID_1\n"
	    "END_GROUP=GridStructure\n"
	    "GROUP=PointStructure\n"
	    "END_GROUP=PointStructure\n"
	    "END\n");

	if (!AppendMeta(struct_meta, &ic, cbuf)) {
	    status = 1;
	    Error("Error in AppendMeta() for (tail)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}


	/* Write the StructMetadata attribute.  Other global attributes have been set when the HDF is opened. */
	/* I didn't find DFACC_RDWR in hdf manual????*/
	if ((sds_file_id = SDstart(tile_fname, DFACC_RDWR)) == FAIL) {
	    status = 1;  
	    sprintf(message, "Cannot open %s for DFACC_RDWR", tile_fname);
	    Error(message, "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	} 
	
	if (SDsetattr(sds_file_id, SPACE_STRUCT_METADATA, DFNT_CHAR8, strlen(struct_meta), (VOIDP)struct_meta) == FAIL) {
	    status = 1;
	    Error("Error write global attributes", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}
	if (SDend(sds_file_id) == FAIL) {
	    status = 1;
	    sprintf(message, "Error in SDend() for %s", tile_fname);
	    Error(message, "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}

	 
	/* Setup the HDF Vgroup */

	if ((hdf_id = Hopen(tile_fname, DFACC_RDWR, 0)) == FAIL) {
	    status = 1;
	    sprintf(message, "Error in Hopen () for %s", tile_fname);
	    Error(message, "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}

	/* Start the Vgroup access */

	if (Vstart(hdf_id) == FAIL) {
	    status = 1;
	    sprintf(message, "Error in Vstart () for %s", tile_fname);
	    Error(message, "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}

	/* Create Root Vgroup for Grid */

	vgroup_id[0] = Vattach(hdf_id, -1, "w");
	if (vgroup_id[0] == FAIL) {
	    status = 1;
	    Error("Error in getting Grid Vgroup ID (Vattach)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}

	if (Vsetname(vgroup_id[0], grid_name) == FAIL) {
	    status = 1;
	    Error("Error in setting Grid Vgroup name (Vsetname)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}

	if (Vsetclass(vgroup_id[0], "GRID") == FAIL) {
	    status = 1;
	    Error("Error in setting Grid Vgroup class (Vsetclass)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}

	/* Create Data Fields Vgroup */

	vgroup_id[1] = Vattach(hdf_id, -1, "w");
	if (vgroup_id[1] == FAIL) {
	    status = 1;
	    Error("Error in getting Data Fields Vgroup ID (Vattach)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}
	if (Vsetname(vgroup_id[1], "Data Fields") == FAIL) {
	    status = 1;
	    Error("Error setting Data Fields Vgroup name (Vsetname)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}
	if (Vsetclass(vgroup_id[1], "GRID Vgroup") == FAIL) {
	    status = 1;
	    Error("Error in setting Data Fields Vgroup class (Vsetclass)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}
	if (Vinsert(vgroup_id[0], vgroup_id[1]) == FAIL) {
	    status = 1;
	    Error("Error in inserting Data Fields Vgroup (Vinsert)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}

	/* Create Attributes Vgroup */

	vgroup_id[2] = Vattach(hdf_id, -1, "w");
	if (vgroup_id[2] == FAIL) {
	    status = 1;
	    Error("Error in getting Attributes Vgroup ID (Vattach)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}
	if (Vsetname(vgroup_id[2], "Grid Attributes") == FAIL) {
	    status = 1;
	    Error("Error in setting Attributes Vgroup name (Vsetname)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}
	if (Vsetclass(vgroup_id[2], "GRID Vgroup") == FAIL) { 
	    status = 1;
	    Error("Error in setting Attributes Vgroup class (Vsetclass)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}
	if (Vinsert(vgroup_id[0], vgroup_id[2]) == FAIL) {
	    status = 1;
	    Error("Error in inserting Attributes Vgroup (Vinsert)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}

	/* Attach SDSs to Data Fields Vgroup */

	sds_file_id = SDstart(tile_fname, DFACC_RDWR);
	if (sds_file_id == FAIL) {
	    status = 1;
	    Error("Error in opening output file for DFACC_RDWR(2)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}
	for (isds = 0; isds < nsds; isds++) {
	    sds_index = SDnametoindex(sds_file_id, sds[isds].name);
	    if (sds_index == FAIL) {
	        status = 1;
	        sprintf(message, "Error in getting SDS index (SDnametoindex) for %s", sds[isds].name); 
		Error(message, "PutSpaceDefHDF", 0);
	        goto CLEANUP;
	    }
	    sds_id = SDselect(sds_file_id, sds_index);
	    if (sds_id == FAIL) {
	        status = 1;
	        Error("Error in getting SDS ID (SDselect)","PutSpaceDefHDF", 0);
	        goto CLEANUP;
	    }
	    if (Vaddtagref(vgroup_id[1], DFTAG_NDG, SDidtoref(sds_id)) == FAIL) {
	        status = 1;
	        Error("Error in adding reference tag to SDS (Vaddtagref)", "PutSpaceDefHDF", 0);
	        goto CLEANUP;
	    }
	    if (SDendaccess(sds_id) == FAIL) {
	        status = 1;
	        Error("Error in SDendaccess", "PutSpaceDefHDF", 0);
	        goto CLEANUP;
	    }
	}
	if (SDend(sds_file_id) == FAIL) {
	    status = 1;
	    Error("Error in SDend", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}
	    
	

	/* Detach Vgroups */

	if (Vdetach(vgroup_id[0]) == FAIL) {
	    status = 1;
	    Error("Error in detaching from Grid Vgroup (Vdetach)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}
	if (Vdetach(vgroup_id[1]) == FAIL) {
	    status = 1;
	    Error("Error in detaching from Data Fields Vgroup (Vdetach)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}
	if (Vdetach(vgroup_id[2]) == FAIL) {
	    status = 1;
	    Error("Error in detaching from Attributes Vgroup (Vdetach)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}

	/* Close access */

	if (Vend(hdf_id) == FAIL) { 
	    status = 1;
	    Error("Error in end Vgroup access (Vend)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}
	if (Hclose(hdf_id) == FAIL) {
	    status = 1;
	    Error("Error in end HDF access (Hclose)", "PutSpaceDefHDF", 0);
	    goto CLEANUP;
	}

	CLEANUP:
	return status;
}

/*********************************************************************************
    Append the characters in *s to *cbuf.

    Copied from LEDAPS space.c
*/
bool AppendMeta(char *cbuf, int *ic, char *s) {

	char message[MSGLEN];
	int nc, i;

	if (ic < 0) return false;
	nc = strlen(s);
	if (nc <= 0) return false;
	if (*ic + nc > MYHDF_MAX_NATTR_VAL) {
		sprintf(message, "Array struct_meta is too short. Needs at least %d bytes", *ic + nc);
		Error(message, "AppendMeta", 1);
	      	return false;
	}
	  
	for (i = 0; i < nc; i++) {
	  cbuf[*ic] = s[i];
	  (*ic)++;
	}

	cbuf[*ic] = '\0';

	return true;
}

/*********************************************************************************
    set dimension names 
*/
int PutSDSDimInfo(int32 sds_id, char *dimname, int irank)
{
    int32 dimid;
    char message[MSGLEN];
    int status;
    
    if ((dimid = SDgetdimid(sds_id, irank)) == FAIL) {
	status = 1;
        sprintf(message, "Error in SDgetdimid() for rank %d", irank);
	Error(message, "PutSDSDimInfo", status);
	return status;
    } 

    if (SDsetdimname(dimid, dimname) == FAIL) {
        status = 1;
        sprintf(message, "Error in SDsetdimname() for rank %d", irank);
	Error(message, "PutSDSDimInfo", status);
	return status;
    }

    return 0;
}

bool PutAttrDouble(int32 sds_id, Myhdf_attr_t *attr, double *val)
/* 
!C******************************************************************************

!Description: 'PutAttrDouble' writes an attribute from a parameter of type
 'double' to a HDF file.
 
!Input Parameters:
 sds_id         SDS id
 attr           Attribute data structure; the following fields are used:
                   name, type, nval

!Output Parameters:
 val            An array of values from the HDF attribute (converted from
                  type 'double' to the native type
 (returns)      Status:
                  'true' = okay
		      'false' = error writing the attribute information

!Team Unique Header:

 ! Design Notes:
   1. The values in the attribute are converted from the stored type to 
      'double' type.
   2. The HDF file is assumed to be open for SD (Science Data) access.
   3. If the attribute has more than 'MYHDF_MAX_NATTR_VAL' values, an error
      status is returned.
   4. Error messages are handled with the 'RETURN_ERROR' macro.
!END****************************************************************************
*/
{
    char8 val_char8[MYHDF_MAX_NATTR_VAL];
    int8 val_int8[MYHDF_MAX_NATTR_VAL];
    uint8 val_uint8[MYHDF_MAX_NATTR_VAL];
    int16 val_int16[MYHDF_MAX_NATTR_VAL];
    uint16 val_uint16[MYHDF_MAX_NATTR_VAL];
    int32 val_int32[MYHDF_MAX_NATTR_VAL];
    uint32 val_uint32[MYHDF_MAX_NATTR_VAL];
    float32 val_float32[MYHDF_MAX_NATTR_VAL];
    float64 val_float64[MYHDF_MAX_NATTR_VAL];
    int i;
    void *buf;
    buf=NULL;
    bool status = true;
  
    if (attr->nval <= 0  ||  attr->nval > MYHDF_MAX_NATTR_VAL) {
        Error("invalid number of values", "PutAttrDouble", 1);
        status = false;
    }
  
    switch (attr->type) {
      case DFNT_CHAR8:
        for (i = 0; i < attr->nval; i++) {
          if (     val[i] >= ((double)MYHDF_CHAR8H)) val_char8[i] = MYHDF_CHAR8H;
          else if (val[i] <= ((double)MYHDF_CHAR8L)) val_char8[i] = MYHDF_CHAR8L;
          else if (val[i] >= 0.0) val_char8[i] = (char8)(val[i] + 0.5);
          else                    val_char8[i] = -((char8)(-val[i] + 0.5));
        }
        buf = (void *)val_char8;
        break;
  
      case DFNT_INT8:
        for (i = 0; i < attr->nval; i++) {
          if (     val[i] >= ((double)MYHDF_INT8H)) val_int8[i] = MYHDF_INT8H;
          else if (val[i] <= ((double)MYHDF_INT8L)) val_int8[i] = MYHDF_INT8L;
          else if (val[i] >= 0.0) val_int8[i] =   (int8)( val[i] + 0.5);
          else                    val_int8[i] = -((int8)(-val[i] + 0.5));
        }
        buf = (void *)val_int8;
        break;
  
      case DFNT_UINT8:
        for (i = 0; i < attr->nval; i++) {
          if (     val[i] >= ((double)MYHDF_UINT8H)) val_uint8[i] = MYHDF_UINT8H;
          else if (val[i] <= ((double)MYHDF_UINT8L)) val_uint8[i] = MYHDF_UINT8L;
          else if (val[i] >= 0.0) val_uint8[i] =   (uint8)( val[i] + 0.5);
          else                    val_uint8[i] = -((uint8)(-val[i] + 0.5));
        }
        buf = (void *)val_uint8;
        break;
  
      case DFNT_INT16:
        for (i = 0; i < attr->nval; i++) {
          if (     val[i] >= ((double)MYHDF_INT16H)) val_int16[i] = MYHDF_INT16H;
          else if (val[i] <= ((double)MYHDF_INT16L)) val_int16[i] = MYHDF_INT16L;
          else if (val[i] >= 0.0) val_int16[i] =   (int16)( val[i] + 0.5);
          else                    val_int16[i] = -((int16)(-val[i] + 0.5));
        }
        buf = (void *)val_int16;
        break;
  
      case DFNT_UINT16:
        for (i = 0; i < attr->nval; i++) {
          if (     val[i] >= ((double)MYHDF_UINT16H)) val_uint16[i] = MYHDF_UINT16H;
          else if (val[i] <= ((double)MYHDF_UINT16L)) val_uint16[i] = MYHDF_UINT16L;
          else if (val[i] >= 0.0) val_uint16[i] =   (uint16)( val[i] + 0.5);
          else                    val_uint16[i] = -((uint16)(-val[i] + 0.5));
        }
        buf = (void *)val_uint16;
        break;
  
      case DFNT_INT32:
        for (i = 0; i < attr->nval; i++) {
          if (     val[i] >= ((double)MYHDF_INT32H)) val_int32[i] = MYHDF_INT32H;
          else if (val[i] <= ((double)MYHDF_INT32L)) val_int32[i] = MYHDF_INT32L;
          else if (val[i] >= 0.0) val_int32[i] =   (int32)( val[i] + 0.5);
          else                    val_int32[i] = -((int32)(-val[i] + 0.5));
        }
        buf = (void *)val_int32;
        break;
  
      case DFNT_UINT32:
        for (i = 0; i < attr->nval; i++) {
          if (     val[i] >= ((double)MYHDF_UINT32H)) val_uint32[i] = MYHDF_UINT32H;
          else if (val[i] <= ((double)MYHDF_UINT32L)) val_uint32[i] = MYHDF_UINT32L;
          else if (val[i] >= 0.0) val_uint32[i] =   (uint32)( val[i] + 0.5);
          else                    val_uint32[i] = -((uint32)(-val[i] + 0.5));
        }
        buf = (void *)val_uint32;
        break;
  
      case DFNT_FLOAT32:
        for (i = 0; i < attr->nval; i++) {
          if (     val[i] >= ((double)MYHDF_FLOAT32H)) val_float32[i] = MYHDF_FLOAT32H;
          else if (val[i] <= ((double)MYHDF_FLOAT32L)) val_float32[i] = MYHDF_FLOAT32L;
          else if (val[i] >= 0.0) val_float32[i] =   (float32)( val[i] + 0.5);
          else                    val_float32[i] = -((float32)(-val[i] + 0.5));
        }
        buf = (void *)val_float32;
        break;
  
      case DFNT_FLOAT64:
        if (sizeof(float64) == sizeof(double))
          buf = (void *)val;
        else {
          for (i = 0; i < attr->nval; i++)
            val_float64[i] = val[i];
          buf = (void *)val_float64;
        }
        break;
  
      default: 
      {
        Error("unimplmented type", "PutAttrDouble", 1);
        status = false;
      }
    }
  
    if (SDsetattr(sds_id, attr->name, attr->type, attr->nval, buf) == FAIL) {
        Error("Error in setting attribute", "PutAttrDouble", 1);
        status = false;
    }
  
    return status;
}


bool PutAttrString(int32 sds_id, Myhdf_attr_t *attr, char *string)
/* 
!C******************************************************************************

!Description: 'PutAttrString' writes an attribute from a parameter of type
 'double' to a HDF file.
 
!Input Parameters:
 sds_id         SDS id
 attr           Attribute data structure; the following fields are used:
                   name, type, nval

!Output Parameters:
 val            An array of values from the HDF attribute (converted from
                  type 'double' to the native type
 (returns)      Status:
                  'true' = okay
		      'false' = error writing the attribute information

!Team Unique Header:

 ! Design Notes:
   1. The values in the attribute are converted from the stored type to 
      'double' type.
   2. The HDF file is assumed to be open for SD (Science Data) access.
   3. If the attribute has more than 'MYHDF_MAX_NATTR_VAL' values, an error
      status is returned.
   4. Error messages are handled with the 'RETURN_ERROR' macro.
!END****************************************************************************
*/
{
    char8 val_char8[MYHDF_MAX_NATTR_VAL];
    int i;
    void *buf;
    bool status = true;
  
    if (attr->nval <= 0  ||  attr->nval > MYHDF_MAX_NATTR_VAL) {
	Error("invalid number of values", "PutAttrString", 1);
        status = false;
    }
  
    if (attr->type != DFNT_CHAR8) {
        Error("invalid type -- not string (char8)", "PutAttrString", 1);
        status = false;
    }
  
    if (sizeof(char8) == sizeof(char))
      buf = (void *)string;
    else {
      for (i = 0; i < attr->nval; i++) 
        val_char8[i] = (char8)string[i];
      buf = (void *)val_char8;
    }
  
    if (SDsetattr(sds_id, attr->name, attr->type, attr->nval, buf) == FAIL) {
        Error("Error in setting attribute", "PutAttrString", 1);
        status = false;
    }
  
    return status;
}
