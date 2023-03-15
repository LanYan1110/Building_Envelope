#include "mesh_processor.h"
#include "point_cloud_processor.h"

int mesh_processor(std::string input_mesh, std::string output_mesh, double stop_ratio,std::string evaluation)
{
    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
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

    // In this example, the simplification stops when the number of undirected edges
    // drops below 10% of the initial count
    //double stop_ratio = (argc > 2) ? std::stod(argv[2]) : 0.1;
    SMS::Count_ratio_stop_predicate<Mesh> stop(stop_ratio);
    int r = SMS::edge_collapse(surface_mesh, stop);
    std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    std::cout << "\nFinished!\n" << r << " edges removed.\n" << surface_mesh.number_of_edges() << " final edges.\n";
    std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms" << std::endl;
    CGAL::IO::write_polygon_mesh(output_mesh, surface_mesh, CGAL::parameters::stream_precision(17));
    

        // get the number of vertices
    int num_vertices = 0;
    for (Vertex_iterator vit = surface_mesh.vertices_begin(); vit != surface_mesh.vertices_end(); ++vit) {
        ++num_vertices;
    }

    int num_faces = 0;
    for (Face_iterator fit =surface_mesh.faces_begin(); fit != surface_mesh.faces_end(); ++fit) {
        ++num_faces;
    }

    std::ofstream evaluation_file;
    evaluation_file.open(evaluation, std::ios_base::app);
    evaluation_file <<clear_slash(input_mesh)<< "," <<num_vertices << ","
	<< num_faces<<","<<time<< "\n";
    evaluation_file.close();
    
    return 0;


}


