#include "constructor.h"

Alpha_shape_3 alpha_shape_constructor(std::string input_path) {
    
    //function to construct alpha shape from a point cannot overload functions distinguished by return type aloncloud
    //input: path to point cloud
    //return: alpha shape

    std::vector<Point> points;
    std::ifstream is(input_path);

    // read points from file
    int n;
    is >> n;
    double x, y, z;
    for (int i = 0; i < n; ++i)
    {
        is >> x >> y >> z;
        points.push_back(Point(x, y, z));
    }

    std::cerr << points.size() << " points read.\n";

    // compute alpha shape
    Alpha_shape_3 as(points.begin(), points.end());
    Alpha_shape_3::NT alpha_solid = as.find_alpha_solid();
    as.set_alpha(alpha_solid);

    std::cerr << "alpha_solid = " << alpha_solid << "\n";
    std::cerr << as.number_of_solid_components() << " number of solid components\n";

    return as;

}