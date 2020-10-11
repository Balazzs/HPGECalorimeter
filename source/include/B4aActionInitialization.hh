#pragma once

#include "G4VUserActionInitialization.hh"
#include "DataLogger.hh"

class B4DetectorConstruction;

class B4aActionInitialization : public G4VUserActionInitialization
{
  public:
    B4aActionInitialization(B4DetectorConstruction*, DataLogger&);
    virtual ~B4aActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;

  private:
    B4DetectorConstruction* fDetConstruction;
    DataLogger&             dataLogger;
};
