#include "DataLogger.hh"

void DataLogger::LogPhotonCreation (double energy)
{
	photonFile << energy << std::endl;
}

void DataLogger::LogEventEnd ()
{
	photonFile << std::endl;
}