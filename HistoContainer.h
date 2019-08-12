#ifndef HistoContainer_h
#define HistoContainer_h
#include<vector>
#include<string>
#include "ROOT/RResultPtr.hxx"
#include "ROOT/RDF/HistoModels.hxx"

struct Variable1D{
  std::string varName;//name of variable
  std::string colX;//defn of col
  ROOT::RDF::TH1DModel h1D;
};

struct Variable2D{
  std::string varName;//name of variable
  std::string colX;//defn of col
  std::string colY;
  ROOT::RDF::TH2DModel h2D;
};


struct Variable3D{
  std::string varName;//name of variable
  std::string colX;//defn of col
  std::string colY;
  std::string colZ;
  ROOT::RDF::TH3DModel h3D;
};

struct HistoContainer {
  std::string selectionName;//topLevelDir
  std::string dir;//first subdir
  std::string subdir;//sub subdir
  std::vector<ROOT::RDF::RResultPtr<::TH1D >>   h1Dvec;
  std::vector<ROOT::RDF::RResultPtr<::TH2D >>   h2Dvec;
  std::vector<ROOT::RDF::RResultPtr<::TH3D >>   h3Dvec;
};
#endif
