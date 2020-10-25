#pragma once

#include <G4Types.hh>
#include <G4String.hh>

enum class PrimaryEnergyMode {
	Mono,		//Given monoenergetic gamma
	Table,		//Energy probabilities from table: U_238.txt
	Simdecay	//Simulated U_238 decay using RadioactiveDecayProcess
};

enum class DetectorGeometryType {
  Empty,
  Simple,
  Real
};

struct Settings {
	bool					valid = true;
	
	G4String				macro;
	G4String				session;
	G4int					nThreads = 0;
	
	G4double				sampleDistance			= 5;	//cm
	G4bool					constructSampleHolder	= false;
	
	PrimaryEnergyMode 		primaryEnergyMode 		= PrimaryEnergyMode::Mono;
	G4double 				primaryEnergy 			= 1;	//MeV
	
	DetectorGeometryType	detectorGeometry		= DetectorGeometryType::Real;
};

Settings ReadSettings (int argc,char** argv);