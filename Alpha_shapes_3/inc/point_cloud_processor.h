#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/grid_simplify_point_set.h>
#include <CGAL/property_map.h>

#include <CGAL/Alpha_shape_3.h>
#include <CGAL/Alpha_shape_cell_base_3.h>
#include <CGAL/Alpha_shape_vertex_base_3.h>
#include <CGAL/Delaunay_triangulation_3.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/grid_simplify_point_set.h>
#include <CGAL/random_simplify_point_set.h>
#include <CGAL/IO/read_points.h>
#include <CGAL/IO/OBJ.h>
#include <CGAL/Surface_mesh.h>

#include <CGAL/IO/read_points.h>
#include <CGAL/IO/write_points.h>
#include <CGAL/hierarchy_simplify_point_set.h>
#include <CGAL/wlop_simplify_and_regularize_point_set.h>
#include <CGAL/Memory_sizer.h>
#include <CGAL/Timer.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/wlop_simplify_and_regularize_point_set.h>


// bounding box includes
// #include <CGAL/Aff_transformation_3.h>
// #include <CGAL/Optimal_bounding_box/oriented_bounding_box.h>
// #include <CGAL/bounding_box.h>
#include <CGAL/optimal_bounding_box.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <CGAL/Polygon_mesh_processing/measure.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/squared_distance_3.h> //for 3D functions

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
#include <vector>
namespace fs = std::filesystem;

typedef CGAL::Exact_predicates_inexact_constructions_kernel Gt;

typedef CGAL::Alpha_shape_vertex_base_3<Gt>          Vb;
typedef CGAL::Alpha_shape_cell_base_3<Gt>            Fb;
typedef CGAL::Triangulation_data_structure_3<Vb, Fb>  Tds;
typedef CGAL::Delaunay_triangulation_3<Gt, Tds>       Triangulation_3;
typedef CGAL::Alpha_shape_3<Triangulation_3>         Alpha_shape_3;

typedef Gt::Point_3                                  Point;
typedef Alpha_shape_3::Alpha_iterator                Alpha_iterator;
typedef CGAL::Surface_mesh<Point>                              Mesh;

typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Parallel_if_available_tag Concurrency_tag;

std::string clear_slash(std::string const& path_of_file, std::string const& d_slash = "/\\");
void random_simplify(std::string input_path,std::string output_dir);
void grid_simplify(std::string input_path,std::string output_dir);
void hierachy_simplify(std::string input_path,std::string output_dir);
int wlop_simplify(std::string input_path, std::string output_dir);