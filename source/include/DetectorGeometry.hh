#include "G4VPhysicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "SettingsReader.hh"

G4VPhysicalVolume* ConstructDetectorGeometry (DetectorGeometryType  type,
                                              G4VPhysicalVolume*&   absorberPV,//TODO ugly
                                              bool                  fCheckOverlaps,
                                              bool                  constructSampleHolder = false,
                                              double                sampleHolderDistance  = 5.0 *cm);