#include "Module.h"
#include <TApplication.h>
#include <algorithm>
#include <iomanip>
#include <sstream>


Module::Module(ROOT::RDF::RNode dfnode, std::vector<VariableBlock> vars, std::string nomweight,
               std::string mfilter, std::string nodeName, 
               json wtvars,
               json colvars):
 rdfNode_(dfnode),
 vars_(vars),
 nomweight_(nomweight),
 filter_(mfilter),
 nodeName_(nodeName),
 wtvars_(wtvars),
 colvars_(colvars){
}

bool Module::replace(std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = str.find(from);
  if(start_pos == std::string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}

void Module::setHistograms(HistoContainer& hcontainer) {
  auto nominalNode = rdfNode_.Filter(filter_);
  auto defCols = nominalNode.GetDefinedColumnNames();

  std::cout << "Module with Selection:" << nodeName_ << std::endl;
  for(auto& var : vars_) {
    //check if this varblock is applicable to this region
    if(!var.plotForRegion(nodeName_))   continue;
    std::cout << "Info for VarBlock:" << var.colName() << std::endl;

    //book 1D histos
    for(auto& [systdir, vars1d] : var.var1D()) {
      if(systdir == "nominal"){
        for(auto& var1d : vars1d) {
          hcontainer.systh1Dvecmap[systdir].emplace_back(nominalNode.Histo1D(var1d.h1D, var1d.varName, nomweight_)); 
          for(auto& [wtype, wvars] : wtvars_.items()) {
            for(auto& [key, wv]: wvars.items()) { 
              auto h1model = var1d.h1D;
              h1model.fName = h1model.fName + "_" + TString(wv.get<std::string>());
              std::string newWeight = nomweight_ + "_" +wv.get<std::string>();
              hcontainer.systh1Dvecmap[wtype].emplace_back(nominalNode.Histo1D(h1model, var1d.varName, std::string_view(newWeight.c_str()))); 
            }
          }
        }//loop over var1d
      }//end if statement
      else {//col vars
        for(auto& [colsyst, cvars] : colvars_.items()){
          for(auto& [key, colv]: cvars.items()) { 
            auto cv = colv.get<std::string>();
            for(auto& var1d : vars1d) {
              if(var1d.varName.find(cv) == std::string::npos)   continue;//only plot vars affected by this variation
              std::string newfilter = filter_;
              replace(newfilter, colsyst, cv);
              hcontainer.systh1Dvecmap[systdir].emplace_back(rdfNode_.Filter(newfilter).Histo1D(var1d.h1D, var1d.varName, nomweight_));
            }//loop over vars
          }//cvars loop
        }//colvars_ loop
      }//end of else
      std::cout << "Histos defined for syst:" << hcontainer.systh1Dvecmap[systdir].size() << std::endl;
    }//loop over 1d maps

    //loop over 2D maps
    //book 2D histos
    for(auto& [systdir, vars2d] : var.var2D()) {
      if(systdir == "nominal"){
        for(auto& var2d : vars2d) {
          bool foundx = std::find(defCols.begin(), defCols.end(), var2d.colX) != defCols.end();
          bool foundy = std::find(defCols.begin(), defCols.end(), var2d.colY) != defCols.end();
          std::cout << var2d.h2D.fName << "\t" << var2d.colX << "(" << foundx << ")\t" 
                    << var2d.colY << "(" << foundx << ")" << std::endl;
          
          //auto h2n = nominalNode.Histo2D({var2d.h2D.fName, "", 100, 0., 5, 100,0,100}, var2d.colX, var2d.colY, std::string_view(nomweight_.c_str())); 
          //hcontainer.systh2Dvecmap[systdir].push_back(h2n);
          //hcontainer.systh2Dvecmap[systdir].emplace_back(nominalNode.Histo2D(var2d.h2D, var2d.colX, var2d.colY, nomweight_)); 
          for(auto& [wtype, wvars] : wtvars_.items()) {
            for(auto& [key, wv]: wvars.items()) { 
              auto h2model = var2d.h2D;
              h2model.fName = h2model.fName + "_" + TString(wv.get<std::string>());
              std::string newWeight = nomweight_ + "_" + wv.get<std::string>();
              //hcontainer.systh2Dvecmap[wtype].emplace_back(nominalNode.Histo2D(h2model, var2d.colX, var2d.colY, newWeight)); 
            }
          }
        }//loop over var2d
      }//end if statement
      else {//col vars
        /*
        for(auto& [colsyst, cvars] : colvars_.items()){
          for(auto& [key, colv]: cvars.items()) { 
            auto cv = colv.get<std::string>();
            for(auto& var2d : vars2d) {
              if(var2d.colX.find(cv) == std::string::npos 
                 && var2d.colY.find(cv) == std::string::npos)   continue;//only plot vars affected by this variation
              std::string newfilter = filter_;
              replace(newfilter, colsyst, cv);
              //hcontainer.systh2Dvecmap[systdir].emplace_back(rdfNode_.Filter(newfilter).Histo2D(var2d.h2D, var2d.colX, var2d.colY, nomweight_)); 
            }//loop over vars
          }//cvars loop
        }//colvars_ loop
        */
      }//end of else
      std::cout << "2DHistos defined for syst:" << hcontainer.systh2Dvecmap[systdir].size() << std::endl;
    }//loop over 2d maps


  }//Loop over var blocks
 
}

    /*
    //book 2d histos
    for(auto& var2d : var.var2D("nominal")) {
      hcontainer.h2Dvec.emplace_back(nominalNode.Histo2D(var2d.h2D, var2d.colX, var2d.colY, nomweight_));
    }//end loop of 2d vars
    
    //book 3d histos
    for(auto& var3d : var.var3D("nominal")) {
      hcontainer.h3Dvec.emplace_back(nominalNode.Histo3D(var3d.h3D, var3d.colX, var3d.colY, var3d.colZ, nomweight_));
    }//end loop of 3d vars
    */
