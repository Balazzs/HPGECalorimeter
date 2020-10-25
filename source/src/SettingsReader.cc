#include "SettingsReader.hh"

#include "G4UIcommand.hh"

static void PrintUsage() {
  G4cerr << " Usage: " << G4endl;
  G4cerr << " hpge [-m macro ] [-u UIsession] [-t nThreads] [-d sample distance in cm]" << G4endl;
  G4cerr << "   note: -t option is available only for multi-threaded mode."
          << G4endl;
}

Settings ReadSettings (int argc,char** argv)
{
  Settings settings;
  
  if ( argc > 9 ) {
    PrintUsage();
    settings.valid = false;
    return settings;
  }
  
  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-m" ) settings.macro          = argv[i+1];
    else if ( G4String(argv[i]) == "-u" ) settings.session        = argv[i+1];
    else if ( G4String(argv[i]) == "-d" ) { settings.sampleDistance        = G4UIcommand::ConvertToDouble(argv[i+1]);
                                            settings.constructSampleHolder = true; }
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
