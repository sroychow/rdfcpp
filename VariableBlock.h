#ifndef __VariableBlock__h
#define __VariableBlock__h
#include<iostream>
#include<fstream>
#include<map>
#include<vector>
#include "TROOT.h"
#include "TFile.h"
#include "TString.h"
//#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDF/HistoModels.hxx"
#include "json.hpp"
#include "HistoContainer.h"
using json = nlohmann::json;

class VariableBlock {
  public:
    VariableBlock(const std::string& colName, const json j);
    ~VariableBlock(){}
   void setValues(const json varjson);
   std::string colName() { return collectionName_; }    
   std::vector<std::string> appliesTo() { return appliesTo_; }

   std::map<std::string,  Variable1D> var1D(){ return variables1D_; }

   std::map<std::string,  Variable2D> var2D() { return variables2D_; }

   std::map<std::string,  Variable3D> var3D() { return variables3D_; }

   
  private:
    std::string collectionName_;    
    std::vector<std::string> appliesTo_;//e.g. ['Signal*','Sideband*','Dimuon]
    std::map<std::string,  Variable1D> variables1D_;// e.g.'corrected_pt':   ('muon p_{T} (Rochester corr.)',  100, 25, 65)
    std::map<std::string,  Variable2D> variables2D_;//
    std::map<std::string,  Variable3D> variables3D_;//
};

#endif
