#pragma once

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "SettingsReader.hh"

class G4VPhysicalVolume;
using CLHEP::cm;

class B4DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  B4DetectorConstruction(DetectorGeometryType geometryType          = DetectorGeometryType::Real,
                         G4double             sampleDistance        = 5.0 *cm,
                         G4bool               constructSampleHolder = false);
  
public:
  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();
  
  const G4VPhysicalVolume* GetAbsorberPV() const;
  
private:
  void DefineMaterials();
  
private:
  DetectorGeometryType  geometryType;
  G4double              sampleDistance;
  G4bool                constructSampleHolder;
  G4VPhysicalVolume*    absorberPV;     // the absorber physical volume
  G4bool                fCheckOverlaps; // option to activate checking of volumes overlaps
};

inline const G4VPhysicalVolume* B4DetectorConstruction::GetAbsorberPV() const { 
  return absorberPV; 
}
