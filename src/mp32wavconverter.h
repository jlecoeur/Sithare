//
//  MP32WAVConverter.h
//  Sithare
//
//  Created by Maxime Grimonet on 01/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Sithare_MP32WAVConverter_h
#define Sithare_MP32WAVConverter_h

#include <mpg123.h>
#include <string>

using namespace std;

enum MP32WAVType {
    f32,
    s16
};

class MP32WAVConverter
{    
public:
    MP32WAVConverter();
    bool convert(string, string, MP32WAVType, long bufferSize);

private:
    void cleanup(mpg123_handle *);

};


#endif
