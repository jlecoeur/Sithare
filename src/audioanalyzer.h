//
//  AudioAnalyzer.h
//  Sithare
//
//  Created by Maxime Grimonet, Julien Lecoeur, Charlotte Govin, Antoine Adam Jean-Noel Bettinelli, on 09/02/12.
//  Copyright (c) 2012. All rights reserved.
//


#define MATRIX_TYPE CV_32F

#ifndef Sithare_AudioExtactor_h
#define Sithare_AudioExtactor_h

#include "mp32wavconverter.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <smileCommon.hpp>
#include <configManager.hpp>
#include <commandlineParser.hpp>
#include <componentManager.hpp>


#define MODULE "SMILExtract"


using namespace std;
using namespace cv;
class AudioAnalyzer
{
	public:
		AudioAnalyzer();
		AudioAnalyzer(string, string);
		Mat extract(string);

	private:
		string dataPath;								// Path to the Data folder
		string configFileName;						// Name of the configuration file for openSMILE
		MP32WAVConverter * mp32WAVConverter;	// The tool to convert MP3 to WAV

		int audioToCSV(string, string);
		Mat CSVToCovariance(string); 
};

#endif

