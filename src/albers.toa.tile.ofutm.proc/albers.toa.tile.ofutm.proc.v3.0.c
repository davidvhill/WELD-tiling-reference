/*  Re- projection Landsat reflectance images from UTM to CONUS Albers
 
    The input parameter file gives the bands 1-7,  band saturation flag, DT and ACCA cloud masks, and
    the sensor-solar geometry file for each acquisition; such information for one acquisition is considered
    a record which is ended by a line of a single character, '\n'.  

    At the end of the re- projection, output a list of the scenes used in re- projection each tile.
    Only band 1 file names and ACCA file names are given in the list; other input band can be
    derived from band 1 file name by naming convention.

    Apr, 2014 [VAL]
    version 2.2 temporal sorting was introduced to keep the order of tiles
    consistent
    
    July, 2013 [VAL]

    version 2.0 for TM & ETM


    January 25, 2010
    v1.5
    use float64 for coordinates

    January 17, 2010
    v1.4
    Tile ID is given on command line.
    Dynamically allocate memory for the tile SDS instead of using static arrays (actually
    a lot of WELD USA code is reused here.

    December 22, 2010
    v1.3 
    If an acquisition is on the projected earth edge, it can fall on the tiles on both
    the earth side and the west side of the projected earth; use the UTM coordinates of
    all its pixels  to find out on which tiles the acquisition falls.  An acquisition is 
    on the projected earth edge in the global sinusoidal projection if its utm zone
    is 1 or 60.  For non-edge UTM, use just the edge pixels of the acquisition to find
    out the acquisition-tile mapping, just like the v1.1.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <mfhdf.h>
#include "L457.comp.tile.handler.h"
#include "L457.toa.acq.handler.h"
#include "weld.comp.util.h"


void usage(char *commandname);
void help(char *commandname);


int main(int argc, char* argv[])
{
	/*  Commandline parameters*/
	char region[100];		/* WORLD|CONUS|Alaska. Purposely make the array very big to accommodate any "crazy" command-line parameters*/
	char time_period[100];		/* for naming output files only */
	char tile_id[100];		/* e.g., h10v10 */
	char geometry_option[100];	/* the option whether or not to include geometry SDS in output */
	char l1t_index_option[100];	/* the option whether or not to generate an l1t_index SDS */
	char version[100];		/* The version ID */
	char acq_list[MAX_NAME_LEN];	/* The list of input acquisitions */
	char as_pointers[MAX_NAME_LEN]; /* An old version for update */
	char outdir[MAX_NAME_LEN];		/* output directory */

	char *env_lut_dir;		    /* lookup table directory */
	uint8 as_pntrs=0;
	l457_tile_info_t comp;   	/*  The output tile. It can be an file created from scratch, or an updated of the existing file*/
	alldoy_t alldoy;	/*  A book-keep tile of all the input DOY, used to remove the same-day within-path redundancy*/

	/* Command-line parameter name and value */
	char para_name[MAX_NAME_LEN], para_val[MAX_NAME_LEN];
	/* When 2-D array is mapped to 1-D, the index in the 1-D array*/
	int k_tile, k_ref, k_thm;
	int irow_ref, icol_ref, irow_thm, icol_thm;
	int i, irow, icol;
	int base;

	/* The interpolation grid is spaced every "spacing" rows and columns on the WELD tile.*/
	int spacing = SIMPLICIAL_SPACING; 
	int16 n_zone;				/* Number of unique UTM zones in the input for this tile*/
	int16 *uniq_utm_zone;		/* All the unique UTM zones*/
	int16 *acq_utm_zone;		/* The UTM zone for each acquisition */
	int16 *zone_index;			/* Each acquisition's index in array uniq_utm_zone */ 
	int16 izone;
	char  *this_acq_is_used;		/* An indicator array for all the acquisitions. Whether an acquisition
						          has data for this tile; 1 yes and 0 no  */
	double *utm_x, *utm_y;			/* UTM coordinates for every pixel in a tile */

	/* The attributes of all the SDS in the tile */
	sds_info_t  all_sds[NSDS_MAX];

	/* To store one input UTM acquisition. Only keep one acquisition in memory at any time */
	l457_acq_info_t acq;
	float32 pixel_size;	/* should be 30.0. Will check */
	l457acq_fname_t *all_acq_fname;  /* All the input UTM acquistion file names in the given list*/

	/* The L1T information of the band 1 filenames plus solar geometry for all the acquisitions.
	   As an example of 3 acquisitions, all_l1tinfo will contain three strings:
           "L7045028_02820080706_B10.hdf center_sz=29.640991 center_sa=134.639969",
    	   "L7045028_02820080722_B10.hdf center_sz=31.998859 center_sa=136.539413",
    	   "L7045029_02920080706_B10.hdf center_sz=28.876621 center_sa=132.151764",
	 */
	char **all_l1tinfo;
	char **all_inp_pointers;
	char **ptr, **ptr0;			/* temporary */

	/* The l1t_index SDS has been defined as uint8 type, but the variables working around it are
	   defined as int type for easier code update later when the code is run for the global data.
	   By the time of update, only l1t_index SDS type need to be changed to int, but the rest of 
	   the supporting variables can stay as the int type, alleviate the need to extensive substitution.
	*/
	int nacq_from_comp;		/* Number of acquisitions from the given inputs' l1t metadata */
	int nacq_from_acq_list;		/* Number of acquisitions from the input acquisition list; some 
					   	   	   	   may not have valid data for this tile */
	int all_nacq;			/* The sum of the TM and ETM Acq */
	int all_inpntrs;
	int iacq;   		/* Acquisition index for the those from the input acquisition list */	
	int overall_iacq;	/* The overall acquisition index when additionally those from the existing file also considered */
	int used_nacq=0;	/* Number of acquisitions that are actually have valid data for this tile, all acquisitions considered */

	/* Two observation */
	l457_obs_t new_obs;

	char *chpos, *basepos;
	int len;
	char *inside_mask;
	char tempstr[MAX_LINE_LEN];
	char tempinp[MAX_LINE_LEN];
	long *Nnofillpxs=NULL;

	/* At the end of the re-projection, rename the re-projection based on the new DOY range */
	int16 doymin, doymax;
	char comp_fname_with_DOYrange[MAX_NAME_LEN];
	char alldoy_fname_with_DOYrange[MAX_NAME_LEN];
	char geometry_flag;		/* 1: generate 4 geometry SDS, 0: do not */
	char l1t_index_flag; 		/* 1: generate l1t_index SDS, 0: do not */
	/* Currently l1tindex SDS is of uint8, but declare l1t_index_fill int so that in the future if there is
	   type change l1t_index SDS, there in no need to change the declaration of l1tindex  
	*/
	int l1t_index_fill = uint16_65535;
	double tmp_ndvi;
	int tot_sds; /* total number of sds in tile*/
	int status;
	int cYear;
	char sensKey[8];//sensor combination key { L45 | L57 }
	char PT_Key[8]; //product type key { PTR | TOA }
	char message[MSGLEN];
	uint8 isCldOrSat=0;
	all_l1tinfo=all_inp_pointers=ptr=ptr0=NULL;

	set_blank_qa_sds_info(all_sds);
	/***********************************************************************
	 *Command line parsing
	 ***********************************************************************/

	if (argc != 10) {
		status = 1;
		if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
			help(argv[0]);
			exit(status);
		}

		fprintf(stderr, "%d\tIncorrect number of command-line parameters; \n", status);
		fprintf(stderr, "\tthe following parameters are supplied:\n");
		for (i=1; i< argc; i++)
			fprintf(stderr, "\t%s\n", argv[i]);
		fprintf(stderr, "\n");

		usage(argv[0]);
		exit(status);
	}
	status = 0;

	/* utm_x and utm_y will store the UTM coordinates of a UTM zone for each pixel on the 
	   WELD tile. Although there may be multiple unique UTM zones among the L1T input
	   for this tile, we work zone by zone and keep the UTM coordinates of only one
	   zone in memory at any moment.
	*/
	utm_x = utm_y    = NULL;
	acq_utm_zone     = NULL;
	uniq_utm_zone    = NULL;
	zone_index       = NULL;
	this_acq_is_used = NULL;

	all_acq_fname = NULL;
	all_l1tinfo   = NULL;
	ptr=NULL;
	ptr0=NULL;
	inside_mask=NULL;


	/* The command-line parameters can be given in any order, although it is more logical as shown in usage() */
	for (i = 1; i < argc; i++) {
		chpos = strchr(argv[i], '=');
		if (chpos == NULL)  	
			status = 1;
		else {
			len = chpos - argv[i];
			strncpy(para_name, argv[i], len);
			para_name[len] = '\0';
			
			strcpy(para_val, chpos+1);
		}

		if (status == 1 || strlen(para_name) == 0 || strlen(para_val) == 0) {
			status = 1;
			Error("Wrong format on command line", "main", status);	
			usage(argv[0]);
			exit(status);
		}

		if (strcmp(para_name, "region") == 0) 
			strcpy(region, para_val);

		else if (strcmp(para_name, "time_period") == 0) 
			strcpy(time_period, para_val);

		else if (strcmp(para_name, "tile_id") == 0) 
			strcpy(tile_id, para_val);

		else if (strcmp(para_name, "geometry_flag") == 0) 
			strcpy(geometry_option, para_val);

		else if (strcmp(para_name, "l1t_index_flag") == 0) 
			strcpy(l1t_index_option, para_val);
		
		else if (strcmp(para_name, "version") == 0) 
			strcpy(version, para_val);

		else if (strcmp(para_name, "acq_list") == 0) 
			strcpy(acq_list, para_val);

		else if (strcmp(para_name, "as_pointers") == 0)
			strcpy(as_pointers, para_val);

		else if (strcmp(para_name, "out_dir") == 0) 
			strcpy(outdir, para_val);

		else {
			status = 1;
			sprintf(message, "Parameter \"%s\" is not supported", para_name);
			Error(message, "main", status);
			exit(status);
		}
	}

	/***********************************************************************
	 *CALLOCATIONS AND Initializations
	 ***********************************************************************/
	/* setup luts for vis */
	env_lut_dir=NULL;
	env_lut_dir=getenv("WELDCOMM");
	if(env_lut_dir==NULL){
		status = 1;
		sprintf(message, "Could get the Environmental variable WELDCOMM ");
		Error(message, "main", status);
		exit(status);
	}

	/* setup sensor key */
	chpos=strchr(time_period,'.');
	chpos++;
	cYear=atoi(chpos);
	if(cYear>1999){
		strcpy(sensKey,"L57");
	}else{
		strcpy(sensKey,"L45");
	}
	sensKey[3]='\0';


	if (strcmp(region, "CONUS") == 0 || strcmp(region, "conus") == 0)
		strcpy(region, "CONUS");	/* Use the standard name */
	else if(strcmp(region, "Alaska") == 0 || strcmp(region, "alaska") == 0)
		strcpy(region, "Alaska");   	/* Use the standard name */
	else { 
		status = 1;
		sprintf(message, "The input region %s is not considered", region);
		Error(message, "main", status);
		exit(status);
	}
	
	if (strcmp(as_pointers, "true") == 0
		|| strcmp(as_pointers, "TRUE") == 0){
		as_pntrs=1;
	}
	status = setup_weld_v1point6_proj_para(&albers_global, region);

	if (status == 1) {
		fprintf(stderr, "Error in setup_proj_para\n");
		exit(status);
	}
	
	status=asign_tileHV(tile_id, &comp.tile_h,  &comp.tile_v, region, message);
	if(status !=0){
		fprintf(stderr, "%s \n", message);
		exit(status);
	}

	/*  Whether or not output the geometry SDS */
	if (strcmp(geometry_option, "true") == 0) 
		geometry_flag = 1;
	else if (strcmp(geometry_option, "false") == 0) 
		geometry_flag = 0;
	else {
		status = 1;
		Error("The geometry parameter must be:  geometry_flag=true OR geometry_flag=false", "main", status);
		exit(status);
	}
	comp.geometry_flag = geometry_flag;

	/*  Whether or not output the l1t index */
	if (strcmp(l1t_index_option, "true") == 0) 
		l1t_index_flag = 1; 
	else if (strcmp(l1t_index_option, "false") == 0)
		l1t_index_flag = 0; 
	else {
		status = 1;
		Error("The l1t_index parameter must be:  l1t_index_flag=true OR l1t_index_flag=false", "main", status);
		exit(status);
	}
	comp.l1t_index_flag = l1t_index_flag;

	/*  Remove the trailing '/' in string outdir if there is one*/
	len = strlen(outdir);
	if (outdir[len-1] == '/') outdir[len-1] = '\0';

	comp.nrow     = TILEDIM;
	comp.ncol     = TILEDIM;
	alldoy.nrow   = TILEDIM;
	alldoy.ncol   = TILEDIM;

	nacq_from_comp = 0;
	all_nacq = 0;
	all_inpntrs = 0;

	/*  Set the pointers to NULL before open any acquisition*/
	acq_set_blank(&acq);

	sprintf(comp.fname,          "%s/%s.%s.%s.%s.%s.hdf", outdir, sensKey, region, time_period, tile_id, version);
	sprintf(alldoy.fname, "%s/alldoy.%s.%s.%s.%s.%s.hdf", outdir, sensKey, region, time_period, tile_id, version);

	strcpy(PT_Key, "TOA");
	if(!as_pntrs){
		if ((status = l457_tile_create(&comp, all_sds, region, &tot_sds)) != 0) {
			Error("Error in l457_tile_create", "main", 0);
			goto CLEANUP;
		}
	}else{
		strcpy(PT_Key, "PTR");
		if ((status = l457_pointer_tile_create(&comp, all_sds, region, &tot_sds)) != 0) {
			Error("Error in l457_pointer_tile_create", "main", 0);
			goto CLEANUP;
		}
	}


	if ((status = alldoy_tile_create(&alldoy)) != 0) {
		Error("Error in alldoy_tile_create", "main", 0);
		goto CLEANUP;
	}

