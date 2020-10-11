#include "G4VPhysicalVolume.hh"
#include "G4SystemOfUnits.hh"

enum class DetectorGeometryType {
  Empty,
  Simple,
  Real
};

G4VPhysicalVolume* ConstructDetectorGeometry (DetectorGeometryType  type,
                                              G4VPhysicalVolume*&   absorberPV,//TODO ugly
                                              bool                  fCheckOverlaps,
                                              bool                  constructSampleHolder = false,
                                              double                sampleHolderDistance  = 5.0 *cm);