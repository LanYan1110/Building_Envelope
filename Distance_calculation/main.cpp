
#include "inc/evaluation.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
#include <vector>
#include <chrono>

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

std::string clear_slash(std::string const& path_of_file, std::string const& d_slash="/\\"){
    size_t index_of_slash = path_of_file.find_last_of(d_slash);
    std::string file_name = path_of_file.substr(index_of_slash + 1);
    return file_name;
}

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
    // std::string s_reconstructed_obj="C:/Users/seuya/Documents/Thesis/distance_calculation/BIMobject_Demo_Model_Revit.xyz";
    std::string reconstructed_obj="C:/Users/seuya/Documents/Thesis/distance_calculation/BIMobject_Demo_Model_Revit.ifc_0.5off.obj";
    std::string original_obj="C:/Users/seuya/Documents/Thesis/distance_calculation/BIMobject_Demo_Model_Revit.ifc_0.5off.obj";
    std::string distances="C:/Users/seuya/Documents/Thesis/distance_calculation/"+clear_slash(original_obj)+"distances.txt";
    std::string sampled_points="C:/Users/seuya/Documents/Thesis/distance_calculation/"+clear_slash(original_obj)+"sampled_points.xyz";
    std::vector<Point> r_points;
    std::vector<std::vector<std::size_t>> r_faces2;
    
    // read polygon soup
    if(!CGAL::IO::read_polygon_soup(reconstructed_obj, r_points, r_faces2) ||
    r_points.size() == 0 || r_faces2.size() == 0){
       std::cerr << "Error: can not read input file.\n";
       return 1;
    }

    // orient polygon soup
    PMP::orient_polygon_soup(r_points, r_faces2);
    
    // convert to polygon mesh
    Mesh r_surface_mesh;
    if(PMP::is_polygon_soup_a_polygon_mesh(r_faces2)){
       PMP::polygon_soup_to_polygon_mesh(r_points, r_faces2, r_surface_mesh);
    }

    

    // sample the points
    std::vector<Point> r_sample_points;
    //std::OutputIterator outputIterator(r_sample_points);
    // construct output iterator for the point vector
    std::back_insert_iterator<std::vector<Point>> outputIterator(r_sample_points);
    
    r_sample_points.push_back(Point(0,0,0));
    //open file
    // std::ifstream input(s_reconstructed_obj);
    //  double x, y, z;
	//  for (int i = 0; i <4838; ++i){
	//  	input >> x >> y >> z;
	//  	r_sample_points.push_back(Point(x, y, z));
	//  }
    
    PMP::triangulate_faces(r_surface_mesh);
    PMP::sample_triangle_mesh(r_surface_mesh,outputIterator);

    // write the sampled points to a file
    std::ofstream out(sampled_points);
    for (auto p: r_sample_points){
        out << p.x() << " " << p.y() << " " << p.z() << std::endl;
    }
    out.close();

    std::vector<Point> points;
    std::vector<std::vector<std::size_t>> faces1;
    // read polygon soup
    if(!CGAL::IO::read_polygon_soup(reconstructed_obj, points, faces1) ||
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

    Tree tree;
    tree.insert(faces(surface_mesh).first, faces(surface_mesh).second, surface_mesh);
    tree.build(); // CGAL_assertion( idx < data.size() ) fails


    //time the process
    auto start = std::chrono::high_resolution_clock::now();
   for (auto p: r_sample_points){
        Point closest = tree.closest_point(p);
        //std::cout << "Closest point: " << closest<< std::endl;
        // open the distance text file and append the distance
        std::ofstream myfile;
        myfile.open (distances, std::ios_base::app);
        //std::cout<<"current distance is: "<<CGAL::squared_distance(p,closest)<< std::endl;
        myfile << CGAL::squared_distance(p,closest) << std::endl;
        myfile.close();
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = (finish - start);
    std::cout << "Elapsed time: " << elapsed.count() << " s"<<std::endl;

    return 0;
}