/*
	if ((waterCounts  = (uint8*)calloc((TILEDIM*TILEDIM), sizeof(uint8))) == NULL ||
		(validCounts  = (uint8*)calloc((TILEDIM*TILEDIM), sizeof(uint8))) == NULL ||
		(soilCounts = (uint8*)calloc((TILEDIM*TILEDIM), sizeof(uint8))) == NULL ||
		(snowCounts = (uint8*)calloc((TILEDIM*TILEDIM), sizeof(uint8))) == NULL ) {
		status = 4;
		Error("Cannot allocate memory", "main", status);
		goto CLEANUP;
	}
*/
	/*****************************************************
	* Set #valid #water and #soil to 0
	******************************************************/
/*
	for (irow = 0; irow < TILEDIM; irow++) {
		for (icol = 0; icol < TILEDIM; icol++) {
			k_tile = irow*TILEDIM+icol;
			validCounts[k_tile]=0;
			waterCounts[k_tile]=0;
			soilCounts[k_tile]=0;

			snowCounts[k_tile]=0;
		}
	}
*/

	/* Read in the input filenames and find the number of acquisitions */
	status =get_l457acq_number(acq_list, &nacq_from_acq_list);
	if (status != 0) {
    		Error("Error in get_l457acq_number", "main", 0);
    		goto CLEANUP;
	}
	/*allocate the all_acq_fname */
	all_acq_fname=(l457acq_fname_t*)calloc(nacq_from_acq_list, sizeof(l457acq_fname_t));
	Nnofillpxs=(long*)calloc(nacq_from_acq_list, sizeof(long));
