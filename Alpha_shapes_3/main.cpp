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
    std::string processed_dir1 = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/Processed_point_cloud/whole_alpha_shapes/random_simplify/";

    for (size_t i = 0; i < input_files1.size(); i++) {
        random_simplify(input_files1[i], processed_dir1);
    }

    // for (size_t i = 0; i < input_files1.size(); i++) {
    //     // Export path for one alpha shape model. .off format
    //     std::string export_off = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/OFF/" + clear_slash(input_files1[i])+ ".off";
    //     std::cout << export_off << std::endl;
    //     std::cout <<input_files1[i] << std::endl;
    //     alpha_shape_constructor(input_files1[i],export_off);
    //     std::string export_obj = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/OBJ/" + clear_slash(input_files1[i]) + ".obj";
    //     off_to_obj(export_off, export_obj);
    // }
    
    return 0;
}