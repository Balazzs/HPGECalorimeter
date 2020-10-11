#include "DetectorGeometry.hh"

#include "G4Material.hh"

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

//Predeclare
void                DefineSampleHolder  (G4LogicalVolume* worldLV, G4double sampleDistance, bool fCheckOverlaps);
G4VPhysicalVolume*  DefineEmptySpace    (bool fCheckOverlaps);
void                DefineSimpleVolumes (G4LogicalVolume* worldLV, G4VPhysicalVolume*& absorberPV, bool fCheckOverlaps);
void                DefineVolumes       (G4LogicalVolume* worldLV, G4VPhysicalVolume*& absorberPV, bool fCheckOverlaps);
//

G4VPhysicalVolume* ConstructDetectorGeometry (DetectorGeometryType  type,
                                              G4VPhysicalVolume*&   absorberPV,//TODO ugly
                                              bool                  fCheckOverlaps,
                                              bool                  constructSampleHolder,
                                              double                sampleHolderDistance)
{
  auto worldPhysicalVolume = DefineEmptySpace (fCheckOverlaps);
  auto worldLV = worldPhysicalVolume->GetLogicalVolume ();
  
  if (constructSampleHolder) {
    DefineSampleHolder (worldLV, sampleHolderDistance, fCheckOverlaps);
  }
  
  switch (type) {
    case DetectorGeometryType::Empty:
      //Do nothing
      break;
    case DetectorGeometryType::Simple:
      DefineSimpleVolumes (worldLV, absorberPV, fCheckOverlaps);
    case DetectorGeometryType::Real:
    default:
      DefineVolumes (worldLV, absorberPV, fCheckOverlaps);
  }
  
  return worldPhysicalVolume;
}

constexpr G4double crystalDiameter            = 51. *mm;
constexpr G4double crystalLength              = 42. *mm;
constexpr G4double crystalHoleDiameter        = 7.5 *mm;
constexpr G4double crystalHoleDepth           = 37.5*mm;

constexpr G4double endCapThickness            = 1.5 *mm;
constexpr G4double endCapOuterDiameter        = 7.6 *cm;
constexpr G4double endCapInnerDiameter        = endCapOuterDiameter - endCapThickness;
constexpr G4double endCapLength               = 7.  *cm;//TODO...

constexpr G4double crystalHolderThickness     = 0.76 *mm;
constexpr G4double crystalHolderInnerDiameter = crystalDiameter; //TODO actually there is a 0.5 mm boron layer on the outside of the crystal
constexpr G4double crystalHolderOuterDiameter = crystalHolderInnerDiameter + crystalHolderThickness;
constexpr G4double crystalHolderExtraLength   = 8. * mm;//TODO random guess
constexpr G4double crystalHolderLength        = crystalLength + crystalHolderExtraLength;
constexpr G4double crystalHolderEndThickness  = 3.2 *mm;

constexpr G4double protrusionLength           = 8. * mm;//TODO random guess
constexpr G4double protrusionThickness        = crystalHolderEndThickness;
constexpr G4double protrusionOuterDiameter    = 30 *mm;//TODO random guess
constexpr G4double protrusionInnerDiameter    = protrusionOuterDiameter - protrusionThickness;

constexpr G4double teflonDiameter             = protrusionInnerDiameter;
constexpr G4double teflonLength               = protrusionLength - protrusionThickness;//...

constexpr G4double sampleDiameter             = 6.  *cm;
constexpr G4double sampleHeight               = 0.5 *cm;
constexpr G4double sampleHolderThickness      = 1.  *mm;
constexpr G4double sampleHolderInnerDiameter  = sampleDiameter;
constexpr G4double sampleHolderOuterDiameter  = sampleDiameter + 2 * sampleHolderThickness;

// Positions - endcap starts at 0

constexpr G4double windowDistance          = 5. *mm;

constexpr G4double calorimeterShift        =  endCapLength / 2;
constexpr G4double START                   = -endCapLength / 2;

constexpr G4double endCapShift             = START + endCapLength / 2;
constexpr G4double endCapDisk1Shift        = START + endCapThickness / 2;
constexpr G4double endCapDisk2Shift        = START + endCapLength - endCapThickness / 2;

constexpr G4double crystalShift            = START + windowDistance + crystalLength / 2;
constexpr G4double crystalHoleRelShift     = crystalLength / 2 - crystalHoleDepth / 2;

constexpr G4double crystalHolderShift      = START + windowDistance + crystalHolderLength / 2;
//constexpr G4double crystalHolderDisk1Shift = START + windowDistance + crystalHolderThickness / 2;//What is on the front?
constexpr G4double crystalHolderDisk2Shift = START + windowDistance + crystalHolderLength - crystalHolderThickness / 2;

constexpr G4double protrusionShift         = START + windowDistance + crystalHolderLength + protrusionLength / 2;
constexpr G4double protrusionDiskShift     = START + windowDistance + crystalHolderLength + protrusionLength - protrusionThickness / 2;
constexpr G4double teflonShift             = START + windowDistance + crystalHolderLength + teflonLength / 2;

//
constexpr G4double calorSize = endCapLength * 1.1;

constexpr auto worldSize = 1.1 * calorSize;
constexpr auto ZSpace    = 1.0 *m;

G4VPhysicalVolume* DefineEmptySpace (bool fCheckOverlaps)
{
  auto defaultMaterial  = G4Material::GetMaterial("G4_AIR");
  
  auto worldS = new G4Box("World", worldSize/2, worldSize/2, worldSize/2 + ZSpace);
  
  auto worldLV = new G4LogicalVolume(worldS,           // its solid
                                     defaultMaterial,  // its material
                                     "World");         // its name
  
  auto worldPV = new G4PVPlacement(0,                // no rotation
                                   G4ThreeVector(),  // at (0,0,0)
                                   worldLV,          // its logical volume
                                   "World",          // its name
                                   0,                // its mother  volume
                                   false,            // no boolean operation
                                   0,                // copy number
                                   fCheckOverlaps);  // checking overlaps
  
  return worldPV;
}

