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
   bool plotForRegion(std::string reg);

   std::vector<Variable1D> var1D(std::string syst){
     if(systvar1D_.find(syst) != systvar1D_.end())  
       return systvar1D_[syst]; 
   }

   std::vector<Variable2D> var2D(std::string syst){
     if(systvar2D_.find(syst) != systvar2D_.end())  
       return systvar2D_[syst]; 
   }

   std::vector<Variable3D> var3D(std::string syst){
     if(systvar3D_.find(syst) != systvar3D_.end())  
       return systvar3D_[syst]; 
   }
   
   std::map<std::string, std::vector<Variable1D>> var1D(){
       return systvar1D_; 
   }

   std::map<std::string, std::vector<Variable2D>> var2D(){
       return systvar2D_; 
   }

   std::map<std::string, std::vector<Variable3D>> var3D(){
       return systvar3D_; 
   }
   
   void print();
  private:
    std::string collectionName_;    
    std::vector<std::string> appliesTo_;//e.g. ['Signal*','Sideband*','Dimuon]
    std::map<std::string, std::vector<Variable1D>> systvar1D_;//
    std::map<std::string, std::vector<Variable2D>> systvar2D_;
    std::map<std::string, std::vector<Variable3D>> systvar3D_;
};

#endif
