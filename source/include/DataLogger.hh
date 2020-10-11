#pragma once

#include <fstream>

class DataLogger {
public:
	void LogPhotonCreation (double energy);
	void LogEventEnd ();
	
private:
	std::ofstream photonFile = std::ofstream ("photonEnergies.txt");
};