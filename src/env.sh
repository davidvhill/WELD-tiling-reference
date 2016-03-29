###############################################################################
#	INSTALLATION
#	Need to update the header and library locations for the local system.
###############################################################################

#   Set ledaps ROOT
export LEDAPS_ROOT="/weldgfs2/d4/p65/ledaps.sep.2013"

#   TIFF and GeoTIFF headers and libraries.
export TIFFINC="/opt/libtiff/include"
export TIFFLIB="/opt/libtiff/lib"

export GEOTIFFINC="/opt/libgeotiff/include"
export GEOTIFFLIB="/opt/libgeotiff/lib"

#   GCTP headers and libraries
export GCTPINC="/opt/Toolkit/hdfeos-2.17v1.00/include"
export GCTPLIB="/opt/Toolkit/hdfeos-2.17v1.00/lib"

#   HDF4 headers and libraries
export HDFINC="/opt/Toolkit/hdf4/include"
export HDFLIB="/opt/Toolkit/hdf4/lib"


#	No need to change the way of linking
export TIFFLINK="-lgeotiff  -ltiff -ltiffxx -lm"
export GEOTIFFLINK="${TIFFLINK}"
export GCTPLINK="-lgctp -lm"
export HDFLINK="-lmfhdf -ldf -lz -lm -ljpeg"

export INSTALL_WELD=`pwd`
export WELDCOMM="${INSTALL_WELD}/weld.common.files.v3.0"

export pversion="v3.0"

###############################################################################
# RUNNING THE CODE (change it for L8)
###############################################################################
export SCRIPT_WELD="${INSTALL_WELD}"
export JOURNAL_WELD="${INSTALL_WELD}/journals"
export FAILLOG_WELD="${INSTALL_WELD}/faillogs"
export TILECODE_WELD="${INSTALL_WELD}/albers.toa.tile.ofutm.proc"





