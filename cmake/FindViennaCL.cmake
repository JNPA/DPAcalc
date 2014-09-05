

# Dependencies

# Include dir
find_path(VIENNACL_INCLUDE_DIR viennacl/forwards.h
    PATHS ${CMAKE_SOURCE_DIR}/../dependencies/viennacl
    DOC "The ViennaCL include path")