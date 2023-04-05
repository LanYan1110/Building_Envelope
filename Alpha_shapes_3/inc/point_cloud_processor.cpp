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

void hierachy_simplify(std::string input_path,std::string output_dir) {
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

    // Simplification: hierachy_simplify

    CGAL::Timer task_timer; task_timer.start();
    // simplification by clustering using erase-remove idiom
    points.erase(CGAL::hierarchy_simplify_point_set(points,
                                                    CGAL::parameters::size(100)// Max cluster size
                                                                    .maximum_variation(0.01)), // Max surface variation
                points.end());
    std::size_t memory = CGAL::Memory_sizer().virtual_size();
    std::cout << points.size () << " point(s) kept, computed in "
                << task_timer.time() << " seconds, "
                << (memory>>20) << " Mib allocated." << std::endl;

        // Export path for simplified point cloud
        std::string export_path = output_dir + clear_slash(input_path) + "_hierichy_simplify_" + ".xyz";
        std::ofstream os(export_path);
        if (!os.is_open()) {
            std::cerr << "Error: could not open file " << export_path << std::endl;
            exit(1);
        }

        os << points.size() << std::endl;
        for (int j = 0; j < points.size(); ++j){
            os << points[j].x() << " " << points[j].y() << " " << points[j].z() << std::endl;
        }
        
        os.close();

        std::cerr << "Number of output points" << points.size() << ".\n";
        std::cerr << "Percentage of points removed" << (input_size - points.size()) / (double)input_size << ".\n";
    
}

int wlop_simplify(std::string input_path,std::string output_dir) {
    //function to simplify point cloud using wlop algorithm
    //input: point cloud, .XYZ file
    //input: output directory for processed point cloud, .XYZ file
    //return: 0 or 1

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

    // Simplification: wlop_simplify
    std::vector<Point> output;
    //parameters
    const double retain_percentage =5;   // percentage of points to retain.
    //compute the optimal neighbor radius
    double neighbor_radius;
    //compute the bbox of the point set
    std::array<Point, 8> obb_points;
    CGAL::oriented_bounding_box(points, obb_points,CGAL::parameters::use_convex_hull(true));
    double diag=0; double cur_dist=0;
    for (int i = 1; i < 8; i++) {
       cur_dist=CGAL::squared_distance(obb_points[0],obb_points[i]);
       if(cur_dist>diag){
              diag=cur_dist;
       }
    }
    
    neighbor_radius=4*sqrt(diag/points.size());
    std::cout << "neighbor_radius: " << neighbor_radius << std::endl;
    
    // Output filename
    std::string output_filename = output_dir + clear_slash(input_path) + "_wlop_simplify_" + ".xyz";
    
    CGAL::wlop_simplify_and_regularize_point_set<Concurrency_tag>
        (points, std::back_inserter(output),
        CGAL::parameters::select_percentage(retain_percentage).
        neighbor_radius (neighbor_radius));

    std::ofstream out(output_filename, std::ofstream::out);
	out <<output.size() << "\n";
	for (int i = 0; i < output.size(); i++) {
		out << output[i].x() << " " << output[i].y() << " " << output[i].z() << "\n";
	}
	out.close();
    
    
    return 0;
}
