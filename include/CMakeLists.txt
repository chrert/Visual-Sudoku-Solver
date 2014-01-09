add_headers(typedefs.hpp
            settings.hpp)

add_subdirectory(classification)
add_subdirectory(gui)
add_subdirectory(imgproc)
add_subdirectory(utils)
add_subdirectory(solver)

forward_vars()
