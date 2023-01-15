
#include "inc/GetAllPoints.h"


#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <filesystem>

// openCASCADE includes
#include <TopoDS.hxx>
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <gp_GTrsf.hxx>

// IfcOpenShell includes
#include <ifcparse/IfcFile.h>
#include <ifcgeom/IfcGeom.h>
#include <ifcgeom/IfcGeomRepresentation.h>
#include <ifcgeom_schema_agnostic/kernel.h>
#include <ifcgeom_schema_agnostic/IfcGeomIterator.h>


int main(int argc, char** argv) {

	// Read and Parse the input IFC files

	std::string dir = "C:/Users/seuya/Documents/Thesis/Building_Envelope";
	std::string input_file ="AC20-FZK-Haus.ifc";
	std::stringstream ss;
	ss << dir <<"/RawData/"<<input_file;
	std::string input_path = ss.str();

	std::cout << input_path << std::endl;

	IfcParse::IfcFile input(input_path);
	IfcParse::IfcFile* file = &input;

	// Different ways to extract points from IFC files
	
	// Iterate through the IfcProducts
	IfcSchema::IfcProduct::list::ptr prods = file->instances_by_type<IfcSchema::IfcProduct>();


	std::stringstream ss2;
	ss2 << dir << "/Intermediate_Data/" << input_file<<"_vertices";
	std::string output_path = ss2.str();
	std::cout << output_path << std::endl;
	extract_vertices(prods, output_path, file);


	return 0;

}