/*  
	The map coordinate system related functions used by various WELD modules. 
	They include the setup of the projection system, adding the corner 
	coordinates of a WELD tile, and map projection conversion.

	Functions for set up both v1.5 and v2.0 coordinate systems are included.


    	Junchang Ju 04/26/2011


*/


#include "weld.projection.h"


/*********************************************************************************
	Setup the Albers projection parameters for the given region.
	For the coordinate system used for WELD products up to v1.5.
	
*********************************************************************************/
int setup_weld_v1point5_proj_para(albers_proj_para_t *albers, char *region)
{
    char status = 0;
    /*  The lat/lon is in DDDMMMSSS.SSS form*/
    if (strcmp(region, "CONUS") == 0) {
        albers->first_lat   =  29030000;
        albers->second_lat  =  45030000;
        albers->central_lon = -96000000;
        albers->origin_lat  =  23000000;

        albers->Albers_ULX = -2565600.0;
        albers->Albers_ULY =  3314800.0;
    }
    else if (strcmp(region, "Alaska") == 0) {
        albers->first_lat   =   55000000;
        albers->second_lat  =   65000000;
        albers->central_lon = -154000000;
        albers->origin_lat  =   50000000;

        albers->Albers_ULX =  -851700.0;
        albers->Albers_ULY =  2474350.0;

    }
    else {
        status = 1;
        Error("The region must be CONUS|Alaska", "setup_weld_v1point5_proj_para", status);
        goto CLEANUP;
    }

    CLEANUP:

    return status;
}

/*********************************************************************************
	Setup the Albers projection parameters for the given region.

	For the coordinate system used for WELD products after v1.5. The projection
	parameters remain the same as prior to v1.5, but the UL corner coordinates
	in v2.0 are shifted less than 30 meters to line up with NLCD pixels. 

	The UL corner coordinates of the Albers system have been modified for 
	CONUS and Alaska so that the Albers origin coordinates (0,0) corresponds
	to a 30-m pixel center. This change is neccessary for WELD 30-m pixels align
	with the NLCD 30-meter pixels; WELD and NLCD use the same projection
	and exactly the same projection parameters.  Once the modification works
	for the 30-m pixels, it will work for the 60-m L7 or 120-m L5 thermal bands
	as the thermal bands are resampled by EROS to 30 meters.

	The WELD product from v1.6 on uses this new coordinate system, and the next major
	release is v2.0 and so call this version of coordinate v2.0.
**********************************************************************************/
int setup_weld_v1point6_proj_para(albers_proj_para_t *albers, char *region)
{
    char status = 0;
    /*  The lat/lon is in DDDMMMSSS.SSS form*/
    if (strcmp(region, "CONUS") == 0) {
        albers->first_lat   =  29030000;
        albers->second_lat  =  45030000;
        albers->central_lon = -96000000;
        albers->origin_lat  =  23000000;

        albers->Albers_ULX = -2565585.0;
        albers->Albers_ULY =  3314805.0;
    }
    else if (strcmp(region, "Alaska") == 0) {
        albers->first_lat   =   55000000;
        albers->second_lat  =   65000000;
        albers->central_lon = -154000000;
        albers->origin_lat  =   50000000;

        albers->Albers_ULX =  -851715.0;
        albers->Albers_ULY =  2474355.0;

    }
    else {
        status = 1;
        Error("The region must be CONUS|Alaska", "setup_weld_v1point6_proj_para", status);
        goto CLEANUP;
    }


    CLEANUP:

    return status;
}


/*********************************************************************************
    Setup the Sinusoidal projection parameters for the given region.
*/
int setup_sinus_proj_para(sinus_proj_para_t *sinus, char *region)
{
    char status = 1;
    if (strcmp(region, "WORLD") == 0) {
        sinus->radius = 6371007.181;
        sinus->centmer = 0;
        /*  The ULX and ULY are derived from the above parameter
            ULX = radius * (-pi)
            ULY = radius * (pi/2)

            pi = 3.141592653589793238462643383279502884197
        */
        sinus->sinus_ULX = -20015109.3557974174618721;
        sinus->sinus_ULY =  10007554.6778987087309361;
        sinus->modis_tile_size = 1111950.5197665231923262;

        status = 0;
    }

    return status ;
}



/********************************************************************************
	VERSION 1.5;1.6
	Add the lat/lon and Albers coordinates of the center of the four corner pixels
	as global attributes. Also add tile ID h??v?? as a global attributes.

	But "ncdump" cannot show the metadata because "ncdump" is only for general hdf.

********************************************************************************/

