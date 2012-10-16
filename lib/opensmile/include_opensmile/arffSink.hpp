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

ARFF file output (for WEKA)

*/


#ifndef __ARFF_SINK_HPP
#define __ARFF_SINK_HPP

#include <smileCommon.hpp>
#include <dataSink.hpp>

#define COMPONENT_DESCRIPTION_CARFFSINK "This component writes dataMemory data to an ARFF file (WEKA). Depending on your config an instance name field, a frame index, and a frame time field can be added as well as multiple class/target attributes. See the config type documentation for more details."
#define COMPONENT_NAME_CARFFSINK "cArffSink"

#undef class
class DLLEXPORT cArffSink : public cDataSink {
  private:
    FILE * filehandle;
    const char *filename;
    int lag;
    int append, timestamp, number, prname;
    const char *relation;
    const char *instanceBase, *instanceName;
    
    int nClasses; long nInst;
    long inr;
    char **classname;
    char **classtype;
    
    char **targetall;
    char ***targetinst;
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);


  public:
    //static sComponentInfo * registerComponent(cConfigManager *_confman);
    //static cSmileComponent * create(const char *_instname);
    SMILECOMPONENT_STATIC_DECL
    
    cArffSink(const char *_name);

    virtual ~cArffSink();
};




#endif // __ARFF_SINK_HPP
