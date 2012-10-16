//
//  AudioAnalyzer.cpp
//  Sithare
//
//  Created by Maxime Grimonet, Julien Lecoeur, Charlotte Govin, Antoine Adam Jean-Noel Bettinelli, on 09/02/12.
//  Copyright (c) 2012. All rights reserved.
//

#include "audioanalyzer.h"



/************** Ctrl+C signal handler **/
#include  <signal.h>

cComponentManager *cmanGlob = NULL;

void INThandler(int);
int ctrlc = 0;

void INThandler(int sig)
{
  signal(sig, SIG_IGN);
  if (cmanGlob != NULL) cmanGlob->requestAbort();
  signal(SIGINT, INThandler);
  ctrlc = 1;
}
/*******************************************/

// Constructor
AudioAnalyzer::AudioAnalyzer()
{

}


AudioAnalyzer::AudioAnalyzer(string _dataPath, string _configFileName)
{
	dataPath = _dataPath;
	configFileName = _configFileName;
	mp32WAVConverter = new MP32WAVConverter();
}


// Do the feature extraction of a mp3 file and return a matrix (1xn) of mean and covariance
Mat AudioAnalyzer::extract(string mp3FileName)
{
	Mat result;
	
	// Conversion mp3 to WAV
	string wavFileName = dataPath+"/temp_audio.wav";
	mp32WAVConverter->convert(mp3FileName, wavFileName, f32, 4096);
	
	// Feature extraction
	string featuresFileName = dataPath+"/temp_features.csv";
	if (!audioToCSV(wavFileName, featuresFileName) == 1) cout << "Error while extracting features"<<endl;
	
	// Mean and covariance computing
	result = CSVToCovariance(featuresFileName);
	
	return result;
}


// Perform audio extraction with openSMILE
int AudioAnalyzer::audioToCSV(string wavFileName, string csvFileName)
{
	try {
		string wavPath = wavFileName;
		string csvPath = csvFileName;
		string configPath = configFileName;
		
		// set up the smile logger
		LOGGER.setLogLevel(-1);
		//LOGGER.enableConsoleOutput();

		int arg1 = 0;
		char ** arg2 = NULL;

		// commandline parser:
		cCommandlineParser cmdline(arg1,arg2);

		SMILE_MSG(2,"openSMILE starting!");

		// create configManager:
		cConfigManager *configManager = new cConfigManager(&cmdline);
		
		cComponentManager *cMan = new cComponentManager(configManager,componentlist);

		// add the file config reader:
		try{ 
		  configManager->addReader( new cFileConfigReader( configPath.c_str() ) );
		  configManager->readConfig();
		} catch (cConfigException *cc) {
		  return 0;
		}
		
		ConfigInstance * instance = configManager->getInstance("waveSource");
		instance->setStr("filename", (const char*)wavPath.c_str());
		//SMILE_MSG(2, "fichier audio : %s", instance->getStr("filename"));
		
		instance = configManager->getInstance("csvSink");
		instance->setStr("filename", (const char*)csvPath.c_str());
		//SMILE_MSG(2, "fichier de config : %s", instance->getStr("filename"));

		
		/* re-parse the command-line to include options created in the config file */
		cmdline.doParse(1,0); // warn if unknown options are detected on the commandline
		
		/* create all instances specified in the config file */
		cMan->createInstances(0); // 0 = do not read config (we already did that above..)

		/*
		MAIN TICK LOOP :
		*/
		cmanGlob = cMan;
		signal(SIGINT, INThandler); // install Ctrl+C signal handler

		/* run single or mutli-threaded, depending on componentManager config in config file */
		cMan->runMultiThreaded(-1);  //long long nTicks = 

		/* it is important that configManager is deleted BEFORE componentManger! 
		  (since component Manger unregisters plugin Dlls, which might have allocated configTypes, etc.) */
		delete configManager;
		delete cMan;
		
		return 1;
	} catch(cSMILException *c) { 
		// free exception ?? 
		return 0; 
	} 
	return 0;
}



// Compute mean and covariance from a CSV file of features
Mat AudioAnalyzer::CSVToCovariance(string filename)
{

	// CSV Parsing    
    Mat matrice;
    Mat result;
    int nframes = 0, nitems = 0;
    
    // Get the CSV file as a stream
    ifstream inFile (filename.c_str());
     if (inFile.is_open()) {
		 // Retrieve the number of frames of the file (=number of lines - header)
		nframes = count(istreambuf_iterator<char>(inFile), 
                 istreambuf_iterator<char>(), '\n') - 1;
		

		// Go back to the beginning of the file
		inFile.seekg(0);
		int linenum = 0;
		int itemnum = 0;
		string line;
		while (getline (inFile, line))
		{
			//cout << "\nLine #" << linenum << ":" << endl;
			istringstream linestream(line);
			string item;
			itemnum = 0;
			// forget firts two cols
			getline (linestream, item, ';');
			getline (linestream, item, ';');
			while (getline (linestream, item, ';'))
			{
				if (linenum > 0)																		// CHECK WITH NEW CONF FILE !!!!!!!!!
				{
					matrice.at<float>(linenum-1, itemnum) = (float)atof(item.c_str());
					//cout << "Item #" << itemnum << ": " << item << "-"<<matrice.at<float>(linenum-1, itemnum)<< endl;
				}
				itemnum++;
			}
			if (linenum == 0)
			{
				nitems = itemnum;
				matrice.create(nframes, nitems, CV_32F);
			} 
			linenum++;
		}  
		inFile.close();
		//cout << "matrice.cols : " << matrice.cols << " / matrice.rows " << matrice.rows << endl;
    } else  {
		cout << "Error: can't open CSV file." << endl;
    }
    
    // If the matrix has been successfully populated from the CSV file
    if (matrice.rows > 0 && matrice.cols > 0) {
		// Calculate the mean vector the covariance matrix
		Mat covar(0,0,CV_32F);
		Mat mean(0,0,CV_32F);
		calcCovarMatrix(matrice, covar, mean, CV_COVAR_ROWS | CV_COVAR_NORMAL, CV_32F);
		
		//cout << "CSVToCovariance: covariance ("<<covar.cols<< "," << covar.rows << ") computed for "<<nitems<<" features on "<<nframes<<" frames."<<endl;
		
		
		// Init the result matrix :
		// - number of rows : 1
		// - number of columns : nitems (means) + sum(n-i, i=0..n) (variance)
		result.create(1, (int)(nitems*(1 + (float)(nitems+1)/2)), CV_32F);
		int index = 0;
		// Add the mean to the matrix
	
		for(int j=0; j<mean.cols; j++) {
			result.at<float>(0,index) = mean.at<float>(0,j);
			index++;
		}
		
		for(int i=0; i<covar.rows; i++) {
			for(int j=i; j<covar.cols; j++) {
				result.at<float>(0,index) = covar.at<float>(i,j);
				index++;
			}
		}	


    
	}  else {
		cout << "Error while parsing CSV file" << endl;
	}
	
	
	return result;
}
