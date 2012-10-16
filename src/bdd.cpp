//      bddor.cpp
//      
//      Copyright 2012 Maxime <maxime@maxime-VirtualBox>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.
//      
//      


#include "bdd.h"

Bdd::Bdd()
{

}


Bdd::Bdd(string _songPath, string _dataPath, string _openSmileConfigFile)
{
	songPath = _songPath;
	dataPath = _dataPath;
	openSmileConfigFile = _openSmileConfigFile;
	listPath = "songList.txt";
	lyricsListPath = "lyricsList.txt";
	featureFileName = "features.yml";
	lyricsPath = dataPath + "/lyrics/";
	audioAnalyzer = AudioAnalyzer(dataPath, openSmileConfigFile);
	//lyricsAnalyzer = LyricsAnalyzer( ... );
	
}

void Bdd::init() {

	// -- 
	// Checks if the data and lyrics folders exists. If not, creates it.
	// ---
	DIR* rep = NULL;
    rep = opendir(dataPath.c_str()); 
	if (rep == NULL){
		cout<<"Creating Data folder..."<<endl;
        if((mkdir(dataPath.c_str(),00777))==-1) {
             cout<<"Error creating the Data folder.";
        }
	}
	closedir(rep);
    rep = opendir(lyricsPath.c_str()); 
	if (rep == NULL){
		cout<<"Creating Lyrics folder..."<<endl;
        if((mkdir(lyricsPath.c_str(),00777))==-1) {
             cout<<"Error creating the Data folder.";
        }
	}
	closedir(rep);     
	
	
	// ---
	// list files
	// ---
	songList = listFiles();

	// ---
	// Extract audio features
	// ---
	cout << "Extracting features..." << endl;
	Mat line;
	int numFiles = songList.size();
	// first line
	cout << "Processing song 1/" << numFiles << endl; 
	line = audioAnalyzer.extract(songList[0]);
	audioFeatures.create(numFiles,line.cols, CV_32F);
	for (int j = 0; j < line.cols; j++)
	{
		audioFeatures.at<float>(0,j) = line.at<float>(0,j);
	}
	// others lines
	for (int i = 1; i < numFiles; i++)
	{
		cout << "Processing song "<< i+1 << "/" << numFiles << endl; 
		line = audioAnalyzer.extract(songList[1]);
		for (int j = 0; j < line.cols; j++)
		{
			audioFeatures.at<float>(i,j) = line.at<float>(0,j);
		}
	}
	 

	// ---
	// Extract lyrics features
	// ---
	lyricsFeatures.create(10,10, CV_32F);
	
	for (int i = 0; i < numFiles; i++)
	{
		cout << "Processing song "<< i+1 << "/" << numFiles << endl; 		
		tag = Tags(songList[i]);
		if(tag.isLinked())
		{
			lyr = Lyrics(tag.getArtist(), tag.getTitle());
			if(lyr.fetchLyrics())
			{
				lyricsFilesList.push_back(lyr.saveLyrics(lyricsPath));
			}
			else
			{
				lyricsFilesList.push_back("");
			}
		}	
	}
	

}


void Bdd::save()
{
	// save song list
	saveLists();

	// save audio and lyrics feature matrix
	saveFeatures();
}


void Bdd::load()
{
	// load song list
	songList = loadSongList();
	
	lyricsFilesList = loadLyricsList();

	// load audio feature matrix
	audioFeatures = loadAudioFeatures();

	// load lyrics feature matrix
	lyricsFeatures = loadLyricsFeatures();
	
}


void Bdd::update() 		// NOT IMPLEMENTED YET ( do the same as init() )
{
	init();				
}


Mat Bdd::getAudioFeatures()
{
	return audioFeatures; 
}


Mat Bdd::getLyricsFeatures()
{
	return lyricsFeatures;
}


vector<string> Bdd::getSongList()
{
	return songList;
}


