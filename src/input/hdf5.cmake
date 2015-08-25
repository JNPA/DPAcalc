# needs the c++ components of hdf5
find_package (HDF5 COMPONENTS CXX REQUIRED)

# set the include paths and the libraries needed by hdf5
set (DPACALC_EXTRA_INCLUDES ${DPACALC_EXTRA_INCLUDES} ${HDF5_INCLUDE_DIRS} )
set (DPACALC_EXTRA_LIBS ${DPACALC_EXTRA_LIBS} ${HDF5_CXX_LIBRARIES} )

#HDF5_DEFINITIONS -- work's without, and I am not sure where to put it
