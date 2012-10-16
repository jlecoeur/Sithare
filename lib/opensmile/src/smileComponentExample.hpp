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

example for a cSmileComponent descendant

*/


#ifndef __CSMILECOMPONENTEXAMPLE_HPP
#define __CSMILECOMPONENTEXAMPLE_HPP

#include <smileCommon.hpp>
#include <smileComponent.hpp>

#define COMPONENT_DESCRIPTION_CSMILECOMPONENTEXAMPLE "example for a cSmileComponent descendant"
#define COMPONENT_NAME_CSMILECOMPONENTEXAMPLE "cSmileComponentExample"


#undef class
class DLLEXPORT cSmileComponentExample : public cSmileComponent {
  private:

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    virtual void mySetEnvironment();
    virtual int myRegisterInstance();
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);
    virtual int processComponentMessage( cComponentMessage *_msg );

  public:
    SMILECOMPONENT_STATIC_DECL

    cSmileComponentExample(const char *_name);
    virtual ~cSmileComponentExample();
};




#endif // __CSMILECOMPONENTEXAMPLE_HPP
