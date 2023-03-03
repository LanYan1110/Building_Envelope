#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/grid_simplify_point_set.h>
#include <CGAL/property_map.h>

#include <CGAL/Alpha_shape_3.h>
#include <CGAL/Alpha_shape_cell_base_3.h>
#include <CGAL/Alpha_shape_vertex_base_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/IO/OBJ.h>

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
//#include <glob>
#include <vector>

namespace fs = std::filesystem;

#include <unordered_set>
#include <unordered_map>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Gt;

typedef CGAL::Alpha_shape_vertex_base_3<Gt>          Vb;
typedef CGAL::Alpha_shape_cell_base_3<Gt>            Fb;
typedef CGAL::Triangulation_data_structure_3<Vb, Fb>  Tds;
typedef CGAL::Delaunay_triangulation_3<Gt, Tds>       Triangulation_3;
typedef CGAL::Alpha_shape_3<Triangulation_3>         Alpha_shape_3;

typedef Gt::Point_3                                  Point;
typedef Alpha_shape_3::Alpha_iterator                Alpha_iterator;


std::string clear_slash(std::string const& path_of_file, std::string const& d_slash = "/\\")
{
    size_t index_of_slash = path_of_file.find_last_of(d_slash);
    std::string file_name = path_of_file.substr(index_of_slash + 1);
    return file_name;
}


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

void single_alpha_shape(std::string input_path, std::string output_path) {
    std::vector<Point> points;
    std::ifstream is(input_path);

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
    for (const Alpha_shape_3::Facet& f : filtered_regular_facets)
    {
        output << 3;

        for (int i = 0; i < 3; ++i)
        {
            Alpha_shape_3::Vertex_handle vh = f.first->vertex(as.vertex_triple_index(f.second, i));
            output << " " << vids[vh];
        }
        output << "\n";
    }
}

//void multiple_alpha_shape(std::string input_path, std::string output_path) {
//    
//    std::ifstream file(input_path);
//    std::string line;
//    std::vector<std::vector<Point>> products;
//    std::vector<Point> product;
//
//    while (std::getline(file, line)) {
//        if (line.find("Product") != std::string::npos) {
//            if (!product.empty()) products.push_back(product);
//            product.clear();
//        }
//        else {
//            double x, y, z;
//            sscanf(line.c_str(), "%lf %lf %lf", &x, &y, &z);
//            product.push_back(Point(x, y, z));
//        }
//    }
//    products.push_back(product); // adding last product
//    file.close();
//
//    // Generate output path
//    std::ofstream output(output_path);
//
//    // Compute 3D alpha shapes
//    std::vector<Alpha_shape_3> alpha_shapes;
//
//    int num_of_v= 0;
//    int num_of_f= 0;
//    std::vector<int> num_of_v_s= {};
//    std::vector<int> num_of_f_s= {};
//
//    for (auto points : products) {
//        // Compute Alpha Shape for every product
//        Alpha_shape_3 as(points.begin(), points.end());
//        Alpha_shape_3::NT alpha_solid = as.find_alpha_solid();
//        as.set_alpha(alpha_solid);
//        alpha_shapes.emplace_back(as);
//        num_of_v = num_of_v + points.size();
//        num_of_v_s.emplace_back(num_of_v);
//    }
//
//}



int main()
{
    
    //Alpha shape for the whole building
    std::string whole_dir = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/vertices/whole_alpha_shapes/";
    std::vector<std::string> input_files1 = GetInputs(whole_dir);

    for (size_t i = 0; i < input_files1.size(); i++) {
        // Export path for one alpha shape model
        std::string export_path1 = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/OFF/whole_alpha_shapes/" + clear_slash(input_files1[i])+ ".off";
        std::cout << export_path1 << std::endl;
        std::cout << "input " << input_files1[i] << std::endl;
        single_alpha_shape(input_files1[i], export_path1);
    }

    //Alpha shape for whole building
    //std::string indi_dir = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/vertices/indi_alpha_shapes/";
    //std::vector<std::string> input_files2 = GetInputs(indi_dir);

    //for (size_t i = 0; i < input_files2.size(); i++) {
    //    // Export path for one alpha shape model
    //    std::string export_path2 = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/OFF/whole_alpha_shapes/" + clear_slash(input_files2[i]) + ".off";
    //    std::cout << export_path2 << std::endl;
    //    std::cout << "input " << input_files2[i] << std::endl;
    //    single_alpha_shape(input_files2[i], export_path2);
    //    //multiple_alpha_shape(input_files2[i], export_path2);
    //}

    //Alpha shape for individual products, testing
    std::string indi_dir_test = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/vertices/indi_alpha_shapes/test/";
    std::vector<std::string> input_files3 = GetInputs(indi_dir_test);

    for (size_t i = 0; i < input_files3.size(); i++) {
        // Export path for one alpha shape model
        std::string export_path3 = "C:/Users/seuya/Documents/Thesis/Intermediate_Data/OFF/whole_alpha_shapes/" + clear_slash(input_files3[i]) + ".off";
        std::cout << export_path3 << std::endl;
        std::cout << "input " << input_files3[i] << std::endl;
        //single_alpha_shape(input_files1[i], export_path1);
        //multiple_alpha_shape(input_files3[i], export_path3);
    }

    return 0;
}