//	Nvalidpxs=(long*)calloc(nacq_from_acq_list, sizeof(long));
	all_acq_fname=(l457acq_fname_t*)calloc(nacq_from_acq_list, sizeof(l457acq_fname_t));
	status = get_l457acq_fnames(acq_list, all_acq_fname, nacq_from_acq_list);
	if (status != 0) {
    		Error("Error in get_l457acq_fnames", "main", 0);
    		goto CLEANUP;
	}



	/* If an old file is given, then all_nacq will be non-zero and the number of
	   acquisitions from the given acquisition list is added to it.
	*/
	all_nacq += nacq_from_acq_list;

	if ((inside_mask = (char *) calloc(TILEDIM*TILEDIM, sizeof(char))) == NULL 	) {
		status = 4;
		Error("Cannot allocate memory", "main", 0);
		goto CLEANUP;
	}


	/* The the band 1 filename and associate scene center solar geometry is collectively  termed l1t information
	   which will be written out as a global attribute L1T_METADATA in the output re-projection.
	   If an acquisition that intersects a tile does not contain any valid data for this tile
	   its l1t information will not be written out. If an acquisition contains valid data for the tile
	   but the data are not selected for the re-projection, its l1t information will still be written out.

	   The band 1 filenames can be used for QA, and the scene center solar geometry information 
	   will be used in subsequent atmospheric correction. 
	*/
	/* Expand  all_l1tinfo to hold the information of all L1T acquisitions. 
	   We delay the updating of all_l1tinfo with the new acquisitions until here because we need to
           save the given inputs' l1tinfo (if there is any) in all_l1tinfo first.
	
	   The all_l1tinfo is updated as new acquisitions are read in; no function is created for the updating from the acquisitions.
	*/
	if ((ptr = (char **) calloc(all_nacq, sizeof(char*))) == NULL ||
		(ptr0 = (char **) calloc(all_nacq, sizeof(char*))) == NULL
		) {
		status = 4;
		Error("Cannot allocate memory", "main", 0);
		goto CLEANUP;
	} 

	for (iacq = nacq_from_comp; iacq < all_nacq; iacq++){
		if ((ptr[iacq] = (char *) calloc(MAX_NAME_LEN, sizeof(char))) == NULL ||
			(ptr0[iacq]= (char *) calloc(MAX_NAME_LEN, sizeof(char))) == NULL ){
			status = 4;
			Error("Cannot allocate memory", "main", 0);
			goto CLEANUP;
		}
	}

	all_l1tinfo = ptr;
	all_inp_pointers = ptr0;
	if ((this_acq_is_used =(char*) calloc(all_nacq, sizeof(char)))  == NULL) {
		status = 4;
		Error("Cannot allocate memory", "main", status);
		goto CLEANUP;
	}
	/* All the acquisitions recorded in the re-projection attribute l1t_metadata have valid data on this tile */
	for (iacq = 0; iacq < nacq_from_comp; iacq++) 
		this_acq_is_used[iacq] = 1;	
	/* Whether an acquisition in the acq_list has valid data for this tile. Initialize to NO (0)
	 * for (iacq = nacq_from_acq_list; iacq < all_nacq; iacq++)
	 * this is wrong (causes memory leak) should be nacq_from_comp instead of nacq_from_acq_list;
	 **************************************************************************/
	for (iacq = nacq_from_comp; iacq < all_nacq; iacq++)
		this_acq_is_used[iacq] = 0;	

	if ((utm_x = (double*)calloc(TILEDIM * TILEDIM, sizeof(double))) == NULL ||
	    (utm_y = (double*)calloc(TILEDIM * TILEDIM, sizeof(double))) == NULL) {
		status = 1;
		Error("Cannot allocate memory", "main", status);
		goto CLEANUP;
	}

	if ((acq_utm_zone =    (int16*)calloc(nacq_from_acq_list, sizeof(int16))) == NULL ||
        (uniq_utm_zone =   (int16*)calloc(nacq_from_acq_list, sizeof(int16))) == NULL ||
        (zone_index =      (int16*)calloc(nacq_from_acq_list, sizeof(int16))) == NULL) {
       		status = 4;
    		Error("Cannot allocate memory", "main", status);
    		goto CLEANUP;
	}
	
	/*  Find out the unique UTM zone numbers from all the acquisitions on this tile, and
	    each acquisition's index in the unique utm zone array.
	*/
	n_zone = 0;
	for (iacq = 0; iacq < nacq_from_acq_list; iacq++) {
		status = read_utm_zone( &acq_utm_zone[iacq], all_acq_fname[iacq].acq_fname);
		if (status != 0) {
			sprintf(message, "Error in  read_utm_zone() for %s", all_acq_fname[iacq].acq_fname);
			Error(message, "main", 0);
			goto CLEANUP;
		}
		if (iacq == 0) {
			uniq_utm_zone[0] = acq_utm_zone[0];
			zone_index[0] = 0;
			n_zone = 1;
		}
		else {
			zone_index[iacq] = -1;
			for (i = 0; i < n_zone; i++) {
				if (acq_utm_zone[iacq] == uniq_utm_zone[i]) {
					zone_index[iacq] = i;
					break;
				}
			}
			if (zone_index[iacq] == -1) {
				uniq_utm_zone[n_zone] = acq_utm_zone[iacq];
				zone_index[iacq] = n_zone;
				n_zone++;
			}
		}
	}

	/***********************************************************************
	 *COMPOSITING starts
	 ***********************************************************************/
	/*  	The original implementation uses a lot of memory and PROBABLY is a little slower too.  
		It first derives the UTM coordinates for every pixel of the tile and for all the possible
		UTM zones, then at each pixel location loop through all the acquisitions. A pixel in
		the tile is addressed only once in memory. (April 8, 2010. Probably not ONCE: the 
		same location is addressed consecutively nacq_tile times.) But the corresponding pixels
		in different acquistions will be far apart and addressing is slower.	

		The new implementation uses much less memory, and it may be faster:
		First find out how many unique UTM zones are concerned. Then loop through each zone and derive
		UTM coordinates for each zone; at any time only one tile of UTM coordinates is in memory.
		Then loop through acquisitions of this UTM zone,  and for each acquisition loop through the pixels on
		the tiles. At any time there is only one acquistion in memory and it greatly saves memory.
		Because addresses increases only incrementally with small steps, the chance it is in the cache
		is very very high and thus can be faster. Only problem: each pixel of the Landsat TILE will be 
		addressed multiple times in memory and can be slower.

		Use the new implementation because it allows to do re-projection for many tiles in parallel.
	*/

	pixel_size = REF_PIXSIZE;	   /*  Set in header file. Can also read from hdf, but I need this info here.
					       Later in read_acq it is checked whether pixel size is indeed REF_PIXSIZE */

	//go through UTM zones
	for (izone = 0; izone < n_zone; izone++) {
		/*  Interpolate for UTM coordinates for a unique zone*/
		interpolate_utm_xy_albers(albers_global,
				   uniq_utm_zone[izone],
				   pixel_size,
				   TILEDIM,
				   TILEDIM,
                   comp.tile_h,
                   comp.tile_v,
				   spacing,
				   utm_x,
				   utm_y);
		//go through each scene in the utm zone izone
		for (iacq = 0; iacq < nacq_from_acq_list; iacq++) {
			if (zone_index[iacq] != izone)
				continue;

			overall_iacq = iacq + nacq_from_comp;
			strcpy(acq.acq_hdf,  all_acq_fname[iacq].acq_fname);
			strcpy(acq.dtclhdf,  all_acq_fname[iacq].dtclfname);
			strcpy(acq.accahdf,  all_acq_fname[iacq].accafname);


			//printf("Before : ACQUISITION:  %s \n", all_acq_fname[iacq].acq_fname);
			/*add the fmask  */
			status = acq_grab(&acq, 0, 1);
			Nnofillpxs[iacq]=0;
//			Nvalidpxs[iacq]=0;
			for (irow = 0; irow < TILEDIM; irow++) {
				base = irow * TILEDIM;
				for (icol = 0; icol < TILEDIM; icol++) {
					k_tile = base+icol;

					irow_ref = floor((acq.ymax_utm - utm_y[k_tile]) / pixel_size);
					icol_ref = floor((utm_x[k_tile] - acq.xmin_utm) / pixel_size);

					if (acq.nrow_ref == acq.nrow_thm) {	 /* thermal is resampled to reflective resolution */
						irow_thm = irow_ref;
						icol_thm = icol_ref;
					}
					else {
						irow_thm = irow_ref/2;
						icol_thm = icol_ref/2;
					}
					/*Pixel is not in the UTM scene*/
					if (irow_ref < 0 || irow_ref >= acq.nrow_ref ||
					    icol_ref < 0 || icol_ref >= acq.ncol_ref)
						continue;

					k_ref = irow_ref * acq.ncol_ref + icol_ref;
					//increment jnobs
					if(acq.b03[k_ref]!=int16_minus32768){
						/*	A location cannot be sensed more than once a day by Landsat. If the acquisition date of
							a pixel in one acquisition is the same as that of another acquisition, the pixel must be in the
							redundant overlapping strip within the WRS path, and only the one that comes first is selected.

							At very high latitude, is it possible to have more than one observation from different
							paths on the same day???  Maybe paths should be considered too.
						*/
						if (isRedundant(acq.doy, &alldoy, k_tile) == 1){
							continue;
						}
						status = alldoy_tile_update(acq.doy, &alldoy, k_tile);
						if (status != 0) {
							Error("Error in update_alldoy_tile()", "main", 0);
							goto CLEANUP;
						}
						comp.nobs[k_tile]++;

					}else{
						continue;
					}

					//check if the obs is ok
					Nnofillpxs[iacq]++;
				} /* icol */
			} /* irow */
			/*  Free up all the dynamically allocated memory for this acquisition. */
			acq_close(&acq);
		} /*iacq*/

	} /*izone*/


	//due to path 1
	if ((status = alldoy_tile_close(&alldoy)) != 0) {
		Error("Error in alldoy_tile_close()", "main", 0);
		goto CLEANUP;
	}
	if ((status = alldoy_tile_create(&alldoy)) != 0) {
		Error("Error in alldoy_tile_create", "main", 0);
		goto CLEANUP;
	}


	for (izone = 0; izone < n_zone; izone++) { 
		/*  Interpolate for UTM coordinates for a unique zone*/
		/*  Interpolate for UTM coordinates for a unique zone*/
		interpolate_utm_xy_albers(albers_global,
				   uniq_utm_zone[izone],
				   pixel_size,
				   TILEDIM,
				   TILEDIM,
                   comp.tile_h,
                   comp.tile_v,
				   spacing,
				   utm_x,
				   utm_y);
		for (iacq = 0; iacq < nacq_from_acq_list; iacq++) {
			if (zone_index[iacq] != izone)
				continue;
			if(Nnofillpxs[iacq]==0)
				continue;// no point

			overall_iacq = iacq + nacq_from_comp;
			strcpy(acq.acq_hdf,  all_acq_fname[iacq].acq_fname);
			strcpy(acq.dtclhdf,  all_acq_fname[iacq].dtclfname);
			strcpy(acq.accahdf,  all_acq_fname[iacq].accafname);
			if(geometry_flag) strcpy(acq.geomhdf,  all_acq_fname[iacq].geomfname);

			//printf("Zone:%d ; ACQUISITION:  %s  NNF %ld ; NVAL %ld \n", izone, all_acq_fname[iacq].acq_fname, Nnofillpxs[iacq],  Nvalidpxs[iacq]);
			/*add the fmask  */

			status = acq_grab(&acq, geometry_flag, 1);
			if (status != 0) {
				Error("Error in acq_grab()", "main", 0);
				goto CLEANUP;
			}

			/* Update all_l1tinfo given this  acquisition. The info for an acquisition is in the format:
				L7045028_02820080706_B10.hdf center_sz=29.640991 center_sa=134.639969
			*/
			basepos = strrchr(acq.acq_hdf, '/');
			if (basepos == NULL)
				basepos = acq.acq_hdf;
			else
				basepos++;		/* skip '/' */

			if (acq_get_inp_pointer(all_acq_fname[iacq].acq_fname, tempinp, tempstr)!=0){
				status = 4;
				Error("Cannot allocate memory", "main", status);
				goto CLEANUP;
			}
			tempinp[21]='\0';

			sprintf(all_l1tinfo[overall_iacq],  "%s center_sz=%f center_sa=%f %c", tempinp, acq.center_sz, acq.center_sa, '\0');
			sprintf(all_inp_pointers[overall_iacq], "%s center_sz=%f center_sa=%f %c", basepos, acq.center_sz, acq.center_sa, '\0');
			/* printf("%s \n",all_l1tinfo[overall_iacq]);
			printf("%s \n",all_inp_pointers[overall_iacq]);*/

			for (irow = 0; irow < TILEDIM; irow++) { 
				base = irow * TILEDIM;
				for (icol = 0; icol < TILEDIM; icol++) {
					k_tile = base+icol;
					
					irow_ref = floor((acq.ymax_utm - utm_y[k_tile]) / pixel_size);
					icol_ref = floor((utm_x[k_tile] - acq.xmin_utm) / pixel_size);

					if (acq.nrow_ref == acq.nrow_thm) {	 /* thermal is resampled to reflective resolution */
						irow_thm = irow_ref;
						icol_thm = icol_ref;
					}
					else {
						irow_thm = irow_ref/2;
						icol_thm = icol_ref/2;
					}
					/*Pixel is not in the UTM scene*/
					if (irow_ref < 0 || irow_ref >= acq.nrow_ref || 
					    icol_ref < 0 || icol_ref >= acq.ncol_ref) 
						continue;

					k_ref = irow_ref * acq.ncol_ref + icol_ref;
					k_thm = irow_thm * acq.ncol_thm + icol_thm;

					/*	In some rare cases a few pixels at the edges of the acquisitions may not have
					  	geometry information although there are valid observations there. This is due
					  	to the geometry algorithm. The chance of occurance is 1 in a million.
					 */
					if (geometry_flag == 1) {
						if (acq.vz[k_ref] == int16_minus32768)
							continue;
					}

					/*   	If pixel is fill in any band, do not consider in re-projection. However, if there is
						no geometry information at the pixel on the tile yet, but there is geometry in the
						input and the geometry is desired,  write this geometry to the tile.
					*/
					if (acq.b01[k_ref] == int16_minus32768 || acq.b02[k_ref] == int16_minus32768 ||
					    acq.b03[k_ref] == int16_minus32768 || acq.b04[k_ref] == int16_minus32768 ||
					    acq.b05[k_ref] == int16_minus32768 || acq.b61[k_thm] == int16_minus32768 ||
					    acq.b07[k_ref] == int16_minus32768){
						if (comp.geometry_flag == 1) {
							if (comp.vz[k_tile] == int16_minus32768 && acq.vz[k_ref] != int16_minus32768) {
								/*If geometry is not fill, pass through the geometry and DOY*/
								comp.doy[k_tile] = acq.doy;
								comp.vz[k_tile] = acq.vz[k_ref];
								comp.va[k_tile] = acq.va[k_ref];
								comp.sz[k_tile] = acq.sz[k_ref];
								comp.sa[k_tile] = acq.sa[k_ref];
							}
						}
						continue;
					}

					/*NOW THE INPUT REACHING THIS POINT IS NONFILL IN ALL BANDS*/

					/*	A location cannot be sensed more than once a day by Landsat. If the acquisition date of
						a pixel in one acquisition is the same as that of another acquisition, the pixel must be in the
						redundant overlapping strip within the WRS path, and only the one that comes first is selected.

						At very high latitude, is it possible to have more than one observation from different
						paths on the same day???  Maybe paths should be considered too.
					*/
					if (isRedundant(acq.doy, &alldoy, k_tile) == 1)
						continue;

					status = alldoy_tile_update(acq.doy, &alldoy, k_tile);
					if (status != 0) {
						Error("Error in update_alldoy_tile()", "main", 0);
						goto CLEANUP;
					}
					/* The pixel from the new acquistion */
					new_obs.b01 = acq.b01[k_ref];
					new_obs.b02 = acq.b02[k_ref];
					new_obs.b03 = acq.b03[k_ref];
					new_obs.b04 = acq.b04[k_ref];
					new_obs.b05 = acq.b05[k_ref];
					new_obs.b61 = acq.b61[k_thm];
					new_obs.b62 = acq.b62[k_thm];
					new_obs.b07 = acq.b07[k_ref];
					new_obs.ndvi  = int16_minus32768;
					if(new_obs.b04 >= 0 && new_obs.b03 >= 0 && new_obs.b04 <= 10000 && new_obs.b03 <= 10000 ){
						if((new_obs.b04 + new_obs.b03) != 0  ){
							tmp_ndvi=((double)(new_obs.b04 - new_obs.b03)/(double)(new_obs.b04 + new_obs.b03));
							if((tmp_ndvi < -1.0)){
								new_obs.ndvi = -10000;
							}else if(tmp_ndvi > 1.0){
								new_obs.ndvi = 10000;
							}else if(!isnan(tmp_ndvi)){
								new_obs.ndvi = asInt16(tmp_ndvi*10000.0);
							}
						}
					}

					new_obs.doy   = acq.doy;
					new_obs.sat   = acq.sat[k_ref];
					new_obs.dtcld = acq.dtcld[k_ref];
					new_obs.acca  = acq.acca[k_ref];
					//this is critical;
					new_obs.qa_comp=comp.cpath[k_tile];
					//essential that cpath was defined before;
					new_obs.lsnsr_i=7; /*by default and it will be Landsat 7 ETM*/
					if(acq.isTM==1){
						new_obs.lsnsr_i=5; /* Landsat 5 TM*/
						if(strstr(acq.acq_hdf, L4_KEY)!= NULL){
							new_obs.lsnsr_i=4; /* if this key is found - Landsat 4 TM*/
						}
					}
					if (geometry_flag == 1) {
						new_obs.vz = acq.vz[k_ref];
						new_obs.va = acq.va[k_ref];
						new_obs.sz = acq.sz[k_ref];
						new_obs.sa = acq.sa[k_ref];
					}

					isCldOrSat=0;
					isCldOrSat=!(is_valid_clear_snex(acq.b02[k_ref], acq.b03[k_ref],
												     acq.b04[k_ref], acq.b05[k_ref],
												     acq.sat[k_ref],acq.acca[k_ref],
												     acq.dtcld[k_ref]));

					if (comp.b03[k_tile] == int16_minus32768 ){ 	/* Fill value at this location in the tile */
								l457_tile_update(&comp, k_tile, &new_obs, overall_iacq, icol_ref, irow_ref);
								this_acq_is_used[overall_iacq] = 1;
								continue; // no need to go further
					}

				} /* icol */
			} /* irow */
			/*  Free up all the dynamically allocated memory for this acquisition. */
			acq_close(&acq);
		} /*iacq*/
	} /*izone*/

	/***********************************************************************
	 *COMPOSITING IS OVER	 ***********************************************************************/

	/* do proper L1T tracking from scratch */
	for (iacq = 0; iacq < all_nacq; iacq++){
		this_acq_is_used[iacq] = 0;
	}

	//extra check for used L1T acquisition
	for (irow = 0; irow < TILEDIM; irow++) {
		for (icol = 0; icol < TILEDIM; icol++) {
			i = irow*TILEDIM+icol;
			if(comp.b03[i]==int16_minus32768){
				continue;
			}
			this_acq_is_used[comp.l1t_index[i]]=1;
		}
	}

	if (l1t_index_flag == 1) {

		l457_tile_update_comp_l1t_index(&comp,  all_l1tinfo, all_inp_pointers,
				this_acq_is_used, all_nacq, l1t_index_fill);

		/*****************************************************************
		 fprintf(stdout, "after --------------------\n");
		for (iacq = 0; iacq < all_nacq; iacq++) {
			fprintf(stdout, "%d:\t %d  %s\n", iacq,  this_acq_is_used[iacq], all_l1tinfo[iacq]);
		}
		*****************************************************************/
	
		/* Generate a final l1tinfo in which only acquisitions with valid data are kept and write it to the re-projection attribute */
		used_nacq=0;
		for (iacq = 0; iacq < all_nacq; iacq++) {
			if(this_acq_is_used[iacq]==0)continue;
			used_nacq++;
		}

		status = l457_tile_write_l1tinfo_alt(&comp, all_l1tinfo, all_inp_pointers, this_acq_is_used, all_nacq);
		if (status != 0) {
			Error("Error in l457_tile_write_l1tinfo", "main", 0);
			goto CLEANUP;
		}
	}

	/* Get the DOY range before the tile is closed. */
	doy_range(&comp, &doymin, &doymax);	

	sprintf(comp_fname_with_DOYrange,            "%s/%s.%s.%s.%s.doy%03dto%03d.%s.%s.hdf",  outdir, sensKey, region, time_period, tile_id, doymin, doymax, PT_Key, version);
	sprintf(alldoy_fname_with_DOYrange,   "%s/alldoy.%s.%s.%s.%s.doy%03dto%03d.%s.%s.hdf",  outdir, sensKey, region, time_period, tile_id, doymin, doymax, PT_Key, version);

	if(!as_pntrs){
		if ((status = l457_tile_write(&comp)) != 0) {
			Error("Error in l457_tile_write()", "main", 0);
			goto CLEANUP;
		}
	}else{

		if ((status = l457_pointer_tile_write(&comp)) != 0) {
			Error("Error in l457_pointer_tile_write()", "main", 0);
			goto CLEANUP;
		}
	}

	if ((status = l457_tile_close(&comp)) != 0) {
		Error("Error in l457_tile_close()", "main", 0);
		goto CLEANUP;
	}

	if ((status = alldoy_tile_write(&alldoy)) != 0) {
	Error("Error in alldoy_tile_close()", "main", 0);
		goto CLEANUP;
	}

	if ((status = alldoy_tile_close(&alldoy)) != 0) {
		Error("Error in alldoy_tile_close()", "main", 0);
		goto CLEANUP;
	}

	if (used_nacq == 0) {
		/* If none of the acquisition has written to this tile and this tile is
	   	   	   just created, delete this tile since it is empty.
	        	*/
		remove(comp.fname);
		remove(alldoy.fname);
	}
	else {
		/*  Rename the re-projected tile and the alldoy tile in compliance with WELD product naming convention.
	    	   	    But first need to add HDF-EOS georeference information for the re-projection tile.
			*/
		if ((status = PutSpaceDefHDF(comp.fname, comp.tile_h, comp.tile_v, all_sds, albers_global, tot_sds)) != 0) {
			sprintf(message, "Error in PutSpaceDefHDF() for %s", comp.fname);
			Error(message, "main", 0);
			goto CLEANUP;
		}
		rename(comp.fname, comp_fname_with_DOYrange);
		rename(alldoy.fname, alldoy_fname_with_DOYrange);
	}

	CLEANUP:

	if (status != 0) {
		remove(comp.fname);
		remove(alldoy.fname);
	}

	if (all_acq_fname != NULL) free(all_acq_fname);
	if (Nnofillpxs != NULL) free(Nnofillpxs);
