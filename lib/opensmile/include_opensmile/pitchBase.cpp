/*F******************************************************************************
 *
 * openSMILE - open Speech and Music Interpretation by Large-space Extraction
 *       the open-source Munich Audio Feature Extraction Toolkit
 * Copyright (C) 2008-2009  Florian Eyben, Martin Woellmer, Bjoern Schuller
 *
 *
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
 *
 *
 * If you use openSMILE or any code from openSMILE in your research work,
 * you are kindly asked to acknowledge the use of openSMILE in your publications.
 * See the file CITING.txt for details.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ******************************************************************************E*/


/*  openSMILE component:

Base class for pitch components, provides standard output values and configuration

*/



#include <pitchBase.hpp>

#define MODULE "cPitchBase"


SMILECOMPONENT_STATICS(cPitchBase)

SMILECOMPONENT_REGCOMP(cPitchBase)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CPITCHBASE;
  sdescription = COMPONENT_DESCRIPTION_CPITCHBASE;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")

  // if the inherited config type was found, we register our configuration variables
  SMILECOMPONENT_IFNOTREGAGAIN( {} // <- this is only to avoid compiler warnings...
    // name append has a special role: it is defined in cDataProcessor, and can be overwritten here:
	  // if you set description to NULL, the existing description will be used, thus the following call can
  	// be used to update the default value:
    //ct->setField("nameAppend",NULL,"processed");

    // detectable pitch range:
    ct->setField("maxPitch","Maximum detectable pitch in Hz", 620.0);  
    ct->setField("minPitch","Minimum detectable pitch in Hz", 52.0);  

    ct->setField("nCandidates","The number of F0 candidates to output [1-20] (0 disables ouput of candidates AND their voicing probs.)",3);
    ct->setField("scores","1/0 = on/off: output of F0 candidates scores, if available",1);
    ct->setField("voicing","1/0 = on/off: output of voicing probability for F0 candidates",1);

    ct->setField("F0C1","1/0 = on/off: output of raw best F0 candidate without thresholding in unvoiced segments",0);
    ct->setField("voicingC1","1/0 = on/off: output of output voicing (pseudo) probability for best candidate",0);
    ct->setField("F0raw","1/0 = on/off: output of raw F0 (best candidate), > 0 only for voiced segments (using voicingCutoff threshold)",0);
    ct->setField("voicingClip","1/0 = on/off: output of voicing of raw F0 (best candidate), > 0 only for voiced segments (using voicingCutoff threshold)",0);
    
    ct->setField("voicingCutoff","This sets the voicing (pseudo) probability threshold for pitch detection. Frames with voicing probability values above this threshold will be considered as voiced.",1.0);

    ct->setField("inputFieldSearch","A part of the name to find the pitch detectors input field by ('Mag' searches e.g. for *Mag*, and will match fftMag fields)","Mag");

    // overwrite this in derived components:
    ct->setField("octaveCorrection","If this pitch detector algorithm offers algorithm specific low-level octave correction, enable it",0);

    ct->setField("processArrayFields",NULL,0);
  )

  // The configType gets automatically registered with the config manger by the SMILECOMPONENT_IFNOTREGAGAIN macro
  
  // we now create out sComponentInfo, including name, description, success status, etc. and return that
  SMILECOMPONENT_MAKEINFO_ABSTRACT(cPitchBase);
}

SMILECOMPONENT_CREATE_ABSTRACT(cPitchBase)

//-----

cPitchBase::cPitchBase(const char *_name) :
  cVectorProcessor(_name),
  inData(NULL), f0cand(NULL), candVoice(NULL), candScore(NULL)
{

}

