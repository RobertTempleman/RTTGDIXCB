/*************************************************
*                                                *
*  EasyBMP Cross-Platform Windows Bitmap Library *
*                                                *
*  Author: Paul Macklin                          *
*   email: macklin01@users.sourceforge.net       *
* support: http://easybmp.sourceforge.net        *
*                                                *
*          file: EasyBMP_VariousBMPutilities.h   *
*    date added: 05-02-2005                      *
* date modified: 12-01-2006                      *
*       version: 1.06                            *
*                                                *
*   License: BSD (revised/modified)              *
* Copyright: 2005-6 by the EasyBMP Project       *
*                                                *
* description: Defines BMP class                 *
*                                                *
*************************************************/
#ifndef _EasyBMP_BMP_h_
#define _EasyBMP_BMP_h_

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
//#include "EasyBMP_BMP.h"


bool SafeFread( char* buffer, int size, int number, FILE* fp );
bool EasyBMPcheckDataSize( void );

class BMP{
 public:

  int BitDepth;
  int Width;
  int Height;
  RGBApixel** Pixels;
  RGBApixel* Colors;
  int XPelsPerMeter;
  int YPelsPerMeter;

  ebmpBYTE* MetaData1;
  int SizeOfMetaData1;
  ebmpBYTE* MetaData2;
  int SizeOfMetaData2;
   
  bool Read32bitRow( ebmpBYTE* Buffer, int BufferSize, int Row );   
  bool Read24bitRow( ebmpBYTE* Buffer, int BufferSize, int Row );   
  bool Read8bitRow(  ebmpBYTE* Buffer, int BufferSize, int Row );  
  bool Read4bitRow(  ebmpBYTE* Buffer, int BufferSize, int Row );  
  bool Read1bitRow(  ebmpBYTE* Buffer, int BufferSize, int Row );
   
  bool Write32bitRow( ebmpBYTE* Buffer, int BufferSize, int Row );   
  bool Write24bitRow( ebmpBYTE* Buffer, int BufferSize, int Row );   
  bool Write8bitRow(  ebmpBYTE* Buffer, int BufferSize, int Row );  
  bool Write4bitRow(  ebmpBYTE* Buffer, int BufferSize, int Row );  
  bool Write1bitRow(  ebmpBYTE* Buffer, int BufferSize, int Row );
 
  ebmpBYTE FindClosestColor( RGBApixel& input );

 public: 

  int TellBitDepth( void );
  int get_width( void );
  int get_height( void );
  int TellNumberOfColors( void );
  void SetDPI( int HorizontalDPI, int VerticalDPI );
  int get_vertical_DPI( void );
  int get_horizontal_DPI( void );
  
  BMP();
  BMP( BMP& Input );
  ~BMP();
  RGBApixel* operator()(int i,int j);
 
  RGBApixel get_pixel( int i, int j ) const;
  bool SetPixel( int i, int j, RGBApixel NewPixel );
 
  bool CreateStandardColorTable( void );
 
  bool SetSize( int NewWidth, int NewHeight );
  bool SetBitDepth( int NewDepth );
  bool WriteToFile( const char* FileName );
  bool read_from_file( const char* FileName );
 
  RGBApixel GetColor( int ColorNumber );
  bool SetColor( int ColorNumber, RGBApixel NewColor ); 
};

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


#endif
