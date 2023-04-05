
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
#include <numeric>
#include <cmath>

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

    // enter testing type
    std::string type;
    std::cout << "Enter the type of alpha shape test:unprocessed,wlop_simplify,mesh_simplify";
    std::cin >> type;

    // directory of the orginal ifc meshes
    std::string input_mesh_dir="C:/Users/seuya/Documents/Thesis/RawData/obj/";
    std::vector<std::string> input_mesh=GetInputs(input_mesh_dir);

    // directory of the reconstructed ifc meshes
    double grid_size;
    std::cout<<"Enter the grid size: ";
    std::cin>>grid_size;

    std::string reconstructed_mesh_dir="C:/Users/seuya/Documents/Thesis/Intermediate_Data/OBJ/"+type+"/"+std::to_string(grid_size)+"/";
    std::vector<std::string> reconstructed_mesh=GetInputs(reconstructed_mesh_dir);
 
    // path to the evaluation file
    std::string evaluation="C:/Users/seuya/Documents/Thesis/Intermediate_Data/Evaluation/evaluation_distance_"+std::to_string(grid_size)+"_"+type+".csv";
    // write to the evaluation file
    std::ofstream evaluation_file;
    evaluation_file.open(evaluation);
    evaluation_file<<"name"<<","<<"number of points" << "," << "time" << "," << "min" << "," << "max" <<
    ","<<"mean"<<","<<"SD"<<","<<"0.025-0.5_percentage"<<","<<">0.05-0.1_percentage"<<
    ","<<">0.1_percentage"<<std::endl;
    evaluation_file.close();

    // directory of the sampled points
    std::string sampled_points_dir="C:/Users/seuya/Documents/Thesis/Intermediate_Data/Points/mesh_simplify/"+std::to_string(grid_size)+"/";
    std::filesystem::create_directories(sampled_points_dir);
    // directory of the distances, three different ones
    std::string distances_dir="C:/Users/seuya/Documents/Thesis/Intermediate_Data/Distances/mesh_simplify/"+std::to_string(grid_size)+"/";
    std::string threshold_distances_dir="C:/Users/seuya/Documents/Thesis/Intermediate_Data/Distances/mesh_simplify/"+std::to_string(grid_size)+"/";
    std::filesystem::create_directories(distances_dir);

    for (int i=0;i<input_mesh.size();i++){
        auto start = std::chrono::high_resolution_clock::now();
        std::string original_obj=input_mesh[i];
        std::string reconstructed_obj=reconstructed_mesh[i];
        std::string sampled_points_path=sampled_points_dir+clear_slash(input_mesh[i])+"_p.txt";
        std::cout<<"sampled points: "<<sampled_points_path<<std::endl;
        std::string log_distances=distances_dir+clear_slash(input_mesh[i])+"l_d.txt";
        std::cout<<"distances: "<<log_distances<<std::endl;
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
        // PMP::sample_triangle_mesh(r_surface_mesh,outputIterator,
        // CGAL::parameters::use_monte_carlo_sampling(true).
        // number_of_points_on_faces(10));
        PMP::sample_triangle_mesh(r_surface_mesh,outputIterator,
        CGAL::parameters::number_of_points_on_faces(1));

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
        int count1=0; int count2=0; int count3=0;
        double t_dist=0;
        // open in append mode
        std::ofstream out(sampled_points_path,std::ios::app);
        std::ofstream out2(log_distances,std::ios::app);

        for (auto p: r_sample_points){

            // calculate the distance
            Point closest = tree.closest_point(p);
            double c_distance=CGAL::squared_distance(p,closest);
            // push the distance to the vector
            distances_vector.push_back(c_distance);
            // if the distance is between 0.025 and 0.05, count it
            if(c_distance>0.025 && c_distance<0.05){
                t_dist=1;
                count1++;}
            // if the distance is between 0.05 and 0.1, count it
            if(c_distance>0.05 && c_distance<0.1){
                t_dist=2;
                count2++;}
            // if the distance is bigger than 0.1, count it
            if(c_distance>0.1){
                t_dist=3;
                count3++;}
            // write the points to a file
            out << p.x() << " " << p.y() << " " << p.z() <<""<<t_dist<<std::endl;
            //reset the t_dist value to 0 for the next point
            t_dist=0;
            
        }

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = (finish - start);
        std::cout << "Elapsed time: " << elapsed.count() << " s"<<std::endl;

        // find the min and max distance
        auto min_element = std::min_element(distances_vector.begin(), distances_vector.end());
        auto max_element = std::max_element(distances_vector.begin(), distances_vector.end());

        // find the mean value of all the calculated distances
        const auto sum = std::accumulate(distances_vector.begin(),distances_vector.end(), 0.0);
        const auto mean =  sum / distances_vector.size();

        // find the median value of all the calculated distances
        double median;
        int n = distances_vector.size();
        std::sort(distances_vector.begin(), distances_vector.end());

        if (n % 2 == 0) {
            // even number of elements
            median=(distances_vector[n/2 - 1] +distances_vector[n/2]) / 2.0;
        } else {
            // odd number of elements
            median=distances_vector[n/2];
        }

        // find the standard deviation of all the calculated distances
        // Calculate the sum of squared differences
        double sum_sq_diff = 0;
        for (double d : distances_vector) {
            sum_sq_diff += pow(d - mean, 2);
            double c_l_d=log(d);
            out2<<c_l_d<<std::endl;
        }

    // Calculate the standard deviation
    double std_dev = sqrt(sum_sq_diff / n);

    // calculate the percentage of points with distance between 0.025 and 0.05
    double percentage1 = (double)count1/(double)r_sample_points.size();
    double percentage2 = (double)count2/(double)r_sample_points.size();
    double percentage3 = (double)count3/(double)r_sample_points.size();


    // write to the evaluation file
    std::ofstream evaluation_file(evaluation, std::ios::app);
    evaluation_file<<clear_slash(input_mesh[i])<<","<<r_sample_points.size()<< "," <<std::setprecision(2)<<std::fixed<<elapsed.count() 
    << "," << std::setprecision(4)<<std::fixed<<*min_element << "," << *max_element <<","<<mean<<","<<std_dev<<","<<percentage1<<
    ","<<percentage2<<","<<percentage3<<std::endl;
    
    evaluation_file.close();
    }
    
    return 0;
}