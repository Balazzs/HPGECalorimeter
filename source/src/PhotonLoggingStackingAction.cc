#include "PhotonLoggingStackingAction.hh"
#include "G4Gamma.hh"
#include "G4Track.hh"

PhotonLoggingStackingAction::PhotonLoggingStackingAction (DataLogger& logger_) :
  logger (logger_)
{
}

G4ClassificationOfNewTrack PhotonLoggingStackingAction::ClassifyNewTrack(const G4Track* track)
{
  const G4ParticleDefinition* particleDefinition = track->GetDefinition();
  
  if (particleDefinition == G4Gamma::Definition())
  {
    G4double energy = track->GetKineticEnergy();
    logger.LogPhotonCreation (energy);
  }
  
  return fUrgent;
}
