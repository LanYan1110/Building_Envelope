#include "mesh_processor.h"

int mesh_processor(std::string input_mesh, std::string output_mesh, double stop_ratio)
{
    std::vector<Point> points;
    std::vector<std::vector<std::size_t>> faces;
    // read polygon soup
    if(!CGAL::IO::read_polygon_soup(input_mesh, points, faces) ||
     points.size() == 0 || faces.size() == 0){
        std::cerr << "Error: can not read input file.\n";
        return 1;
    }

    // orient polygon soup
    PMP::orient_polygon_soup(points,faces);
    
    // convert to polygon mesh
    Mesh surface_mesh;
    if(PMP::is_polygon_soup_a_polygon_mesh(faces)){
        PMP::polygon_soup_to_polygon_mesh(points, faces, surface_mesh);
    }

    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
    // In this example, the simplification stops when the number of undirected edges
    // drops below 10% of the initial count
    //double stop_ratio = (argc > 2) ? std::stod(argv[2]) : 0.1;
    SMS::Count_ratio_stop_predicate<Mesh> stop(stop_ratio);
    int r = SMS::edge_collapse(surface_mesh, stop);
    std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    std::cout << "\nFinished!\n" << r << " edges removed.\n" << surface_mesh.number_of_edges() << " final edges.\n";
    std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms" << std::endl;
    CGAL::IO::write_polygon_mesh(output_mesh, surface_mesh, CGAL::parameters::stream_precision(17));
    return EXIT_SUCCESS;
}


