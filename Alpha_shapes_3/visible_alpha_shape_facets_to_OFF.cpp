#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Alpha_shape_3.h>
#include <CGAL/Alpha_shape_cell_base_3.h>
#include <CGAL/Alpha_shape_vertex_base_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/IO/OBJ.h>

#include <fstream>
#include <sstream>
#include <vector>

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
    std::unordered_map< Alpha_shape_3::Vertex_handle, std::size_t> vids;
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

void multiple_alpha_shape(std::string input_path, std::string output_path) {
    std::vector<Point> points;
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

    // Generate output path
    std::ofstream output(output_path);

    // Compute 3D alpha shapes
    std::vector<Alpha_shape_3> alpha_shapes;

    std::vector< Alpha_shape_3::Facet > regular_facets;
    std::vector<Alpha_shape_3::Facet> filtered_regular_facets;
    std::unordered_map< Alpha_shape_3::Vertex_handle, std::size_t> vids;


    for (auto product : products) {
        // Compute Alpha solid for every product
        Alpha_shape_3 as(product.begin(), product.end());
        Alpha_shape_3::NT alpha_solid = as.find_alpha_solid();
        as.set_alpha(alpha_solid);

        //write_alpha_shapes_to_file(as, outoff);

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
        std::vector<Alpha_shape_3::Facet> filtered_regular_facets;
        as.get_alpha_shape_facets(std::back_inserter(regular_facets), Alpha_shape_3::REGULAR);

        // Extract vertices and facets for every 3D alpha shape
        std::unordered_map< Alpha_shape_3::Vertex_handle, std::size_t> vids;

        // Extract visible facets from every 3D alpha shape
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

        //Output to off format
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

 
}



int main()
{
    
    //read input
    std::string dir = "C:/Users/seuya/Documents/Thesis/Building_Envelope";
    std::string input_file = "01_General_Classroom1111.ifc_0.1";
    std::stringstream ss;
    ss << dir << "/Intermediate_Data/vertices/" << input_file;
    std::string input_path = ss.str();

    // output path for single alpha shape
    std::stringstream ss2;
    ss2 << dir << "/Intermediate_Data/" << "single_alpha_shape_"<<input_file << ".off";
    std::string output_path = ss2.str();
    std::cout << output_path << std::endl;

    single_alpha_shape(input_path, output_path);

    // output path for individual alpha shape
    std::stringstream ss3;
    ss3 << dir << "/Intermediate_Data/" <<"multiple_alpha_shape_"<< input_file << ".off";
    std::string output_path2 = ss3.str();
    std::cout << output_path2 << std::endl;

    return 0;
}