//	if (Nvalidpxs != NULL) free(Nvalidpxs);
	if (inside_mask != NULL) free(inside_mask);
	if (utm_x != NULL) free(utm_x);
	if (utm_y != NULL) free(utm_y);
	if (uniq_utm_zone  != NULL)   free(uniq_utm_zone);
	if (acq_utm_zone   != NULL)   free(acq_utm_zone);  
	if (zone_index	   != NULL)   free(zone_index);	 
	if (this_acq_is_used != NULL) free(this_acq_is_used);

	set_free_qa_sds_info(all_sds);

	if (all_l1tinfo != NULL) {
		for (iacq=0; iacq < all_nacq; iacq++) {
			if (all_l1tinfo[iacq] != NULL)
				free(all_l1tinfo[iacq]);
		}
		free(all_l1tinfo);
	}

	if (all_inp_pointers != NULL) {
		for (iacq=0; iacq < all_nacq; iacq++) {
			if (all_inp_pointers[iacq] != NULL)
				free(all_inp_pointers[iacq]);
		}
		free(all_inp_pointers);
	}

	return status;
}


/******************************************************************************************
	Print the command-line usage of the main function.
*******************************************************************************************/
void usage(char *commandname)
{
	fprintf(stderr, "A CALLING EXAMPLE :\n"
			"%s region=CONUS\\\n"
			" time_period=month07.2008\\\n"
			" tile_id=h05v02\\\n"
			" geometry_flag=true\\\n"
			" l1t_index_flag=true\\\n"
			" version=v3.0\\\n"
			" acq_list=acq.list.month07.2008.h05v02.txt\\\n"
			" as_pointers=false\\\n"
			" out_dir=../../month07.2008\n\n", commandname);
}

