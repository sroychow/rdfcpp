#include "RDFProcessor.h"
#include "TString.h"
#include <TApplication.h>
#include "TStopwatch.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "json.hpp"

//using ROOT::RDF::RNode;
using json = nlohmann::json;

int main() {
  const std::string& varFile="inputdata/variablesColumns.json";
  const std::string& weightFile="inputdata/weightColumns.json";
  const std::string& selectionsFile="inputdata/selections.json";
  const std::string& systematicsFile="inputdata/systematics.json";
  const std::string& samplesFile="inputdata/samples.json";
 
  json samplesjson;
  std::ifstream sf(samplesFile.c_str());
  sf >> samplesjson;
  sf.close();

  std::string sampleName = "ST_s-channel_4f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1";
  std::string inputDir = "/eos/user/r/roy4/data/NanoAOD2016-V1/";

  float samplexsec = samplesjson[sampleName]["xsec"];
  std::string sampleType = samplesjson[sampleName]["datatype"];
  std::vector< std::string > sampleFiles;
  for(auto& [subdir, treeList] :  samplesjson[sampleName]["dir"].items()) {
    std::cout << subdir << std::endl;
    std::cout << treeList << std::endl;
    for(auto& t : treeList) {
      auto file = inputDir + "/" + subdir + "/" + t.get<std::string>();
      std::cout << file << std::endl;
      sampleFiles.emplace_back(file);
    }
  }
  std::cout << "Sample name = " << sampleName << std::endl;
  std::cout << "Sample type = " << sampleType << std::endl;
  std::cout << "Sample xec = " << samplexsec << std::endl;
  //std::cout << "Sample xec = " << sampleDirs << std::endl;
  //const std::string_view inFile = "/eos/user/r/roy4//data/NanoAOD2016-V1/QCD_Pt-30to50_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/tree.root";
  //const std::string_view inFile = "/eos/user/r/roy4/data/NanoAOD2016-V1/SingleMuon_Run2016C/tree.root";
  //const std::string_view inFile = "/eos/user/r/roy4/data/NanoAOD2016-V1/ST_s-channel_4f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1/tree.root";

  
  ROOT::EnableImplicitMT(8);
  RDFProcessor* rf = new RDFProcessor("Events", sampleFiles, sampleType);
  rf->setVariables(varFile);
  rf->setWeights(weightFile);
  rf->setSelections(selectionsFile);
  rf->setSystematics(systematicsFile);
  std::cout << rf->getVariables().size() << std::endl;
  TStopwatch timer;
  timer.Start();
  rf->defineColumnsofInterest();
  rf->branchModules();
  rf->writeOutput(); 
  timer.Stop();
  std::cout << "Realtime/CpuTime = " << timer.RealTime() << "/" << timer.CpuTime() << std::endl;
  
  return 0;
}
