#pragma once

#include <G4Types.hh>
#include <G4String.hh>

struct Settings {
  bool     valid = true;
  
  G4String macro;
  G4String session;
  G4int    nThreads = 0;
  
  G4double sampleDistance         = 5;
  G4bool   constructSampleHolder  = false;
};

Settings ReadSettings (int argc,char** argv);