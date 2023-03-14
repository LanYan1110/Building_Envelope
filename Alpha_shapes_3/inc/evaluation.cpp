#include "evaluation.h"

int evaluation(std::string obj_point_cloud,std::string ifc_obj,std::string distances)
{
    //Function to evaluate the accuracy of the output .obj file
    //Input: obj_point_cloud: path to the .obj sampled point cloud
    //       input_point_cloud: path to the ifc .obj
    //       distances: path to the file where the distances will be stored
    // 
    // mesh 
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
    PMP::orient_polygon_soup(points,faces);

    // convert to polygon mesh
    Mesh ref1;
    if(PMP::is_polygon_soup_a_polygon_mesh(faces)){
        PMP::polygon_soup_to_polygon_mesh(points, faces, ref1);
    }

    // triangulate the polygon mesh
    PMP::triangulate_faces(ref1);

    // Compute the distance between the mesh and the point cloud
    double precision=0.0001;
    // Time the process
    auto start = std::chrono::high_resolution_clock::now();

    for (auto p: points){
        std::vector <Point> points2;
        points2.push_back(p);
        double distance=PMP::approximate_max_distance_to_point_set(ref1,points2,precision);
        std::cout << "Distance: " << distance << std::endl;
        // open the distance text file and append the distance
        std::ofstream myfile;
        myfile.open (distances, std::ios_base::app);
        myfile << distance << std::endl;
        myfile.close();
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;

    return 0;
	
}




int point_set_to_mesh_distances(std::string obj_point_cloud,std::string ifc_obj,std::string distances){
    //read the point cloud and the mesh
    //read the point cloud from .xyz file
    std::vector<Point> o_points;
    std::ifstream input(obj_point_cloud);
    double x, y, z;
	for (int i = 0; i <4838; ++i){
		input >> x >> y >> z;
		o_points.push_back(Point(x, y, z));
	}

    // read the mesh from .obj file
    std::vector<Point> points;
    std::vector<std::vector<std::size_t>> triangles;

   //CGAL::IO::read_OBJ(ifc_obj, points,faces);
   // read polygon soup
    if(!CGAL::IO::read_polygon_soup(ifc_obj, points, triangles) ||
     points.size() == 0 || triangles.size() == 0){
        std::cerr << "Error: can not read input file.\n";
        return 1;
    }
    // orient polygon soup
    PMP::orient_polygon_soup(points,triangles);
    // convert to polygon mesh
    Mesh m;
    if(PMP::is_polygon_soup_a_polygon_mesh(triangles)){
        PMP::polygon_soup_to_polygon_mesh(points, triangles, m);
    }
    // triangulate the polygon mesh
    PMP::triangulate_faces(m);

    // compute the AABB tree for the mesh
    Tree tree(faces(m).first, faces(m).second, m);

    // find the closest point for every point
    for (auto p: o_points){
        Point closest = tree.closest_point(p);
        std::cout << "Closest point: " << closest << std::endl;
        // open the distance text file and append the distance
        std::ofstream myfile;
        myfile.open (distances, std::ios_base::app);
        std::cout<<"current distance is: "<<CGAL::squared_distance(p,closest)<< std::endl;
        myfile << CGAL::squared_distance(p,closest) << std::endl;
        myfile.close();
    }




    return 0;
}