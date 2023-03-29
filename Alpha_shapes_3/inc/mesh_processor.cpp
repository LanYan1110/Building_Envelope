#include "mesh_processor.h"
#include "point_cloud_processor.h"


void collapse_gh(Mesh& mesh,double stop_ratio){
    // initialize gh policy
    GHPolicies gh_policies(mesh);

   SMS::Bounded_normal_change_filter<> filter;
   SMS::Count_ratio_stop_predicate<Mesh> stop(stop_ratio);

   /*int r = SMS::edge_collapse(mesh, stop,CGAL::parameters::
   get_cost(gh_policies.get_cost()).get_placement(gh_policies.get_placement()).filter(filter));*/

   //int r = SMS::edge_collapse(mesh, stop);
//    int r = SMS::edge_collapse(mesh, stop,CGAL::parameters::
//    get_cost(gh_policies.get_cost()).get_placement(gh_policies.get_placement()));
   int r = SMS::edge_collapse(mesh, stop,CGAL::parameters::filter(filter));
   std::cout << "\nFinished!\n" << r << " edges removed.\n" << mesh.number_of_edges() << " final edges.\n";
   return;
}

int mesh_processor(std::string input_mesh, std::string output_mesh,
int& out_v, int& out_f)
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
    // double stop_ratio =0.1;
    // SMS::Count_ratio_stop_predicate<Mesh> stop(0.1);

    double stop_ratio;
    std::cout << "Enter the stop ratio: ";
    std::cin >> stop_ratio;
    collapse_gh(surface_mesh, stop_ratio);

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

    out_v = num_vertices;
    out_f = num_faces;
    
    return 0;


}


