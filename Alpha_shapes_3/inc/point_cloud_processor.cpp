#include "point_cloud_processor.h"

void random_simplify(
    std::string input_path,std::string output_path) {
    
    //function to construct alpha shape from a point
    //input: point cloud, .XYZ file
    //input: output path for processed point cloud, .XYZ file
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
    std::cerr <<"Number of input points "<<points.size() << ".\n";

    //Simplification: Random_simplify_point_set
    std::vector<std::vector<Point>> copiedVecs(5, std::vector<Point>(points.size()));
    for (int i = 0; i < 5; ++i) {
        std::copy(points.begin(), points.end(), copiedVecs[i].begin());
    }

    int j=1; // control percentage
    for (int i = 0; i < 5; ++i) {
        double percentage = 0.1*j;
        auto iterator_to_first_to_remove= CGAL::random_simplify_point_set(copiedVecs[i],percentage); 
        copiedVecs[i].erase(iterator_to_first_to_remove, points.end());
        // Optional: after erase(), shrink_to_fit to trim excess capacity
        copiedVecs[i].shrink_to_fit();
        j++;

        std::cerr << "Number of output points" << copiedVecs[i].size() << ".\n";
        std::cerr << "Percentage of points removed" << (input_size - copiedVecs[i].size()) / (double)input_size << ".\n";
    
    }

    // print simplified point cloud
    for (int i = 0; i < 5; ++i) {
        std::cout << "Number of output points" << copiedVecs[i].size() << ".\n";
    }

    //Output simplified point cloud
    std::ofstream os(output_path);
    if (!os.is_open()) {
        std::cerr << "Error: could not open file " << output_path << std::endl;
        exit(1);
    }
    // os << copiedVecs[i].size() << std::endl;
    // for (int i = 0; i < copiedVecs[i].size(); ++i)
    // {
    //     os << random_simplified[0][i].x() << " " << random_simplified[0][i].y() << " " << random_simplified[0][i].z() << std::endl;
    // }

    
    os.close();
 
}

