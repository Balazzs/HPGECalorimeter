#pragma once

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;

class B4DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  B4DetectorConstruction();
  virtual ~B4DetectorConstruction();
  
public:
  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();
  
  const G4VPhysicalVolume* GetAbsorberPV() const;
  
private:
  void DefineMaterials();
  
private:
  G4VPhysicalVolume*  absorberPV;     // the absorber physical volume
  G4bool              fCheckOverlaps; // option to activate checking of volumes overlaps
};

// inline functions

inline const G4VPhysicalVolume* B4DetectorConstruction::GetAbsorberPV() const { 
  return absorberPV; 
}
