#include <iostream>
#include <string>
#include <tclap/CmdLine.h>
#include "tags.h"
#include "lyrics.h"
#include "mp32wavconverter.h"
#include "bdd.h"
#include "lyricsanalyzer.h"
#include "audioanalyzer.h"
#include "similarity.h"

using namespace TCLAP;
using namespace std;


int main (int argc, char* argv[])
{
																		

	try 
	{
		// -----------------------------------------------------------------------
		// DECLARATIONS
		// -----------------------------------------------------------------------

		// Declarations
		string fileName, artist, title, inputFolder, outputFolder, outputFile, openSmileConfigFile;
		string convertTo;															// DEBUG ONLY
		bool doLyrics, doTag;													// DEBUG ONLY
		bool doBddBuild, doBddUpdate, doFindSimilar;
		int k;
		
		// Objects declarations
		Tags tag;																		// DEBUG ONLY
		Bdd *bddor;
		Similarity *sim;


		
		// -----------------------------------------------------------------------
		// COMMAND LINE DECLARATION
		// -----------------------------------------------------------------------
		
		// Command line parser
		CmdLine cmd("Sithare : Smart Music Recommandation, 2012 ECL", ' ', "0.9");

		// ---
		// Single-file-options (debug)
		// ---
		// input file argument
		ValueArg<string> inputFileArg("i","inputFile","Input mp3 file", false, "default.mp3", "string");
		cmd.add( inputFileArg );		
		// convert from mp3 to wav, output file argument
		ValueArg<string> convertFileArg("c","convert","Convert to output wav file", false, "default.wav", "string");
		cmd.add( convertFileArg );			
		// lyrics switch argument
		SwitchArg lyricsSwitch("l","lyrics","Fetch lyrics from the web", false);
		cmd.add( lyricsSwitch );
		// tag for this song
		SwitchArg tagSwitch("t", "tags", "Show tags for this song", false);
		cmd.add( tagSwitch );

		// ---
		// Global options (final)
		// ---
		// BDD Initialization
		SwitchArg bddSwitch("b", "build-bdd", "Extract features and build BDD", false);
		cmd.add( bddSwitch );
		// BDD update
		SwitchArg bddUpdateSwitch("u", "update-bdd", "Extract remaining features and update BDD", false);
		cmd.add( bddUpdateSwitch );
		// Input folder (mp3 files)
		ValueArg<string> inputFolderArg("I", "inputFolder", "MP3 files folder", false, "Musique", "string");
		cmd.add( inputFolderArg );
		// Find similar files
		SwitchArg findSwitch("f", "find-similar", "Find similar musics", false);
		cmd.add( findSwitch);
		// Number of similar songs to look for
		ValueArg<int> kppvArg("k", "kppv", "Number of similar songs to return", false, 5, "int");
		cmd.add( kppvArg );
		// Data folder (generated data)
		ValueArg<string> outputFolderArg("D", "data", "Generated data folder", false, "Data", "string");
		cmd.add( outputFolderArg );
		// output file
		ValueArg<string> outputFileArg("o", "output", "Output file containing similar songs", false, "similars.txt", "string");
		cmd.add( outputFileArg );
		// openSmile Configuration file
		ValueArg<string> openSmileConfigArg("C", "config", "OpenSmile Configuration File", false, "conf/sithare.conf", "string");
		cmd.add( openSmileConfigArg );
		

		// -----------------------------------------------------------------------
		// COMMAND LINE PARSING
		// -----------------------------------------------------------------------
		cmd.parse( argc, argv );
		// input file
		fileName = inputFileArg.getValue();
		// convert path															
		convertTo = convertFileArg.getValue();									// DEBUG ONLY
		// lyrics switch
		doLyrics = lyricsSwitch.getValue();										// DEBUG ONLY
		// tag switch
		doTag = tagSwitch.getValue();												// DEBUG ONLY
		// inputFolder
		inputFolder = inputFolderArg.getValue();
		// find similar switch
		doFindSimilar = findSwitch.getValue();
		// kppv
		k = kppvArg.getValue();
		// outputFolder
		outputFolder = outputFolderArg.getValue();
		// outputFile
		outputFile = outputFileArg.getValue();
		// BDD build switch
		doBddBuild = bddSwitch.getValue();
		// BDD update switch
		doBddUpdate =  bddUpdateSwitch.getValue();
		// OpenSmile Config file
		openSmileConfigFile = openSmileConfigArg.getValue();


		// Objects creation
		bddor = new Bdd( inputFolder, outputFolder, openSmileConfigFile);
		sim  = new Similarity(bddor, k);

		// -----------------------------------------------------------------------
		// -t TAG
		// -----------------------------------------------------------------------
		// Option -t
		if(doTag)
		{
			tag = Tags(fileName);
			if(tag.isLinked())
			{
				artist = tag.getArtist();
				title = tag.getTitle();
				
				cout << "Titre	: " << tag.getTitle() << endl;
				cout << "Artiste	: " << tag.getArtist() << endl;
				cout << "Album	: " << tag.getAlbum() << endl << endl;
			}
		}
	

		// -----------------------------------------------------------------------
		// -l LYRICS
		// -----------------------------------------------------------------------
		if(doLyrics)
		{
			tag = Tags(fileName);
			if(tag.isLinked())
			{
				artist = tag.getArtist();
				title = tag.getTitle();
			}
			
			Lyrics lyr(artist, title);
			string lyrics;
			//~ // ---
			//~ // Test de lyricsMuse
			//~ // ---
			//~ lyr.fetchLyricsMuse();
			//~ cout << lyr.getLyrics() << endl;
			//~ lyr.freeLyrics();
			//~ 
			//~ // ---
			//~ // Test de AzLyrics
			//~ // ---
			//~ lyr.fetchAzLyrics();
			//~ cout << lyr.getLyrics() << endl;
			//~ lyr.freeLyrics();
			
			// ---
			// Test de tous les sites
			// ---
			lyr.fetchLyrics();
			cout << lyr.getLyrics() << endl;
		}


		// -----------------------------------------------------------------------
		// -c CONVERT
		// -----------------------------------------------------------------------
		// Option -c
		if(convertTo != "default.wav")
		{
			string outFile = outputFileArg.getValue();
			MP32WAVConverter converter = MP32WAVConverter();
			converter.convert(fileName.c_str(), convertTo.c_str(), s16, 4096);
		}


		// -----------------------------------------------------------------------
		// -b BDD Build
		// -----------------------------------------------------------------------
		if(doBddBuild)
		{
			bddor->init();
			bddor->save();			
		}


		// -----------------------------------------------------------------------
		// -u BDD Update
		// -----------------------------------------------------------------------
		if(doBddUpdate)
		{
			bddor->load();
			bddor->update();
			bddor->save();			
		}

		

		// -----------------------------------------------------------------------
		// -f Find Similar songs
		// -----------------------------------------------------------------------
		if(doFindSimilar)
		{
			// mat initialisation
			Mat indice(1,k,CV_32S);
			Mat distance(1,k,CV_32F);
			int songID = 0;

			bddor->load();
			vector<string> songList = bddor->getSongList();			

			// Find song id
			for (unsigned i = 0; i < songList.size(); i++)
			{
				if(songList[i] == fileName)
				{
					songID = i;
					break;
				}
			}
			cout << "songID : " << songID << endl;

			// build index and process research
			sim->buildIndex();			
			sim->audio_knn(songID, indice, distance);



			// print result
			cout << "Songs similar to : " << songList[songID] << endl;
			MatIterator_<int> it_ind = indice.begin<int>();
			MatIterator_<float> it_dist = distance.begin<float>();
			for (int i = 0; i < k; i++, it_ind++, it_dist++)
			{
				//cout << songList[i] << endl;
				cout << indice.at<int>(0,i) << " : "  << songList[indice.at<int>(0,i) ] << " (distance " << distance.at<float>(0,i) << ")" << endl;
			}
			
		}

		// -----------------------------------------------------------------------
		// TO CONTINUE 	:-D
		// -----------------------------------------------------------------------		

	}
	catch (ArgException &e)  // catch any exceptions
		{ cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
		}	
	return 0;	
}
