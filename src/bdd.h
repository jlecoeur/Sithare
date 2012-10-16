//      bddor.h
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

#ifndef BDD_H
#define BDD_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include <dirent.h>

#ifndef WIN32
    #include <sys/types.h>
#endif

#include "audioanalyzer.h"
#include "lyrics.h"
#include "lyricsanalyzer.h"
#include "tags.h"

using namespace std;
using namespace cv;

class Bdd
{
	public:
		Bdd();
		Bdd(string, string, string);
		void init();
		void update();
		void save();
		void load();
		Mat getAudioFeatures();
		Mat getLyricsFeatures();
		vector<string> getSongList();
		int clean();

		
	private:
		string songPath;
		string dataPath;
		string listPath;
		string lyricsListPath;
		string lyricsPath;
		string featureFileName;
		string openSmileConfigFile;
		vector<string> songList;
		vector<string> lyricsFilesList;
		Mat audioFeatures;
		Mat lyricsFeatures;
	
		vector<string> listFiles();
		void listFilesInFolder(vector<string>&, string);
		bool saveLists();
		vector<string> loadSongList();
		vector<string> loadLyricsList();
		bool saveFeatures();
		Mat loadAudioFeatures();
		Mat loadLyricsFeatures();

		AudioAnalyzer audioAnalyzer;
		LyricsAnalyzer lyricsAnalyzer;
		Tags tag;
		Lyrics lyr;
			
};

#endif /* BDD_H */ 
