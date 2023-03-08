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
#include <TCollection_AsciiString.hxx>
#include <StlAPI_Writer.hxx>
#include <BRep_Builder.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangle.hxx>
#include <OSD_Path.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Geom_Surface.hxx>
class BRepAdaptor_HSurface;

#include <ifcparse/IfcFile.h>
#include <ifcgeom/IfcGeom.h>
#include <ifcgeom/IfcGeomRepresentation.h>
#include <ifcgeom_schema_agnostic/kernel.h>
#include <ifcgeom_schema_agnostic/IfcGeomIterator.h>


// Function that extract all vertices from an Ifc File
void ifc_product_sampler(IfcSchema::IfcProduct::list::ptr prods, std::string output, IfcParse::IfcFile* file, std::string input);
void ifc_sampler(IfcSchema::IfcProduct::list::ptr prods, std::string out_points, 
IfcParse::IfcFile* file, std::string input,
int& input_v, int& input_f,int& out_v);