int add_corner_coordinate(tile_info_t *tile, albers_proj_para_t albers_para)
{
	int tile_h, tile_v;
	char tileid[128];
	int status;

	float64 ULX, ULY;
	float64 URX, URY;
	float64 LLX, LLY;
	float64 LRX, LRY;

	float64 ULlon, ULlat;
	float64 URlon, URlat;
	float64 LLlon, LLlat;
	float64 LRlon, LRlat;
	
	char *cULX   = "UL corner of UL pixel Albers X";
	char *cULY   = "UL corner of UL pixel Albers Y";
	char *cULlon = "UL corner of UL pixel longitude";
	char *cULlat = "UL corner of UL pixel latitude";

	char *cURX   = "UR corner of UR pixel Albers X";
	char *cURY   = "UR corner of UR pixel Albers Y";
	char *cURlon = "UR corner of UR pixel longitude";
	char *cURlat = "UR corner of UR pixel latitude";

	char *cLLX   = "LL corner of LL pixel Albers X";
	char *cLLY   = "LL corner of LL pixel Albers Y";
	char *cLLlon = "LL corner of LL pixel longitude";
	char *cLLlat = "LL corner of LL pixel latitude";

	char *cLRX   = "LR corner of LR pixel Albers X";
	char *cLRY   = "LR corner of LR pixel Albers Y";
	char *cLRlon = "LR corner of LR pixel longitude";
	char *cLRlat = "LR corner of LR pixel latitude";

	tile_h = tile->tile_h;
	tile_v = tile->tile_v;
   
	status = 0;
	/*  tile id */
	sprintf(tileid, "h%02dv%02d", tile_h, tile_v);
	if (SDsetattr(tile->sd_id,   "TileID", DFNT_CHAR8, strlen(tileid), (VOIDP)tileid) == FAIL) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	/*  pixel size*/
	if (SDsetattr(tile->sd_id,   "Pixel Size", DFNT_FLOAT32, 1, (VOIDP)&tile->pixel_size) == FAIL) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	/* UL x,y*/
	ULX = albers_para.Albers_ULX + (tile_h * TILEDIM) * REF_PIXSIZE;	
	ULY = albers_para.Albers_ULY - (tile_v * TILEDIM) * REF_PIXSIZE;   
	albers2latlon(albers_para,ULX, ULY, &ULlon, &ULlat);
	if ((SDsetattr(tile->sd_id,   cULX,   DFNT_FLOAT64, 1, (VOIDP)&ULX) == FAIL) || 
	    (SDsetattr(tile->sd_id,   cULY,   DFNT_FLOAT64, 1, (VOIDP)&ULY) == FAIL)) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}
   
	/* UR x,y*/
	URX = ULX + TILEDIM * REF_PIXSIZE;  
	URY = ULY; 
	albers2latlon(albers_para,URX, URY, &URlon, &URlat);
	if ((SDsetattr(tile->sd_id,   cURX,   DFNT_FLOAT64, 1, (VOIDP)&URX) == FAIL) || 
	    (SDsetattr(tile->sd_id,   cURY,   DFNT_FLOAT64, 1, (VOIDP)&URY) == FAIL)) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}
   
	/*  LL x, y*/
	LLX = ULX; 
	LLY = ULY - TILEDIM * REF_PIXSIZE; 
	albers2latlon(albers_para,LLX, LLY, &LLlon, &LLlat);
	if ((SDsetattr(tile->sd_id,   cLLX,   DFNT_FLOAT64, 1, (VOIDP)&LLX) == FAIL) || 
	    (SDsetattr(tile->sd_id,   cLLY,   DFNT_FLOAT64, 1, (VOIDP)&LLY) == FAIL)) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}
   
	/* LR x, y */
	LRX = URX; 
	LRY = LLY; 
	albers2latlon(albers_para,LRX, LRY, &LRlon, &LRlat);
	if ((SDsetattr(tile->sd_id,   cLRX,   DFNT_FLOAT64, 1, (VOIDP)&LRX) == FAIL) || 
	    (SDsetattr(tile->sd_id,   cLRY,   DFNT_FLOAT64, 1, (VOIDP)&LRY) == FAIL)) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	/*  UL lon lat*/
	if ((SDsetattr(tile->sd_id, cULlon, DFNT_FLOAT64, 1, (VOIDP)&ULlon) == FAIL) ||
	    (SDsetattr(tile->sd_id, cULlat, DFNT_FLOAT64, 1, (VOIDP)&ULlat) == FAIL)) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	/* UR lon lat*/
	if ((SDsetattr(tile->sd_id, cURlon, DFNT_FLOAT64, 1, (VOIDP)&URlon) == FAIL) ||
	    (SDsetattr(tile->sd_id, cURlat, DFNT_FLOAT64, 1, (VOIDP)&URlat) == FAIL)) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	/* LL lon lat*/
	if ((SDsetattr(tile->sd_id, cLLlon, DFNT_FLOAT64, 1, (VOIDP)&LLlon) == FAIL) ||
	    (SDsetattr(tile->sd_id, cLLlat, DFNT_FLOAT64, 1, (VOIDP)&LLlat) == FAIL)) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	/* LR lon lat*/
	if ((SDsetattr(tile->sd_id, cLRlon, DFNT_FLOAT64, 1, (VOIDP)&LRlon) == FAIL) ||
	    (SDsetattr(tile->sd_id, cLRlat, DFNT_FLOAT64, 1, (VOIDP)&LRlat) == FAIL)) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	return 0;
}



/********************************************************************************
	VERSION 2.0  Add the lat/lon and Albers coordinates of the center of the four corner pixels
	as global attributes. Also add tile ID h??v?? as a global attributes.

	But "ncdump" cannot show the metadata because "ncdump" is only for general hdf.

********************************************************************************/

