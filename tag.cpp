//      tag.cpp
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


/* Notes:
 * 
 * - la méthode isLinked() permet d'éviter la plupart des erreurs
 * - un meme objet tag peut servir pour plusieur fichiers en faisant :
 * 		Tag myTag;
 * 		myTag = Tag("premier_fichier")
 * 		...
 * 		myTag = Tag("second_fichier")
 *			...
 * - seuls les champs Artiste, Album, et Titre sont supportés, cf tag.h pour les autres
 *
 */ 

#include "tag.h"

using namespace std;

Tag::Tag()
{
	linked = false;
}

Tag::Tag(string fileName)
{
	myTag = ID3_Tag();
	linked = setFile((char*)fileName.c_str());
}


bool Tag::isLinked()
{
	return linked;
}

bool Tag::setFile(char* fileName)
{
	// Try ID3V1
	myTag.Link(fileName, ID3TT_ID3V1);
	if(myTag.HasV1Tag())
	{
		return true;
	}	
	else
	{
		//~ cout << "ERROR : no ID3V1 tag in \"" << fileName <<  "\"" << endl;

		// Try ID3V2
		//myTag.Link(fileName, ID3TT_ID3V2);
		myTag.Link(fileName, ID3TT_ALL);
		if(myTag.HasV2Tag())
		{
			return true;
		}
		else
		{
			cout << "ERROR : no ID3V1/ID3V2 tag in \"" << fileName <<  "\"" << endl;
			return false;
		}
	}
}


string Tag::getArtist()
{
	ID3_Frame *myFrame = NULL;

	myFrame = myTag.Find ( ID3FID_LEADARTIST );
	if(myFrame != 0)
	{
			char buffer[ 1024 ];
			myFrame->Field ( ID3FN_TEXT ).Get ( buffer, 1024 );

			string str(buffer);
			return str;
	}
	else
	{
		return "NaN";
	}
}


string Tag::getAlbum()
{
	ID3_Frame *myFrame = NULL;

	myFrame = myTag.Find ( ID3FID_ALBUM );
	if(myFrame != 0)
	{
			char buffer[ 1024 ];
			myFrame->Field ( ID3FN_TEXT ).Get ( buffer, 1024 );

			string str(buffer);
			return str;
	}
	else
	{
		return "NaN";
	}
}


string Tag::getTitle()
{
	ID3_Frame *myFrame = NULL;
	myFrame = myTag.Find ( ID3FID_TITLE );
	if(myFrame != 0)
	{
			char buffer[ 1024 ];
			myFrame->Field ( ID3FN_TEXT ).Get ( buffer, 1024 );
			
			string str(buffer);
			return str;
	}
	else
	{
		return "NaN";
	}
}
