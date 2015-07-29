#Copyright 2014 João Amaral
#
#This file is part of DPA Calc.
#
#DPA Calc is free software: you can redistribute it and/or modify 
#it under the terms of the GNU General Public License as published 
#by the Free Software Foundation, either version 3 of the License, 
#or (at your option) any later version.
#
#DPA Calc is distributed in the hope that it will be useful, 
#but WITHOUT ANY WARRANTY; without even the implied warranty 
#of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
#See the GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License 
#along with DPA Calc. If not, see http://www.gnu.org/licenses/.

set ( CONFIG_DONE y )

#
# Basic settings
#
set ( CONFIG_BATCH_SIZE 256 )
set ( CONFIG_KEY_HYPOTESIS 8 )
set ( CONFIG_FULL_KEY_SIZE 128 )
set ( CONFIG_KNOWNDATA_BITS 128 )
set ( CONFIG_TRACETYPE_FLOAT y )
# CONFIG_TRACETYPE_DOUBLE is not set
set ( CONFIG_INPUT_QUEUE_SIZE 30 )
# CONFIG_NO_TRICKS is not set
SET ( CONFIG_NUM_THREADS_PER_DEVICE 1 )
SET ( CONFIG_BLOCK_SIZE 16 )
SET ( CONFIG_TEST_PARAM_START_DEV 0 )
SET ( CONFIG_TEST_PARAM_END_DEV 1 ) 
#
# Compilation options
#
set ( CONFIG_BUILD_DEBUG y )
# CONFIG_BUILD_RELEASE is not set
set ( CONFIG_DEBUG_OPTIONS "-O3 -g" )

#
# Class choices
#
set ( CONFIG_CLASS_INPUT "hdf5" )
set ( CONFIG_CLASS_INTERMEDIATE "aes128" )
set ( CONFIG_CLASS_POWERMODEL "hammingweight" )
set ( CONFIG_CLASS_STATISTIC "pearsonNormWithGPU2" )
set ( CONFIG_CLASS_EXEC "cxx11threads" )
set ( CONFIG_CLASS_OUTPUT "coarseresult" )
