cmake_minimum_required(VERSION 3.14.2)
project("ImageStacker")

add_subdirectory(src)

#OpenCV
find_package(OpenCV REQUIRED)
target_link_libraries("ImageStacker" ${OpenCV_LIBS} )