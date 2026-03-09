find_package(Doxygen REQUIRED)

set(AWESOME_CSS_DIR "${CMAKE_SOURCE_DIR}/vendor/doxygen-awesome-css")

configure_file(
        "${CMAKE_SOURCE_DIR}/doc/doxyfile.in"
        "${CMAKE_BINARY_DIR}/doc/Doxyfile"
        @ONLY
)

add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_BINARY_DIR}/doc/Doxyfile"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Generating doxygen documentation"
        VERBATIM
)