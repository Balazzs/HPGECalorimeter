#include "B4PrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include <fstream>

B4PrimaryGeneratorAction::B4PrimaryGeneratorAction(G4bool generateFromTable)
 : G4VUserPrimaryGeneratorAction(),
   particleGun            (new G4GeneralParticleSource()),
   generateGammaFromTable (generateFromTable)
{
  auto particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
  particleGun->SetParticleDefinition(particleDefinition);
}

struct Peak {
  double      E;
  double      probability;
  std::string isotope_name;
};

std::vector<Peak> LoadCumulativeProbability ()
{
  std::vector<Peak> peaks;
  
  std::ifstream file ("U_238.txt");
  
  //header
  std::string header_text;
  file >> header_text >> header_text >> header_text;
  
  std::cout << "Hello" << header_text << std::endl;
  
  //data lines
  Peak p;
  double cum_prob = 0;
  while (file >> p.isotope_name >> p.E >> p.probability)
  {
    cum_prob += p.probability;
    p.probability = cum_prob;
    
    peaks.push_back(p);
    std::cout << p.probability << "\t" << p.E << std::endl;
  }
  
  //normalize
  for (Peak& peak: peaks) {
    peak.probability /= cum_prob;
  }
  
  return peaks;
}

double GenerateRandomEnergy ()
{
  static std::vector<Peak> peaks = LoadCumulativeProbability();
  
  double r = G4UniformRand();
  
  for(const Peak& p : peaks) {
    if (r <= p.probability) {
      return p.E;
    }
  }
  
  G4ExceptionDescription msg;
    msg << "Error in probability distribution. Random number out of range";
    G4Exception("B4PrimaryGeneratorAction::GeneratePrimaries()",
      "MyCode0003", FatalException, msg);
  
  return 0;
}

void B4PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //Default gun position
  {
    G4double worldZHalfLength = 0.;
    auto worldLV = G4LogicalVolumeStore::GetInstance()->GetVolume("World");
    
    // Check that the world volume has box shape
    G4Box* worldBox = nullptr;
    if (  worldLV ) {
      worldBox = dynamic_cast<G4Box*>(worldLV->GetSolid());
    }
    
    if ( worldBox ) {
      worldZHalfLength = worldBox->GetZHalfLength();  
    }
    else  {
      G4ExceptionDescription msg;
      msg << "World volume of box shape not found." << G4endl;
      msg << "Perhaps you have changed geometry." << G4endl;
      msg << "The gun will be place in the center.";
      G4Exception("B4PrimaryGeneratorAction::GeneratePrimaries()",
        "MyCode0002", JustWarning, msg);
    }
    
    // Set gun position
    particleGun->SetParticlePosition(G4ThreeVector(0., 0., -worldZHalfLength));
  }
  
  if (generateGammaFromTable) {
    double energy = GenerateRandomEnergy () * MeV;
    particleGun->GetCurrentSource()->GetEneDist()->SetMonoEnergy (energy);
    G4cout << "GPS energy:" << energy << G4endl;
  }
  
  particleGun->GeneratePrimaryVertex(anEvent);
}
