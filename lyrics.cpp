//      lyrics.cpp
//      
//      Copyright 2012 Julien Lecoeur <julien.lecoeur@gmail.com>
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


#include "lyrics.h"

using namespace std;

Lyrics::Lyrics()
{
	
}


Lyrics::Lyrics(string art, string tit)
{
	artist = art;
	title = tit;
}


string Lyrics::getArtist()
{
	return artist;
}


string Lyrics::getTitle()
{
	return title;
}


string Lyrics::getLyrics()
{
	return lyrics;
}


void Lyrics::freeLyrics()
{
	lyrics = "";
}


int writer(char *data, size_t size, size_t nmemb, std::string *buffer){
	int result = 0;
	if(buffer != NULL) {
		buffer -> append(data, size * nmemb);
		result = size * nmemb;
	}
	return result;
} 


void Lyrics::replaceAll(string& str, string substr1, string substr2)
{
	size_t found;
	found = str.find(substr1);
	while( found != string::npos )
	{
		str.replace(found, substr1.length(), substr2);
		found = str.find(substr1);
	}
}


void Lyrics::eraseAll(string& str, string substr)
{
	size_t found;
	found = str.find(substr);
	while( found != string::npos )
	{
		str.erase(found, substr.length());
		found = str.find(substr);
	}
}


bool Lyrics::fetchLyrics()
{
	if(fetchAzLyrics())
	{
		return true;
	}
	else if(fetchLyricsMuse())
	{
		return true;
	}
	// else if( ... )
	// {
	//
	// }
	else
	{
		return false;
	}
	
}


/*
 * Lyrics Muse part
 * 
 * http://www.lyricsmuse.com/
 * 
 */ 

string Lyrics::getLyricsMuseURL()
{
	string artistURL, titleURL;

	artistURL.assign(artist);
	titleURL.assign(title);
	
	// ------------------
	// Pre-process artist
	// ------------------
	// lower case			
	int length;
	length = artistURL.size();
	for (int i = 0; i < length; i++)
	{
		artistURL[i] = tolower(artistURL[i]);
	}	
	// remove non standart characters
	eraseAll(artistURL, " feat.");
	eraseAll(artistURL, "'");	
	eraseAll(artistURL, ".");	
	eraseAll(artistURL, "!");
	eraseAll(artistURL, ";");
	eraseAll(artistURL, ",");
	eraseAll(artistURL, "(");	
	eraseAll(artistURL, ")");	
	// replace non standart characters
	replaceAll(artistURL, "&", "and-amp");
	replaceAll(artistURL, " ", "-");
	replaceAll(artistURL, "é", "e");
	replaceAll(artistURL, "è", "e");
	replaceAll(artistURL, "ë", "e");	
	replaceAll(artistURL, "à", "a");
	replaceAll(artistURL, "ù", "u");	
	
	// ------------------
	// Pre-process title
	// ------------------
	// lower case			
	length = titleURL.size();
	for (int i = 0; i < length; i++)
	{
		titleURL[i] = tolower(titleURL[i]);
	}	
	// remove non standart characters
	eraseAll(titleURL, " feat.");
	eraseAll(titleURL, "'");	
	eraseAll(titleURL, ".");	
	eraseAll(titleURL, "!");
	eraseAll(titleURL, ";");
	eraseAll(titleURL, ",");
	eraseAll(titleURL, "(");	
	eraseAll(titleURL, ")");	
	// replace non standart characters
	replaceAll(titleURL, "&", "and-amp");
	replaceAll(titleURL, " ", "-");
	replaceAll(titleURL, "é", "e");
	replaceAll(titleURL, "è", "e");
	replaceAll(titleURL, "ë", "e");	
	replaceAll(titleURL, "à", "a");
	replaceAll(titleURL, "ù", "u");
	


	// Get url
	string url = "http://www.lyricsmuse.com/tracks/";
	url = url + artistURL + "-" + titleURL;
	
	return url;
	
}


bool Lyrics::fetchLyricsMuse()
{
	CURL *curl;				// That is the connection, see: curl_easy_init
	string buffer;			// See: CURLOPT_WRITEDATA
	size_t found;

		
	//-----------------
	// web query
	// ----------------
	string url = getLyricsMuseURL();
	//cout << "url	: " << url << endl<<endl;
 		
	// Initialize web query
	curl = curl_easy_init();
	
	if (curl){
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		curl_easy_setopt(curl, CURLOPT_HEADER, 0);					// No we don't need the Header of the web content. Set to 0 and curl ignores the first line
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);			// Don't follow anything else than the particular url requested
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);		// Function Pointer "writer" manages the required buffer size
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer ); 		// Data Pointer &buffer stores downloaded web content
	}
	else{
		fprintf(stderr,"Error 1\n");	// Badly written error message
		return 0;											
	}
	
	// Fetch the data
   curl_easy_perform(curl);
   	
	//Close the connection
	curl_easy_cleanup(curl);
	

	//------------------------
	// Post-Process lyrics
	// -----------------------
	found = buffer.find("<div id=\"song_lyrics\">");
	if(found != string::npos)
	{
		// cut before the lyrics
		buffer = buffer.substr(found+29, string::npos);
		// cut after the lyrics
		found = buffer.find("</div>");
		buffer = buffer.substr(0,found);
		// remove <br/>
		replaceAll(buffer, "<br/>", "\n");
		// remove <p/>
		replaceAll(buffer, "</p>", "\n");
		// remove <p>
		eraseAll(buffer, "<p>");			
		
		lyrics = buffer;

		return true;
	}
	else
	{
		cout << "ERROR : No lyrics for that song" << endl;
		return false;
	}
}


