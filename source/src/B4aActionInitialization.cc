#include "B4aActionInitialization.hh"
#include "B4PrimaryGeneratorAction.hh"
#include "B4RunAction.hh"
#include "B4aEventAction.hh"
#include "B4aSteppingAction.hh"
#include "B4DetectorConstruction.hh"
#include "PhotonLoggingStackingAction.hh"
#include "SettingsReader.hh"

B4aActionInitialization::B4aActionInitialization (B4DetectorConstruction&  detConstruction_,
                                                  Settings&                settings_,
                                                  DataLogger&              logger) :
  G4VUserActionInitialization   ( ),
  detConstruction               (detConstruction_),
  settings                      (settings_),
  dataLogger                    (logger)
{}

void B4aActionInitialization::BuildForMaster() const
{
  SetUserAction(new B4RunAction);
}

void B4aActionInitialization::Build() const
{
  SetUserAction(new B4PrimaryGeneratorAction (settings.primaryEnergyMode == PrimaryEnergyMode::Table));
  SetUserAction(new B4RunAction);
  auto eventAction = new B4aEventAction;
  SetUserAction(eventAction);
  SetUserAction(new B4aSteppingAction(&detConstruction, eventAction));
  SetUserAction(new PhotonLoggingStackingAction (dataLogger));
}  