void cPitchBase::fetchConfig()
{
  cVectorProcessor::fetchConfig();

  maxPitch = getDouble("maxPitch");
  if (maxPitch < 0.0) maxPitch = 0.0;
  SMILE_IDBG(2,"maxPitch = %lf",maxPitch);

  minPitch = getDouble("minPitch");
  if (minPitch < 0.0) minPitch = 0.0;
  if (minPitch > maxPitch) minPitch = maxPitch;
  SMILE_IDBG(2,"minPitch = %lf",minPitch);

  nCandidates = getInt("nCandidates");
  if (nCandidates < 1) nCandidates = 1;
  if (nCandidates > 20) nCandidates = 20;
  SMILE_IDBG(2,"nCandidates = %i",nCandidates);
  scores = getInt("scores");
  SMILE_IDBG(2,"scores = %i",scores);
  voicing = getInt("voicing");
  SMILE_IDBG(2,"voicing = %i",voicing);


  F0C1 = getInt("F0C1");
  SMILE_IDBG(2,"F0C1 = %i",F0C1);
  voicingC1 = getInt("voicingC1");
  SMILE_IDBG(2,"voicingC1 = %i",voicingC1);
  F0raw = getInt("F0raw");
  SMILE_IDBG(2,"F0raw = %i",F0raw);
  voicingClip = getInt("voicingClip");
  SMILE_IDBG(2,"voicingClip = %i",voicingClip);

  voicingCutoff = (FLOAT_DMEM)getDouble("voicingCutoff");
  SMILE_IDBG(2,"voicingCutoff = %f",voicingCutoff);

  octaveCorrection = getInt("octaveCorrection");
  SMILE_IDBG(2,"octaveCorrection = %i",octaveCorrection);

  inputFieldPartial = getStr("inputFieldSearch");
  SMILE_IDBG(2,"inputFieldSearch = '%s'",inputFieldPartial);
}


int cPitchBase::setupNewNames(long nEl)
{
//  fsSec = getFrameSizeSec();
  
  findInputField(inputFieldPartial, 0, nEl);

  int n=0;
    //TODO: usw addNameAppendField to preserve base name
  // candidates (in ranked order):
  if (nCandidates>0) { // also save number of valid candidates
    writer->addField("nCandidates", 1); n++; 
  } 
  if (nCandidates>0) { writer->addField("F0Cand", nCandidates); n += nCandidates; }
  if ((nCandidates>0)&&(voicing)) { 
    writer->addField("candVoicing", nCandidates); n += nCandidates; 
  }
  if ((nCandidates>0)&&(scores)) { 
    writer->addField("candScores", nCandidates); n += nCandidates; 
  }

  if (candScore == NULL) candScore = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nCandidates);
  if (candVoice == NULL) candVoice = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nCandidates);
  if (f0cand == NULL) f0cand = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nCandidates);

  //// these are provided mainly for compatibility and to be able to use max. candidate directly without a post smoothing component
  // raw first candidate:
  if (F0C1) { writer->addField( "F0C1", 1 ); n++; }
  // raw voicing prob. of first candidate: 
  if (voicingC1) { writer->addField( "voicingC1", 1 ); n++;}
  // raw first candidate after voicing thresholding (unvoiced regions are zero...):
  if (F0raw) { writer->addField( "F0raw", 1 ); n++; }
  if (voicingClip) { writer->addField( "voicingClip", 1 ); n++; }
  
  // save a little meta data for the pitchSmoother component
  cVectorMeta *mdata = writer->getLevelMetaDataPtr();
  if (mdata != NULL) {
    //TODO: mdata ID!
    mdata->fData[0] = voicingCutoff; 
  }

  namesAreSet = 1;
  return n;
}

int cPitchBase::pitchDetect(FLOAT_DMEM * _inData, long _N, double _fsSec, double _baseT, FLOAT_DMEM *_f0cand, FLOAT_DMEM *_candVoice, FLOAT_DMEM *_candScore, long _nCandidates)
{
  // to be implemented by child class

  // return actual number of candidates on success (-1 on failure...)
  return 0;
}

int cPitchBase::addCustomOutputs(FLOAT_DMEM *dstCur, long NdstLeft)
{
  // to be implemented by child class

  // return the number of custom outputs that were added..
  return 0;
}

