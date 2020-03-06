//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// 
/// \file B4DetectorConstruction.cc
/// \brief Implementation of the B4DetectorConstruction class

#include "B4DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4SubtractionSolid.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#define UNUSED(expr) do { (void)(expr); } while (0)

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal 
G4GlobalMagFieldMessenger* B4DetectorConstruction::fMagFieldMessenger = nullptr; 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4DetectorConstruction::B4DetectorConstruction()
 : G4VUserDetectorConstruction(),
   absorberPV    (nullptr),
   fCheckOverlaps(true)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4DetectorConstruction::~B4DetectorConstruction()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4DetectorConstruction::Construct()
{
  // Define materials 
  DefineMaterials();
  
  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4DetectorConstruction::DefineMaterials()
{ 
  // Lead material defined using NIST Manager
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Ge");
  nistManager->FindOrBuildMaterial("G4_Al");
  nistManager->FindOrBuildMaterial("G4_AIR");
  nistManager->FindOrBuildMaterial("G4_TEFLON");
  
  // Liquid argon material
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;  
  G4double density; 
  // Vacuum
  new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);
  
  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4DetectorConstruction::DefineVolumes()
{  
  // Geometry parameters
  G4double crystalDiameter            = 51. *mm;
  G4double crystalLength              = 42. *mm;
  G4double crystalHoleDiameter        = 7.5 *mm;
  G4double crystalHoleDepth           = 37.5*mm;
  
  G4double endCapThickness            = 1.5 *mm;
  G4double endCapOuterDiameter        = 7.6 *cm;
  G4double endCapInnerDiameter        = endCapOuterDiameter - endCapThickness;
  G4double endCapLength               = 7.  *cm;//TODO...
  
  G4double crystalHolderThickness     = 0.76 *mm;
  G4double crystalHolderInnerDiameter = crystalDiameter; //TODO actually there is a 0.5 mm boron layer on the outside of the crystal
  G4double crystalHolderOuterDiameter = crystalHolderInnerDiameter + crystalHolderThickness;
  G4double crystalHolderExtraLength   = 8. * mm;//TODO random guess
  G4double crystalHolderLength        = crystalLength + crystalHolderExtraLength;
  G4double crystalHolderEndThickness  = 3.2 *mm;
  
  G4double protrusionLength           = 8. * mm;//TODO random guess
  G4double protrusionThickness        = crystalHolderEndThickness;
  G4double protrusionOuterDiameter    = 30 *mm;//TODO random guess
  G4double protrusionInnerDiameter    = protrusionOuterDiameter - protrusionThickness;
  
  G4double teflonDiameter             = protrusionInnerDiameter;
  G4double teflonLength               = protrusionLength - protrusionThickness;//...
  
  // Positions - endcap starts at 0
  
  G4double windowDistance          = 5. *mm;
  
  G4double calorimeterShift        =  endCapLength / 2;
  G4double START                   = -endCapLength / 2;
  
  G4double endCapShift             = START + endCapLength / 2;
  G4double endCapDisk1Shift        = START + endCapThickness / 2;
  G4double endCapDisk2Shift        = START + endCapLength - endCapThickness / 2;
  
  G4double crystalShift            = START + windowDistance + crystalLength / 2;
  G4double crystalHoleRelShift     = crystalLength / 2 - crystalHoleDepth / 2;
  
  G4double crystalHolderShift      = START + windowDistance + crystalHolderLength / 2;
  //G4double crystalHolderDisk1Shift = START + windowDistance + crystalHolderThickness / 2;//What is on the front?
  G4double crystalHolderDisk2Shift = START + windowDistance + crystalHolderLength - crystalHolderThickness / 2;
  
  G4double protrusionShift         = START + windowDistance + crystalHolderLength + protrusionLength / 2;
  G4double protrusionDiskShift     = START + windowDistance + crystalHolderLength + protrusionLength - protrusionThickness / 2;
  G4double teflonShift             = START + windowDistance + crystalHolderLength + teflonLength / 2;
  
  
  //
  G4double calorSize = endCapLength * 1.1;
  
  auto worldSize = 1.1 * calorSize;
  auto ZSpace    = 1.0 *m;
  
  // Get materials
  auto defaultMaterial  = G4Material::GetMaterial("G4_AIR");
  auto absorberMaterial = G4Material::GetMaterial("G4_Ge");
  auto vacuum           = G4Material::GetMaterial("Galactic");
  auto aluminium        = G4Material::GetMaterial("G4_Al");
  auto teflon           = G4Material::GetMaterial("G4_TEFLON");
  
  if ( ! defaultMaterial || ! absorberMaterial || !vacuum || !aluminium ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined."; 
    G4Exception("B4DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }
  
  //
  // World
  //
  
  auto worldS
    = new G4Box("World",           // its name
                 worldSize/2, worldSize/2, worldSize/2 + ZSpace); // its size
  
  auto worldLV
    = new G4LogicalVolume(
                 worldS,           // its solid
                 defaultMaterial,  // its material
                 "World");         // its name
  
  auto worldPV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 worldLV,          // its logical volume
                 "World",          // its name
                 0,                // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 
  
  //
  // Calorimeter
  //
  
  auto calorimeterSolid          = new G4Tubs ("Calorimeter", 0, endCapOuterDiameter/2, endCapLength/2, 0, 2*M_PI);
  auto calorimeterLogicalVolume  = new G4LogicalVolume (calorimeterSolid, vacuum, "Calorimeter");
  auto calorimeterPhysicalVolume = new G4PVPlacement (0, G4ThreeVector(0, 0, calorimeterShift),
                                                      calorimeterLogicalVolume,
                                                      "Calorimeter",
                                                      worldLV,
                                                      false, 0, fCheckOverlaps);
  
  //------------------------
  //--------Endcap----------
  auto endCapSolid          = new G4Tubs ("EndCap", endCapInnerDiameter/2, endCapOuterDiameter/2, endCapLength/2, 0, 2*M_PI);
  auto endCapLogicalVolume  = new G4LogicalVolume (endCapSolid, aluminium, "EndCap");
  auto endCapPhysicalVolume = new G4PVPlacement (0, G4ThreeVector(0, 0, endCapShift),
                                                 endCapLogicalVolume,
                                                 "EndCap",
                                                 calorimeterLogicalVolume,
                                                 false, 0, fCheckOverlaps);
  
  auto endCapDisk1Solid          = new G4Tubs ("EndCapDisk1", 0, endCapOuterDiameter/2 - endCapThickness/2, endCapThickness/2, 0, 2*M_PI);
  auto endCapDisk1LogicalVolume  = new G4LogicalVolume (endCapDisk1Solid, aluminium, "EndCapDisk1");
  auto endCapDisk1PhysicalVolume = new G4PVPlacement (0, G4ThreeVector(0, 0, endCapDisk1Shift),
                                                      endCapDisk1LogicalVolume,
                                                      "EndCapDisk1",
                                                      calorimeterLogicalVolume,
                                                      false, 0, fCheckOverlaps);
  
  auto endCapDisk2Solid          = new G4Tubs ("EndCapDisk2", 0, endCapOuterDiameter/2 - endCapThickness/2, endCapThickness/2, 0, 2*M_PI);
  auto endCapDisk2LogicalVolume  = new G4LogicalVolume (endCapDisk2Solid, aluminium, "EndCapDisk2");
  auto endCapDisk2PhysicalVolume = new G4PVPlacement (0, G4ThreeVector(0, 0, endCapDisk2Shift),
                                                      endCapDisk2LogicalVolume,
                                                      "EndCapDisk2",
                                                      calorimeterLogicalVolume,
                                                      false, 0, fCheckOverlaps);
  
  //------------------------
  //--------Crystal---------
  auto crystalCylinderSolid = new G4Tubs ("CrystalBase", 0, crystalDiameter/2,     crystalLength/2,    0, 2*M_PI);
  auto crystalHoleSolid     = new G4Tubs ("CrystalHole", 0, crystalHoleDiameter/2, crystalHoleDepth/2, 0, 2*M_PI);
  auto crystalSolid         = new G4SubtractionSolid ("Crystal", crystalCylinderSolid, crystalHoleSolid, 0, G4ThreeVector(0, 0, crystalHoleRelShift));
  
  auto crystalLogicalVolume  = new G4LogicalVolume (crystalSolid, absorberMaterial, "Crystal");
  auto crystalPhysicalVolume = new G4PVPlacement (0, G4ThreeVector(0, 0, crystalShift),//
                                                 crystalLogicalVolume,
                                                 "Crystal",
                                                 calorimeterLogicalVolume,
                                                 false, 0, fCheckOverlaps);
  
  absorberPV = crystalPhysicalVolume;
  
  //------------------------
  //----Crystal holder------
  auto crystalHolderSolid          = new G4Tubs ("CrystalHolder", crystalHolderInnerDiameter/2, crystalHolderOuterDiameter/2, crystalHolderLength/2, 0, 2*M_PI);
  auto crystalHolderLogicalVolume  = new G4LogicalVolume (crystalHolderSolid, aluminium, "CrystalHolder");
  auto crystalHolderPhysicalVolume = new G4PVPlacement (0, G4ThreeVector(0, 0, crystalHolderShift),
                                                        crystalHolderLogicalVolume,
                                                        "CrystalHolder",
                                                        calorimeterLogicalVolume,
                                                        false, 0, fCheckOverlaps);
  
  /*auto crystalHolderDisk1Solid          = new G4Tubs ("CrystalHolderDisk1", 0, crystalHolderOuterDiameter/2 - crystalHolderThickness/2, crystalHolderThickness/2-1*nm, 0, 2*M_PI);
  auto crystalHolderDisk1LogicalVolume  = new G4LogicalVolume (crystalHolderDisk1Solid, aluminium, "CrystalHolderDisk1");
  auto crystalHolderDisk1PhysicalVolume = new G4PVPlacement (0, G4ThreeVector(0, 0, crystalHolderDisk1Shift),
                                                             crystalHolderDisk1LogicalVolume,
                                                             "CrystalHolderDisk1",
                                                             calorimeterLogicalVolume,
                                                             false, 0, fCheckOverlaps);*/
  
  auto crystalHolderDisk2Solid          = new G4Tubs ("CrystalHolderDisk2", teflonDiameter/2, crystalHolderOuterDiameter/2 - crystalHolderThickness/2, crystalHolderThickness/2, 0, 2*M_PI);
  auto crystalHolderDisk2LogicalVolume  = new G4LogicalVolume (crystalHolderDisk2Solid, aluminium, "CrystalHolderDisk2");
  auto crystalHolderDisk2PhysicalVolume = new G4PVPlacement (0, G4ThreeVector(0, 0, crystalHolderDisk2Shift),
                                                             crystalHolderDisk2LogicalVolume,
                                                             "CrystalHolderDisk2",
                                                             calorimeterLogicalVolume,
                                                             false, 0, fCheckOverlaps);
  
  //------------------------
  //-------protrusion-------
  auto protrusionSolid          = new G4Tubs ("CrystalHolderProtrusion", protrusionInnerDiameter/2, protrusionOuterDiameter/2, protrusionLength/2, 0, 2*M_PI);
  auto protrusionLogicalVolume  = new G4LogicalVolume (protrusionSolid, aluminium, "CrystalHolderProtrusion");
  auto protrusionPhysicalVolume = new G4PVPlacement (0, G4ThreeVector(0, 0, protrusionShift),
                                                     protrusionLogicalVolume,
                                                     "CrystalHolderProtrusion",
                                                     calorimeterLogicalVolume,
                                                     false, 0, fCheckOverlaps);
  
  auto protrusionDiskSolid          = new G4Tubs ("CrystalHolderProtrusionDisk", 0, protrusionOuterDiameter/2 - protrusionThickness/2, protrusionThickness/2, 0, 2*M_PI);
  auto protrusionDiskLogicalVolume  = new G4LogicalVolume (protrusionDiskSolid, aluminium, "CrystalHolderProtrusionDisk");
  auto protrusionDiskPhysicalVolume = new G4PVPlacement (0, G4ThreeVector(0, 0, protrusionDiskShift),
                                                         protrusionDiskLogicalVolume,
                                                         "CrystalHolderProtrusionDisk",
                                                         calorimeterLogicalVolume,
                                                         false, 0, fCheckOverlaps);
  
  auto protrusionTeflonSolid          = new G4Tubs ("CrystalHolderProtrusionTeflon", 0, teflonDiameter/2, teflonLength/2, 0, 2*M_PI);
  auto protrusionTeflonLogicalVolume  = new G4LogicalVolume (protrusionTeflonSolid, teflon, "CrystalHolderProtrusionTeflon");
  auto protrusionTeflonPhysicalVolume = new G4PVPlacement (0, G4ThreeVector(0, 0, teflonShift),
                                                           protrusionTeflonLogicalVolume,
                                                           "CrystalHolderProtrusionTeflon",
                                                           calorimeterLogicalVolume,
                                                           false, 0, fCheckOverlaps);
  
  UNUSED(calorimeterPhysicalVolume);
  UNUSED(endCapPhysicalVolume);
  UNUSED(endCapDisk1PhysicalVolume);
  UNUSED(endCapDisk2PhysicalVolume);
  UNUSED(crystalPhysicalVolume);
  UNUSED(crystalHolderPhysicalVolume);
  UNUSED(crystalHolderDisk2PhysicalVolume);
  UNUSED(protrusionPhysicalVolume);
  UNUSED(protrusionDiskPhysicalVolume);
  UNUSED(protrusionTeflonPhysicalVolume);
  
  //
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());
  calorimeterLogicalVolume->SetVisAttributes(G4VisAttributes::GetInvisible());
  
  crystalLogicalVolume->SetVisAttributes (new G4VisAttributes(G4Color(1.0, 0.5, 0.5)));
  //
  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4DetectorConstruction::ConstructSDandField()
{ 
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue;
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);
  
  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
