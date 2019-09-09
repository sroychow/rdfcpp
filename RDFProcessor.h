#ifndef __RDFProcessor__h
#define __RDFProcessor__h
#include<iostream>
#include<fstream>
#include<map>
#include<vector>
#include<tuple>
#include "TROOT.h"
#include "TFile.h"
#include "TString.h"
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDF/HistoModels.hxx"
#include "ROOT/RResultPtr.hxx"
#include "json.hpp"
#include "VariableBlock.h"
#include "Module.h"
#include "HistoContainer.h"
using ROOT::RDF::RNode;

using json = nlohmann::json;

class RDFProcessor {
 public:
  RDFProcessor(std::string_view treeName, std::vector<std::string>& fileNames, std::string dataType = "MC", float xsec = 1.);
  ~RDFProcessor(){}
  void setInputTree(){}
  void setVariables(const std::string& varFile);
  void setWeights(const std::string& weightFile);
  void setSelections(const std::string& selFile);
  void setSystematics(const std::string& selFile);
  void defineColumnsofInterest();
  void branchModules();
  std::vector<VariableBlock> getVariables(){ return vars_;};
  void writeOutput();
 private:
  ROOT::RDF::RNode rdfTree_;

  std::vector<std::string> inFilenames_;
 
  std::string outputDir_; 
  std::string outputFile_;
  bool pretend_;//False, 
  bool isMC_;
  std::string dataType_;
  float targetLumi_;
  float xsec_;    
  json varjson_;
  json seljson_;
  json systjson_;
  json hweightsjson_;
  json colvarmap_; 
  json weightvarmap_; 
  std::vector<VariableBlock> vars_; 
  std::vector<HistoContainer> histoVec_;
};
#endif
