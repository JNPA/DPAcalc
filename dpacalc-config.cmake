#
# Automatically generated make config: don't edit
# DPA calc
# Thu Oct  3 07:39:47 2013
#
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
SET ( CONFIG_BLOCK_SIZE 32 )
SET ( CONFIG_TEST_PARAM_START_DEV 0 )
SET ( CONFIG_TEST_PARAM_END_DEV 1 ) 
#
# Compilation options
#
set ( CONFIG_BUILD_DEBUG y )
# CONFIG_BUILD_RELEASE is not set
set ( CONFIG_DEBUG_OPTIONS "-g" )

#
# Class choices
#
set ( CONFIG_CLASS_INPUT "bin1" )
set ( CONFIG_CLASS_INTERMEDIATE "aes128round1" )
set ( CONFIG_CLASS_POWERMODEL "hammingweight" )
set ( CONFIG_CLASS_STATISTIC "pearsonNormWithGPU2" )
set ( CONFIG_CLASS_EXEC "cxx11threads" )
set ( CONFIG_CLASS_OUTPUT "coarseresult" )
