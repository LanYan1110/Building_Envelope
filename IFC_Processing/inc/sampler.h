//#define USE_IFC4

#ifdef USE_IFC4
#define IfcSchema Ifc4
#else
#define IfcSchema Ifc2x3
#endif 

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string.hpp>


#include <TopoDS.hxx>
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <BRepTools.hxx>
#include <StdPrs_ToolRFace.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <Hatch_Hatcher.hxx>
#include <Adaptor3d_IsoCurve.hxx>
class BRepAdaptor_HSurface;

#include <ifcparse/IfcFile.h>
#include <ifcgeom/IfcGeom.h>
#include <ifcgeom/IfcGeomRepresentation.h>
#include <ifcgeom_schema_agnostic/kernel.h>
#include <ifcgeom_schema_agnostic/IfcGeomIterator.h>


// Function that extract all vertices from an Ifc File
void ifc_sampler(IfcSchema::IfcProduct::list::ptr prods, std::string output, IfcParse::IfcFile* file, std::string input);