int add_corner_coordinates_2_0(int32 tile_sd_id, int tile_h, int tile_v, float32 pixel_size, albers_proj_para_t albers_para )
{

	char tileid[128];
	int status;

	float64 ULX, ULY;
	float64 URX, URY;
	float64 LLX, LLY;
	float64 LRX, LRY;

	float64 ULlon, ULlat;
	float64 URlon, URlat;
	float64 LLlon, LLlat;
	float64 LRlon, LRlat;

	char *cULX   = "UL corner of UL pixel Albers X";
	char *cULY   = "UL corner of UL pixel Albers Y";
	char *cULlon = "UL corner of UL pixel longitude";
	char *cULlat = "UL corner of UL pixel latitude";

	char *cURX   = "UR corner of UR pixel Albers X";
	char *cURY   = "UR corner of UR pixel Albers Y";
	char *cURlon = "UR corner of UR pixel longitude";
	char *cURlat = "UR corner of UR pixel latitude";

	char *cLLX   = "LL corner of LL pixel Albers X";
	char *cLLY   = "LL corner of LL pixel Albers Y";
	char *cLLlon = "LL corner of LL pixel longitude";
	char *cLLlat = "LL corner of LL pixel latitude";

	char *cLRX   = "LR corner of LR pixel Albers X";
	char *cLRY   = "LR corner of LR pixel Albers Y";
	char *cLRlon = "LR corner of LR pixel longitude";
	char *cLRlat = "LR corner of LR pixel latitude";


	status = 0;
	/*  tile id */
	sprintf(tileid, "h%02dv%02d", tile_h, tile_v);
	if (SDsetattr(tile_sd_id,   "TileID", DFNT_CHAR8, strlen(tileid), (VOIDP)tileid) == FAIL) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	/*  pixel size*/
	if (SDsetattr(tile_sd_id,   "Pixel Size", DFNT_FLOAT32, 1, (VOIDP)&pixel_size) == FAIL) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	/* UL x,y*/
	ULX = albers_para.Albers_ULX + (tile_h * TILEDIM) * (float64)(pixel_size);
	ULY = albers_para.Albers_ULY - (tile_v * TILEDIM) * (float64)(pixel_size);
	albers2latlon(albers_para,ULX, ULY, &ULlon, &ULlat);
	if ((SDsetattr(tile_sd_id,   cULX,   DFNT_FLOAT64, 1, (VOIDP)&ULX) == FAIL) ||
	    (SDsetattr(tile_sd_id,   cULY,   DFNT_FLOAT64, 1, (VOIDP)&ULY) == FAIL)) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	/* UR x,y*/
	URX = ULX + TILEDIM * (float64)(pixel_size);
	URY = ULY;
	albers2latlon(albers_para,URX, URY, &URlon, &URlat);
	if ((SDsetattr(tile_sd_id,   cURX,   DFNT_FLOAT64, 1, (VOIDP)&URX) == FAIL) ||
	    (SDsetattr(tile_sd_id,   cURY,   DFNT_FLOAT64, 1, (VOIDP)&URY) == FAIL)) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	/*  LL x, y*/
	LLX = ULX;
	LLY = ULY - TILEDIM * (float64)(pixel_size);
	albers2latlon(albers_para,LLX, LLY, &LLlon, &LLlat);
	if ((SDsetattr(tile_sd_id,   cLLX,   DFNT_FLOAT64, 1, (VOIDP)&LLX) == FAIL) ||
	    (SDsetattr(tile_sd_id,   cLLY,   DFNT_FLOAT64, 1, (VOIDP)&LLY) == FAIL)) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	/* LR x, y */
	LRX = URX;
	LRY = LLY;
	albers2latlon(albers_para,LRX, LRY, &LRlon, &LRlat);
	if ((SDsetattr(tile_sd_id,   cLRX,   DFNT_FLOAT64, 1, (VOIDP)&LRX) == FAIL) ||
	    (SDsetattr(tile_sd_id,   cLRY,   DFNT_FLOAT64, 1, (VOIDP)&LRY) == FAIL)) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	/*  UL lon lat*/
	if ((SDsetattr(tile_sd_id, cULlon, DFNT_FLOAT64, 1, (VOIDP)&ULlon) == FAIL) ||
	    (SDsetattr(tile_sd_id, cULlat, DFNT_FLOAT64, 1, (VOIDP)&ULlat) == FAIL)) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	/* UR lon lat*/
	if ((SDsetattr(tile_sd_id, cURlon, DFNT_FLOAT64, 1, (VOIDP)&URlon) == FAIL) ||
	    (SDsetattr(tile_sd_id, cURlat, DFNT_FLOAT64, 1, (VOIDP)&URlat) == FAIL)) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	/* LL lon lat*/
	if ((SDsetattr(tile_sd_id, cLLlon, DFNT_FLOAT64, 1, (VOIDP)&LLlon) == FAIL) ||
	    (SDsetattr(tile_sd_id, cLLlat, DFNT_FLOAT64, 1, (VOIDP)&LLlat) == FAIL)) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	/* LR lon lat*/
	if ((SDsetattr(tile_sd_id, cLRlon, DFNT_FLOAT64, 1, (VOIDP)&LRlon) == FAIL) ||
	    (SDsetattr(tile_sd_id, cLRlat, DFNT_FLOAT64, 1, (VOIDP)&LRlat) == FAIL)) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

	return 0;
}


