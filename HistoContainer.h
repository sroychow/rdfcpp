#ifndef HistoContainer_h
#define HistoContainer_h
#include<vector>
#include<string>
#include "ROOT/RResultPtr.hxx"
#include "ROOT/RDF/HistoModels.hxx"

struct Variable1D{
  std::string varName;//name of variable
  std::string colX;//defn of col
  std::string systdir;
  ROOT::RDF::TH1DModel h1D;
};

struct Variable2D{
  std::string varName;//name of variable
  std::string colX;//defn of col
  std::string colY;
  std::string systdir;
  ROOT::RDF::TH2DModel h2D;
};


struct Variable3D{
  std::string varName;//name of variable
  std::string colX;//defn of col
  std::string colY;
  std::string colZ;
  std::string systdir;
  ROOT::RDF::TH3DModel h3D;
};
//directory structure - selection/systdir
//create one such container for each selection
struct HistoContainer {
  std::string selectionName;//topLevelDir
  std::map<std::string, std::vector<ROOT::RDF::RResultPtr<::TH1D >> >  systh1Dvecmap;
  std::map<std::string, std::vector<ROOT::RDF::RResultPtr<::TH2D >> >  systh2Dvecmap;
  std::map<std::string, std::vector<ROOT::RDF::RResultPtr<::TH3D >> >  systh3Dvecmap;
};
#endif
