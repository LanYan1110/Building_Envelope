#include "sampler.h"
#include "helper.h"

void ifc_product_sampler(IfcSchema::IfcProduct::list::ptr prods, std::string output, IfcParse::IfcFile* file, std::string input) {
	std::vector<std::vector<gp_Pnt>> products;
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
				prod->data().type()->name() == "IfcBuildingProxy" ||
				prod->data().type()->name() == "IfcAnnotation") {
				continue;
			}
			//std::cout << prod->data().type()->name() << std::endl;
			//std::cout << prod->data().type()->name() << std::endl;

			std::vector<gp_Pnt> product;

			//sampling points
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
							product.emplace_back(p);
							i++;

						}
						v = v + V_interval;
					}
					u = u + U_interval;
				}
				//std::cout << "Points per surface" << i << std::endl;
			}
			products.emplace_back(product);
		}

	}

	//ofstream out(output, std::ofstream::out,);
	//std::ofstream out;
	//ofstream out(output, std::ofstream::out);
	ofstream out(output, std::ios::out | std::ios::app);
	int count = 0;
	for (auto product : products) {
		out << "Procduct" << count << "\n";
		out << product.size() << "\n";
		for (int i = 0; i < product.size(); i++) {
			out << product[i].X() << " " << product[i].Y() << " " << product[i].Z() << "\n";
		}
	}

	out.close();

	//ofstream out2(evaluation, std::ofstream::out);
	//out2 << num_of_v <<","<< num_of_f<<"\n";
	//out2.close();

	std::cout << "Number of Vertices are: " << num_of_v << std::endl;
	std::cout << "Number of faces are: " << num_of_f << std::endl;

	return;
}


void ifc_sampler(IfcSchema::IfcProduct::list::ptr prods, std::string out_points, 
IfcParse::IfcFile* file, std::string input,
int& input_v, int& input_f,int& out_v) {
	//Description: This function samples points from the ifc file and saves them in a xyz file
	//Parameters: prods: list of products in the ifc file
	//			  out_points: path to the output file
	//			  file: input ifc file
	//			  input: path to the input ifc file
	//			  input_v: number of vertices in the input ifc file
	//			  input_f: number of faces in the input ifc file
	//			  out_v: number of vertices in the output file (to be put in the alpha shape)
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
				prod->data().type()->name() == "IfcBuildingElementProxy" ||
				prod->data().type()->name() == "IfcAnnotation") {
				continue;
			}

			//std::cout << prod->data().type()->name() << std::endl;
			//Sampling points over each surface of the current IfcProduct
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
				// std::vector<gp_Pnt> product; // vector of points for each product
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

	input_v = num_of_v;
	input_f = num_of_f;
	out_v=points.size();

	//Output the generated point cloud to the IFC file
	ofstream out(out_points, std::ofstream::out);
	out << points.size() << "\n";
	for (int i = 0; i < points.size(); i++) {
		out << points[i].X() << " " << points[i].Y() << " " << points[i].Z() << "\n";
	}
	out.close();

	return;
}
