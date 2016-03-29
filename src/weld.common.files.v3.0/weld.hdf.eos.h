#ifndef WELD_HDF_EOS_H
#define WELD_HDF_EOS_H

#include <mfhdf.h>
#include "error.h"
#include "weld.projection.h"
#include "weld.h"
#include "sds.name.def.h"
#include "weld.setup.sds.h"		// defines sds_info_t


#define MYHDF_MAX_NATTR_VAL 40000    /* Originally 3000; too small. Junchang Ju*/
#define NPROJ_PARAM_HDFEOS (13)
#define SPACE_STRUCT_METADATA ("StructMetadata.0")


/* Possible ranges for data types */

#define MYHDF_CHAR8H     (        255  )
#define MYHDF_CHAR8L     (          0  )
#define MYHDF_INT8H      (        127  )
#define MYHDF_INT8L      (       -128  )
#define MYHDF_UINT8H     (        255  )
#define MYHDF_UINT8L     (          0  )
#define MYHDF_INT16H     (      32767  )
#define MYHDF_INT16L     (     -32768  )
#define MYHDF_UINT16H    (      65535u )
#define MYHDF_UINT16L    (          0u )
#define MYHDF_INT32H     ( 2147483647l )
#define MYHDF_INT32L     (-2147483647l )
#define MYHDF_UINT32H    ( 4294967295ul)
#define MYHDF_UINT32L    (          0ul)
#define MYHDF_FLOAT32H   (3.4028234e+38f)
#define MYHDF_FLOAT32L   (1.1754943e-38f)
#define MYHDF_FLOAT64H   (1.797693134862316e+308)
#define MYHDF_FLOAT64L   (2.225073858507201e-308)


/* Structure to store information about the HDF attribute */
typedef struct {
  int32 id, type, nval; /* id, data type and number of values */
  char *name;           /* attribute name */
} Myhdf_attr_t;

bool AppendMeta(char *cbuf, int *ic, char *s);
int PutSDSDimInfo(int32 sds_id, char *dimname, int irank);
bool PutAttrDouble(int32 sds_id, Myhdf_attr_t *attr, double *val);
bool PutAttrString(int32 sds_id, Myhdf_attr_t *attr, char *string);

int PutSpaceDefHDF(char *tile_fname, int tile_h, int tile_v, sds_info_t *sds,
									albers_proj_para_t albers_para, int nsds);
int world_PutSpaceDefHDF(char *tile_fname, int tile_h, int tile_v, sds_info_t *sds, int nsds);
#endif
