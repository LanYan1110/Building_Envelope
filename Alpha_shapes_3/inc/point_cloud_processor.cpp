#include "point_cloud_processor.h"

std::string clear_slash(std::string const& path_of_file, std::string const& d_slash){
    size_t index_of_slash = path_of_file.find_last_of(d_slash);
    std::string file_name = path_of_file.substr(index_of_slash + 1);
    return file_name;
}

void random_simplify(std::string input_path,std::string output_dir) {
    //function to simplify point cloud using random_simplify_point_set
    //input: point cloud, .XYZ file
    //input: output directory for processed point cloud, .XYZ file
    //return: none

    std::vector<Point> points;
    std::ifstream is(input_path);
    if (!is.is_open()) {
    std::cerr << "Error: could not open file " << input_path << std::endl;
    exit(1);
    }

    // read points from file
    int n;
    is >> n;
    double x, y, z;
    for (int i = 0; i < n; ++i)
    {
        is >> x >> y >> z;
        points.push_back(Point(x, y, z));
    }
    int input_size = points.size();

    //Simplification: Random_simplify_point_set
    std::vector<std::vector<Point>> copiedVecs(5, std::vector<Point>(points.size()));
    for (int i = 0; i < 5; ++i) {
        std::copy(points.begin(), points.end(), copiedVecs[i].begin());
    }

    int j=1; // control percentage
    // reduce 10%, 20%, 30%, 40%, 50% of points
    for (int i = 0; i < 5; ++i) {
        double percentage = 10*j;
        auto iterator_to_first_to_remove= CGAL::random_simplify_point_set(copiedVecs[i],percentage); 
        copiedVecs[i].erase(iterator_to_first_to_remove, copiedVecs[i].end());
        // Optional: after erase(), shrink_to_fit to trim excess capacity
        copiedVecs[i].shrink_to_fit();

        // Export path for simplified point cloud
        std::string export_path = output_dir + clear_slash(input_path) + "_random_simplify_" + std::to_string(percentage) + ".xyz";
        std::ofstream os(export_path);
        if (!os.is_open()) {
            std::cerr << "Error: could not open file " << export_path << std::endl;
            exit(1);
        }

        os << copiedVecs[i].size() << std::endl;
        for (int j = 0; j < copiedVecs[i].size(); ++j){
            os << copiedVecs[i][j].x() << " " << copiedVecs[i][j].y() << " " << copiedVecs[i][j].z() << std::endl;
        }
        
        os.close();
        j++;

        std::cerr << "Number of output points" << copiedVecs[i].size() << ".\n";
        std::cerr << "Percentage of points removed" << (input_size - copiedVecs[i].size()) / (double)input_size << ".\n";
    
    }
}


void grid_simplify(std::string input_path,std::string output_dir) {
    //function to simplify point cloud using grid_simplify_point_set
    //input: point cloud, .XYZ file
    //input: output directory for processed point cloud, .XYZ file
    //return: none

    std::vector<Point> points;
    std::ifstream is(input_path);
    if (!is.is_open()) {
    std::cerr << "Error: could not open file " << input_path << std::endl;
    exit(1);
    }

    // read points from file
    int n;
    is >> n;
    double x, y, z;
    for (int i = 0; i < n; ++i)
    {
        is >> x >> y >> z;
        points.push_back(Point(x, y, z));
    }
    int input_size = points.size();

    // Simplification: Grid_simplify_point_set
    std::vector<std::vector<Point>> copiedVecs(5, std::vector<Point>(points.size()));
    for (int i = 0; i < 5; ++i) {
        std::copy(points.begin(), points.end(), copiedVecs[i].begin());
    }

    std::vector<double> cell_size = {0.05, 0.1, 0.5, 1.0, 1.5};
    unsigned int min_points_per_cell=1;

    for (int i = 0; i < 5; ++i) {
        auto iterator_to_first_to_remove
        = CGAL::grid_simplify_point_set
        (copiedVecs[i], cell_size[i],
            CGAL::parameters::min_points_per_cell(min_points_per_cell)); // optional
        copiedVecs[i].erase(iterator_to_first_to_remove, copiedVecs[i].end());
        // Optional: after erase(), shrink_to_fit to trim excess capacity
        copiedVecs[i].shrink_to_fit();

        // Export path for simplified point cloud
        std::string export_path = output_dir + clear_slash(input_path) + "_grid_simplify_" + std::to_string(cell_size[i]) + ".xyz";
        std::ofstream os(export_path);
        if (!os.is_open()) {
            std::cerr << "Error: could not open file " << export_path << std::endl;
            exit(1);
        }

        os << copiedVecs[i].size() << std::endl;
        for (int j = 0; j < copiedVecs[i].size(); ++j){
            os << copiedVecs[i][j].x() << " " << copiedVecs[i][j].y() << " " << copiedVecs[i][j].z() << std::endl;
        }
        
        os.close();

        std::cerr << "Number of output points" << copiedVecs[i].size() << ".\n";
        std::cerr << "Percentage of points removed" << (input_size - copiedVecs[i].size()) / (double)input_size << ".\n";
    
    }
}