/********************************************************************************
    Add sinusoidal coordinates of the four corner pixels as global attributes.
    These coordinates define the bounding box of the tile (so not the centers
    of the corner pixels).
    It is impossible to add the coorner lat/lon because of the edge tiles.

    Also add tile ID h??v?? as a global attributes. USELESS???

*/
/********************************************************************************/

int world_add_corner_coordinates(int32 tile_sd_id, int tile_h, int tile_v, float32 pixel_size)
{
	int status=0;


    char tileid[30];

    float64 sinus_UL_X, sinus_UL_Y;
    float64 sinus_UR_X, sinus_UR_Y;
    float64 sinus_LL_X, sinus_LL_Y;
    float64 sinus_LR_X, sinus_LR_Y;

    int modis_tile_h, modis_tile_v;     /*  The modis tile ID,  0-35, and 0-17 */
    int mini_tile_h, mini_tile_v;       /*  The mini tiles nested within a modis tile,  0-6, 0-6*/
	sinus_proj_para_t sinus_para;
	setup_sinus_proj_para(&sinus_para, "WORLD");
    modis_tile_h = tile_h / NMINI;
    modis_tile_v = tile_v / NMINI;
    mini_tile_h  = tile_h % NMINI;
    mini_tile_v  = tile_v % NMINI;

   sprintf(tileid, "hh%02dvv%02d.h%dv%d", modis_tile_h, modis_tile_v,  mini_tile_h, mini_tile_v );


	/*  pixel size*/
	if (SDsetattr(tile_sd_id,   "Pixel Size", DFNT_FLOAT32, 1, (VOIDP)&pixel_size) == FAIL) {
		status = 1;
		Error("SDsetattr failed", "add_corner_coordinate", status);
		return status;
	}

    /*Upper Left*/
    sinus_UL_X = sinus_para.sinus_ULX + modis_tile_h * sinus_para.modis_tile_size +
                                       (mini_tile_h  * TILEDIM) * (float64)(pixel_size);

    sinus_UL_Y = sinus_para.sinus_ULY - modis_tile_v * sinus_para.modis_tile_size -
                                       (mini_tile_v  * TILEDIM) * (float64)(pixel_size);



    if ((SDsetattr(tile_sd_id,   "sinus_UL_X",  DFNT_FLOAT64, 1, (VOIDP)&sinus_UL_X) == FAIL) ||
        (SDsetattr(tile_sd_id,   "sinus_UL_Y",  DFNT_FLOAT64, 1, (VOIDP)&sinus_UL_Y) == FAIL)) {
    	status=1;
    	Error("SDsetattr failed", "world_add_corner_coordinate", status);
    	return status;
    }

    /*Upper Right*/
    sinus_UR_X = sinus_UL_X + TILEDIM * (float64)(pixel_size);
    sinus_UR_Y = sinus_UL_Y;
    if ((SDsetattr(tile_sd_id,   "sinus_UR_X",  DFNT_FLOAT64, 1, (VOIDP)&sinus_UR_X) == FAIL) ||
        (SDsetattr(tile_sd_id,   "sinus_UR_Y",  DFNT_FLOAT64, 1, (VOIDP)&sinus_UR_Y) == FAIL)) {
    	status=1;
    	Error("SDsetattr failed", "world_add_corner_coordinate", status);
    	return status;
    }

    /*Lower Left*/
    sinus_LL_X = sinus_UL_X;
    sinus_LL_Y = sinus_UL_Y - TILEDIM * (float64)(pixel_size);
    if ((SDsetattr(tile_sd_id,   "sinus_LL_X",  DFNT_FLOAT64, 1, (VOIDP)&sinus_LL_X) == FAIL) ||
        (SDsetattr(tile_sd_id,   "sinus_LL_Y",  DFNT_FLOAT64, 1, (VOIDP)&sinus_LL_Y) == FAIL)) {
    	status=1;
    	Error("SDsetattr failed", "world_add_corner_coordinate", status);
    	return status;
    }

    /*Lower Right*/
    sinus_LR_X = sinus_UR_X;
    sinus_LR_Y = sinus_LL_Y;
    if ((SDsetattr(tile_sd_id,   "sinus_LR_X",  DFNT_FLOAT64, 1, (VOIDP)&sinus_LR_X) == FAIL) ||
        (SDsetattr(tile_sd_id,   "sinus_LR_Y",  DFNT_FLOAT64, 1, (VOIDP)&sinus_LR_Y) == FAIL)) {
    	status=1;
    	Error("SDsetattr failed", "world_add_corner_coordinate", status);
    	return status;
    }

    if (SDsetattr(tile_sd_id,   "TileID", DFNT_CHAR8, strlen(tileid), (VOIDP)&tileid) == FAIL) {
    	status=1;
    	Error("SDsetattr failed", "world_add_corner_coordinate", status);
    	return status;
    }

    return 0;
}


