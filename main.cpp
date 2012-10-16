/*
 *	Fichier montrant l'utilisation de la classe "Tag"
 *
 * Notes : 			cf tag.cpp
 * 
 * Compilation : 	cf Makefile
 *
 * Appel : 			./Sithare "Nom_de_fichier.mp3"
 *
 */ 


#include <iostream>
#include <string>
#include <tclap/CmdLine.h>
#include "tag.h"
#include "lyrics.h"

using namespace TCLAP;
using namespace std;


int main (int argc, char* argv[])
{
	try 
	{
		// Comand line parser
		CmdLine cmd("Sithare : Smart Music Recommandation, 2012 ECL", ' ', "0.9");
		// input file argument
		ValueArg<string> fileArg("i","inputFile","Input mp3 file",true, "default.mp3", "string");
		cmd.add( fileArg );		
		// lyrics switch argument
		SwitchArg lyricsSwitch("l","lyrics","Fetch lyrics from the web", false);
		cmd.add (lyricsSwitch);	
		// process command line parsing	
		cmd.parse( argc, argv );
		
		Tag tag;
		string fileName, artist, title;


		fileName = fileArg.getValue();
		tag = Tag(fileName);
		if(tag.isLinked())
		{
			artist = tag.getArtist();
			title = tag.getTitle();
			
			cout << "Titre	: " << tag.getTitle() << endl;
			cout << "Artiste	: " << tag.getArtist() << endl;
			cout << "Album	: " << tag.getAlbum() << endl << endl;
		}
	

		if(lyricsSwitch.getValue())
		{
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
	}
	catch (ArgException &e)  // catch any exceptions
		{ cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
		}	
	return 0;	
}
