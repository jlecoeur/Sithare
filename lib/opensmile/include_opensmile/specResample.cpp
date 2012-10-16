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


/*  openSMILE component: resmapler


*/

// TODO: when upsampling we need a post lp filter (time domain) to remove hf artefacts


#include <specResample.hpp>

#define MODULE "cSpecResample"


SMILECOMPONENT_STATICS(cSpecResample)

SMILECOMPONENT_REGCOMP(cSpecResample)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CSPECRESAMPLE;
  sdescription = COMPONENT_DESCRIPTION_CSPECRESAMPLE;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")

  // if the inherited config type was found, we register our configuration variables
  SMILECOMPONENT_IFNOTREGAGAIN( {} // <- this is only to avoid compiler warnings...
    // name append has a special role: it is defined in cDataProcessor, and can be overwritten here:
	  // if you set description to NULL, the existing description will be used, thus the following call can
  	// be used to update the default value:
    //ct->setField("nameAppend",NULL,"processed");

    ct->setField("targetFs","The target sampling frequency in Hz",16000);  
    ct->setField("resampleRatio","Specifies a fixed resample ratio a (a=fsNew/fsCurrent). If set, this overrides targetFs",1.0,0,0);

    ct->setField("inputFieldPartial","The name of the input field to search for. (NULL (default): use full input vector)",(const char*)NULL);
    ct->setField("processArrayFields",NULL,0); // TODO? support array field processing...
  )

  // The configType gets automatically registered with the config manger by the SMILECOMPONENT_IFNOTREGAGAIN macro
  
  // we now create out sComponentInfo, including name, description, success status, etc. and return that
  SMILECOMPONENT_MAKEINFO(cSpecResample);
}

SMILECOMPONENT_CREATE(cSpecResample)

//-----

cSpecResample::cSpecResample(const char *_name) :
  cVectorProcessor(_name),
  inData(NULL),
  resampleRatio(1.0),
  inputFieldPartial(NULL),
  antiAlias(1),
  dftWork(NULL)
  //costable(NULL),sintable(NULL)
{

}

void cSpecResample::fetchConfig()
{
  cVectorProcessor::fetchConfig();

  if (isSet("resampleRatio")) {
    resampleRatio = getDouble("resampleRatio");
    if (resampleRatio <= 0.0) {
      SMILE_IERR(1,"invalid resampling ratio (%f) ! must be > 0.0",resampleRatio);
      resampleRatio = 1.0;
    }
    SMILE_IDBG(2,"resampleRatio = '%s'",resampleRatio);
  } else {
    targetFs = getDouble("targetFs");
    if (targetFs <= 0.0) {
      SMILE_IERR(1,"invalid target sampling frequency (targetFs=%f) ! must be > 0.0",targetFs);
      targetFs = 1.0;
    }
    SMILE_IDBG(2,"targetFs = '%s'",targetFs);
    resampleRatio = -1.0;
  }

  inputFieldPartial = getStr("inputFieldPartial");
}

int cSpecResample::configureWriter(sDmLevelConfig &c)
{
  if ((c.lastFrameSizeSec != c.basePeriod)&&(c.lastFrameSizeSec > 0.0))
    c.frameSizeSec = c.lastFrameSizeSec;

  double bT = (double)(c.basePeriod);
  if (bT > 0.0) sr = 1.0/bT;
  else {
    SMILE_IERR(1,"unable to determine sample rate of input! basePeriod <= 0.0 (=%f)!",bT);
    sr = 1.0;
  }

  /* compute resampling parameters: */
  if (resampleRatio == -1.0) { // convert targetFs
    resampleRatio = targetFs/sr;
    SMILE_IDBG(2,"resampleRatio (computed) = %f",resampleRatio);
  } else {
    // compute targetFs from resampling ratio
    targetFs = resampleRatio * sr;
  }

  c.basePeriod = 1.0/targetFs;
  return 1;
}

int cSpecResample::setupNewNames(long nEl)
{
  const sDmLevelConfig *c = reader->getLevelConfig();
  fsSec = (double)(c->frameSizeSec);
  double lastFsSec = (double)(c->lastFrameSizeSec);
  double bT = (double)(c->basePeriod);
  if (bT > 0.0) sr = 1.0/bT;
  else {
    SMILE_IERR(1,"unable to determine sample rate of input! basePeriod <= 0.0 (=%f)!",bT);
    sr = 1.0;
  }

  if (inputFieldPartial != NULL) {
    findInputField(inputFieldPartial, 0, nEl);
  } else {
    nInput=nEl;
    inputStart = 0;
  }
  _Nin = nInput;

  
  // we must round the resampling ratio to the closest integer frame size for the target frames to avoid discontinuities
  
  // detect zero-padded fft input and adjust actual output
  double nd;
  if ((fsSec != lastFsSec)&&(lastFsSec != 0.0)&&(lastFsSec!=bT)) {
    double _Nout0 = round((double)_Nin * resampleRatio * lastFsSec/fsSec);
    double newRatio = _Nout0 / ((double)_Nin * (lastFsSec/fsSec));
    _Nout = (long)_Nout0;
    if (newRatio != resampleRatio) {
      targetFs = sr*newRatio;
      SMILE_IMSG(2,"adjusting resampleRatio from %f to %f to match next integer frame size! (targetFs* = %f)",resampleRatio,newRatio,targetFs);
      resampleRatio = newRatio;
    }
    nd = (double)_Nin * resampleRatio;
  } else {
    double _Nout0 = round((double)_Nin * resampleRatio);
    double newRatio = _Nout0 / (double)_Nin;
    _Nout = (long)_Nout0;
    if (newRatio != resampleRatio) {
      targetFs = sr*newRatio;
      SMILE_IMSG(2,"adjusting resampleRatio from %f to %f to match next integer frame size! (targetFs* = %f)",resampleRatio,newRatio,targetFs);
      resampleRatio = newRatio;
    }
    nd = _Nout0;
  }

  dftWork = smileDsp_initIrdft(_Nin, _Nout, nd, antiAlias);
  
  //int n=0;
  writer->addField("resampled",_Nout);
  namesAreSet = 1;
  return _Nout;
}


// a derived class should override this method, in order to implement the actual processing
int cSpecResample::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  //long i,k;
  // we assume we have complex fft as input...

//  if (!getInputFieldData(src,Nsrc,&inData)) return 0;

  smileDsp_irdft(src, dst, dftWork);
  
  return Ndst;
}

cSpecResample::~cSpecResample()
{
  if (inData != NULL) free(inData);
  smileDsp_freeDftwork(dftWork);
}

