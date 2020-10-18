#include "B4DetectorConstruction.hh"
#include "DetectorGeometry.hh"
#include "G4NistManager.hh"
#include "G4PhysicalConstants.hh"

B4DetectorConstruction::B4DetectorConstruction(G4double sampleDistance_, G4bool constructSampleHolder_)
 : G4VUserDetectorConstruction(),
   sampleDistance         (sampleDistance_),
   constructSampleHolder  (constructSampleHolder_),
   absorberPV             (nullptr),
   fCheckOverlaps         (true)
{
}

G4VPhysicalVolume* B4DetectorConstruction::Construct()
{
  DefineMaterials ();
  return ConstructDetectorGeometry (DetectorGeometryType::Empty, absorberPV, fCheckOverlaps, constructSampleHolder, sampleDistance);
}

void B4DetectorConstruction::DefineMaterials()
{
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Ge");
  nistManager->FindOrBuildMaterial("G4_Al");
  nistManager->FindOrBuildMaterial("G4_AIR");
  nistManager->FindOrBuildMaterial("G4_TEFLON");
  nistManager->FindOrBuildMaterial("G4_CALCIUM_CARBONATE");
  nistManager->FindOrBuildMaterial("G4_PLEXIGLASS");
  
  // Liquid argon material
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;  
  G4double density; 
  // Vacuum
  new G4Material("Galactic", z=1., a=1.01*g/mole, density=universe_mean_density, kStateGas, 77*kelvin, 3.e-18*pascal);
  
  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}


void B4DetectorConstruction::ConstructSDandField()
{
  
}
