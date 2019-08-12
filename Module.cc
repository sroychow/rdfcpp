#include "Module.h"
#include <TApplication.h>
#include <algorithm>
#include <iomanip>
#include <sstream>


Module::Module(ROOT::RDF::RNode dfnode, std::vector<VariableBlock> vars, std::string nomweight,
               json systematics, std::string mfilter, std::string nodeName):
 rdfNode_(dfnode),
 vars_(vars),
 nomweight_(nomweight),
 systjson_(systematics),
 filter_(mfilter),
 nodeName_(nodeName)
{
  dataType_ = "MC";
  isMC_ = (dataType_ == "MC") ? true : false;
}

void Module::setHistograms(std::vector<HistoContainer>& hcontainerVec) {
  auto nominalNode = rdfNode_.Filter(filter_);
  //container for nominal histos
  HistoContainer hcontainer;
  hcontainer.selectionName = nodeName_;
  hcontainer.dir = "nom";
  hcontainer.subdir = "";
  
  //book 1D histos
  for(auto& var : vars_) {
    for(auto& var1d : var.var1D()) {
      hcontainer.h1Dvec.emplace_back(nominalNode.Histo1D(var1d.second.h1D, var1d.first, nomweight_));
    }//end loop of 1d vars
     
    //book 2d histos
    for(auto& var2d : var.var2D()) {
      hcontainer.h2Dvec.emplace_back(nominalNode.Histo2D(var2d.second.h2D, var2d.second.colX, var2d.second.colY, nomweight_));
    }//end loop of 2d vars

    //book 3d histos
    for(auto& var3d : var.var3D()) {
      hcontainer.h3Dvec.emplace_back(nominalNode.Histo3D(var3d.second.h3D, var3d.second.colX, var3d.second.colY, var3d.second.colZ, nomweight_));
    }//end loop of 3d vars
  }
  hcontainerVec.emplace_back(hcontainer);
}
