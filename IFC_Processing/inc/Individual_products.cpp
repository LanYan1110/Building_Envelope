#include "Individual_products.h"

#include "helper.h"

void extract_vertices_elke(IfcSchema::IfcProduct::list::ptr prods, std::string output, IfcParse::IfcFile* file, std::string input)
{
	std::vector<std::vector<gp_Pnt>> points;
	IfcGeom::Kernel my_kernel(file);
	helperCluster* hCluster = new helperCluster;
	helper* h = new helper(input);

	int ifc_p_i = 0;

	for (auto it = prods->begin(); it != prods->end(); it++){
		// Get the placement
		IfcSchema::IfcProduct* prod = *it;
		ifc_p_i++;

		if (prod->hasRepresentation()){
			if (prod->data().type()->name() == "IfcSite" ||
				prod->data().type()->name() == "IfcStair" ||
				prod->data().type()->name() == "IfcSpace" ||
				prod->data().type()->name() == "IfcRailing" ||
				prod->data().type()->name() == "IfcAnnotation") {
				continue;
			}

			std::vector<gp_Pnt> prod_points;
			
			TopoDS_Shape shape = h->getObjectShape(prod);
			TopExp_Explorer expl_v;

			for (expl_v.Init(shape, TopAbs_VERTEX); expl_v.More(); expl_v.Next()){
				TopoDS_Vertex vertex = TopoDS::Vertex(expl_v.Current());
				gp_Pnt p = BRep_Tool::Pnt(vertex);
				prod_points.emplace_back(p);
			}
		points.emplace_back(prod_points);
		}
	}

	ofstream out(output, std::ofstream::out);

	if (points.size() >= 4) {
		for (int i = 0; i < points.size(); i++) {
			out << "Product" << i << "\n";
			//out << points[i].size() << "\n";
			for (int j = 0; j < points[i].size(); j++) {
				out << points[i][j].X() << " " << points[i][j].Y() << " " << points[i][j].Z() << "\n";
			}
		}
	}


	out.close();
	return;
}
