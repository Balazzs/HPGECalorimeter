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

#include <memory>

#define U_238_DECAY_SIMULATION

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

struct Settings {
  bool valid = true;
  
  G4String macro;
  G4String session;
  G4int    nThreads = 0;
  
  G4double sampleDistance = 5;
};

void PrintUsage() {
  G4cerr << " Usage: " << G4endl;
  G4cerr << " hpge [-m macro ] [-u UIsession] [-t nThreads]" << G4endl;
  G4cerr << "   note: -t option is available only for multi-threaded mode."
          << G4endl;
}

Settings GetSettingsFromCommandLineArguments (int argc,char** argv)
{
  Settings settings;
  
  if ( argc > 7 ) {
    PrintUsage();
    settings.valid = false;
    return settings;
  }
  
  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-m" ) settings.macro   = argv[i+1];
    else if ( G4String(argv[i]) == "-u" ) settings.session = argv[i+1];
#ifdef G4MULTITHREADED
    else if ( G4String(argv[i]) == "-t" ) settings.nThreads = G4UIcommand::ConvertToInt(argv[i+1]);
#endif
    else {
      PrintUsage();
      settings.valid = false;
      return settings;
    }
  }
  
  return settings;
}

std::unique_ptr<G4RunManager> GetRunManager (const Settings& settings)
{
  // Construct the default run manager
  //
#ifdef G4MULTITHREADED
  auto runManager = new G4MTRunManager;
  if ( settings.nThreads > 0 ) { 
    runManager->SetNumberOfThreads(settings.nThreads);
  } 
  return std::unique_ptr<G4RunManager> (runManager);
#else
  return std::unique_ptr<G4RunManager> (new G4RunManager);
#endif
}

std::unique_ptr<G4UIExecutive> SetupInteractiveMode (const Settings& settings, int argc,char** argv)
{
  if ( ! settings.macro.size() ) {
    return std::make_unique<G4UIExecutive> (argc, argv, settings.session);
  } else {
    return nullptr;
  }
}

void StartUIOrProcessMacro (G4UIExecutive* ui, const Settings& settings)
{
  // Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if ( settings.macro.size() ) {
    // batch mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+settings.macro);
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
}

int main(int argc,char** argv)
{
  Settings settings = GetSettingsFromCommandLineArguments (argc, argv);
  
  auto ui = SetupInteractiveMode (settings, argc, argv);
  auto runManager = GetRunManager(settings);
  
  DataLogger logger;
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
  
  StartUIOrProcessMacro (ui.get (), settings);
}
