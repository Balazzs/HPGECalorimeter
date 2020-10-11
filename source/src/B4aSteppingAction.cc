#include "B4aSteppingAction.hh"
#include "B4aEventAction.hh"
#include "B4DetectorConstruction.hh"
#include "G4Gamma.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"

B4aSteppingAction::B4aSteppingAction(const B4DetectorConstruction* detectorConstruction_,
                                     B4aEventAction*               eventAction_)
  : G4UserSteppingAction  (),
    detectorConstruction  (detectorConstruction_),
    eventAction           (eventAction_)
{}

static void LogExitMomentum (const G4Step& step, B4aEventAction& eventAction)
{
  auto volume = step.GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  
  if (step.IsLastStepInVolume() && volume->GetName() == "World") {
    if (step.GetTrack()->GetDefinition() == G4Gamma::Definition()) {
      auto* photon = step.GetTrack()->GetDynamicParticle();
      eventAction.SetExitDirection (photon->GetMomentumDirection());
    }
  }
}

static G4double GetChargedStepLength (const G4Step& step)
{
  const bool isCharged = step.GetTrack()->GetDefinition()->GetPDGCharge() != 0.0;
  if (isCharged) {
    return step.GetStepLength();
  }
  
  return 0;
}

void B4aSteppingAction::UserSteppingAction(const G4Step* step)
{
  auto volume        = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  auto energyDeposit = step->GetTotalEnergyDeposit();
  auto stepLength    = GetChargedStepLength(*step);
  
  const bool isInAbsorberVolume = volume == detectorConstruction->GetAbsorberPV();
  if (isInAbsorberVolume) {
    eventAction->AddAbs(energyDeposit, stepLength);
  }
  
  LogExitMomentum (*step, *eventAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
