/*
 * sds.name.def.h
#ifndef SDS_NAME_DEF_H_
 *
 * 	Purpose: to keep all sds name definitions in one file
 * 			 and control those from one location
 *
 *  upd: Jun 12, 2013 [VAL]
 *  	 Made flexible with versions of TOA
 *
 *  Created on: Jun 12, 2013
 *      Author: kovalskyyv
 *
 */



#ifndef SDS_NAME_DEF_H_
#define SDS_NAME_DEF_H_


/*  Input acquistion*/
#define SDS_NAME_LEN 64


#ifndef PRODUCT_AND_PGE_VERSIONS

	#define PRODUCT_VERSION_AN  ("PRODUCT_VERSION")
	#define PGE_VERSION_AN  ("PGE_VERSION")

	/*********************************************************
	 * PRODUCT_VERSION shold be the same for all
	 *********************************************************/
	#define PRODUCT_VERSION  ("3.0")

	/* PGE_VERSIONS ARE DIFFERRENT */
	#define PGE_VERSION_TOA  ("3.0")
	#define PGE_VERSION_ACCA  ("2.0")
	#define PGE_VERSION_DTCL  ("2.0")
	#define PGE_VERSION_GEOM  ("1.5")
	/*near real time compositing */
    #define PGE_VERSION_NRT  ("3.0")
	/*seasonal compositing */
	#define PGE_VERSION_2PASS  ("3.0")
	/*SWAPPING SRF */
	#define PGE_VERSION_SRF  ("3.0")

#endif



#ifndef L7_KEY
#define L7_KEY  ("LE7")
#endif
#ifndef L4_KEY
#define L4_KEY  ("LT4")
#endif

#ifndef TOA_SDS_NAMES
#define TOA_SDS_NAMES
	/*  Used in compositing UTM acquisitions  only */
	#define INPUT_UTM_ZONE_NUMBER         ("UTM_ZONE_NUMBER")
	#define INPUT_PRODUCT_UL_CORNER_MAPX  ("PRODUCT_UL_CORNER_MAPX")
	#define INPUT_PRODUCT_UL_CORNER_MAPY  ("PRODUCT_UL_CORNER_MAPY")
	#define INPUT_GRID_CELL_SIZE          ("GRID_CELL_SIZE")
	#define INPUT_ACQUISITION_DATE        ("ACQUISITION_DATE")
	#define INPUT_CENTER_SZ		      ("SolarZenith")		/* scene center */
	#define INPUT_CENTER_SA		      ("SolarAzimuth")		/* scene center */
	/* FOR ACQUISITIONS AND FOR TILES */
	#define    B01_SDS_NAME      ("Band1_TOA_REF")
	#define    B02_SDS_NAME      ("Band2_TOA_REF")
	#define    B03_SDS_NAME      ("Band3_TOA_REF")
	#define    B04_SDS_NAME      ("Band4_TOA_REF")
	#define    B05_SDS_NAME      ("Band5_TOA_REF")
	#define    B61_SDS_NAME      ("Band61_TOA_BT")
	#define    B62_SDS_NAME      ("Band62_TOA_BT")
	#define    B07_SDS_NAME      ("Band7_TOA_REF")
	#define   NDVI_SDS_NAME      ("NDVI_TOA")
	#define    DOY_SDS_NAME      ("Day_Of_Year")
	#define    SAT_SDS_NAME      ("Saturation_Flag")
	#define  DTCLD_SDS_NAME      ("DT_Cloud_State")
	#define   ACCA_SDS_NAME      ("ACCA_State")
	#define   NOBS_SDS_NAME      ("Num_Of_Obs")
	#define  CPATH_SDS_NAME      ("Composite_Path")
    #define  SENRS_SDS_NAME      ("Sensor")

	#define     VZ_SDS_NAME      ("Sensor_Zenith")
	#define     VA_SDS_NAME      ("Sensor_Azimuth")
	#define     SZ_SDS_NAME      ("Solar_Zenith")
	#define     SA_SDS_NAME      ("Solar_Azimuth")
	#define     NBAR_SZ_SDS_NAME ("NBAR_Solar_Zenith")
	#define L1T_INDEX_SDS_NAME   ("L1T_Index")
	#define L1T_COL_SDS_NAME   ("L1T_Column")
	#define L1T_ROW_SDS_NAME   ("L1T_Row")
	/* FOR ALL DOY COMPOSUTING */
	#define ALLDOY1     ("All_Day_Of_Year_part1")
	#define ALLDOY2     ("All_Day_Of_Year_part2")
	#define ALLDOY_27   ("All_Day_Of_Year")
#endif

#ifndef SRF_SDS_NAMES
#define SRF_SDS_NAMES
	#define    SRF_ACQ_KEY ("_SRF")
	/* FOR ACQUISITIONS  */
	#define    AB01_SRF_SDS_NAME      ("band1")
	#define    AB02_SRF_SDS_NAME      ("band2")
	#define    AB03_SRF_SDS_NAME      ("band3")
	#define    AB04_SRF_SDS_NAME      ("band4")
	#define    AB05_SRF_SDS_NAME      ("band5")
	#define    AB07_SRF_SDS_NAME      ("band7")
	/*AND FOR TILES*/
	#define    B01_SRF_SDS_NAME      ("Band1_SRF_REF")
	#define    B02_SRF_SDS_NAME      ("Band2_SRF_REF")
	#define    B03_SRF_SDS_NAME      ("Band3_SRF_REF")
	#define    B04_SRF_SDS_NAME      ("Band4_SRF_REF")
	#define    B05_SRF_SDS_NAME      ("Band5_SRF_REF")
	#define    B07_SRF_SDS_NAME      ("Band7_SRF_REF")
	#define   NDVI_SRF_SDS_NAME      ("NDVI_SRF")
#endif


#ifndef COMP_ATTR_NAMES
#define COMP_ATTR_NAMES
	#define L1T_METADATA ("L1T_Index_Metadata")
    #define INP_POINETR ("INPUT_POINTER")
	#define VALID_RANGE ("valid_range")
#endif

#ifndef ACQ_FNAME_KEYS
#define ACQ_FNAME_KEYS
	#define TOA_VERSION     ("v3.0")
	#define    ACQ_KEY      ("_TOA")
	#define    GEOMKEY      ("_GEOM")
	#define    ACCAKEY      ("_ACCA")
	#define    DTCLKEY      ("_DTCLD")
#endif

#endif /* SDS_NAME_DEF_H_ */
