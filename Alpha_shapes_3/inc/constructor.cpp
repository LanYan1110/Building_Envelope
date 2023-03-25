#include "constructor.h"
#include "point_cloud_processor.h"

void indi_alpha_shape_constructor
(std::string input_path,std::string output_path){
    // function to construct alpha shape for every IfcProduct
    // input: point cloud, .XYZ file
    // input: output path for alpha shape, off file
    // output: None

    //read the points for each IfcProduct
    std::ifstream file(input_path);
    std::string line;
    std::vector<std::vector<Point>> products;
    std::vector<Point> product;
 
    while (std::getline(file, line)) {
        if (line.find("Product") != std::string::npos) {
            if (!product.empty()) products.push_back(product);
            product.clear();
        }
        else {
            double x, y, z;
            sscanf(line.c_str(), "%lf %lf %lf", &x, &y, &z);
            product.push_back(Point(x, y, z));
        }
    }
    products.push_back(product); // adding last product
    file.close();

    std::cout<<"Number of products: "<<products.size()<<std::endl;
    
    // print out the number of points for each product
    for (int i=0;i<products.size();i++) {
        std::cout<<"Product "<<i<<" has "<<products[i].size()<<" points."<<std::endl;
    }


    // const std::vector<Alpha_shape_3> alphaShapes;

    // std::vector<std::unique_ptr<Alpha_shape_3>> alphaShapes;

    // for (auto product : products) {
    //     // Compute Alpha solid for every product
    //     Alpha_shape_3* as=new Alpha_shape_3(product.begin(), product.end());
    //     Alpha_shape_3::NT alpha_solid = as.find_alpha_solid();
    //     as.set_alpha(alpha_solid);
    // }

    // // Write alpha shapes to one single off file
    // std::ofstream output_file(output_path);
    // output_file << "OFF" << std::endl;

    return;
}

void alpha_shape_constructor
(std::string input_path,std::string output_path,
int& out_v, int& out_f,double alpha) {
    
    //function to construct alpha shape from a point cannot overload functions distinguished by return type aloncloud
    //input: point cloud, .XYZ file
    //input: output path for alpha shape, obj file
    //output: None
    
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

    std::cerr <<"Number of input points"<<points.size() << ".\n";

    // compute alpha shape
    Alpha_shape_3 as(points.begin(), points.end());
    //Alpha_shape_3::NT alpha_solid = as.find_alpha_solid();
    // as.set_alpha(alpha_solid);
    // as.set_alpha(*opt);
    as.set_alpha(alpha);

    //std::cerr << "alpha_solid = " << alpha_solid << "\n";
    //std::cerr << as.number_of_solid_components() << " number of solid components\n";

    // if (as.number_of_solid_components() == 0) {
    // std::cerr << "Error: alpha shape has no solid components" << std::endl;
    // exit(1);
    // }

    // collect alpha-shape facets accessible from the infinity 
    // marks the cells that are in the same component as the infinite vertex by flooding 
    std::unordered_set< Alpha_shape_3::Cell_handle > marked_cells;
    std::vector< Alpha_shape_3::Cell_handle > queue;
    queue.push_back(as.infinite_cell());

    while (!queue.empty())
    {
        Alpha_shape_3::Cell_handle back = queue.back();
        queue.pop_back();

        if (!marked_cells.insert(back).second) continue; //already visited

        for (int i = 0; i < 4; ++i)
        {
            if (as.classify(Alpha_shape_3::Facet(back, i)) == Alpha_shape_3::EXTERIOR &&
                marked_cells.count(back->neighbor(i)) == 0)
                queue.push_back(back->neighbor(i));
        }
    }

    // filter regular facets to restrict them to those adjacent to a marked cell
    std::vector< Alpha_shape_3::Facet > regular_facets;
    as.get_alpha_shape_facets(std::back_inserter(regular_facets), Alpha_shape_3::REGULAR);

    std::vector<Alpha_shape_3::Facet> filtered_regular_facets;
    for (Alpha_shape_3::Facet f : regular_facets)
    {
        if (marked_cells.count(f.first) == 1)
            filtered_regular_facets.push_back(f);
        else
        {
            f = as.mirror_facet(f);
            if (marked_cells.count(f.first) == 1)
                filtered_regular_facets.push_back(f);
        }
    }

    // dump into OFF format
    // assign an id per vertex
    std::unordered_map<Alpha_shape_3::Vertex_handle, std::size_t> vids;
    points.clear();

    for (Alpha_shape_3::Facet f : filtered_regular_facets)
    {
        for (int i = 1; i < 4; ++i)
        {
            Alpha_shape_3::Vertex_handle vh = f.first->vertex((f.second + i) % 4);
            if (vids.insert(std::make_pair(vh, points.size())).second)
                points.push_back(vh->point());
        }
    }

    std::ofstream output(output_path);
    output << "OFF\n " << points.size() << " " << filtered_regular_facets.size() << " 0\n";
    std::copy(points.begin(), points.end(), std::ostream_iterator<Point>(output, "\n"));
    
    for (const Alpha_shape_3::Facet& f : filtered_regular_facets){
        output << 3;

        for (int i = 0; i < 3; ++i){
            Alpha_shape_3::Vertex_handle vh = f.first->vertex(as.vertex_triple_index(f.second, i));
            output << " " << vids[vh];
        }
        output << "\n";
    }

    out_v = points.size();
    out_f = filtered_regular_facets.size();
    
    return;

}