#pragma once

#include "G4VUserActionInitialization.hh"
#include "DataLogger.hh"

class B4DetectorConstruction;
class Settings;

class B4aActionInitialization : public G4VUserActionInitialization
{
  public:
    B4aActionInitialization(B4DetectorConstruction&, Settings&, DataLogger&);
    
    virtual void BuildForMaster() const;
    virtual void Build() const;
    
  private:
    B4DetectorConstruction& detConstruction;
    Settings&               settings;
    DataLogger&             dataLogger;
};
