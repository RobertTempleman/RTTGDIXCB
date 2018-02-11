#include <iostream>
#include <cmath>
#include <cctype>
#include <cstring>
#include <cstdio>

#ifndef _DefaultXPelsPerMeter_
#define _DefaultXPelsPerMeter_
#define DefaultXPelsPerMeter 3780
// set to a default of 96 dpi 
#endif

#ifndef _DefaultYPelsPerMeter_
#define _DefaultYPelsPerMeter_
#define DefaultYPelsPerMeter 3780
// set to a default of 96 dpi
#endif

#include "EasyBMP_DataStructures.h"
#include "EasyBMP_BMP.h"
#include "EasyBMP_VariousBMPutilities.h"

#ifndef _EasyBMP_Version_
#define _EasyBMP_Version_ 1.06
#define _EasyBMP_Version_Integer_ 106
#define _EasyBMP_Version_String_ "1.06"
#endif

#ifndef _EasyBMPwarnings_
#define _EasyBMPwarnings_
#endif

void SetEasyBMPwarningsOff( void );
void SetEasyBMPwarningsOn( void );
bool GetEasyBMPwarningState( void );

