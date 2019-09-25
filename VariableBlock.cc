#include "VariableBlock.h"

VariableBlock::VariableBlock(const std::string& colName, const json j){
 collectionName_ =  colName;
 setValues(j); 
}

bool VariableBlock::plotForRegion(std::string reg) {
  bool doplot = false;
  for(auto& ap : appliesTo_) {
    if(reg.find(ap) !=std::string::npos) {
      return true;
    }
  }
  return false;
}
void VariableBlock::setValues(const json varjson) {
  for(auto& [info, val] : varjson.items()) {
    if (info == "appliesTo" ) {
      for(auto& v: val) {
        appliesTo_.emplace_back(v.get<std::string>());
      }
    }
    else if ( info == "D1variables" or info == "D1newvariables") {
      for(auto& [systdir, varinfo] : val.items()) {
	//std::cout << "SystDir=" << systdir << std::endl;
	for(auto& [cname, defn]: varinfo.items()) {
	  //std::cout << "colname=" << cname << std::endl;
	  Variable1D v1d;
	  v1d.systdir = systdir;
	  v1d.varName = cname;
	  v1d.colX = defn[0].get<std::string>();
	  v1d.h1D = ROOT::RDF::TH1DModel(cname.c_str(), defn[1].get<std::string>().c_str(), 
                                         defn[2].get<int>(), defn[3].get<float>(), defn[4].get<float>());
	  systvar1D_[systdir].emplace_back(v1d);
	}//loop over 1D
      }//loop over systdirs
    }//end else-if 1D 
    else if ( info == "D2variables") {
      for(auto& [systdir, varinfo] : val.items()) {
	//std::cout << "SystDir=" << systdir << std::endl;
	for(auto& [cname, defn]: varinfo.items()) {
	  Variable2D v2d;
	  v2d.varName = cname;
	  v2d.systdir = systdir;
	  v2d.colX = defn[7].get<std::string>();
	  v2d.colY = defn[8].get<std::string>();
	  v2d.h2D = ROOT::RDF::TH2DModel(cname.c_str(), defn[0].get<std::string>().c_str(), 
                                         defn[1].get<int>(), defn[2].get<float>(), defn[3].get<float>(), 
                                         defn[4].get<int>(), defn[5].get<float>(), defn[6].get<float>());
	  systvar2D_[systdir].emplace_back(v2d);
	}
      }
    }//end else-if 2D    
    else if ( info == "D3variables") {
      for(auto& [systdir, varinfo] : val.items()) {
	//std::cout << "SystDir=" << systdir << std::endl;
	for(auto& [cname, defn]: varinfo.items()) {
	  Variable3D v3d;
	  v3d.varName = cname;
	  v3d.systdir = systdir;
	  v3d.colX = defn[10].get<std::string>();
	  v3d.colY = defn[11].get<std::string>();
	  v3d.colZ = defn[12].get<std::string>();
	  v3d.h3D = ROOT::RDF::TH3DModel(cname.c_str(), defn[0].get<std::string>().c_str(), 
                                         defn[1].get<int>(), defn[2].get<float>(), defn[3].get<float>(), 
                                         defn[4].get<int>(), defn[5].get<float>(), defn[6].get<float>(), 
                                         defn[7].get<int>(), defn[8].get<float>(), defn[9].get<float>());
	  systvar3D_[systdir].emplace_back(v3d);
	}
      }
    }//end else-if 3D    
  }//loop over varblock info
}

void VariableBlock::print() {
  std::cout << "Variable Block info:" << collectionName_ << std::endl;
  std::cout << "Applies to:" ;
  for(auto& ap : appliesTo_) std::cout << ap << ",";
  std::cout << std::endl;
  std::cout << "1D Variable info:" << std::endl;
  for(auto& [syst, varVec] : systvar1D_) {
    std::cout << "Systematics Directory:" << syst << std::endl;
    for(auto& v : varVec) {
      std::cout << "Var name=:" <<  v.varName << "\t" 
                << "Expression:" << v.colX
                << "    TH1D(" 
                << v.h1D.fName << ","
                << v.h1D.fNbinsX << "," 
                << v.h1D.fXLow << "," 
                << v.h1D.fXUp << ")" 
                << std::endl;  
    }    
  }    

  std::cout << "2D Variable info:" << std::endl;
  for(auto& [syst, varVec] : systvar2D_) {
    std::cout << "Systematics Directory:" << syst << std::endl;
    for(auto& v : varVec) { 
      std::cout << "Var name=:" <<  v.varName << "\t" 
                << "Expression(x,y):(" 
                << v.colX << "," << v.colY << ")" 
                << std::endl;      
      std::cout << "TH2D(" 
                << v.h2D.fName << ","
                << v.h2D.fNbinsX << "," 
                << v.h2D.fXLow << "," 
                << v.h2D.fXUp << ","  
                << v.h2D.fNbinsY << "," 
                << v.h2D.fYLow << ","
                << v.h2D.fYUp 
                << ")" 
                << std::endl;
    }  
  }        

  std::cout << "3D Variable info:" << std::endl;
  for(auto& [syst, varVec] : systvar3D_) {
    std::cout << "Systematics Directory:" << syst << std::endl;
    for(auto& v : varVec) 
      std::cout << "Var name=:" <<  v.varName << "\t" << "Expression(x,y,z):" << v.colX << "," << v.colY << "," << v.colZ << std::endl;      
  }        
}
