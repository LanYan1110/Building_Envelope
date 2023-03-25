
#include "inc/evaluation.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <algorithm>

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

    // directory of the orginal ifc meshes
    std::string input_mesh_dir="C:/Users/seuya/Documents/Thesis/RawData/obj/";
    //std::vector<std::string> input_mesh=GetInputs(input_mesh_dir);
    std::vector<std::string> input_mesh;
    input_mesh.emplace_back("C:/Users/seuya/Documents/Thesis/RawData/obj/AC20-FZK-Haus.obj");


    // directory of the reconstructed ifc meshes
    std::string reconstructed_mesh_dir="C:/Users/seuya/Documents/Thesis/Intermediate_Data/OBJ/mesh_simplify/";
    //std::vector<std::string> reconstructed_mesh=GetInputs(reconstructed_mesh_dir);
    std::vector<std::string> reconstructed_mesh;
    reconstructed_mesh.emplace_back("C:/Users/seuya/Documents/Thesis/Intermediate_Data/OBJ/mesh_simplify/AC20-FZK-Haus.ifcr_p.xyz.obj");

    // path to the evaluation file
 //   std::string evaluation="C:/Users/seuya/Documents/Thesis/Intermediate_Data/Evaluation/evaluation_distance_mesh_simplify.csv";
 //   // write to the evaluation file
 //   std::ofstream evaluation_file;
	//evaluation_file.open(evaluation);
 //   evaluation_file<<"name"<<","<<"number of points" << "," << "time" << "," << "min" << "," << "max" << std::endl;
 //   evaluation_file.close();

    for (int i=0;i<input_mesh.size();i++){
        auto start = std::chrono::high_resolution_clock::now();
        std::string original_obj=input_mesh[i];
        std::string reconstructed_obj=reconstructed_mesh[i];
        std::string sampled_points="C:/Users/seuya/Documents/Thesis/Intermediate_Data/Points/mesh_simplify/"+clear_slash(input_mesh[i])+"_p.txt";
        std::string distances="C:/Users/seuya/Documents/Thesis/Intermediate_Data/Distances/mesh_simplify/"+clear_slash(input_mesh[i])+"_d.txt";
        std::cout<<"original obj: "<<original_obj<<std::endl;
        std::cout<<"reconstructed obj: "<<reconstructed_obj<<std::endl;

        // read reconstructed obj and sample points
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
        // construct output iterator for the point vector
        std::back_insert_iterator<std::vector<Point>> outputIterator(r_sample_points);

        PMP::triangulate_faces(r_surface_mesh);
        PMP::sample_triangle_mesh(r_surface_mesh,outputIterator);

        // write the sampled points to a file
        std::ofstream out(sampled_points);
        for (auto p: r_sample_points){
            out << p.x() << " " << p.y() << " " << p.z() << std::endl;
        }
        out.close();

        // read original obj and construct AABB tree
        std::vector<Point> points;
        std::vector<std::vector<std::size_t>> faces1;
        // read polygon soup
        if(!CGAL::IO::read_polygon_soup(original_obj, points, faces1) ||
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
        tree.build(); 

        std::vector<double> distances_vector;
        for (auto p: r_sample_points){
            Point closest = tree.closest_point(p);
            //std::cout << "Closest point: " << closest<< std::endl;
            // open the distance text file and append the distance
            std::ofstream myfile;
            myfile.open (distances, std::ios_base::app);
            //std::cout<<"current distance is: "<<CGAL::squared_distance(p,closest)<< std::endl;
            distances_vector.emplace_back(CGAL::squared_distance(p,closest));
            myfile <<std::fixed<<std::setprecision(8)<<CGAL::squared_distance(p,closest) << std::endl;
            myfile.close();
        }

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = (finish - start);
        std::cout << "Elapsed time: " << elapsed.count() << " s"<<std::endl;

        // find the min and max distance
        auto min_element = std::min_element(distances_vector.begin(), distances_vector.end());
        auto max_element = std::max_element(distances_vector.begin(), distances_vector.end());

        // write to the evaluation file
        //std::ofstream evaluation_file(evaluation, std::ios::app);
        //evaluation_file<<clear_slash(input_mesh[i])<<","<<r_sample_points.size()<< "," <<std::setprecision(2)<<std::fixed<<elapsed.count() 
        //<< "," << std::setprecision(4)<<std::fixed<<*min_element << "," << *max_element << std::endl;
        //evaluation_file.close();
    }
    
    return 0;
}