/*
 * AZ Lyrics  part
 * 
 * http://www.azlyrics.com/
 * 
 */ 
 

string Lyrics::getAzLyricsURL()
{
	string artistURL, titleURL;

	artistURL.assign(artist);
	titleURL.assign(title);
	
	// ------------------
	// Pre-process artist
	// ------------------
	// lower case			
	int length;
	length = artistURL.size();
	for (int i = 0; i < length; i++)
	{
		artistURL[i] = tolower(artistURL[i]);
	}	
	// remove non standart characters
	eraseAll(artistURL, " feat.");
	eraseAll(artistURL, "'");	
	eraseAll(artistURL, ".");	
	eraseAll(artistURL, "!");
	eraseAll(artistURL, ";");
	eraseAll(artistURL, ",");
	eraseAll(artistURL, "(");	
	eraseAll(artistURL, ")");	
	eraseAll(artistURL, " ");
	eraseAll(artistURL, "&");
	
	// replace non standart characters
	replaceAll(artistURL, "é", "e");
	replaceAll(artistURL, "è", "e");
	replaceAll(artistURL, "ë", "e");		
	replaceAll(artistURL, "à", "a");
	replaceAll(artistURL, "ù", "u");
	
	
	// ------------------
	// Pre-process title
	// ------------------
	// lower case			
	length = titleURL.size();
	for (int i = 0; i < length; i++)
	{
		titleURL[i] = tolower(titleURL[i]);
	}	
	// remove non standart characters
	eraseAll(titleURL, " feat.");
	eraseAll(titleURL, "'");	
	eraseAll(titleURL, ".");	
	eraseAll(titleURL, "!");
	eraseAll(titleURL, ";");
	eraseAll(titleURL, ",");
	eraseAll(titleURL, "(");	
	eraseAll(titleURL, ")");
	eraseAll(titleURL, " ");
	eraseAll(titleURL, "&");		
	
	// replace non standart characters
	replaceAll(titleURL, "é", "e");
	replaceAll(titleURL, "è", "e");
	replaceAll(titleURL, "ë", "e");
	replaceAll(titleURL, "à", "a");
	replaceAll(titleURL, "ù", "u");
	


	// Get url
	string url = "http://www.azlyrics.com/lyrics/";
	url = url + artistURL + "/" + titleURL + ".html";
	
	return url;
	
}


bool Lyrics::fetchAzLyrics()
{
	CURL *curl;				// That is the connection, see: curl_easy_init
	string buffer;			// See: CURLOPT_WRITEDATA
	size_t found;

		
	//-----------------
	// web query
	// ----------------
	string url = getAzLyricsURL();
	//cout << "url	: " << url << endl<<endl;
 		
	// Initialize web query
	curl = curl_easy_init();
	
	if (curl){
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		curl_easy_setopt(curl, CURLOPT_HEADER, 0);					// No we don't need the Header of the web content. Set to 0 and curl ignores the first line
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);			// Don't follow anything else than the particular url requested
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);		// Function Pointer "writer" manages the required buffer size
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer ); 		// Data Pointer &buffer stores downloaded web content
	}
	else{
		fprintf(stderr,"Error 1\n");	// Badly written error message
		return 0;											
	}
	
	// Fetch the data
   curl_easy_perform(curl);
   	
	//Close the connection
	curl_easy_cleanup(curl);
	

	//------------------------
	// Post-Process lyrics
	// -----------------------
	found = buffer.find("<!-- start of lyrics -->");
	if(found != string::npos)
	{
		// cut before the lyrics
		buffer = buffer.substr(found+24, string::npos);
		// cut after the lyrics
		found = buffer.find("<!-- end of lyrics -->");
		buffer = buffer.substr(0,found);
		// remove <br/> <i> and </i>
		eraseAll(buffer, "<br>");	
		eraseAll(buffer, "<i>");
		eraseAll(buffer, "</i>");		
		
		lyrics = buffer;

		return true;
	}
	else
	{
		cout << "ERROR : No lyrics for that song" << endl;
		return false;
	}
	
}
