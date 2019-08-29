#ifndef __Module__h
#define __Module__h
#include<iostream>
#include<fstream>
#include<map>
#include<vector>
#include<set>
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
  Module(ROOT::RDF::RNode dfnode, std::vector<VariableBlock> vars, std::string nomweight, std::string mfilter, 
         std::string nodeName, json wtvars, json colvars);
  ~Module(){}
  void setHistograms(HistoContainer& hcontainer);
  bool replace(std::string& str, const std::string& from, const std::string& to);
  void setHistogramforFilter(std::string filter, std::string systdir, std::vector<Variable1D>& var1d);
 private:
  ROOT::RDF::RNode rdfNode_;
  std::vector<VariableBlock> vars_; 
  std::string nomweight_;
  std::string filter_;
  std::string nodeName_;
  json wtvars_;
  json colvars_;
};
#endif
