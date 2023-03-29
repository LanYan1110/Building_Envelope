#include "inc/constructor.h"
#include "inc/off_to_obj_converter.h"
#include "inc/point_cloud_processor.h"
#include "inc/mesh_processor.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <chrono>
#include <iomanip>
#include <Eigen/Dense>

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


int main()
{
    // Read raw point cloud files
    // read the grid size from input
    double grid_size;
    std::cout << "Enter the grid size: ";
    std::cin >> grid_size;
    // Input path for raw point cloud
    std::string whole_dir = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/Raw_point_cloud/whole_alpha_shapes/"+std::to_string(grid_size)+"/";
    std::vector<std::string> input_files1 = GetInputs(whole_dir);
    // Output path for evaluation matrix
    std::vector<std::string> output_names={"alpha_shape_0.5","alpha_shape_0.25","alpha_shape_0.1"};
    std::vector<std::string> output_path;
    std::string output_dir="C:/Users/seuya/Documents/Thesis/Intermediate_Data/evaluation/";
    std::ofstream evaluation_file;

    for (int i=0;i<3;i++) {
        std::string c_output_path=output_dir+output_names[i]+".csv";
        output_path.emplace_back(c_output_path);
        evaluation_file.open(output_path[i]);
        evaluation_file<<"name"<<","<<"Out_v"<<","<<"Out_f"<<","<<"Time"<<"\n";
        evaluation_file.close();
    }

    std::vector<std::string> file_names;
    // Original point cloud files
    // Vectors that store evaluation indictors
	std::vector<int> out_v_unprocessed;
	std::vector<int> out_f_unprocessed;
	std::vector<double> time_unprocessed;
	int out_v=0; int out_f=0;

    double alpha;
    std::cout << "Enter the alpha value: ";
    std::cin >> alpha;

    // alpha shape
    // for (size_t i = 0; i < input_files1.size(); i++) {
    //     //time the process
    //     auto start1 = std::chrono::high_resolution_clock::now();
    //     // make a new folder for each file
    //     std::string export_dir="C:/Users/seuya/Documents/Thesis/Intermediate_Data/OFF/unprocessed/"+std::to_string(grid_size)+"/";
    //     std::filesystem::create_directories(export_dir);
    //     std::string export_path =export_dir+clear_slash(input_files1[i])+ ".off";
    //     std::cout<<export_path<<std::endl;
    //     alpha_shape_constructor(input_files1[i],export_path,out_v,out_f,alpha);
    //     std::string export_obj_dir="C:/Users/seuya/Documents/Thesis/Intermediate_Data/OBJ/unprocessed/"+std::to_string(grid_size)+"/";
    //     std::filesystem::create_directories(export_obj_dir);
    //     std::string export_obj = export_obj_dir+ clear_slash(input_files1[i]) + ".obj";
    //     std::cout<<export_obj<<std::endl;
    //     off_to_obj(export_path, export_obj);
    //     auto finish1 = std::chrono::high_resolution_clock::now();
    //     std::chrono::duration<double> elapsed1 = finish1 - start1;
    //     evaluation_file.open(output_path[0], std::ios_base::app);
    //     evaluation_file <<clear_slash(input_files1[i])<< "," <<out_v<< ","<<out_f<<","<<
    //     std::fixed << std::setprecision(2)<<elapsed1.count()<< "\n";
    //     evaluation_file.close();
    // }

    //test on multiple 3D alpha shapes
    //std::string test_file = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/Raw_point_cloud/indi_alpha_shapes/AC20-FZK-Haus.ifcr_p.xyz";
    //std::string export_path = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/OFF/indi_unprocessed/" + clear_slash(test_file)+ "test.off";
    //indi_alpha_shape_constructor(test_file,export_path);

    // Process point cloud files
    //Wlop_simplify input point cloud, construct alpha shape, and evaluate the result
    //wlop simplify
    // std::string processed_dir = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/Processed_point_cloud/whole_alpha_shapes/wlop_simplify/";
    //for (size_t i = 0; i < processed_dir.size(); i++){
    //   wlop_simplify(input_files1[i], processed_dir);
    //}

    // alpha shape
    // std::vector<std::string> input_files2 = GetInputs(processed_dir);
    // for (size_t i = 0; i < input_files2.size(); i++) {
    //     std::cout<<input_files2[i]<<std::endl;
    //     auto start2 = std::chrono::high_resolution_clock::now();
    //     std::string export_path = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/OFF/wlop_simplify/" + clear_slash(input_files2[i])+ ".off";
    //     alpha_shape_constructor(input_files2[i],export_path,out_v,out_f);
    //     std::string export_obj = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/OBJ/wlop_simplify/" + clear_slash(input_files2[i]) + ".obj";
    //     off_to_obj(export_path, export_obj);
    //     auto finish2 = std::chrono::high_resolution_clock::now();
    //     std::chrono::duration<double> elapsed = finish2 - start2;
    //     evaluation_file.open(output_path[1], std::ios_base::app);
    //     evaluation_file <<clear_slash(input_files2[i])<< "," <<out_v<< ","<<out_f<<","<<
    //     std::fixed << std::setprecision(2)<<elapsed.count()<< "\n";
    //     evaluation_file.close();
    // }

    // Process output alpha shapes
    //creaye a new folder for the output
    std::string unprocessed_obj_dir = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/OBJ/unprocessed/"+std::to_string(grid_size)+"/";
    std::string mesh_simplify_dir = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/OBJ/mesh_simplify/"+std::to_string(grid_size)+"/";
    std::filesystem::create_directories(mesh_simplify_dir);
    std::vector<std::string> input_files3 = GetInputs(unprocessed_obj_dir);
    for (size_t i = 0; i < input_files3.size(); i++) {
        auto start3 = std::chrono::high_resolution_clock::now();
        std::string simplified_obj = mesh_simplify_dir+ clear_slash(input_files1[i])+ ".obj";
        mesh_processor(input_files3[i], simplified_obj,out_v,out_f);
        auto finish3 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed3 = finish3 - start3;
        evaluation_file.open(output_path[2], std::ios_base::app);
        evaluation_file <<clear_slash(input_files3[i])<< "," <<out_v<< ","<<out_f<<","<<
        std::fixed << std::setprecision(2)<<elapsed3.count()<< "\n";
        evaluation_file.close();
    }

    return 0;
}