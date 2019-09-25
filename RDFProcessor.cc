#include "RDFProcessor.h"
#include "TString.h"
#include <TApplication.h>
#include <algorithm>
#include <iomanip>
#include <sstream>

using ROOT::RDF::RNode;

RDFProcessor::RDFProcessor(std::string_view treeName, std::vector<std::string>& fileNames, std::string dataType, float xsec) :
  rdfTree_(ROOT::RDataFrame(treeName, fileNames))
{
  inFilenames_ = fileNames;
  targetLumi_ = 35.922;//this should also be parmetrized:TODO
  xsec_ = xsec;
  dataType_ = dataType;
  isMC_ = dataType == "MC" ? true : false;
  outputFile_ = "test.root";
}

void RDFProcessor::setVariables(const std::string& varFile) {
  std::ifstream fin(varFile.c_str());
  fin >> varjson_;fin.close(); 
  fin.close();

  for(auto& [collectionName, value] : varjson_.items()) {
    //std::cout << "collectionName=" << collectionName << "\n\n\n";
    vars_.emplace_back(VariableBlock(collectionName, value));
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

  for(auto& [stype, smap] : systjson_.items()) {
    if(stype == "ColumnVariations") 
      colvarmap_ = smap;
    else if (stype == "WeightVariations")
      weightvarmap_ = smap;
    else std::cout << "Unknown systematic type\n";
  }
}

void RDFProcessor::defineColumnsofInterest() {
  
  std::vector<std::string> colNames = rdfTree_.GetColumnNames();
  //define columns for variables

  for(auto& varBlock: vars_) {
    for(auto& [syst, varVec] : varBlock.var1D()) {
      for(auto& var : varVec) {
         if(std::find( colNames.begin(), colNames.end(), var.varName) != colNames.end() )   continue;
         //std::cout << var.varName << " = " << var.colX << " needs to be defined" << std::endl;
         rdfTree_ = rdfTree_.Define(var.varName, var.colX);
      }
    }
  }
  std::cout << "#Defined Columns after variable definition=" << rdfTree_.GetDefinedColumnNames().size() << std::endl;
  if(dataType_ == "MC") {
    //define weight histos
    auto runs = ROOT::RDataFrame("Runs", inFilenames_);
    double genEventSumw = runs.Sum("genEventSumw").GetValue();
    std::cout << "genEventSumw " <<  std::setprecision(15) << std::fixed << genEventSumw <<  " weighted events\n";
    std::cout << "xsec " <<  xsec_ <<  " pb\n";
    std::stringstream lumiweight;
    lumiweight << "(" << targetLumi_ << "*" << xsec_ << ")/" << std::fixed << genEventSumw;
    std::cout << "lumiweight " << lumiweight.str() <<  " (|Generator_weight| not accounted for)\n";
    rdfTree_ = rdfTree_.Define("lumiweight", lumiweight.str());
  }
  
  for(auto& [wcol, wtdefs] : hweightsjson_[dataType_].items()) {
    std::cout << "Weight Column:" << wcol << std::endl;
    for(auto& [wtvar, wtvardict] : wtdefs.items()) {
      if(wtvar == "nominal") {
        rdfTree_ = rdfTree_.Define(wcol, wtvardict.get<std::string>());
      }
      else {
        for(auto& [wt, wtexpr] : wtvardict.items()) {
	  rdfTree_ = rdfTree_.Define(wt, wtexpr.get<std::string>());
        }
      }
    }
  }
  std::cout << "#Defined Columns after variable + weight definition=" << rdfTree_.GetDefinedColumnNames().size() << std::endl;
}

void RDFProcessor::branchModules() {
  //Loop over selections, i.e., filters.
  for(auto& [selname, selval] : seljson_.items()) {
    auto nodefilter = selval[dataType_]["cut"].get<std::string>();
    auto nodeweight = selval[dataType_]["weight"].get<std::string>();
    json weightvars = hweightsjson_[dataType_][nodeweight];
    std::cout << "Module created with filter name:" << selname << std::endl; 
    std::cout << "Cut: "    << nodefilter << std::endl; 
    std::cout << "Weight: " << nodeweight << std::endl; 
    Module m(rdfTree_, vars_, nodeweight, nodefilter, selname, weightvarmap_, colvarmap_);
    HistoContainer h;
    h.selectionName = selname;
    m.setHistograms(h);
    histoVec_.emplace_back(h);
  }
}

void RDFProcessor::writeOutput() {
  TFile* fout = TFile::Open(outputFile_.c_str(), "recreate");
  fout->cd();
  for(auto& hv : histoVec_) {
    TString selDir(hv.selectionName);
    for(auto& [systDir, h1Vec] : hv.systh1Dvecmap) {
      fout->cd();
      TString subdir = selDir + "/" + TString(systDir);
      fout->mkdir(subdir);
      fout->cd(subdir);
      for(auto& h : h1Vec)
        h.GetValue().Write();
    } 
    
    for(auto& [systDir, h2Vec] : hv.systh2Dvecmap) {
      fout->cd();
      fout->cd(selDir + "/" + TString(systDir));
      for(auto& h : h2Vec)
        h.GetValue().Write();
    }
    /*
    for(auto& [systDir, h3Vec] : hv.systh3Dvecmap) {
      fout->cd();
      fout->cd(selDir + "/" + TString(systDir));
      for(auto& h : h3Vec)
        h.GetValue().Write();
    }
    */
  }
  fout->Save();
  fout->Close();
}


