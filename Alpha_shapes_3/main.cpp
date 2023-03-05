#include "inc/constructor.h"
#include "inc/simplifier.h"
#include "inc/exporter.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
#include <vector>

#include <boost/log/core.hpp>

std::vector<std::string> GetInputs() {

	std::vector<std::string> inputpatharray;

	std::string dir = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/vertices/whole_alpha_shapes/";
	struct stat sb;
	for (const auto& entry : fs::directory_iterator(dir)) {
		std::filesystem::path outfilename = entry.path();
		std::string outfilename_str = outfilename.string();
		const char* path = outfilename_str.c_str();

		if (stat(path, &sb) == 0 && !(sb.st_mode & S_IFDIR))
			std::cout << path << std::endl;
			inputpatharray.emplace_back(path);
	}
		
	std::cout << inputpatharray.size() << std::endl;

	while (true) {
		if (inputpatharray.size()==0) {
			std::cout << "Please enter the filepath of the IFC file" << std::endl;
			std::cout << "[INFO] if multiple files entered, please seperate them by pressing enter!" << std::endl;
			std::cout << "[INFO] finish by empty line + enter" << std::endl;

			while (true) {
				std::string single_input;
				getline(std::cin, single_input);
				
				if (single_input.size() == 0 && inputpatharray.size() == 0) {
					std::cout << "[INFO] No IFC files are inputted." << std::endl;
					std::cout << "Please enter the filepath of the IFC file" << std::endl;
					std::cout << "[INFO] if multiple files entered, please seperate them by pressing enter!" << std::endl;
				}

				else if (single_input.size() == 0) {
					break;
				}

				inputpatharray.emplace_back(single_input);
			}
		}
		else {
			break;
		}
	}

	bool hasError = false;

	return inputpatharray;
}


int main()
{
    
    // Read point cloud from file
	//input 
	std::vector<std::string> input_file_array=GetInputs();

	std::vector<std::string> segments;
	std::string root_dir;
	boost::split(segments, input_file_array[0], boost::is_any_of("/"));

	for (size_t i = 0; i < segments.size() - 4; i++) {
		root_dir += segments[i] + "/";
	}

	std::cout<<root_dir<<std::endl;
	segments.clear();
    

    for (size_t i = 0; i < input_file_array.size(); i++) {

        // Export path for one alpha shape model
        boost::split(segments, input_file_array[i], boost::is_any_of("/"));
		std::string export_path = root_dir + "Intermediate_Data/OFF" + segments[segments.size() - 1] + ".xyz";
        std::cout << export_path << std::endl;

        // Construct alpha shape
        Alpha_shape_3 as=alpha_shape_constructor(input_files1[i]);
        // Simplify alpha shape
        Alpha_shape_3 as_simplified=alpha_shape_simplifier(as);
        // Export alpha shape
        alpha_shape_exporter(as_simplified, export_path);


    }

    return 0;
}