/**************************************************************************************************************
	help function.

**************************************************************************************************************/
void help(char *commandname)
{
	fprintf(stderr, "The code requires 9 command-line parameters, given in the format name=value.\n"
			"The parameters can be given in any order, but it is more logical to be in\n"
			"the order as shown in the following calling examples.\n");

	fprintf(stderr, "\n############################################	CALLING EXAMPLE ONE:\n"
			"%s region=CONUS\\\n"
			" time_period=month07.2008\\\n"
			" tile_id=h05v02\\\n"
			" geometry_flag=true\\\n"
			" l1t_index_flag=true\\\n"
			" version=v3.0\\\n"
			" acq_list=acq.list.month07.2008.h05v02.txt\\\n"
			" as_pointers=false\\\n"
			" out_dir=../../month07.2008\n\n", commandname);

	fprintf(stderr, "In the above example,\n"
			"\"region=CONUS\" means re-projection for CONUS; it can also be \"region=Alaska\" for Alaska.\n\n"

			"\"time_period=month07.2008\" means string \"month07.2008\" is used to name the output re-projection\n"
			"\tfor July 2008; see the WELD product naming convention for all the possible time periods.\n\n"

			"\"tile_id=h10v10\" means compositing for tile h10v10.\n\n"

			"\"geometry_flag=true\" means the four geometry SDS will be included in the output re-projection; \n"
			"\t\"geometry_flag=false\" means do not include them.\n\n"

			"\"l1t_index_flag=true\" means that an index SDS the value in which refers to a list of L1T \n"
			"\tacquisitions considered in generating the re-projection. The L1T list is given as a HDF global attribute.\n"
			"\t\"l1t_index_flag=false\" means do not generate the index SDS.\n\n"

			"\"version=v3.0\" mean string \"v1.6\" is used to name the re-projection; see WELD product naming conventions.\n\n"

			"\"acq_list=acq.list.month07.2008.h10v10.txt\" means the input UTM filenames are given in \n"
			"\t\"acq.list.month07.2008.h10v10.txt\". The list is generated by the script \"utm.input.for.re-projection.sh\";\n"
			"\tsee the documentation for \"utm.input.for.re-projection.sh\" for the format of the input acquisition list.\n\n"

			"\"as_pointers=false\" means a re-projection will be created as a TOA product with complete list of sds's. \n"
			"if set to true the tile will ve limited number of sds's only needed to generate next level of product (SRF/NBAR) \n\n"

			"\"out_dir=/month01.2008\" means the re-projection will be saved in directory \"month07.2008\".\n"
			"\tAn output re-projection filename cannot be directly given on the command line as the re-projection\n"
			"\tfilename depends the day of year range.\n\n"
			);

	fprintf(stderr, "NOTE: a companion tile for each re-projection file will also be created in the output directory, with filename \n"
			"\tstarting with \"alldoy.\" (It is used internally to remove the same-day within-path UTM data redundancy.\n"
			"\tDo not remove it if the re-projection is planned to be updated with new acquisitions later.)\n\n"
			);

}