/*********************************************************************************
NAME:     albers2latlon()

PURPOSE:  For the given Albers coordinate (x,y) find the corresponding latitude and longitude.

Args:    albers_para-  Albers projection parameters 
         ax-     input albers easting (x)
         ay-     input albers northing (y)
         lon-    output longitude
         lat-    output latitude
*********************************************************************************/
void albers2latlon(albers_proj_para_t albers_para, double ax, double ay, double* lon, double* lat)
{
	 /* INPUT PROJ PARAMS */
	 double incoor[2];       //input Albers coordinates to be passed
	 long insys = 3;         //Albers Conic Equal Area
	 long inzone = -1;       //Ignored for Albers
	 double inparm[15];      //parameters for Albers
	 long inunit = 2;        //meters
	 long indatum = 12;      //WGS 84

	 /* OUTPUT PROJ PARAMS */
	 double outcoor[2];      //output coordinates to be received: lon and lat. NOTE the order
	 long outsys = 0;        //Geographic
	 long outzone = -1;      //Ignored for Albers
	 double outparm[15];     //parameters Lat Lon
	 long outunit = 4;       //Degrees of arc
	 long outdatum = 12;     //WGS 84

	 /*Error redirection*/
	 long iflg;                            /* error number*/
	 long ipr = 2;                         /* error message print to both terminal and file */
	 char efile[] = "gctp_error_msg.txt";  /* name of error file   (required by gctp)   */

	 /*Projection parameter redirection*/
	 long jpr = -1;                       /*don't print the projection params*/
	 char pfile[] = "gctp_proj_para.txt";   /*file to print the returned projection parameters*/

	 /* other GCTP parameters. Not used here */
	 char fn27[] = " ";    /* file contaiming  nad27 parameters.  requered by gctp  */
	 char fn83[] = " ";    /* file contaiming  nad27 parameters.  requered by gctp  */

	 /*Initialising parameters of both projections all to zeros first. No further initialized for UTM.*/
	 /*They have to be initialized this way. A bit odd! */
	 int k;
	 for (k = 0; k < 15; k++){
		inparm[k]=0.0;
		outparm[k]=0.0;
	 }

	 /* Standard parameters for Albers conus*/
	 inparm[0] = 0.0;         //major elipsoid axis a (standard for the datum)
	 inparm[1] = 0.0;         //minor elipsoid axis b (standard for the datum)
	 inparm[2] = albers_para.first_lat;     //1st standard paralel
	 inparm[3] = albers_para.second_lat;    //2nd standard paralel
	 inparm[4] = albers_para.central_lon;   //central meridian
	 inparm[5] = albers_para.origin_lat;    //latitude of origin


	 /* Finally, Call GCTP */
	 incoor[0]= ax;
	 incoor[1]= ay;

	 gctp(incoor, &insys, &inzone, inparm, &inunit, &indatum,
		  &ipr, efile, &jpr, pfile,
		  outcoor, &outsys, &outzone, outparm, &outunit, &outdatum,
		  fn27, fn83,&iflg);

	 *lon=outcoor[0];
	 *lat=outcoor[1];
}


/**************************************************************************************
	WELD albers to MODIS sinusoidal.

NAME:     albers2sinusoidal()

PURPOSE:  For the given Albers coordinate (x,y) find the corresponding sinusoidal x, y

Args:    albers_para-  Albers projection parameters 
         ax-     input albers easting (x)
         ay-     input albers northing (y)
         sx-    output sinusoidal x
         sy-    output sinusoidal y
**************************************************************************************/
void albers2sinusoidal(albers_proj_para_t albers_para, double ax, double ay, double *sx, double *sy)
{
	/* INPUT PROJ PARAMS */
	double incoor[2];       //input Albers coordinates to be passed
	long insys = 3;         //Albers Conic Equal Area
	long inzone = -1;       //Ignored for Albers
	double inparm[15];      //parameters for Albers
	long inunit = 2;        //meters
	long indatum = 12;      //WGS 84

	/* OUTPUT PROJ PARAMS */
	double outcoor[2];     //output coordinates to be returned
	long outsys = 16;      //Sinusoidal Equal Area
	long outzone = -1;     //Not required
	double outparm[15];
	long outunit = 2;      //meters
	long outdatum = 12;    //WGS 84


	/*Error redirection*/
	long iflg;                            /* error number*/
	long ipr = 2;                         /* error message print to both terminal and file */
	char efile[] = "gctp_error_msg.txt";  /* name of error file   (required by gctp)   */
	
	/*Projection parameter redirection*/
	long jpr = -1;                         /*don't print the projection params*/
	char pfile[] = "gctp_proj_para.txt";   /*file to print the returned projection parameters to*/
	
	/* other GCTP parameters. Not used here */
	char fn27[] = " ";    /* file contaiming  nad27 parameters.  requered by gctp  */
	char fn83[] = " ";    /* file contaiming  nad27 parameters.  requered by gctp  */
	
	//initialising parameters of both projections all to zeros first (enough for UTM)
	int k;
	for (k = 0; k < 15; k++){
	       inparm[k]=0.0;
	       outparm[k]=0.0;
	}
	

 	/* Standard parameters for WELD Albers */
 	inparm[0] = 0.0;         //major elipsoid axis a (standard for the datum)
 	inparm[1] = 0.0;         //minor elipsoid axis b (standard for the datum)
 	inparm[2] = albers_para.first_lat;     //1st standard paralel
 	inparm[3] = albers_para.second_lat;    //2nd standard paralel
 	inparm[4] = albers_para.central_lon;   //central meridian
 	inparm[5] = albers_para.origin_lat;    //latitude of origin

	/* Standard parameters for modis sinusoidal*/
	outparm[0]=6371007.181;        //major elipsoid axis a (standard for the datum)
	outparm[1]=0.0;        //minor elipsoid axis b (standard for the datum)
	outparm[2] = 0;           //lst standard paralel
	outparm[4] = 0;           //central meridian
	
	incoor[0]= ax;
	incoor[1]= ay;
	
	/* Call GCTP -----------------*/
	gctp(incoor, &insys, &inzone, inparm, &inunit, &indatum,
	     &ipr, efile, &jpr, pfile,
	     outcoor, &outsys, &outzone, outparm, &outunit, &outdatum,
	     fn27, fn83,&iflg);
	
	*sx=outcoor[0];
	*sy=outcoor[1];
}

