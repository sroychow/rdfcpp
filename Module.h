#ifndef __Module__h
#define __Module__h
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
#include "HistoContainer.h"
using ROOT::RDF::RNode;

using json = nlohmann::json;

class Module {
 public:
  Module(ROOT::RDF::RNode dfnode, std::vector<VariableBlock> vars, std::string nomweight, 
         json systematics, std::string mfilter, std::string nodeName);
  ~Module(){}
  void setHistograms(std::vector<HistoContainer>& hcontainerVec);
 private:
  ROOT::RDF::RNode rdfNode_;
  bool isMC_;
  std::string dataType_;
  std::vector<VariableBlock> vars_; 
  std::string nomweight_;
  json systjson_;
  std::string filter_;
  std::string nodeName_;
};
#endif
