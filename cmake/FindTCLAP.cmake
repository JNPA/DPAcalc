include(LibFindMacros)

# Dependencies

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(TCLAP_PKGCONF tclap)

# Include dir
find_path(TCLAP_INCLUDE_DIR
	NAMES tclap/CmdLine.h
	PATHS ${TCLAP_PKGCONF_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/../dependencies/tclap/include
	    )