/*********************************************************************************
NAME:     albers2utm()

PURPOSE:  For the given Albers coordinate (x,y) find the corresponding UTM coordinates
          in the specified UTM zone.  

Args: 
         albers_para - Albers projection parameters for this region.
         ax-     input albers easting (x)
         ay-     input albers northing (y)
         uzone-  output utm zone
         ux-     output utm easting
         uy-     output utm northing
*********************************************************************************/
void albers2utm(albers_proj_para_t albers_para, double ax, double ay, int utm_zone, double* ux, double* uy)
{
	 /* INPUT PROJ PARAMS */
	 double incoor[2];       //input Albers coordinates to be passed
	 long insys = 3;         //Albers Conic Equal Area
	 long inzone = -1;       //Ignored for Albers
	 double inparm[15];      //parameters for Albers
	 long inunit = 2;        //meters
	 long indatum = 12;      //WGS 84

	 /* OUTPUT PROJ PARAMS */
	 double outcoor[2];      //output coordinates to be received
	 long outsys = 1;        //UTM
	 long outzone;           //UTM ZONE number. To be passed in.
	 double outparm[15];     //parameters for UTM
	 long outunit = 2;       //meters
	 long outdatum = 12;     //WGS 84

	 /*Error redirection*/
	 long iflg;                            /* error number*/
	 long ipr = 2;                         /* error message print to both terminal and file */
	 char efile[] = "gctp_error_msg.txt";  /* name of error file   (required by gctp)   */

	 /*Projection parameter redirection*/
	 long jpr = -1;                       /*don't print the projection params*/
	 char pfile[] = "gctp_proj_para.txt";   /*file to print the returned projection parameters*/

	 /* other GCTP parameters. Not used here */
	 char fn27[] = " ";    /* file contaiming  nad27 parameters.  requered by gctp  */
	 char fn83[] = " ";    /* file contaiming  nad27 parameters.  requered by gctp  */

	 /*Initialising parameters of both projections all to zeros first. No further initialized for UTM.*/
	 /*They have to be initialized this way. A bit odd! */
	 int k;
	 for (k = 0; k < 15; k++){
		inparm[k]=0.0;
		outparm[k]=0.0;
	 }

	 /* Standard parameters for Albers for this region*/
	 inparm[0] = 0.0;         //major elipsoid axis a (standard for the datum)
	 inparm[1] = 0.0;         //minor elipsoid axis b (standard for the datum)
	 inparm[2] = albers_para.first_lat;     //1st standard paralel
	 inparm[3] = albers_para.second_lat;    //2nd standard paralel
	 inparm[4] = albers_para.central_lon;   //central meridian
	 inparm[5] = albers_para.origin_lat;    //latitude of origin


	 /* Finally, Call GCTP */
	 incoor[0]= ax;
	 incoor[1]= ay;
	 outzone = utm_zone;

	 gctp(incoor, &insys, &inzone, inparm, &inunit, &indatum,
		  &ipr, efile, &jpr, pfile,
		  outcoor, &outsys, &outzone, outparm, &outunit, &outdatum,
		  fn27, fn83,&iflg);

	 *ux=outcoor[0];
	 *uy=outcoor[1];
}

