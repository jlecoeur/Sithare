//      lyrics.h
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


#ifndef LYRICS_H
#define LYRICS_H

#include <stdio.h>
#include <iostream>

#include <curl/curl.h>
#include <string>
#include <algorithm>


class Lyrics
{
	public:
		Lyrics();
		Lyrics(std::string, std::string);
		bool fetchLyricsMuse();
		bool fetchAzLyrics();
		std::string getArtist();
		std::string getTitle();
		std::string getLyrics();
		void freeLyrics();
			
	private:
		void replaceAll(std::string&, std::string, std::string);
		void eraseAll(std::string&, std::string); 
		std::string getLyricsMuseURL();
		std::string getAzLyricsURL();
		std::string artist;
		std::string title;
		std::string lyrics;
		
};

#endif /* LYRICS_H */ 
