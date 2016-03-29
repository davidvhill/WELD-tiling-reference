#!/bin/bash
#	NOTE: modified by V.Kovalskyy to fit monthly compositing
#   Run composite for the tiles in the given list.
#
#	This script requires 6 command-line parameters:
#	1, <tile-pathrow.map.txt>:   	 list of the mapping between WELD world tiles and acquisitions.
# 	2, <world_MTL_list.txt>:     	 list of all MTL files
#	3, <world_geometry_list.txt>:	 list of all geometry files 
#	4, <world_TOA_list.txt>:	 list of all TOA files, including the saturation files.
#	5, <world_DTCLD_list.txt>:	 list of all DTCLD files.
#	6, <world_ACCA_list.txt>:	 list of all ACCA files
#	7, <out_dir>:			 output directory
#	8, <time_period>:	         time period of the composit
#	9, <FAIL_TILE_LOG>:	         time period of the composit
#									
#	From the tile-pathrow mapping in parameters, the 
#   	Multiple processes of this script can be invoked for different sets of tiles. 
#	For different processes, the output directory should ideally be different so that
#	I/O wait can be reduced. The lists in parameters 2-6 are the same for different processes.

if [ $# -ne 9 ]
then
	echo "$0  <tile-acq.map.txt> <world_MTL_list.txt> <world_GEOM_list.txt> <world_TOA_list.txt> <world_ACCA_list.txt> <world_DTCLD_list.txt> <out_dir> <time_period> <FAIL_TILE_LOG> "
	exit 1
fi

ptr_flag=false

MAP=$1
ALL_MTL=$2
ALL_GEOM=$3
ALL_TOA=$4
ALL_DTCLD=$5
ALL_ACCA=$6
OUT_WELD=$7
TIME_PERIOD=$8
FAIL_TILE_LOG=$9


YEAR=${TIME_PERIOD:7:4}
WEEK=${TIME_PERIOD:4:2}

sweek=$((($WEEK-1)*7+1))
eweek=$((($WEEK)*7))
echo "sweek $sweek"
echo "eweek $eweek"
echo "WEEK $WEEK"
echo "process # $$ $COMP_VERSION"

cat $MAP |
while read line 
do
	echo $line
	# Historically (although short) the last character of a line in the
	# tile-acquisition mapping file	is ";" and so only (NF-1) fields in a line

	tileid=`echo $line | awk -F";" '{print $1}'`
	comp_input=temp.${tileid}.comp.input.$$.txt
	rm -f ${comp_input}
	for pr in `echo $line | awk -F";" '{for(i=2;i<=(NF-1);i++) print $i}' `
	do
	    	set `echo $pr | tr "_"  " " `
	    	p=$1
		p3d=`  echo $p   | awk '{printf("%03d", $1)}' `
	    	r=$2
		r3d=`  echo $r   | awk '{printf("%03d", $1)}' `
		
		acq_pattern="${p3d}${r3d}${YEAR}"
		grep $acq_pattern $ALL_TOA   >> tmp.check.$$.txt
		if [ ! -s tmp.check.$$.txt ]
		then	
			rm -f tmp.check.$$.txt
			continue
		fi
		for doy in `seq $sweek $eweek`
		do	
			doy3d=`  echo $doy   | awk '{printf("%03d", $1)}' `
			acq_pattern="${p3d}${r3d}${YEAR}${doy3d}"
			rm -f tmp.check.$$.txt
			grep $acq_pattern $ALL_TOA   >> tmp.check.$$.txt
			if [ -s tmp.check.$$.txt ]
			then
			    	grep $acq_pattern $ALL_TOA   >> ${comp_input}
				grep $acq_pattern $ALL_DTCLD >> ${comp_input} 
				grep $acq_pattern $ALL_ACCA  >> ${comp_input} 
				grep $acq_pattern $ALL_GEOM  >> ${comp_input}
				# Add a blank line between acquisitions
				echo >> ${comp_input} 
			fi
		done
	done
	rm -f tmp.check.$$.txt
	if [ -s ${comp_input} ]
	then

		echo "${TILECODE_WELD}/albers.toa.tile.ofutm.proc.v3.0 region=CONUS time_period=${TIME_PERIOD} tile_id=$tileid geometry_flag=true l1t_index_flag=true version=v3.0 acq_list=${comp_input}  as_pointers=${ptr_flag} out_dir=${OUT_WELD}"

		${TILECODE_WELD}/albers.toa.tile.ofutm.proc.v3.0  region=CONUS\
							        time_period=${TIME_PERIOD}\
				                            	tile_id=$tileid\
								geometry_flag=true\
								l1t_index_flag=true\
								version=v3.0\
								acq_list=${comp_input}\
								as_pointers=${ptr_flag}\
								out_dir=${OUT_WELD}
		

		
		
		if [ $? -ne 0 ]
		then
			#note failure in the log file
			echo "pr# $$ : Error in tile code for $tileid ${TIME_PERIOD} " >> $FAIL_TILE_LOG
	    		continue
		fi
		rm -f ${comp_input}
		rm -f ${comp_input}
	fi
done
rm -f doy.check.$$.txt
