#include "DataExporter.h"

DataExporter::DataExporter (int objSide)
{
	samplePeriod = 0.25f;

	int dimm = 1<<objSide;
	size = dimm*dimm*dimm;
	voxels = new float[size];
}

DataExporter::DataExporter (int objSide, float samplePeriod)
{
	this->samplePeriod = samplePeriod;

	int dimm = 1<<objSide;
	size = dimm*dimm*dimm;
	voxels = new float[size];
}

DataExporter::~DataExporter()
{
	if (outFile && outFile.is_open()) {
		outFile.close();
	}

	delete voxels;
}

void DataExporter::init(glm::vec3 objPos, float objSize)
{
	if (outFile && outFile.is_open() && outVoxels && outVoxels.is_open()) {
		save();
	}
	
	time (&rawTime);
	timeInfo = localtime(&rawTime);
	timeString.resize(0);
	new (&timeString) std::string(asctime(timeInfo));
	timeString.pop_back();
	replace(timeString.begin(), timeString.end(), ' ', '_');
	replace(timeString.begin(), timeString.end(), ':', '-');
	
	std::stringstream ss;
	ss << "Milo_data_" << timeString << ".milo";
	
	std::cout << "Saving data file " << ss.str() << std::endl;
	
	outFile.open(ss.str(), std::ofstream::out | std::ofstream::trunc);
	
	if (!outFile) {
		std::cout << std::strerror(errno) << std::endl;
	}
	
	outFile << std::scientific;
	
	outFile << "s " << objSize << " p " <<  objPos.x << " " << objPos.y << " " << objPos.z << std::endl;
	
	initTick = SDL_GetTicks();
	
	lastTime = ((float) initTick)/1000.0f;
	
	// Voxels

	if (outVoxels && outVoxels.is_open()) {
		exportVoxels();
	
		outVoxels.close();
	}
	
	std::stringstream ssv;
	ssv << "Milo_data_" << timeString << ".vox";
	
	std::cout << "Saving voxels file " << ssv.str() << std::endl;
	
	outVoxels.open(ssv.str(), std::ofstream::out | std::ofstream::trunc);
	
	if (!outVoxels) {
		std::cout << std::strerror(errno) << std::endl;
	}
	
	outVoxels << std::scientific;
}
	
void DataExporter::update(glm::quat& q, StereoKinectHeadTracking* tracking)
{
	int currentTick = SDL_GetTicks();
	currentTime = ((float) currentTick)/1000.0f;

	//std::cout << currentTime;
	
	if (currentTime - lastTime < samplePeriod) return;
	
	lastTime = currentTime;
	
	outFile << currentTime << " q " << q.x << " " << q.y << " " << q.z << " " << q.w;
	
	outFile << " s";
	std::vector<StereoKinectHeadTracking::SensorRelPoint> skel = tracking->GetSkeletonPositions();
	for (int i = 0; i < skel.size(); ++i) {
		outFile << " " << skel[i].state << " " <<skel[i].vwPos.x << " " << skel[i].vwPos.y << " " << skel[i].vwPos.z;
	}
	outFile << std::endl;
}

void DataExporter::meltedCell(int index, bool filled)
{
	voxels[index] = filled? 0f : currentTime;
}

void DataExporter::save()
{
	if (outFile && outFile.is_open())
		outFile.close();
	
	if (outVoxels && outVoxels.is_open()) {
		exportVoxels();
	
		outVoxels.close();
	}
}

void DataExporter::exportVoxels()
{
	outVoxels << voxels[0];
	for (int i = 1; i < size; ++i) {
		outVoxels << " " << voxels[i];
	}
	outVoxels << std::endl;
}
