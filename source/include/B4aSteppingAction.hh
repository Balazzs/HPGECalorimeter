#ifndef B4aSteppingAction_h
#define B4aSteppingAction_h

#include "G4UserSteppingAction.hh"

class B4DetectorConstruction;
class B4aEventAction;

class B4aSteppingAction : public G4UserSteppingAction
{
public:
  B4aSteppingAction(const B4DetectorConstruction* detectorConstruction, B4aEventAction* eventAction);
  virtual ~B4aSteppingAction() = default;
  
  virtual void UserSteppingAction(const G4Step* step);
  
private:
  const B4DetectorConstruction* detectorConstruction;
  B4aEventAction*               eventAction;
};

#endif
