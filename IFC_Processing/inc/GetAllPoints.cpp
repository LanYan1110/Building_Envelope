
#include "GetAllPoints.h"

void extract_vertices(IfcSchema::IfcProduct::list::ptr prods,std::string output, IfcParse::IfcFile* file)
{

	std::vector<gp_Pnt> points;
	IfcGeom::Kernel my_kernel(file);

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

		if (prod->hasRepresentation())
		{
			if (prod->data().type()->name() == "IfcSite" ||
				prod->data().type()->name() == "IfcStair" ||
				prod->data().type()->name() == "IfcOpeningElement" ||
				prod->data().type()->name() == "IfcDoor" ||
				prod->data().type()->name() == "IfcWindow" ||
				prod->data().type()->name() == "IfcSpace" ||
				prod->data().type()->name() == "IfcRailing" ||
				prod->data().type()->name() == "IfcAnnotation") {
				continue;
			}
			//std::cout << prod->data().type()->name() << std::endl;

			IfcSchema::IfcProductRepresentation* rep = prod->Representation();
			IfcTemplatedEntityList< ::IfcSchema::IfcRepresentation >::ptr p = rep->Representations();
			//std::cout << "Number of IfcRepresentation within one ifcproduct"<<p->size() << std::endl;
			
				for (auto re =p->begin(); re != p->end(); re++) {
					IfcSchema::IfcRepresentation* cur = *re;

					if (cur->Items()) {
						IfcTemplatedEntityList< ::IfcSchema::IfcRepresentationItem >::ptr li = cur->Items();
						for (auto cur_it = li->begin(); cur_it != li->end(); cur_it++) {

							IfcSchema::IfcRepresentationItem* p = *cur_it;

							std::cout << p->data().type()->name() << std::endl;
							//p->data().type()->name() == "IfcExtrudedAreaSolid"
							if (true) 
							{
								std::cout << p->data().type()->name() << std::endl;
								IfcGeom::IfcRepresentationShapeItems s= my_kernel.convert(p);
								std::cout << "size" << s.size() << std::endl;
								if (s.size() > 0) {
									for (int i = 0; i <s.size(); i++) {
										std::cout <<"i: " <<i<<std::endl;
										// the problem is with this line
										IfcGeom::IfcRepresentationShapeItem cur_item = s[0];
										TopoDS_Shape shape = cur_item.Shape();
										gp_GTrsf placement = cur_item.Placement();
										TopExp_Explorer expl_v;
										for (expl_v.Init(shape, TopAbs_VERTEX); expl_v.More(); expl_v.Next())
										{
											TopoDS_Vertex vertex = TopoDS::Vertex(expl_v.Current());
											gp_Pnt p = BRep_Tool::Pnt(vertex);
											gp_XYZ p2(p.X(), p.Y(),p.Z());
											placement.Transforms(p2);
											gp_Pnt p3 = gp_Pnt(p2);
											std::cout << p3.X() << " " << p3.Y() << " " << p3.Z() << std::endl;
											points.emplace_back(p3);

										}
									}
								}

							}

								
								std::cout << "\n" << std::endl;
							}					
							

						}

					}
					
				}
				
		}
		
		
	
	//std::cout << "Total Number of IfcProduct: " << ifc_p_i << std::endl;

	// Write points to a text file to be feed into 3D alpha shape



	ofstream out(output, std::ofstream::out);

	out << points.size() << "\n";

	for (int i = 0; i < points.size(); i++) {
		out << points[i].X() << " " << points[i].Y() << " " << points[i].Z() << "\n";
	}

	out.close();
	return;
}
