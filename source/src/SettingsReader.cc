#include "SettingsReader.hh"

#include <fstream>
#include <nlohmann/json.hpp>
#include "G4UIcommand.hh"

using namespace nlohmann;

static void PrintUsage() {
  G4cerr << " Usage: " << G4endl;
  G4cerr << " hpge [-m macro ] [-u UIsession] [-t nThreads]" << G4endl;
  G4cerr << "   note: -t option is available only for multi-threaded mode." << G4endl;
}

constexpr int version = 0;

json ReadJSONFile ()
{
	std::ifstream settingsFile ("settings.json");
	
	if (!settingsFile) {
		G4cerr << "Could not find settings file" << G4endl;
		return json (nullptr);
	}
	
	json j;
	
	settingsFile >> j;
	
	if (j.contains("version") && j["version"].is_number_integer()) {
		if (j["version"].get<int> () > version) {
			G4cerr << "Settings file version newer then program version" << G4endl;
		}
	} else {
		G4cerr << "Invalid JSON file - no file version" << G4endl;
		return json (nullptr);
	}
	
	return j;
}

std::pair<bool, Settings> ReadCommandLineSettings (int argc, char** argv)
{
	Settings settings;
	bool useCommandLine = false;
	
	for ( G4int i=1; i<argc; i=i+2 ) {
		if		( G4String(argv[i]) == "-m" ) {
			settings.macro = argv[i+1];
			useCommandLine = true;
		} else if ( G4String(argv[i]) == "-u" ) {
			settings.session = argv[i+1];
			useCommandLine = true;
		}
	#ifdef G4MULTITHREADED
		else if ( G4String(argv[i]) == "-t" ) {
			settings.nThreads = G4UIcommand::ConvertToInt(argv[i+1]);
			useCommandLine = true;
		}
	#endif
		else if (G4String(argv[i]) == "--no-settings"){
			useCommandLine = true;
		}
		else {
			PrintUsage();
			settings.valid = false;
			useCommandLine = true;
			return {useCommandLine, settings};
		}
	}
	
	return {useCommandLine, settings};
}

Settings ReadSettings (int argc, char** argv)
{
	bool useCommandLine;
	Settings settings;
	
	std::tie(useCommandLine, settings) = ReadCommandLineSettings (argc, argv);
	
	if (useCommandLine) {
		return settings;
	}
	
	json j = ReadJSONFile ();
	
	if (j.empty()) {
		settings.valid = false;
		return settings;
	}
	
	if (j.contains("macro") && j["macro"].is_string()) {
		settings.macro = j["macro"].get<std::string> ();
	} else {
		G4cerr << "No macro specified" << G4endl;
	}
	
	if (j.contains("number of threads") && j["number of threads"].is_number_integer()) {
		settings.nThreads = j["number of threads"].get<int> ();
	}
	
	if (j.contains("sample distance") && j["sample distance"].is_number()) {
		settings.sampleDistance = j["sample distance"].get<double> ();
	}
	
	if (j.contains("sample holder") && j["sample holder"].is_boolean()) {
		settings.constructSampleHolder = j["sample holder"].get<bool> ();
	}
	
	return settings;
}
