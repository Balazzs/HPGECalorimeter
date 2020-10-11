#include "B4DetectorConstruction.hh"
#include "B4aActionInitialization.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "G4UIcommand.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4VModularPhysicsList.hh"

#include "G4RadioactiveDecayPhysics.hh"

#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

#include "Randomize.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"


#define U_238_DECAY_SIMULATION

namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " hpge [-m macro ] [-u UIsession] [-t nThreads]" << G4endl;
    G4cerr << "   note: -t option is available only for multi-threaded mode."
           << G4endl;
  }
}

class MyParticleConstructor : public G4VPhysicsConstructor{
public:
  MyParticleConstructor () :
    G4VPhysicsConstructor("MyParticleConstructor")
  { }
  
  void ConstructProcess () {}
  
  void ConstructParticle()
  {
    G4BosonConstructor  pBosonConstructor;
    pBosonConstructor.ConstructParticle();
    G4LeptonConstructor pLeptonConstructor;
    pLeptonConstructor.ConstructParticle();
    G4MesonConstructor pMesonConstructor;
    pMesonConstructor.ConstructParticle();
    G4BaryonConstructor pBaryonConstructor;
    pBaryonConstructor.ConstructParticle();
    G4IonConstructor pIonConstructor;
    pIonConstructor.ConstructParticle();
    G4ShortLivedConstructor pShortLivedConstructor;
    pShortLivedConstructor.ConstructParticle();
  } 
};



int main(int argc,char** argv)
{
  // Evaluate arguments
  //
  if ( argc > 7 ) {
    PrintUsage();
    return 1;
  }
  
  G4String macro;
  G4String session;
#ifdef G4MULTITHREADED
  G4int nThreads = 0;
#endif
  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
    else if ( G4String(argv[i]) == "-u" ) session = argv[i+1];
#ifdef G4MULTITHREADED
    else if ( G4String(argv[i]) == "-t" ) {
      nThreads = G4UIcommand::ConvertToInt(argv[i+1]);
    }
#endif
    else {
      PrintUsage();
      return 1;
    }
  }
  
  // Detect interactive mode (if no macro provided) and define UI session
  //
  G4UIExecutive* ui = nullptr;
  if ( ! macro.size() ) {
    ui = new G4UIExecutive(argc, argv, session);
  }
  
  DataLogger logger;
  
  // Construct the default run manager
  //
#ifdef G4MULTITHREADED
  auto runManager = new G4MTRunManager;
  if ( nThreads > 0 ) { 
    runManager->SetNumberOfThreads(nThreads);
  }  
#else
  auto runManager = new G4RunManager;
#endif
  
  // Set mandatory initialization classes
  //
  auto detConstruction = new B4DetectorConstruction();
  runManager->SetUserInitialization(detConstruction);
  
  G4VModularPhysicsList* physicsList = new G4VModularPhysicsList ();
  physicsList->RegisterPhysics(new G4EmPenelopePhysics);
  
  #ifdef U_238_DECAY_SIMULATION
  physicsList->RegisterPhysics(new G4RadioactiveDecayPhysics);
  physicsList->RegisterPhysics(new MyParticleConstructor);
  #endif
  
  physicsList->SetVerboseLevel(1);
  runManager->SetUserInitialization(physicsList);
  
  auto actionInitialization = new B4aActionInitialization(detConstruction, logger);
  runManager->SetUserInitialization(actionInitialization);
  
  // Initialize visualization
  //
  G4VisExecutive visManager;
  visManager.Initialize();
  
  // Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if ( macro.size() ) {
    // batch mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macro);
  }
  else  {  
    // interactive mode : define UI session
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    if (ui->IsGUI()) {
      UImanager->ApplyCommand("/control/execute gui.mac");
    }
    ui->SessionStart();
    delete ui;
  }

  delete runManager;
}
