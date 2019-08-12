#include "RDFProcessor.h"
#include "TString.h"
#include <TApplication.h>
#include <algorithm>
#include <iomanip>
#include <sstream>

using ROOT::RDF::RNode;

RDFProcessor::RDFProcessor(std::string_view treeName, std::vector<std::string>& fileNames, std::string dataType) :
  rdfTree_(ROOT::RDataFrame(treeName, fileNames))
{
  inFilenames_ = fileNames;
  targetLumi_ = 35.91;
  xsec_ = 1.;
  dataType_ = dataType;
  isMC_ = dataType == "MC" ? true : false;
  outputFile_ = "test.root";
}

void RDFProcessor::setVariables(const std::string& varFile) {
  std::ifstream fin(varFile.c_str());
  fin >> varjson_;fin.close(); 
  for (auto& el : varjson_.items()) {
    vars_.emplace_back( VariableBlock(el.key(), el.value()) );
  }
}

void RDFProcessor::setWeights(const std::string& weightFile) {
  std::ifstream fin(weightFile.c_str());
  fin >> hweightsjson_;
  fin.close(); 
  
}

void RDFProcessor::setSelections(const std::string& selFile) {
  std::ifstream fin(selFile.c_str());
  fin >> seljson_;
  fin.close(); 
}

void RDFProcessor::setSystematics(const std::string& selFile) {
  std::ifstream fin(selFile.c_str());
  fin >> systjson_;
  fin.close(); 
}

void RDFProcessor::defineColumnsofInterest() {
  
  std::vector<std::string> colNames = rdfTree_.GetColumnNames();
  //define columns for variables
  for(auto& varBlock: vars_) {
    for(auto& var : varBlock.var1D()) {
      if(std::find( colNames.begin(), colNames.end(), var.first) != colNames.end() )   continue;
      std::cout << var.first << " needs to be defined" << std::endl;
      rdfTree_ = (rdfTree_.Define(var.first, var.second.colX));
    }
  }
  std::cout << "#Defined Columns after variable definition=" << rdfTree_.GetDefinedColumnNames().size() << std::endl;
  if(dataType_ == "MC") {
    //define weight histos
    auto runs = ROOT::RDataFrame("Runs", inFilenames_);
    auto genEventSumw = runs.Sum("genEventSumw").GetValue();
    std::cout << "genEventSumw " <<  genEventSumw <<  " weighted events\n";
    std::cout << "xsec " <<  xsec_ <<  " pb\n";
    std::stringstream lumiweight;
    lumiweight << std::setprecision(10) << (targetLumi_*xsec_)/genEventSumw;
    std::cout << "lumiweight " << lumiweight.str() <<  " (|Generator_weight| not accounted for)\n";
    rdfTree_ = rdfTree_.Define("lumiweight", lumiweight.str());
  }

  for(auto& [wcol, expr] : hweightsjson_[dataType_].items()) {
    std::cout << wcol << std::endl;
    rdfTree_ = rdfTree_.Define(wcol, expr.get<std::string>());
  }

  std::cout << "#Defined Columns after variable + histogram definition=" << rdfTree_.GetDefinedColumnNames().size() << std::endl;
}

void RDFProcessor::branchModules() {
  //Loop over selections, i.e., filters.
  for(auto& [selname, selval] : seljson_.items()) {
    auto nodefilter = selval[dataType_]["cut"].get<std::string>();
    auto nodeweight = selval[dataType_]["weight"].get<std::string>();
    std::cout << "Module created with filter name:" << selname << std::endl; 
    std::cout << "Cut: "    << nodefilter << std::endl; 
    std::cout << "Weight: " << nodeweight << std::endl; 
    Module m(rdfTree_, vars_, nodeweight, systjson_, nodefilter, selname);
    m.setHistograms(histoVec_);
  }
}

void RDFProcessor::writeOutput() {
  TFile* fout = TFile::Open(outputFile_.c_str(), "recreate");
  fout->cd();
  for(auto& hv : histoVec_) {
    TString selDir(hv.selectionName);
    TString systDir(hv.dir);
    TString path = selDir + "/" + systDir;
    if(fout->GetDirectory(path) == nullptr) 
      fout->mkdir(path);
    fout->cd(path);
    for(auto& h : hv.h1Dvec)
      h.GetValue().Write();
    fout->cd();
  }
  fout->Save();
  fout->Close();
}


