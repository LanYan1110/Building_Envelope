#include "inc/sampler.h"
#include "inc/helper.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <filesystem>
#include <chrono>
//#include <stdio>
//#include <dirent>

namespace fs = std::filesystem;

// openCASCADE includes
#include <TopoDS.hxx>
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <gp_GTrsf.hxx>

// IfcOpenShell includes
#include <ifcparse/IfcFile.h>
#include <ifcgeom/IfcGeom.h>
#include <ifcgeom/IfcGeomRepresentation.h>
#include <ifcgeom_schema_agnostic/kernel.h>
#include <ifcgeom_schema_agnostic/IfcGeomIterator.h>

std::vector<std::string> GetInputs() {

	std::vector<std::string> inputpatharray;

	std::string dir= "C:/Users/seuya/Documents/Thesis/RawData/ifc/IFC2_3/";
	//std::string dir = "C:/Users/seuya/Documents/Thesis/RawData/ifc/IFC4/";
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

	//TO-DO: check whether input data value are valid or not
	return inputpatharray;
}

int main(int argc, char** argv) {

	//input 
	std::vector<std::string> input_files=GetInputs();

	// Output root directory
	std::vector<std::string> segments;
	std::string root_dir;
	boost::split(segments, input_files[0], boost::is_any_of("/"));

	for (size_t i = 0; i < segments.size() - 4; i++) {
		root_dir += segments[i] + "/";
	}

	std::cout<<root_dir<<std::endl;
	segments.clear();

	// Output path for evaluation matrix
	std::string evaluation= root_dir +"Intermediate_Data/evaluation/evaluation.csv";
	// Open evaluation file and write header line for the csv file
	std::ofstream evaluation_file;
	//evaluation_file.open(evaluation);
	//evaluation_file<<"file_name"<<","<<"Input_Vertices"<<","<<"Input_Faces"<<
	//","<<"Generation_time_of_point_cloud"<<
	//","<<"Pointcloud_points"<<"\n";
	//evaluation_file.close();


	// Vectors that store evaluation indictors
	std::vector<int> input_vertices;
	std::vector<int> input_faces;
	std::vector<int> output_vertices;
	std::vector<double> generation_time;
	std::vector<std::string> file_names;

	for (size_t i = 0; i < input_files.size(); i++) {
		IfcParse::IfcFile input(input_files[i]);
		IfcParse::IfcFile* file = &input;
		int current_v = 0;int current_f = 0;int cur_out_v = 0;

		// Output path for point cloud
		boost::split(segments, input_files[i], boost::is_any_of("/"));

		std::string export_path = root_dir + "Intermediate_Data/Raw_point_cloud/whole_alpha_shapes/" + segments[segments.size() - 1] + "r_p.xyz";
		std::cout << export_path << std::endl;

		// Iterate through the IfcProducts, sample point clouds from each product, and export to .xyz file
		IfcSchema::IfcProduct::list::ptr prods = file->instances_by_type<IfcSchema::IfcProduct>();
		
		auto start = std::chrono::high_resolution_clock::now();
		//ifc_sampler(prods, export_path, file, input_files[i],current_v,current_f,cur_out_v);
		//ifc_product_sampler(prods, export_path, file, input_files[i],current_v,current_f,cur_out_v);
		auto stop = std::chrono::high_resolution_clock::now();
		auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
		auto duration = elapsed_time.count()/ 1000000;
		std::cout << "Elapsed time: " << duration << " seconds" << std::endl;

		//Push back the evaluation indicators to the vectors
		input_vertices.push_back(current_v);
		input_faces.push_back(current_f);
		generation_time.push_back(duration);
		file_names.push_back(segments[segments.size() - 1]);
		output_vertices.push_back(cur_out_v);

		// Output the elapsed time to the evaluation file
		//evaluation_file.open(evaluation, std::ios_base::app);
		//ifc_product_sampler(prods, export_path2, file, input_files[i]);

	}

	// print input vertices and faces
	for (size_t i = 0; i < input_vertices.size(); i++) {
		std::cout << input_vertices[i] << std::endl;
		std::cout << input_faces[i] << std::endl;
	}

	// output evaluation indicators to the evaluation file
	//evaluation_file.open(evaluation, std::ios_base::app);
	//for (size_t i = 0; i < input_vertices.size(); i++) {
	//	evaluation_file << file_names[i] << "," << input_vertices[i] << ","
	//	 << input_faces[i] << "," << generation_time[i] << "," << output_vertices[i] << "\n";
	//}

	return 0;

}