/*********************************************************************************
NAME:       utm2albers()
Args:       uzone-  utm zone
            ux-     utm easting
            uy- utm northing
            albers_para - Albers projection parameters
            ax-     albers (conus) easting to return
            ay- albers (conus) northing to return
Calls:      main

*********************************************************************************/
void utm2albers(int utm_zone, double ux, double uy, albers_proj_para_t albers_para,  double* ax, double* ay)
{
	 /* INPUT PROJ PARAMS */
	 double incoor[2]; 	//input coordinates to be passed
	 long insys = 1; 	//UTM
	 long inzone;		//UTM ZONE
	 double inparm[15];	//parameters for UTM are ignored by gctp
	 long inunit = 2;	//meters
	 long indatum = 12;	//WGS 84


	 /* OUTPUT PROJ PARAMS */
	 double outcoor[2];	//output coordinates to be recieved
	 long outsys = 3;	//Alber Conic Equal Area
	 long outzone = -1;     //Not required
	 double outparm[15];
	 long outunit = 2;      //meters
	 long outdatum = 12;    //WGS 84


	 /*Error redirection*/
	 long iflg;                            /* error number*/
	 long ipr = 2;                         /* error message print to both terminal and file */
	 char efile[] = "gctp_error_msg.txt";  /* name of error file   (required by gctp)   */

	 /*Projection parameter redirection*/
	 long jpr = -1;                         /*don't print the projection params*/
	 char pfile[] = "gctp_proj_para.txt";   /*file to print the returned projection parameters to*/

	 /* other GCTP parameters. Not used here */
	 char fn27[] = " ";    /* file contaiming  nad27 parameters.  requered by gctp  */
	 char fn83[] = " ";    /* file contaiming  nad27 parameters.  requered by gctp  */

	 //initialising parameters of both projections all to zeros first (enough for UTM)
	 int k;
	 for (k = 0; k < 15; k++){
		inparm[k]=0.0;
		outparm[k]=0.0;
	 }

	 /* Standard parameters for this region's Albers projection*/
	 outparm[0]=0.0; 	//major elipsoid axis a (standard for the datum)
	 outparm[1]=0.0; 	//minor elipsoid axis b (standard for the datum)
	 outparm[2]=albers_para.first_lat; 	//1st standard paralel
	 outparm[3]=albers_para.second_lat;	//2nd standard paralel
	 outparm[4]=albers_para.central_lon;	//central meridian
	 outparm[5]=albers_para.origin_lat;	//latitude of origin

	 incoor[0]= ux;
	 incoor[1]= uy;
	 inzone = utm_zone;		//assign zone from parameter

	 /* Call GCTP -----------------*/
	 gctp(incoor, &insys, &inzone, inparm, &inunit, &indatum,
		  &ipr, efile, &jpr, pfile,
		  outcoor, &outsys, &outzone, outparm, &outunit, &outdatum,
		  fn27, fn83,&iflg);

	 *ax=outcoor[0];
	 *ay=outcoor[1];
}


/******************************************************************************************
 latlon2albers
******************************************************************************************/
void latlon2albers(albers_proj_para_t albers_para, double lon, double lat, double *ax, double *ay)
{
	/* INPUT PROJ PARAMS */
	double incoor[2];      //input coordinates to be passed: lon and lat. NOTE the order
	long insys = 0;        //Geographic
	long inzone = -1;      //Ignored for Albers
	double inparm[15];     //parameters Lat Lon
	long inunit = 4;       //Degrees of arc
	long indatum = 12;     //WGS 84
	
	/* OUTPUT PROJ PARAMS */
	double outcoor[2];       //output Albers coordinates to be received.
	long outsys = 3;         //Albers Conic Equal Area
	long outzone = -1;       //Ignored for Albers
	double outparm[15];      //parameters for Albers
	long outunit = 2;        //meters
	long outdatum = 12;      //WGS 84
	
	
	/*Error redirection*/
	long iflg;                            /* error number*/
	long ipr = 2;                         /* error message print to both terminal and file */
	char efile[] = "gctp_error_msg.txt";  /* name of error file   (required by gctp)   */
	
	/*Projection parameter redirection*/
	long jpr = -1;                       /*don't print the projection params*/
	char pfile[] = "gctp_proj_para.txt";   /*file to print the returned projection parameters*/
	
	/* other GCTP parameters. Not used here */
	char fn27[] = " ";    /* file contaiming  nad27 parameters.  requered by gctp  */
	char fn83[] = " ";    /* file contaiming  nad27 parameters.  requered by gctp  */
	
	/*Initialising parameters of both projections all to zeros first. */
	/*They have to be initialized this way. A bit odd! */
	int k;
	for (k = 0; k < 15; k++){
	   inparm[k]=0.0;
	   outparm[k]=0.0;
	}
	
	/* Standard parameters for WELD Albers */
	outparm[0] = 0.0;         //major elipsoid axis a (standard for the datum)
	outparm[1] = 0.0;         //minor elipsoid axis b (standard for the datum)
	outparm[2] = albers_para.first_lat;    //1st standard paralel
	outparm[3] = albers_para.second_lat;    //2nd standard paralel
	outparm[4] = albers_para.central_lon;   //central meridian
	outparm[5] = albers_para.origin_lat;    //latitude of origin
	
	
	/* Finally, Call GCTP */
	incoor[0]= lon;
	incoor[1]= lat;
	
	gctp(incoor, &insys, &inzone, inparm, &inunit, &indatum,
	     &ipr, efile, &jpr, pfile,
	     outcoor, &outsys, &outzone, outparm, &outunit, &outdatum,
	     fn27, fn83,&iflg);
	
	*ax=outcoor[0];
	*ay=outcoor[1];
}



/*******************************************************************************
NAME:       utm2sinus()
Args: 		uzone-	utm zone
			ux- 	utm easting
			uy-	    utm northing
            sinus_para - Sinusoidal projection parameters
			ax- 	sinusoidal (conus) easting to return
			ay-	    sinusoidal (conus) northing to return
Calls:		main

*******************************************************************************/

