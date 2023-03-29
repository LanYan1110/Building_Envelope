#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>

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

// Stop-condition policy
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_ratio_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_length_cost.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_length_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_placement.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Bounded_normal_change_filter.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/GarlandHeckbert_plane_policies.h>

#include <chrono>
#include <fstream>
#include <iostream>

#include <Eigen/Dense>
 
using Eigen::MatrixXd;
typedef CGAL::Exact_predicates_inexact_constructions_kernel Gt;
typedef Gt::Point_3                                  Point;
typedef CGAL::Surface_mesh<Point>                           Mesh;


typedef Mesh::Face_iterator Face_iterator;
typedef Mesh::Vertex_iterator Vertex_iterator;
namespace SMS = CGAL::Surface_mesh_simplification;
namespace PMP = CGAL::Polygon_mesh_processing;

typedef SMS::GarlandHeckbert_plane_policies<Mesh, Gt>                  Classic_plane;
typedef typename SMS::GarlandHeckbert_plane_policies<Mesh, Gt> GHPolicies;

void collapse_gh(Mesh& mesh,double ratio);
int mesh_processor(std::string input_mesh, std::string output_mesh,int& out_v, int& out_f);
