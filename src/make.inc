#SDSU
HDFLIB=-L/opt/Toolkit/hdf4/lib -lmfhdf -ldf -lz -ljpeg
GCTPLIB=-L/opt/Toolkit/hdfeos-2.17v1.00/lib -lGctp 
GEOTIFFLIB=-L/opt/libgeotiff/lib -lgeotiff  
TIFFLIB=-L/opt/libtiff/lib -ltiff 

TIFFINC=-I/opt/libtiff/include
GEOTIFFINC=-I/opt/libgeotiff/include
GCTPINC=-I/opt/Toolkit/hdfeos-2.17v1.00/include
HDFINC=-I/opt/Toolkit/hdf4/include


WELDINC=-I${INSTALL_WELD}/weld.common.files.v3.0
WELDCOMM=${INSTALL_WELD}/weld.common.files.v3.0

CC=gcc
CFLAGS=-O2 -Wall -g
CPPFLAGS=-DMAX_NC_NAME=H4_MAX_NC_NAME -DMAX_VAR_DIMS=H4_MAX_VAR_DIMS ${TIFFINC} ${GEOTIFFINC} ${GCTPINC} ${HDFINC} ${WELDINC}
LDFLAGS=${HDFLIB} ${GEOTIFFLIB} ${TIFFLIB} ${GCTPLIB}
#CPPFLAGS=-DMAX_NC_NAME=H4_MAX_NC_NAME -DMAX_VAR_DIMS=H4_MAX_VAR_DIMS ${GCTPINC} ${HDFINC} ${WELDINC}
#LDFLAGS=${HDFLIB} ${GCTPLIB}

