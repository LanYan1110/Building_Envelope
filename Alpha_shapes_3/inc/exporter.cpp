#include "exporter.h"

void alpha_shape_exporter(Alpha_shape_3 as,std::vector<Point> points,std::string output_path) {
    //function to export alpha shape to .obj file
    //input: alpha shape, path to output file
    //return: none
    
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

    // Output the alpha shape in OBJ format
    std::ofstream output_obj(output_path + ".obj");
    for (const Point& p : points)
        output_obj << "v " << p << "\n";
    for (const Alpha_shape_3::Facet& f : filtered_regular_facets)
    {
        output_obj << "f";
        for (int i = 0; i < 3; ++i)
        {
            Alpha_shape_3::Vertex_handle vh = f.first->vertex(as.vertex_triple_index(f.second, i));
            output_obj << " " << vids[vh] + 1;
        }
        output_obj << "\n";
    }

    std::cerr << "done.\n";
    std::ofstream os(output_path);
    os << as;
    os.close();
}