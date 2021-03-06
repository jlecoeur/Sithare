//
//  MP32WAVConverter.cpp
//  Sithare
//
//  Created by Maxime Grimonet on 01/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "mp32wavconverter.h"
#include <sndfile.h>



#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

MP32WAVConverter::MP32WAVConverter() {
    
}

void MP32WAVConverter::cleanup(mpg123_handle *mh) {
    /* It's really to late for error checks here;-) */
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
}

bool MP32WAVConverter::convert(string input, string output, MP32WAVType type, long bufferSize)  {
    SNDFILE* sndfile = NULL;
    SF_INFO sfinfo;
    mpg123_handle *mh = NULL;
    unsigned char* buffer = NULL;
    size_t buffer_size = 0;
    size_t done = 0;
    int  channels = 0, encoding = 0;
    long rate = 0;
    int  err  = MPG123_OK;
    off_t samples = 0;
    
    
//    cout<< "Input file: "<< input<<endl;;
//    cout<< "Output file: "<< output<<endl;
    
    err = mpg123_init();
    if(err != MPG123_OK || (mh = mpg123_new(NULL, &err)) == NULL) {
        fprintf(stderr, "Basic setup goes wrong: %s", mpg123_plain_strerror(err));
        cleanup(mh);
        return false;
    }
    
    /* Simple hack to enable floating point output. */
    if(type == s16) mpg123_param(mh, MPG123_ADD_FLAGS, MPG123_FORCE_FLOAT, 0.);
    
    /* Let mpg123 work with the file, that excludes MPG123_NEED_MORE messages. */
    if(    mpg123_open(mh, input.c_str()) != MPG123_OK
       /* Peek into track and get first output format. */
       || mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK ) {
        fprintf( stderr, "Trouble with mpg123: %s\n", mpg123_strerror(mh) );
        cleanup(mh);
        return false;
    }
    
    if(encoding != MPG123_ENC_SIGNED_16 && encoding != MPG123_ENC_FLOAT_32) { 
        /* Signed 16 is the default output format anyways; it would actually by only different if we forced it.
         So this check is here just for this explanation. */
        cleanup(mh);
        fprintf(stderr, "Bad encoding: 0x%x!\n", encoding);
        return false;
    }
    /* Ensure that this output format will not change (it could, when we allow it). */
    mpg123_format_none(mh);
    mpg123_format(mh, rate, channels, encoding);
    
    bzero(&sfinfo, sizeof(sfinfo) );
    sfinfo.samplerate = (int)rate;
    sfinfo.channels = channels;
    sfinfo.format = SF_FORMAT_WAV|(encoding == MPG123_ENC_SIGNED_16 ? SF_FORMAT_PCM_16 : SF_FORMAT_FLOAT);
//    printf("Creating WAV with %i channels and %liHz.\n", channels, rate);
    
    sndfile = sf_open(output.c_str(), SFM_WRITE, &sfinfo);
    if(sndfile == NULL){ fprintf(stderr, "Cannot open output file!\n"); cleanup(mh); return -2; }
    
    /* Buffer could be almost any size here, mpg123_outblock() is just some recommendation.
     00088            Important, especially for sndfile writing, is that the size is a multiple of sample size. */
    buffer_size = (bufferSize>0) ? bufferSize : mpg123_outblock(mh);
    buffer = (unsigned char*) malloc( buffer_size );
    
    do {
        sf_count_t more_samples;
        err = mpg123_read( mh, buffer, buffer_size, &done );
        more_samples = encoding == MPG123_ENC_SIGNED_16
        ? sf_write_short(sndfile, (short*)buffer, done/sizeof(short))
        : sf_write_float(sndfile, (float*)buffer, done/sizeof(float));
        if(more_samples < 0 || more_samples*mpg123_encsize(encoding) != (int)done) {
            fprintf(stderr, "Warning: Written number of samples does not match the byte count we got from libmpg123: %li != %li\n", (long)(more_samples*mpg123_encsize(encoding)), (long)done);
        }
        samples += more_samples;
        /* We are not in feeder mode, so MPG123_OK, MPG123_ERR and MPG123_NEW_FORMAT are the only possibilities.
         We do not handle a new format, MPG123_DONE is the end... so abort on anything not MPG123_OK. */
    } while (err==MPG123_OK);
    
    if(err != MPG123_DONE)
        fprintf( stderr, "Warning: Decoding ended prematurely because: %s\n",
                err == MPG123_ERR ? mpg123_strerror(mh) : mpg123_plain_strerror(err) );
    
    sf_close( sndfile );
    
    samples /= channels;
 //   printf("%li samples written.\n", (long)samples);
    cleanup(mh);
    
    return true;
}