void utm2sinus(int utm_zone, double ux, double uy, sinus_proj_para_t sinus_para,  double* ax, double* ay)
{
	 /* INPUT PROJ PARAMS */
	 double incoor[2]; 	//input coordinates to be passed
	 long insys = 1; 	//UTM
	 long inzone;		//UTM ZONE
	 double inparm[15];	//parameters for UTM are ignored by gctp
	 long inunit = 2;	//meters
	 long indatum = 12;	//WGS 84


	 /* OUTPUT PROJ PARAMS */
	 double outcoor[2];	//output coordinates to be recieved
	 long outsys = 16;	//Sinusoidal Conic Equal Area
	 long outzone = -1;     //Not required
	 double outparm[15];
	 long outunit = 2;      //meters
	 long outdatum = 12;    //WGS 84. Does not matter


	 /*Error redirection*/
	 long iflg;                            /* error number*/
	 long ipr = 2;                         /* error message print to both terminal and file */
	 char efile[] = "gctp_error_msg.txt";  /* name of error file   (required by gctp)   */

	 /*Projection parameter redirection*/
	 long jpr = -1;                         /*don't print the projection params*/
	 char pfile[] = "gctp_proj_para.txt";   /*file to print the returned projection parameters to*/

	 /* other GCTP parameters. Not used here */
	 char fn27[] = " ";    /* file contaiming  nad27 parameters.  requered by gctp  */
	 char fn83[] = " ";    /* file contaiming  nad27 parameters.  requered by gctp  */

	 //initialising parameters of both projections all to zeros first (enough for UTM)
	 int k;
	 for (k = 0; k < 15; k++){
		inparm[k]=0.0;
		outparm[k]=0.0;
	 }

	 /* Standard parameters for sinusoidal world */
	 outparm[0] = sinus_para.radius;         //major elipsoid axis. Actually a sphere.
	 outparm[4] = sinus_para.centmer;        //central meridian

	 incoor[0]= ux;
	 incoor[1]= uy;
	 inzone = utm_zone;		//assign zone from parameter

	 /* Call GCTP -----------------*/
	 gctp(incoor, &insys, &inzone, inparm, &inunit, &indatum,
		  &ipr, efile, &jpr, pfile,
		  outcoor, &outsys, &outzone, outparm, &outunit, &outdatum,
		  fn27, fn83,&iflg);

	 *ax=outcoor[0];
	 *ay=outcoor[1];
}


/*******************************************************************************
NAME:     sinus2utm()

PURPOSE:  For the given Sinusoidal coordinate (x,y) find the corresponding UTM coordinates
          in the specified UTM zone.

Args:
         sinus_para - Sinusoidal projection parameters for this region.
         ax-     input sinusoidal x
         ay-     input sinusoidal y
         uzone-  output utm zone
         ux-     output utm easting
         uy-     output utm northing
*******************************************************************************/
void sinus2utm(sinus_proj_para_t sinus_para, double ax, double ay, int utm_zone, double* ux, double* uy)
{
	 /* INPUT PROJ PARAMS */
	 double incoor[2];       //input Sinusoidal coordinates to be passed
	 long insys = 16;         //Sinusoidal Conic Equal Area
	 long inzone = -1;       //Ignored.
	 double inparm[15];      //parameters for input system
	 long inunit = 2;        //meters
	 long indatum = 12;      //WGS 84 . Actually does not matter since the use of a sphere

	 /* OUTPUT PROJ PARAMS */
	 double outcoor[2];      //output coordinates to be received
	 long outsys = 1;        //UTM
	 long outzone;           //UTM ZONE number. To be passed in.
	 double outparm[15];     //parameters for UTM
	 long outunit = 2;       //meters
	 long outdatum = 12;     //WGS 84

	 /*Error redirection*/
	 long iflg;                            /* error number*/
	 long ipr = 2;                         /* error message print to both terminal and file */
	 char efile[] = "gctp_error_msg.txt";  /* name of error file   (required by gctp)   */

	 /*Projection parameter redirection*/
	 long jpr = -1;                       /*don't print the projection params*/
	 char pfile[] = "gctp_proj_para.txt";   /*file to print the returned projection parameters*/

	 /* other GCTP parameters. Not used here */
	 char fn27[] = " ";    /* file contaiming  nad27 parameters.  requered by gctp  */
	 char fn83[] = " ";    /* file contaiming  nad27 parameters.  requered by gctp  */

	 /*Initialising parameters of both projections all to zeros first. No further initialized for UTM.*/
	 /*They have to be initialized this way. A bit odd! */
	 int k;
	 for (k = 0; k < 15; k++){
		inparm[k]=0.0;
		outparm[k]=0.0;
	 }

	 /* Standard parameters for sinusoidal world */
	 inparm[0] = sinus_para.radius;         //major elipsoid axis. Actually a sphere.
	 inparm[4] = sinus_para.centmer;        //central meridian


	 /* Finally, Call GCTP */
	 incoor[0]= ax;
	 incoor[1]= ay;
	 outzone = utm_zone;

	 gctp(incoor, &insys, &inzone, inparm, &inunit, &indatum,
		  &ipr, efile, &jpr, pfile,
		  outcoor, &outsys, &outzone, outparm, &outunit, &outdatum,
		  fn27, fn83,&iflg);

	 *ux=outcoor[0];
	 *uy=outcoor[1];
}
