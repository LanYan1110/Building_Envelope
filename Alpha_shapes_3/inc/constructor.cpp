#include "constructor.h"

void alpha_shape_constructor(std::vector<Point> points) {
    
    //function to construct alpha shape from a point cannot overload functions distinguished by return type aloncloud
    //input: point cloud
    //input: alpha shape to be constructed
    //output: None

    std::cerr << points.size() << " points read.\n";

    // compute alpha shape
    Alpha_shape_3 as(points.begin(), points.end());
    Alpha_shape_3::NT alpha_solid = as.find_alpha_solid();
    as.set_alpha(alpha_solid);

    std::cerr << "alpha_solid = " << alpha_solid << "\n";
    std::cerr << as.number_of_solid_components() << " number of solid components\n";

    if (as.number_of_solid_components() == 0) {
    std::cerr << "Error: alpha shape has no solid components" << std::endl;
    exit(1);
    }



    return;

}