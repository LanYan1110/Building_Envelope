
set( CMAKE_BUILD_TYPE "Release")

cmake_minimum_required(VERSION 3.1...3.23)
project(distance_evaluation)

find_package(CGAL REQUIRED)

set(BCRYPT_LIBRARIES "bcrypt.lib")

set(Boost_INCLUDE_DIR "C:/Users/seuya/Documents/Thesis/IfcOpenshell/_deps/boost_1_74_0")
set(Boost_LIBRARY_DIR "C:/Users/seuya/Documents/Thesis/IfcOpenshell/_deps/boost_1_74_0/stage/lib")

include_directories(${Boost_INCLUDE_DIR})

# create a target per cppfile
file(
  GLOB cppfiles
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)


add_executable(${PROJECT_NAME} main.cpp 
 inc/evaluation.cpp
)

target_link_libraries(${PROJECT_NAME} CGAL::CGAL ${Boost_LIBRARIES})

foreach(cppfile ${cppfiles})
  #create_single_source_cgal_program("${cppfile}")
  #string(REPLACE ".cpp" ".exe" NAME "${cppfile}")
  #set_property(TARGET ${NAME} PROPERTY CXX_STANDARD 17)
endforeach()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
