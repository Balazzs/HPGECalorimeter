#pragma once

#include "G4UserStackingAction.hh"
#include "DataLogger.hh"

class PhotonLoggingStackingAction : public G4UserStackingAction
{
public:
  PhotonLoggingStackingAction (DataLogger& logger);
  
  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*) override;
  
private:
  DataLogger& logger;
};