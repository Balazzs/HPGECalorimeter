#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include "globals.hh"
#include <memory>

class G4ParticleGun;
class G4Event;

class B4PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  B4PrimaryGeneratorAction(G4bool generateGammaFromTable);
  
  virtual void GeneratePrimaries(G4Event* event);
  
private:
  std::unique_ptr<G4GeneralParticleSource>  particleGun;
  G4bool                                    generateGammaFromTable;
};
