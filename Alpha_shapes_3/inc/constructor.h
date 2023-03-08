#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/grid_simplify_point_set.h>
#include <CGAL/property_map.h>

#include <CGAL/Alpha_shape_3.h>
#include <CGAL/Alpha_shape_cell_base_3.h>
#include <CGAL/Alpha_shape_vertex_base_3.h>
#include <CGAL/Delaunay_triangulation_3.h>

#include <CGAL/grid_simplify_point_set.h>
#include <CGAL/random_simplify_point_set.h>
#include <CGAL/IO/read_points.h>
#include <CGAL/IO/OBJ.h>

#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/orientation.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <CGAL/Polygon_mesh_processing/orient_polygon_soup_extension.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

#include <unordered_set>
#include <unordered_map>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Gt;


typedef CGAL::Alpha_shape_vertex_base_3<Gt>          Vb;
typedef CGAL::Alpha_shape_cell_base_3<Gt>            Fb;
typedef CGAL::Triangulation_data_structure_3<Vb, Fb>  Tds;
typedef CGAL::Delaunay_triangulation_3<Gt, Tds>       Triangulation_3;
typedef CGAL::Alpha_shape_3<Triangulation_3>         Alpha_shape_3;

typedef Gt::Point_3                                  Point;
typedef Alpha_shape_3::Alpha_iterator                Alpha_iterator;
typedef CGAL::Surface_mesh<Point>                           Mesh;


void alpha_shape_constructor(std::string input_path,std::string output_path);