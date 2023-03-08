#include "inc/constructor.h"
#include "inc/off_to_obj_converter.h"
#include "inc/point_cloud_processor.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
#include <vector>



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
    std::string whole_dir = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/Raw_point_cloud/whole_alpha_shapes/";
    std::vector<std::string> input_files1 = GetInputs(whole_dir);

    // Process point cloud files
    // random simplify
    std::string processed_dir1 = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/Processed_point_cloud/whole_alpha_shapes/random_simplify/";
    for (size_t i = 0; i < input_files1.size(); i++) {
        //random_simplify(input_files1[i], processed_dir1);
    }

    // grid simplify
    std::string processed_dir2 = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/Processed_point_cloud/whole_alpha_shapes/grid_simplify/";
    for (size_t i = 0; i < input_files1.size(); i++) {
        grid_simplify(input_files1[i], processed_dir2);
    }

    //Hierachy_simplify_point_set
    std::string processed_dir3 = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/Processed_point_cloud/whole_alpha_shapes/hierachy_simplify/";
    for (size_t i = 0; i < input_files1.size(); i++) {
        //grid_simplify(input_files1[i], processed_dir2);
    }
    //Wlop_simplify_and_regularize_point_set
    std::string processed_dir4 = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/Processed_point_cloud/whole_alpha_shapes/wlop_simplify/";


    for (size_t i = 0; i < processed_dir2.size(); i++) {
        // Export path for one alpha shape model. .off format
        std::string export_path = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/OFF/" + clear_slash(input_files1[i])+ ".off";
        std::cout <<input_files1[i] << std::endl;
        alpha_shape_constructor(input_files1[i],export_path);
        std::string export_obj = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/OBJ/" + clear_slash(input_files1[i]) + ".obj";
        off_to_obj(export_path, export_obj);
    }
    
    return 0;
}