#ifndef DATA_EXPORTER_H
#define DATA_EXPORTER_H

#include "main.h"
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <iomanip>
#include "SDL.h"
#include <algorithm>

#include "StereoKinectHeadTracking.h"

class DataExporter {
private:
	float samplePeriod;
	
	std::ofstream outFile;
	std::ofstream outVoxels;
	time_t rawTime;
	struct tm * timeInfo;
	
	float* voxels;
	
	int initTick;
	float lastTime;
	int size;
	
	void exportVoxels();
	
public:
	string timeString;
	
	float currentTime;

	DataExporter(int objSide);

	DataExporter (int objSide, float samplePeriod);
	
	~DataExporter();
	
	
	void init(glm::vec3 objPos, float objSize);
	
	void update(glm::quat& q, StereoKinectHeadTracking* tracking);
	
	void save();
	
	void meltedCell(int index, bool cellMelted);
};

#endif // DATA_EXPORTER_H
