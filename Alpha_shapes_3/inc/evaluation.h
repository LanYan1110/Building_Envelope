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

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_mesh_processing/orient_polygon_soup.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <CGAL/IO/polygon_soup_io.h>
#include <CGAL/Polygon_mesh_processing/distance.h>
#include <CGAL/IO/OBJ.h>

#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/orientation.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <CGAL/Polygon_mesh_processing/orient_polygon_soup_extension.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <CGAL/Polygon_mesh_processing/distance.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Gt;
typedef Gt::Point_3                                  Point;
typedef CGAL::Surface_mesh<Point>                           Mesh;

namespace PMP = CGAL::Polygon_mesh_processing;