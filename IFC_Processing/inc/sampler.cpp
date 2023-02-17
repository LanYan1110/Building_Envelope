#include "sampler.h"
#include "helper.h"

void ifc_product_sampler(IfcSchema::IfcProduct::list::ptr prods, std::string output, IfcParse::IfcFile* file, std::string input) {
	std::vector<gp_Pnt> points;
	IfcGeom::Kernel my_kernel(file);
	helperCluster* hCluster = new helperCluster;
	helper* h = new helper(input);
	int num_of_v = 0;
	int num_of_f = 0;

	for (auto it = prods->begin(); it != prods->end(); it++){
		std::vector<std::vector<gp_Pnt>> points;
		IfcGeom::Kernel my_kernel(file);
		helperCluster* hCluster = new helperCluster;
		helper* h = new helper(input);

		int ifc_p_i = 0;

		for (auto it = prods->begin(); it != prods->end(); it++) {
			// Get the placement
			IfcSchema::IfcProduct* prod = *it;
			ifc_p_i++;

			if (prod->hasRepresentation()) {
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

				for (expl_v.Init(shape, TopAbs_VERTEX); expl_v.More(); expl_v.Next()) {
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


}


void ifc_sampler(IfcSchema::IfcProduct::list::ptr prods, std::string output, IfcParse::IfcFile* file, std::string input){
	std::vector<gp_Pnt> points;
	IfcGeom::Kernel my_kernel(file);
	helperCluster* hCluster = new helperCluster;
	helper* h = new helper(input);
	int num_of_v = 0;
	int num_of_f = 0;

	for (auto it = prods->begin(); it != prods->end(); it++)
	{
		IfcSchema::IfcProduct* prod = *it;

		if (prod->hasRepresentation())
		{
			if (prod->data().type()->name() == "IfcSite" ||
				prod->data().type()->name() == "IfcStair" ||
				prod->data().type()->name() == "IfcSpace" ||
				prod->data().type()->name() == "IfcRailing" ||
				prod->data().type()->name() == "IfcAnnotation") {
				continue;
			}
			//std::cout << prod->data().type()->name() << std::endl;
			//std::cout << prod->data().type()->name() << std::endl;
			double U_interval = 0.1; double V_interval = 0.1;
			TopoDS_Shape shape = h->getObjectShape(prod);
			TopExp_Explorer expl_v;
			for (expl_v.Init(shape, TopAbs_VERTEX); expl_v.More(); expl_v.Next())
			{
				TopoDS_Vertex vertex = TopoDS::Vertex(expl_v.Current());
				num_of_v++;
			}

			for (expl_v.Init(shape, TopAbs_FACE); expl_v.More(); expl_v.Next())
			{
				TopoDS_Face face = TopoDS::Face(expl_v.Current());
				num_of_f++;
				//From TopoDS Face to brep structure
				const Standard_Real Tol = 0.0001;
				BRepTopAdaptor_FClass2d brep(face, Tol);
				BRepAdaptor_Surface b_face(face, Standard_True);
				Standard_Real UMin; Standard_Real UMax; Standard_Real VMin; Standard_Real VMax;
				BRepTools::UVBounds(face, UMin, UMax, VMin, VMax);

				double u = UMin; double v = VMin;

				int i = 0;
				while (u < UMax) {
					v = VMin;
					while (v < VMax) {
						gp_Pnt2d p = gp_Pnt2d(u, v);
						if (brep.TestOnRestriction(p, Tol) == TopAbs_IN) {
							gp_Pnt p = b_face.Value(u, v);
							//std::cout << p.X() << " " << p.Y() << " " << p.Z() << std::endl;
							points.emplace_back(p);
							i++;

						}
						v = v + V_interval;
					}
					u = u + U_interval;
				}
				//std::cout << "Points per surface" << i << std::endl;
			}

		}

	}

	ofstream out(output, std::ofstream::out);
	out << points.size() << "\n";
	for (int i = 0; i < points.size(); i++) {
		out << points[i].X() << " " << points[i].Y() << " " << points[i].Z() << "\n";
	}

	out.close();

	//ofstream out2(evaluation, std::ofstream::out);
	//out2 << num_of_v <<","<< num_of_f<<"\n";
	//out2.close();

	std::cout << "Number of Vertices are: " << num_of_v << std::endl;
	std::cout << "Number of faces are: " << num_of_f << std::endl;


	return;
}
