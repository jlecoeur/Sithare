//      tag.h
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


#ifndef TAG_H
#define TAG_H

#include <iostream>
#include <id3/tag.h>
#include <string>

class Tag
{
	public:
		Tag();
		Tag(std::string);
		std::string getArtist();
		std::string getAlbum();
		std::string getTitle();
		bool isLinked();
			
	private:
		bool setFile(char*);
		ID3_Tag myTag;
		int linked;

};

#endif /* TAG_H */


/*
 * enum ID3_FrameID
 * 
ID3FID_NOFRAME  	 No known frame.
ID3FID_AUDIOCRYPTO  	 Audio encryption.
ID3FID_PICTURE  	 Attached picture.
ID3FID_COMMENT  	 Comments.
ID3FID_COMMERCIAL  	 Commercial frame.
ID3FID_CRYPTOREG  	 Encryption method registration.
ID3FID_EQUALIZATION  	 Equalization.
ID3FID_EVENTTIMING  	 Event timing codes.
ID3FID_GENERALOBJECT  	 General encapsulated object.
ID3FID_GROUPINGREG  	 Group identification registration.
ID3FID_INVOLVEDPEOPLE  	 Involved people list.
ID3FID_LINKEDINFO  	 Linked information.
ID3FID_CDID  	 Music CD identifier.
ID3FID_MPEGLOOKUP  	 MPEG location lookup table.
ID3FID_OWNERSHIP  	 Ownership frame.
ID3FID_PRIVATE  	 Private frame.
ID3FID_PLAYCOUNTER  	 Play counter.
ID3FID_POPULARIMETER  	 Popularimeter.
ID3FID_POSITIONSYNC  	 Position synchronisation frame.
ID3FID_BUFFERSIZE  	 Recommended buffer size.
ID3FID_VOLUMEADJ  	 Relative volume adjustment.
ID3FID_REVERB  	 Reverb.
ID3FID_SYNCEDLYRICS  	 Synchronized lyric/text.
ID3FID_SYNCEDTEMPO  	 Synchronized tempo codes.
ID3FID_ALBUM  	 Album/Movie/Show title.
ID3FID_BPM  	 BPM (beats per minute).
ID3FID_COMPOSER  	 Composer.
ID3FID_CONTENTTYPE  	 Content type.
ID3FID_COPYRIGHT  	 Copyright message.
ID3FID_DATE  	 Date.
ID3FID_PLAYLISTDELAY  	 Playlist delay.
ID3FID_ENCODEDBY  	 Encoded by.
ID3FID_LYRICIST  	 Lyricist/Text writer.
ID3FID_FILETYPE  	 File type.
ID3FID_TIME  	 Time.
ID3FID_CONTENTGROUP  	 Content group description.
ID3FID_TITLE  	 Title/songname/content description.
ID3FID_SUBTITLE  	 Subtitle/Description refinement.
ID3FID_INITIALKEY  	 Initial key.
ID3FID_LANGUAGE  	 Language(s).
ID3FID_SONGLEN  	 Length.
ID3FID_MEDIATYPE  	 Media type.
ID3FID_ORIGALBUM  	 Original album/movie/show title.
ID3FID_ORIGFILENAME  	 Original filename.
ID3FID_ORIGLYRICIST  	 Original lyricist(s)/text writer(s).
ID3FID_ORIGARTIST  	 Original artist(s)/performer(s).
ID3FID_ORIGYEAR  	 Original release year.
ID3FID_FILEOWNER  	 File owner/licensee.
ID3FID_LEADARTIST  	 Lead performer(s)/Soloist(s).
ID3FID_BAND  	 Band/orchestra/accompaniment.
ID3FID_CONDUCTOR  	 Conductor/performer refinement.
ID3FID_MIXARTIST  	 Interpreted, remixed, or otherwise modified by.
ID3FID_PARTINSET  	 Part of a set.
ID3FID_PUBLISHER  	 Publisher.
ID3FID_TRACKNUM  	 Track number/Position in set.
ID3FID_RECORDINGDATES  	 Recording dates.
ID3FID_NETRADIOSTATION  	 Internet radio station name.
ID3FID_NETRADIOOWNER  	 Internet radio station owner.
ID3FID_SIZE  	 Size.
ID3FID_ISRC  	 ISRC (international standard recording code).
ID3FID_ENCODERSETTINGS  	 Software/Hardware and settings used for encoding.
ID3FID_USERTEXT  	 User defined text information.
ID3FID_YEAR  	 Year.
ID3FID_UNIQUEFILEID  	 Unique file identifier.
ID3FID_TERMSOFUSE  	 Terms of use.
ID3FID_UNSYNCEDLYRICS  	 Unsynchronized lyric/text transcription.
ID3FID_WWWCOMMERCIALINFO  	 Commercial information.
ID3FID_WWWCOPYRIGHT  	 Copyright/Legal infromation.
ID3FID_WWWAUDIOFILE  	 Official audio file webpage.
ID3FID_WWWARTIST  	 Official artist/performer webpage.
ID3FID_WWWAUDIOSOURCE  	 Official audio source webpage.
ID3FID_WWWRADIOPAGE  	 Official internet radio station homepage.
ID3FID_WWWPAYMENT  	 Payment.
ID3FID_WWWPUBLISHER  	 Official publisher webpage.
ID3FID_WWWUSER  	 User defined URL link.
ID3FID_METACRYPTO  	 Encrypted meta frame (id3v2.2.x).
ID3FID_METACOMPRESSION  	 Compressed meta frame (id3v2.2.1).
ID3FID_LASTFRAMEID  	 Last field placeholder.
*
*/