vector<string> Bdd::listFiles()
{
	cout<<"Looking for audio files in " << songPath << endl;
	
	vector<string> listT;
	listFilesInFolder(listT, songPath);

	cout << listT.size() << " songs found" << endl;

	return listT;
}

void Bdd::listFilesInFolder(vector<string> &list, string dirName) {
	unsigned char isFolder =0x4;
	DIR* dir;
	struct dirent *readFile;
	dir = opendir(dirName.c_str());
	if (dir == NULL)
	{
		return;
	}
	while( (readFile=readdir(dir)) )
	{
	   char * name = readFile->d_name;
	   string name_str = (string)name;
		if (name[0] != '.' )
		{
		   if ( readFile->d_type == isFolder)
		   {
				listFilesInFolder(list, dirName+"/"+name_str);
		   }
		   else if( name_str.find(".mp3")!=string::npos )
		   {	
			   list.push_back(dirName+"/"+name_str);
		   }
	   }
	}
	closedir(dir);
}


bool Bdd::saveLists()
{
	cout << "Saving song list..." << endl;
	string path = dataPath + "/" + listPath;
	fstream file(path.c_str(), ios::out);
	if(file.is_open())
	{	
		for (unsigned i = 0; i < songList.size() ; i++)
		{
			file << songList[i] << endl;
		}
	}
	else
	{
		return false;
	}
	
	file.close();	

	cout << "Saving lyrics list..." << endl;
	path = dataPath + "/" + lyricsListPath;
	fstream file2(path.c_str(), ios::out);
	if(file2.is_open())
	{	
		for (unsigned i = 0; i < songList.size() ; i++)
		{
			file2 << lyricsFilesList[i] << endl;
		}
	}
	else
	{
		return false;
	}
	
	file2.close();

	return true;
}


vector<string> Bdd::loadSongList()
{
	cout << "Loading song list..." << endl;

	vector<string> listT;
	string str;
	string path = dataPath + "/" + listPath;

	cout << path << endl;

	fstream file(path.c_str(), ios::in);
	if(file.is_open())
	{
		while(getline(file, str))
		{
			listT.push_back(str);
		}
	}
	else
	{
		cout << "ERROR, cannont open " << path << endl;
	}
	
	file.close();

	return listT;
}


vector<string> Bdd::loadLyricsList()
{
	cout << "Loading lyrics list..." << endl;

	vector<string> listT;
	string str;
	string path = dataPath + "/" + lyricsListPath;
	fstream file(path.c_str(), ios::in);
	if(file.is_open())
	{	
		while(getline(file, str))
		{
			listT.push_back(str);
		}
	}
	
	file.close();

	return listT;
}


bool Bdd::saveFeatures()
{
	cout << "Saving audio features matrix..." << endl;
	string path = dataPath + "/" + featureFileName;
	
	FileStorage fs(path.c_str(), FileStorage::WRITE);

	fs << "audioFeatures" << audioFeatures;
	fs << "lyricsFeatures" << lyricsFeatures;

	fs.release();
	
	return true;
}


Mat Bdd::loadAudioFeatures()
{
	cout << "Loading audio features matrix..." << endl;

	Mat m;
	string path = dataPath + "/" + featureFileName;
	
	FileStorage fs(path.c_str(), FileStorage::READ);

	fs["audioFeatures"] >> m;

	fs.release();

	return m;
}


Mat Bdd::loadLyricsFeatures()
{
	cout << "Loading lyrics features matrix..." << endl;	

	Mat m;
	string path = dataPath + "/" + featureFileName;
	
	FileStorage fs(path.c_str(), FileStorage::READ);

	fs["lyricsFeatures"] >> m;

	fs.release();

	return m;
}


int Bdd::clean()
{
	cout << "Delete all Data..." << endl;

	// clean all Data
	string command;
	command = "rm -r " + dataPath;
	int err = system(command.c_str());

	return err;
}
