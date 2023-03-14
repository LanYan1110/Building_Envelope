
#include "inc/evaluation.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
#include <vector>

#include <iostream>
#include <fstream>
#include <CGAL/Surface_mesh/IO/OFF.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/AABB_face_graph_triangle_primitive.h>

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Point;
typedef K::Ray_3 Ray;
typedef CGAL::Surface_mesh<Point> Mesh;
typedef CGAL::AABB_face_graph_triangle_primitive<Mesh> Primitive;
typedef CGAL::AABB_traits<K, Primitive> Traits;
typedef CGAL::AABB_tree<Traits> Tree;
typedef boost::optional<Tree::Intersection_and_primitive_id<Ray>::Type> Ray_intersection;


std::vector<std::string> GetInputs(std::string dir) {

    std::vector<std::string> inputpatharray;

    struct stat sb;
    for (const auto& entry : fs::directory_iterator(dir)) {
        std::filesystem::path outfilename = entry.path();
        std::string outfilename_str = outfilename.string();
        const char* path = outfilename_str.c_str();

        if (stat(path, &sb) == 0 && !(sb.st_mode & S_IFDIR))
            std::cout << path << std::endl;
        inputpatharray.emplace_back(path);
    }

    return inputpatharray;

}


int main(){
    
    std::string obj_point_cloud="C:/Users/seuya/Documents/Thesis/distance_calculation/AC20-FZK-Haus.txt";
    std::string ifc_off="C:/Users/seuya/Documents/Thesis/distance_calculation/AC20-FZK-Haus.ifc.xyz.off";
    std::string ifc_obj="C:/Users/seuya/Documents/Thesis/distance_calculation/AC20-FZK-Haus.obj";
    std::string distances="C:/Users/seuya/Documents/Thesis/distance_calculation/distances_AC20-FZK-Haus.txt";
    std::string ifc_out_off="C:/Users/seuya/Documents/Thesis/distance_calculation/new.off";
    // if(point_set_to_mesh_distances(obj_point_cloud, ifc_off, distances)==0){
    //     std::cout << "Evaluation done" << std::endl;
    // }

    std::vector<Point> o_points;
    std::ifstream input(obj_point_cloud);
    double x, y, z;
	for (int i = 0; i <4838; ++i){
		input >> x >> y >> z;
		o_points.push_back(Point(x, y, z));
	}

    std::vector<Point> points;
    std::vector<std::vector<std::size_t>> faces1;
    // read polygon soup
    if(!CGAL::IO::read_polygon_soup(ifc_obj, points, faces1) ||
    points.size() == 0 || faces1.size() == 0){
        std::cerr << "Error: can not read input file.\n";
        return 1;
    }

    // orient polygon soup
    PMP::orient_polygon_soup(points,faces1);
    
    // convert to polygon mesh
    Mesh surface_mesh;
    if(PMP::is_polygon_soup_a_polygon_mesh(faces1)){
        PMP::polygon_soup_to_polygon_mesh(points, faces1, surface_mesh);
    }


    CGAL::IO::write_polygon_mesh(ifc_out_off, surface_mesh, CGAL::parameters::stream_precision(17));


    Tree tree;
    tree.insert(faces(surface_mesh).first, faces(surface_mesh).second, surface_mesh);
    tree.build(); // CGAL_assertion( idx < data.size() ) fails

   for (auto p: o_points){
        Point closest = tree.closest_point(p);
        //std::cout << "Closest point: " << closest<< std::endl;
        // open the distance text file and append the distance
        std::ofstream myfile;
        myfile.open (distances, std::ios_base::app);
        //std::cout<<"current distance is: "<<CGAL::squared_distance(p,closest)<< std::endl;
        myfile << CGAL::squared_distance(p,closest) << std::endl;
        myfile.close();
    }

    return 0;
}