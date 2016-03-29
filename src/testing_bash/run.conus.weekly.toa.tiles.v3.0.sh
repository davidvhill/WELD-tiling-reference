#!/bin/bash
#	This script invokes multiple processes of the script "tile.process.sh"
#	to process all the world tiles in parallel. 
#
#	User need to update variables:
#	1. NP:  for how many processes to run
#	2. OUT_WELD:  output directories for different processes 
#
#	All processes work on roughly same number of tiles, but the numbers of 
#	acquisitions on the tiles are probably different, so strictly speaking the
#	compositing task is not evenly divied.
#	
#	CODE_WELD is the directory containing the README file, where the
#	sub-directories of code of individual processing steps reside.
#

YEAR=2010
UTMCODE_WELD=".."
pversion=v3.0

###	How many processes. For this test, only run 2.
NP=4
#cd $SCRIPT_WELD
INPUT_WELD="/weldgfs2/d9/p18/for_hankui/apt/v3.0.test.out.albers/utm"
#CATALOGUE INPUTS
MTL_LIST=test.CY2010.mtl.list$$.txt
TOA_LIST=test.CY2010.toa.list$$.txt
ACCA_LIST=test.CY2010.acca.list$$.txt
GEOM_LIST=test.CY2010.geom.list$$.txt
DTCLD_LIST=test.CY2010.dtcld.list$$.txt
$UTMCODE_WELD/util/lsf ${INPUT_WELD}/mtl   | grep MTL.txt  		| egrep "[0-1][0-9][0-9]${time_period}[0-3][0-9][0-9]" > $MTL_LIST
$UTMCODE_WELD/util/lsf ${INPUT_WELD}/toa   | grep TOA.${pversion}.hdf	| egrep "[0-1][0-9][0-9]${time_period}[0-3][0-9][0-9]" > $TOA_LIST
$UTMCODE_WELD/util/lsf ${INPUT_WELD}/geom  | grep GEOM.${pversion}.hdf  | egrep "[0-1][0-9][0-9]${time_period}[0-3][0-9][0-9]" > $GEOM_LIST
$UTMCODE_WELD/util/lsf ${INPUT_WELD}/acca  | grep ACCA.${pversion}.hdf  | egrep "[0-1][0-9][0-9]${time_period}[0-3][0-9][0-9]" > $ACCA_LIST
$UTMCODE_WELD/util/lsf ${INPUT_WELD}/dtcld | grep DTCLD.${pversion}.hdf | egrep "[0-1][0-9][0-9]${time_period}[0-3][0-9][0-9]" > $DTCLD_LIST

OUTPUT_WELD="/weldgfs2/d9/p18/for_hankui/apt/v3.0.test.out.albers"

	all_map=$SCRIPT_WELD/conus.tile.pathrow.map.v1.6.txt


	###	Evenly divide the acquisition list among the processes; the last process
	### may get a few more (but fewer than number NP).  The process number is 1-based.
	
	grep  h05v10  $all_map > tmp.tile.process.PN01proc.map.$$.txt
	grep  h06v10  $all_map > tmp.tile.process.PN02proc.map.$$.txt
	grep  h05v11  $all_map > tmp.tile.process.PN03proc.map.$$.txt
	grep  h06v11  $all_map > tmp.tile.process.PN04proc.map.$$.txt

	for week in  `seq 1 2`
	do
		
		week2d=`  echo $week   | awk '{printf("%02d", $1)}' `
		time_period="week${week2d}.${YEAR}"
		echo $time_period
		OUTPUT_WELD=${OUTPUT_WELD%/}
		
		mkdir -p ${FAILLOG_WELD}/tile
		export FAIL_TILE_LOG=${FAILLOG_WELD}/tile/${comp_version}_${time_period}.fail.tile.log.txt


			    
		# Increase the maximum stack size to 40 MB for stack hungry processes
		ulimit -s 40960

		OUT_WELD=$OUTPUT_WELD/tiles/${time_period}
		mkdir -p $OUT_WELD
		#for process in PN06
		for process in PN01 PN02 PN03 PN04
		do
			# Ideally the suppied directories for different processes should be on disks accessed by
			# different controllers.
			sub_map=tmp.tile.process.${process}proc.map.$$.txt
			echo "bash $SCRIPT_WELD/testing_bash/tile.process.v3.0.albers.sh  $sub_map $MTL_LIST  $TOA_LIST  $ACCA_LIST $GEOM_LIST $DTCLD_LIST $OUT_WELD  ${time_period} $FAIL_TILE_LOG"

			bash $SCRIPT_WELD/testing_bash/weekly.one.tile.process.v3.0.albers.sh  $sub_map \
					  		$MTL_LIST  \
							$TOA_LIST  \
							$ACCA_LIST \
							$GEOM_LIST \
							$DTCLD_LIST\
							$OUT_WELD  \
							${time_period} \
							$FAIL_TILE_LOG	&

			if [ $? -ne 0 ]
			then
				echo "pr# $$ : sub $process failed for ${time_period}" >> $FAIL_TILE_LOG
				exit 1
			fi
		done

		#	Wait for the child processes to finish
		wait

		rm -f temp.nosuchfile*
	done
rm -f tmp*
rm -f test.CY2010*$$.txt
exit 0


