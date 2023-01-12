#define USE_IFC4
//#define USE_IFC2_3
#include "inc/helper.h"
#include "inc/floorProcessor.h"
#include "inc/roomProcessor.h"

#ifdef USE_IFC4
#define IfcSchema Ifc4
#else
#define IfcSchema Ifc2x3
#endif // USE_IFC4

//#ifdef USE_IFC2_3
//#define IfcSchema Ifc2x3
//#else
//#define IfcSchema Ifc4
//#endif // USE_2x3

// basic includes
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <chrono>

// boost includes
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string.hpp>

// openCASCADE includes
#include <TopoDS.hxx>
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
//#include <BRepTopAdaptor_FClass2d.hxx>
//#include <BRepTools.hxx>

// IfcOpenShell includes
#include <ifcparse/IfcFile.h>
#include <ifcparse/IfcHierarchyHelper.h>
#include <ifcgeom/IfcGeom.h>
#include <ifcgeom/IfcGeomRepresentation.h>
#include <ifcgeom_schema_agnostic/kernel.h>
#include <ifcgeom_schema_agnostic/IfcGeomIterator.h>

#if USE_VLD
#include <vld.h>
#endif


int main(int argc, char** argv) {

	IfcParse::IfcFile file("C:/Users/seuya/Documents/Thesis/Building_Envelope/Data/AC20-FZK-Haus.ifc");
	/*IfcParse::IfcFile file("C:/Users/seuya/Documents/Thesis/Data/On4_building_normal.ifc");*/
	//IfcParse::IfcFile file("C:/Users/seuya/Documents/Thesis/Data/AC-20-Smiley-West-10-Bldg.ifc");
	
	IfcParse::IfcFile* file2 = &file;

	IfcSchema::IfcProduct::list::ptr prods = file2->instances_by_type<IfcSchema::IfcProduct>();

	IfcGeom::Kernel my_kernel(file2);

	int num = 0;

	// Iterate through IfcProduct
	//for (auto it = prods->begin(); it != prods->end(); ++it) {
	//	IfcSchema::IfcProduct* prod = *it;
	//	gp_Trsf plane;
	//	my_kernel.convert_placement(prod->Representation(), plane);
	//	const gp_XYZ& xyz = plane.TranslationPart();
	//	pointList.emplace_back(gp_Pnt(xyz.X(), xyz.Y(), xyz.Z()));
	//	num++;
	//}

	// initialize helper
	helperCluster* hCluster = new helperCluster;
	helper* h = new helper("C:/Users/seuya/Documents/Thesis/Building_Envelope/Data/AC20-FZK-Haus.ifc");
	
	//std::vector<gp_Pnt> points = h->getAllPoints(prods);

	// IfcGeom Iterator

	std::vector<gp_Pnt> pointList2;

	IfcGeom::IteratorSettings settings;
	settings.set(IfcGeom::IteratorSettings::DISABLE_TRIANGULATION, true);
	settings.set(IfcGeom::IteratorSettings::USE_WORLD_COORDS, true);
	int num_threads = 1;

	IfcGeom::Iterator<double> geom_iterator(settings,file2,num_threads);

	int i = 0;

	if (geom_iterator.initialize()) {

		IfcGeom::Element<double>* geometry = geom_iterator.get_native();
		IfcGeom::BRepElement<double>* brepElementPtr = static_cast<IfcGeom::BRepElement<double>*>(geometry);
		// Get object shape
		TopoDS_Shape shape = brepElementPtr->geometry().as_compound();
		TopExp_Explorer expl_v;
		
		i = i + 1;

		// Extract vertices
		for (expl_v.Init(shape, TopAbs_VERTEX); expl_v.More(); expl_v.Next())
		{
			TopoDS_Vertex vertex = TopoDS::Vertex(expl_v.Current());
			gp_Pnt p = BRep_Tool::Pnt(vertex);
			pointList2.emplace_back(p);
		}

		// Sample Points
		// Explore the faces of the shape
		
	//	for (expl_v.Init(shape,TopAbs_FACE); expl_v.More(); expl_v.Next())
	//	{
	//		TopoDS_Face face = TopoDS::Face(expl_v.Current());
	//		/*TopoDS_Face face* f = &face;*/

	//		//! Test a point with +- an offset (Tol) and returns
	//		//! On if some points are OUT an some are IN
	//		//! (Caution: Internal use . see the code for more details)
	//		const Standard_Real Tol = 0.0001;

	//		// From TopoDS Face to brep structure
	//		BRepTopAdaptor_FClass2d brep(face,Tol);

	//		//! * UVBounds : Methods to compute the  limits of the
	//		//! boundary  of a  face,  a wire or   an edge in  the
	//		//! parametric space of a face.
	//		//gp_Pnt p = BRep_Tool::Pnt();
	//		//pointList2.emplace_back(p);

	//		/*BRepTools::UVBounds()*/
	//	}
	}

	std::cout << "total_geometry number: " << i << std::endl;
	//IfcSchema::IfcBuildingElement::list::ptr elements = file.instances_by_type<IfcSchema::IfcBuildingElement>();

	//std::cout << "Found " << elements->size() << " elements in " << argv[1] << ":" << std::endl;
	//
	//std::cout << "Number of IfcProduct" << num << std::endl;

	 //write the file to the vertices
	//ofstream vertices("C:/Users/seuya/Documents/Thesis/Building_Envelope/Intermediate_Data/one_element_vertices_jasper", std::ofstream::out);
	//
	//vertices << points.size() << "\n";

	//for (int i = 0; i < points.size(); i++) {
	//	vertices << points[i].X() << " " << points[i].Y() << " " << points[i].Z()<<"\n";
	//}

	vertices.close();

	std::cout << pointList2.size() << std::endl;
	ofstream vertices("C:/Users/seuya/Documents/Thesis/Building_Envelope/Intermediate_Data/one_element_vertices_ifcgeom_tri", std::ofstream::out);

	vertices << pointList2.size() << "\n";

	for (int i = 0; i < pointList2.size(); i++) {
		vertices << pointList2[i].X() << " " << pointList2[i].Y() << " " << pointList2[i].Z() << "\n";
	}

	vertices.close();

	return 0;

}