// a derived class should override this method, in order to implement the actual processing
int cPitchBase::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // we assume we have fft magnitude as input...
  //double _N = (double)(Nsrc);  
  //double F0fft = 1.0/fsSec;
  //double Tsamp = fsSec/_N;
  int nCand=0; long i, j;

  if (!getInputFieldData(src,Nsrc,&inData)) return 0;
 // printf("inputStart: %i  nInput: %i  Nsrc %i\n",inputStart,nInput,Nsrc);
 // for (i=0; i<Nsrc; i++) { if (src[i] != 9.0) fprintf(stderr,"src[%i]\n",i); }
 // for (i=0; i<nInput; i++) { if (inData[i] != 9.0) fprintf(stderr,"inData[%i]\n",i); }
  // todo:  f0cand,candVoice,candScore point to appropriate positions in *dst

  
  // zero candidates
  for (i=0; i<nCandidates; i++) {
    f0cand[i] = 0.0;
    candVoice[i] = 0.0;
    candScore[i] = 0.0;
  }
  // main algorithm to be implemented by derived component
  nCand = pitchDetect(inData,nInput,getFrameSizeSec(),getBasePeriod(),f0cand,candVoice,candScore,nCandidates);

  // post process candidates wrt. minPitch / maxPitch
  // => remove out of range candidates
  if ( nCand > 0 ) {
      for (i=0; (i<nCandidates)&&(nCand>0); i++) {
        if ( (f0cand[i] > maxPitch)||(f0cand[i] < minPitch) ) {
          for (j=i+1; j<nCandidates; j++) {
            f0cand[j-1] = f0cand[j];
            candVoice[j-1] = candVoice[j];
            candScore[j-1] = candScore[j];
          }
          f0cand[j-1]=0;
          candVoice[j-1]=0;
          candScore[j-1]=0;
          nCand--; i--;
        }
      }
  }

  // if candidates remain, add them to output
  if ( nCand >= 0 ) {

    long n=0;
    if (nCandidates>0) {
      *(dst++) = (FLOAT_DMEM)nCand;
      
      // TODO: move max score candidate to 0th (if !octaveCorrection)
      long maxI = 0;
        if (!octaveCorrection) {
          FLOAT_DMEM max = candScore[0]; 
          for (i=1; i<nCandidates; i++) { // find max score (may be equal to voicing prob.)
            if (candScore[i] > max) { max = candScore[i]; maxI = i; }
          }
        }
        // swap
        if (maxI > 0) {
          FLOAT_DMEM tmp;
          tmp = f0cand[0];
          f0cand[0] = f0cand[maxI];
          f0cand[maxI] = tmp;
          tmp = candVoice[0];
          candVoice[0] = candVoice[maxI];
          candVoice[maxI] = tmp;
          tmp = candScore[0];
          candScore[0] = candScore[maxI];
          candScore[maxI] = tmp;
        }

      // add raw candidates to output
      for (i=0; i<nCandidates; i++) {
        *(dst++) = f0cand[i];
      }
      n += nCandidates;
      if (voicing) {
        for (i=0; i<nCandidates; i++) {
          *(dst++) = candVoice[i];
        }
        n += nCandidates;
      }
      if (scores) {
        for (i=0; i<nCandidates; i++) {
          *(dst++) = candScore[i];
        }
        n += nCandidates;
      }

      // now pick best candidate (if octaveCorrection is enabled, always use 0th candidate!)
      if (F0C1||voicingC1||F0raw||voicingClip) {
        long maxI = 0;
        /*if (!octaveCorrection) {
          FLOAT_DMEM max = candScore[0]; 
          for (i=1; i<nCandidates; i++) { // find max score (may be equal to voicing prob.)
            if (candScore[i] > max) { max = candScore[i]; maxI = i; }
          }
        }*/

        //FLOAT_DMEM _f0raw =  ;
        if (F0C1) { *(dst++) = f0cand[maxI]; n++; }
        if (voicingC1) { *(dst++) = candVoice[maxI]; n++; }
        // and apply voicingCutoff
        if (F0raw) {
          if (candVoice[maxI] <= voicingCutoff) *(dst++) = 0.0;
          else *(dst++) = f0cand[maxI]; 
          n++;
        }
        if (voicingClip) {
          if (candVoice[maxI] <= voicingCutoff) *(dst++) = 0.0;
          else *(dst++) = candVoice[maxI]; 
          n++;
        }
      }

    }

    n += addCustomOutputs(dst,Ndst-n);
    return n;

  } else { return 0; }

}

cPitchBase::~cPitchBase()
{
  if (inData != NULL) free(inData);
  if (f0cand != NULL) free(f0cand);
  if (candVoice != NULL) free(candVoice);
  if (candScore != NULL) free(candScore);
}

