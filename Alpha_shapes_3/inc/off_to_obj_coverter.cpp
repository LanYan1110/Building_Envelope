#include "off_to_obj_converter.h"

void off_to_obj(std::string input_filename, std::string output_filename) {
    std::ifstream input_file(input_filename);
    if (!input_file) {
        std::cerr << "Error opening input file: " << input_filename << std::endl;
        return;
    }

    std::string line;
    std::getline(input_file, line);
    if (line != "OFF") {
        std::cerr << "Invalid file format: " << input_filename << std::endl;
        return;
    }

    int num_vertices, num_faces, num_edges;
    std::getline(input_file, line);
    std::istringstream iss(line);
    iss >> num_vertices >> num_faces >> num_edges;

    std::vector<std::vector<float>> vertices(num_vertices, std::vector<float>(3));
    for (int i = 0; i < num_vertices; i++) {
        std::getline(input_file, line);
        std::istringstream iss(line);
        iss >> vertices[i][0] >> vertices[i][1] >> vertices[i][2];
    }

    std::vector<std::vector<int>> faces(num_faces, std::vector<int>(3));
    for (int i = 0; i < num_faces; i++) {
        std::getline(input_file, line);
        std::istringstream iss(line);
        int num_vertices_per_face;
        iss >> num_vertices_per_face;
        if (num_vertices_per_face != 3) {
            std::cerr << "Invalid file format: " << input_filename << std::endl;
            return;
        }
        iss >> faces[i][0] >> faces[i][1] >> faces[i][2];
    }

    input_file.close();

    std::ofstream output_file(output_filename);
    if (!output_file) {
        std::cerr << "Error opening output file: " << output_filename << std::endl;
        return;
    }

    for (int i = 0; i < num_vertices; i++) {
        output_file << "v " << vertices[i][0] << " " << vertices[i][1] << " " << vertices[i][2] << "\n";
    }

    for (int i = 0; i < num_faces; i++) {
        output_file << "f " << faces[i][0]+1 << " " << faces[i][1]+1 << " " << faces[i][2]+1 << "\n";
    }

    output_file.close();
}