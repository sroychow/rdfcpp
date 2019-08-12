#include "VariableBlock.h"

VariableBlock::VariableBlock(const std::string& colName, const json j){
 collectionName_ =  colName;
 setValues(j); 
}

void VariableBlock::setValues(const json varjson) {
  for(auto& [key, value] : varjson.items()) {
    if (key == "appliesTo" ) {
      for(auto& v: value)
        appliesTo_.emplace_back(v);
    } 
    else if ( key == "D1variables" or key == "D1newvariables") {
      for(auto& [cname, defn]: value.items()) {
	Variable1D v1d;
	v1d.varName = cname;
	v1d.colX = defn[0].get<std::string>();
	v1d.h1D = ROOT::RDF::TH1DModel(cname.c_str(), defn[1].get<std::string>().c_str(), defn[2], defn[3], defn[4]);
	variables1D_.insert(std::make_pair(cname, v1d)); 
      } 
    }
    else if ( key == "D2variables") {
      for(auto& [cname, defn]: value.items()) {
	Variable2D v2d;
	v2d.varName = cname;
	v2d.colX = defn[7].get<std::string>();
	v2d.colY = defn[8].get<std::string>();
	v2d.h2D = ROOT::RDF::TH2DModel(cname.c_str(), defn[0].get<std::string>().c_str(), defn[1], defn[2], defn[3], defn[4], defn[5], defn[6]);
	variables2D_.insert(std::make_pair(cname, v2d)); 
      } 
    }
    else if ( key == "D3variables") {
      for(auto& [cname, defn]: value.items()) {
	Variable3D v3d;
	v3d.varName = cname;
	v3d.colX = defn[10].get<std::string>();
	v3d.colY = defn[11].get<std::string>();
	v3d.colZ = defn[12].get<std::string>();
	v3d.h3D = ROOT::RDF::TH3DModel(cname.c_str(), defn[0].get<std::string>().c_str(), defn[1], defn[2], defn[3], defn[4], defn[5], defn[6], defn[7], defn[8], defn[9]);
	variables3D_.insert(std::make_pair(cname, v3d)); 
      } 
    }
  }
}
