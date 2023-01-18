#include "GetAllPoints2.h"

#include "helper.h"

//void extract_vertices2() {
//	IfcGeomObjects::Settings(IfcGeomObjects::USE_WORLD_COORDS, use_world_coords);
//}

void extract_vertices_sample(IfcSchema::IfcProduct::list::ptr prods, std::string output, IfcParse::IfcFile* file, std::string input)
{

	std::vector<gp_Pnt> points;
	IfcGeom::Kernel my_kernel(file);
	helperCluster* hCluster = new helperCluster;
	helper* h = new helper(input);
	

	// Extract vertices by iterating through IfcProducts
	int ifc_p_i = 0;

	for (auto it = prods->begin(); it != prods->end(); it++)
	{
		// Get the placement
		IfcSchema::IfcProduct* prod = *it;
		ifc_p_i++;

		//if (prod->data().type()->name() == "IfcWall") {}
		//if (prod->data().type()->name() == "IfcWallStandardCase") {}
		//if (prod->data().type()->name() == "IfcRoof") {}
		//prod->data().type()->name() == "IfcDoor" ||
		//	prod->data().type()->name() == "IfcWindow" ||

		// preserve or not preserve door and window?
		//prod->
		if (prod->hasRepresentation())
		{
			if (prod->data().type()->name() == "IfcSite" ||
				prod->data().type()->name() == "IfcStair" ||
				prod->data().type()->name() == "IfcOpeningElement" ||
				prod->data().type()->name() == "IfcSpace" ||
				prod->data().type()->name() == "IfcRailing" ||
				prod->data().type()->name() == "IfcAnnotation") {
				continue;
			}
			//std::cout << prod->data().type()->name() << std::endl;
			//std::cout << prod->data().type()->name() << std::endl;

			TopoDS_Shape shape = h->getObjectShape(prod);
			TopExp_Explorer expl_v;


			for (expl_v.Init(shape, TopAbs_FACE); expl_v.More(); expl_v.Next())
			{
				TopoDS_Face face = TopoDS::Face(expl_v.Current());
				//From TopoDS Face to brep structure
				const Standard_Real Tol = 0.0001;
				BRepTopAdaptor_FClass2d brep(face,Tol);
				BRepAdaptor_Surface b_face(face, Standard_True);
				Standard_Real UMin; Standard_Real UMax; Standard_Real VMin; Standard_Real VMax;
				BRepTools::UVBounds(face,UMin,UMax,VMin,VMax);
				

				double U_interval = (UMax - UMin) / 5;
				double V_interval = (VMax - VMin) / 5;
				
				double u = UMin; double v = VMin;

				while (u < UMax) {
					v = VMin;
					while (v < VMax) {
						gp_Pnt2d p = gp_Pnt2d(u, v);
						if (brep.TestOnRestriction(p,Tol)==TopAbs_IN) {
							gp_Pnt p = b_face.Value(u, v);
							std::cout << p.X() << " " << p.Y() << " " << p.Z() << std::endl;
							points.emplace_back(p);
						}
						v = v + V_interval;
					}
					u = u + U_interval;
				}

				bool theDrawUIso = true;
				bool theDrawVIso = true;
				int theNbUIso = 10; //num of u iso curves
				int theNbVIso = 10; //num of v iso curves
				
				BRepAdaptor_HSurface h(b_face);
			}

	
		}

	}

	ofstream out(output, std::ofstream::out);

	out << points.size() << "\n";

	for (int i = 0; i < points.size(); i++) {
		out << points[i].X() << " " << points[i].Y() << " " << points[i].Z() << "\n";
	}

	out.close();
	return;
}
