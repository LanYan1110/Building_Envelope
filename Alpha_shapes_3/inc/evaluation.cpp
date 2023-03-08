#include "evaluation.h"

int evaluation(std::string obj_point_cloud,std::string ifc_obj,std::string distances)
{
    //Function to evaluate the accuracy of the output .obj file
    //Input: obj_point_cloud: path to the .obj sampled point cloud
    //       input_point_cloud: path to the ifc .obj
    //       distances: path to the file where the distances will be stored
    
    std::vector<Point> points;
    std::vector<std::vector<std::size_t>> faces;

   //CGAL::IO::read_OBJ(ifc_obj, points,faces);

   // read polygon soup
    if(!CGAL::IO::read_polygon_soup(ifc_obj, points, faces) ||
     points.size() == 0 || faces.size() == 0){
        std::cerr << "Error: can not read input file.\n";
        return 1;
    }

    // orient polygon soup
    CGAL::Polygon_mesh_processing::orient_polygon_soup(points,faces);

    // convert to polygon mesh
    Mesh ref1;
    if(CGAL::Polygon_mesh_processing::is_polygon_soup_a_polygon_mesh(faces)){
        CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, faces, ref1);
    }

    // triangulate the polygon mesh
    CGAL::Polygon_mesh_processing::triangulate_faces(ref1);

    // Compute the distance between the mesh and the point cloud
    //CGAL::Polygon_mesh_processing::approximate_max_distance_to_point_set
    












    std::cout << "Read " << points.size() << " points and " << faces.size() << " faces" << std::endl;
    
    CGAL::Polygon_mesh_processing::orient_polygon_soup(points,faces);
    std::cout << "Oriented " << points.size() << " points and " << faces.size() << " faces" << std::endl;

    if(CGAL::Polygon_mesh_processing::is_polygon_soup_a_polygon_mesh(faces)){
        std::cout << "The polygon soup is a polygon mesh" << std::endl;
    }
    else{
        std::cout << "The polygon soup is not a polygon mesh" << std::endl;
    }





    return 0;
	
}