void DefineSimpleVolumes (G4LogicalVolume*      worldLV,
                          G4VPhysicalVolume*&   absorberPV,//TODO ugly
                          bool                  fCheckOverlaps)
{
  // Get materials
  auto defaultMaterial  = G4Material::GetMaterial("G4_AIR");
  auto absorberMaterial = G4Material::GetMaterial("G4_Ge");
  auto vacuum           = G4Material::GetMaterial("Galactic");
  auto aluminium        = G4Material::GetMaterial("G4_Al");
  auto CaCO3            = G4Material::GetMaterial("G4_CALCIUM_CARBONATE");
  auto glass            = G4Material::GetMaterial("G4_PLEXIGLASS");
  
  if ( ! defaultMaterial || ! absorberMaterial || !vacuum || !aluminium || !CaCO3 || !glass) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined."; 
    G4Exception("B4DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }
  
  //------------------------
  //--------Crystal---------
  auto crystalCylinderSolid = new G4Tubs ("CrystalBase", 0, crystalDiameter/2,     crystalLength/2,    0, 2*M_PI);
  
  auto crystalLogicalVolume  = new G4LogicalVolume (crystalCylinderSolid, absorberMaterial, "Crystal");
  auto crystalPhysicalVolume = new G4PVPlacement (0, G4ThreeVector(0, 0, crystalShift + calorimeterShift),//
                                                 crystalLogicalVolume,
                                                 "Crystal",
                                                 worldLV,
                                                 false, 0, fCheckOverlaps);
  
  absorberPV = crystalPhysicalVolume;
  
  UNUSED(crystalPhysicalVolume);
  
  //
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());
  crystalLogicalVolume->SetVisAttributes (new G4VisAttributes(G4Color(1.0, 0.5, 0.5)));
}

void DefineVolumes (G4LogicalVolume*      worldLV,
                    G4VPhysicalVolume*&   absorberPV,//TODO ugly
                    bool                  fCheckOverlaps)
{
  // Get materials
  auto defaultMaterial  = G4Material::GetMaterial("G4_AIR");
  auto absorberMaterial = G4Material::GetMaterial("G4_Ge");
  auto vacuum           = G4Material::GetMaterial("Galactic");
  auto aluminium        = G4Material::GetMaterial("G4_Al");
  auto teflon           = G4Material::GetMaterial("G4_TEFLON");
  
  //TODO set temperature?
  
  if (!defaultMaterial || !absorberMaterial || !vacuum || !aluminium) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined.";
    G4Exception("B4DetectorConstruction::DefineVolumes()", "MyCode0001", FatalException, msg);
  }
  
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
}

void DefineSampleHolder (G4LogicalVolume* worldLV, G4double sampleDistance, bool fCheckOverlaps)
{
  auto CaCO3 = G4Material::GetMaterial("G4_CALCIUM_CARBONATE");
  auto glass = G4Material::GetMaterial("G4_PLEXIGLASS");//TODO what is it actually made of?
  
  //TODO set temperature?
  
  if ( !CaCO3 || !glass) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined.";
    G4Exception("DefineSampleHolder()", "MyCode0001", FatalException, msg);
  }
  
  const G4double sampleHolderDisk1Shift  = -sampleDistance - sampleHolderThickness / 2;
  const G4double sampleHolderShift       = sampleHolderDisk1Shift - sampleHeight / 2;
  const G4double sampleShift             = sampleHolderShift;
  
  auto sampleSolid          = new G4Tubs ("Sample", 0, sampleDiameter/2, sampleHeight/2, 0, 2*M_PI);
  auto sampleLogicalVolume  = new G4LogicalVolume (sampleSolid, CaCO3, "Sample");
  auto samplePhysicalVolume = new G4PVPlacement (0, G4ThreeVector(0, 0, sampleShift),
                                                 sampleLogicalVolume,
                                                 "Sample",
                                                 worldLV,
                                                 false, 0, fCheckOverlaps);
  
  auto sampleHolderSideSolid          = new G4Tubs ("SampleHolderSide", sampleHolderInnerDiameter/2, sampleHolderOuterDiameter/2, sampleHeight/2, 0, 2*M_PI);
  auto sampleHolderSideLogicalVolume  = new G4LogicalVolume (sampleHolderSideSolid, glass, "SampleHolderSide");
  auto sampleHolderSidePhysicalVolume = new G4PVPlacement (0, G4ThreeVector(0, 0, sampleHolderShift),
                                                           sampleHolderSideLogicalVolume,
                                                           "SampleHolderSide",
                                                           worldLV,
                                                           false, 0, fCheckOverlaps);
  
  auto sampleHolderDisk1Solid          = new G4Tubs ("SampleHolderDisk1", 0, sampleHolderOuterDiameter/2, sampleHolderThickness/2, 0, 2*M_PI);
  auto sampleHolderDisk1LogicalVolume  = new G4LogicalVolume (sampleHolderDisk1Solid, glass, "SampleHolderDisk1");
  auto sampleHolderDisk1PhysicalVolume = new G4PVPlacement (0, G4ThreeVector(0, 0, sampleHolderDisk1Shift),
                                                            sampleHolderDisk1LogicalVolume,
                                                            "SampleHolderDisk1",
                                                            worldLV,
                                                            false, 0, fCheckOverlaps);
  
  UNUSED(samplePhysicalVolume);
  UNUSED(sampleHolderSidePhysicalVolume);
  UNUSED(sampleHolderDisk1PhysicalVolume);
}