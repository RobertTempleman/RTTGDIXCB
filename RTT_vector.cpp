//#include "stdafx.h"
#include "RTT_vector.h"



RVEC animal;

RVEC::RVEC(float f){x=f;y=f;z=f;};
RVEC::RVEC(float _x, float _y, float _z){x=_x;y=_y;z=_z;};

RVEC RVEC::operator + (const RVEC& v){animal.x=x+v.x;animal.y=y+v.y;animal.z=z+v.z;return animal;};
RVEC RVEC::operator - (const RVEC& v){animal.x=x-v.x;animal.y=y-v.y;animal.z=z-v.z;return animal;};
//RVEC RVEC::operator - (const RVEC& v){return RVEC(x-v.x, y-v.y, z-v.z);};
RVEC RVEC::operator += (const RVEC& v){x+=v.x;y+=v.y;z+=v.z;return *this;};
RVEC RVEC::operator -= (const RVEC& v){x-=v.x;y-=v.y;z-=v.z;return *this;};
RVEC RVEC::operator *= (float s){x*=s;y*=s;z*=s;return *this;};
RVEC RVEC::operator /= (float c){float s=1.0f/c;x*=s;y*=s;z*=s;return *this;};

RVEC operator + (const RVEC& v){return RVEC(v.x,v.y,v.z);};
RVEC operator - (const RVEC& v){return RVEC(-v.x,-v.y,-v.z);};
RVEC operator + (const RVEC& v1, const RVEC& v2){animal.x=v1.x+v2.x;animal.y=v1.y+v2.y;animal.z=v1.z+v2.z;return animal;};
RVEC operator - (const RVEC& v1, const RVEC& v2){animal.x=v1.x-v2.x;animal.y=v1.y-v2.y;animal.z=v1.z-v2.z;return animal;};
RVEC operator * (const RVEC& v, float s){animal.x=v.x*s; animal.y=v.y*s; animal.z=v.z*s;return animal;};
RVEC operator * (float s, const RVEC& v){animal.x=v.x*s; animal.y=v.y*s; animal.z=v.z*s;return animal;};
RVEC operator / (const RVEC& v, float c){float s=1.0f/c;animal.x=v.x*s;animal.y=v.y*s;animal.z=v.z*s;return animal;};

int operator == (const RVEC& v1, const RVEC& v2){
  if (v1.x!=v2.x) return 0;
  if (v1.y!=v2.y) return 0;
  if (v1.z!=v2.z) return 0;
return 1;
}

int operator != (const RVEC& v1, const RVEC& v2){
  if (v1.x!=v2.x) return 1;
  if (v1.y!=v2.y) return 1;
  if (v1.z!=v2.z) return 1;
return 0;
}

float Magnitude(RVEC M){
  return sqrtf(M.x*M.x+M.y*M.y+M.z*M.z);
}

float SquareMagnitude(RVEC M){
  return M.x*M.x+M.y*M.y+M.z*M.z;
}

RVEC Mormalize(RVEC M){
  float mag=1.0f/sqrtf(M.x*M.x+M.y*M.y+M.z*M.z);
  animal.x=M.x*mag;
  animal.y=M.y*mag;
  animal.z=M.z*mag;
  return animal;
}

RVEC Xproduct(const RVEC &a,const RVEC &b){
  animal.x=a.y*b.z-a.z*b.y;
  animal.y=a.z*b.x-a.x*b.z;
  animal.z=a.x*b.y-a.y*b.x;
	return animal;
}

float DProduct (const RVEC& v1, const RVEC& v2)
{
   return v1.x*v2.x + v1.y * v2.y + v1.z*v2.z;
}

// 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5
//                           
//       4 vector for renderin wiv
//                           
// 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5


RVEC4 animal4;

RVEC4::RVEC4(float f){x=f;y=f;z=f;w=f;};
RVEC4::RVEC4(float _x, float _y, float _z,float _w){x=_x;y=_y;z=_z;w=_w;};

RVEC4 RVEC4::operator + (const RVEC4& v){animal4.x=x+v.x;animal4.y=y+v.y;animal4.z=z+v.z;animal4.w=w+v.w;return animal4;};
RVEC4 RVEC4::operator - (const RVEC4& v){animal4.x=x-v.x;animal4.y=y-v.y;animal4.z=z-v.z;animal4.w=w-v.w;return animal4;};
//RVEC4 RVEC4::operator - (const RVEC4& v){return RVEC4(x-v.x, y-v.y, z-v.z);};
RVEC4 RVEC4::operator += (const RVEC4& v){x+=v.x;y+=v.y;z+=v.z;w+=v.w;return *this;};
RVEC4 RVEC4::operator -= (const RVEC4& v){x-=v.x;y-=v.y;z-=v.z;w-=v.w;return *this;};
RVEC4 RVEC4::operator *= (float s){x*=s;y*=s;z*=s;w*=s;return *this;};
RVEC4 RVEC4::operator /= (float c){float s=1.0f/c;x*=s;y*=s;z*=s;w*=s;return *this;};

RVEC4 operator + (const RVEC4& v){return RVEC4(v.x,v.y,v.z,v.w);};
RVEC4 operator - (const RVEC4& v){return RVEC4(-v.x,-v.y,-v.z,-v.w);};
RVEC4 operator + (const RVEC4& v1, const RVEC4& v2){animal4.x=v1.x+v2.x;animal4.y=v1.y+v2.y;animal4.z=v1.z+v2.z;return animal4;};
RVEC4 operator - (const RVEC4& v1, const RVEC4& v2){animal4.x=v1.x-v2.x;animal4.y=v1.y-v2.y;animal4.z=v1.z-v2.z;return animal4;};
RVEC4 operator * (const RVEC4& v, float s){animal4.x=v.x*s; animal4.y=v.y*s; animal4.z=v.z*s;return animal4;};
RVEC4 operator * (float s, const RVEC4& v){animal4.x=v.x*s; animal4.y=v.y*s; animal4.z=v.z*s;return animal4;};
RVEC4 operator / (const RVEC4& v, float c){float s=1.0f/c;animal4.x=v.x*s;animal4.y=v.y*s;animal4.z=v.z*s;return animal4;};

int operator == (const RVEC4& v1, const RVEC4& v2){
  if (v1.x!=v2.x) return 0;
  if (v1.y!=v2.y) return 0;
  if (v1.z!=v2.z) return 0;
  if (v1.w!=v2.w) return 0;
return 1;
}

int operator != (const RVEC4& v1, const RVEC4& v2){
  if (v1.x!=v2.x) return 1;
  if (v1.y!=v2.y) return 1;
  if (v1.z!=v2.z) return 1;
  if (v1.w!=v2.w) return 1;
return 0;
}

float Magnitude(RVEC4 M){
  return sqrtf(M.x*M.x+M.y*M.y+M.z*M.z+M.w*M.w);
}

float SquareMagnitude(RVEC4 M){
  return M.x*M.x+M.y*M.y+M.z*M.w+M.z*M.w;
}

RVEC4 Mormalize(RVEC4 M){
  float mag=1.0f/sqrtf(M.x*M.x+M.y*M.y+M.z*M.z+M.w*M.w);
  animal4.x=M.x*mag;
  animal4.y=M.y*mag;
  animal4.z=M.z*mag;
  animal4.w=M.w*mag;
  return animal4;
}

RVEC4 Xproduct(const RVEC4 &a,const RVEC4 &b){
  animal4.x=a.y*b.z-a.z*b.y;
  animal4.y=a.z*b.x-a.x*b.z;
  animal4.z=a.x*b.y-a.y*b.x;
  animal4.w=1.0f;
	return animal4;
}

float DProduct (const RVEC4& v1, const RVEC4& v2)
{
   return v1.x*v2.x + v1.y * v2.y + v1.z*v2.z + v1.w*v2.w;
}

// 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5
//                           
//       2 vector for fun
//                           
// 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5

template<class T> RVEC2<T> RVEC2<T>::animal2;
template<class T> RVEC2<T> RVEC2<T>::operator + (const RVEC2<T>& v){animal2.x=x+v.x;animal2.y=y+v.y;return animal2;};
//template<class T> RVEC2<T> RVEC2<T>::operator - (const RVEC2<T>& v){animal2.x=x-v.x;animal2.y=y-v.y;return animal2;};
template<class T> RVEC2<T> RVEC2<T>::operator += (const RVEC2<T>& v){x+=v.x;y+=v.y;return *this;};
template<class T> RVEC2<T> RVEC2<T>::operator -= (const RVEC2<T>& v){x-=v.x;y-=v.y;return *this;};
template<class T> RVEC2<T> RVEC2<T>::operator *= (T s){x*=s;y*=s;return *this;};
template<class T> RVEC2<T> RVEC2<T>::operator /= (T c){T s=1.0f/c;x*=s;y*=s;return *this;};
template<class T> RVEC2<T> operator + (const RVEC2<T>& v){return RVEC2<T>(v.x,v.y);};
template<class T> RVEC2<T> operator - (const RVEC2<T>& v){return RVEC2<T>(-v.x,-v.y);};
template<class T> RVEC2<T> operator + (const RVEC2<T>& v1, const RVEC2<T>& v2){RVEC2<T>::animal2.x=v1.x+v2.x;RVEC2<T>::animal2.y=v1.y+v2.y;return RVEC2<T>::animal2;};
template<class T> RVEC2<T> operator - (const RVEC2<T>& v1, const RVEC2<T>& v2){RVEC2<T>::animal2.x=v1.x-v2.x;RVEC2<T>::animal2.y=v1.y-v2.y;return RVEC2<T>::animal2;};
template<class T> RVEC2<T> operator * (const RVEC2<T>& v, T s){RVEC2<T>::animal2.x=v.x*s; RVEC2<T>::animal2.y=v.y*s;return RVEC2<T>::animal2;};
template<class T> RVEC2<T> operator * (T s, const RVEC2<T>& v){RVEC2<T>::animal2.x=v.x*s; RVEC2<T>::animal2.y=v.y*s;return RVEC2<T>::animal2;};
template<class T> RVEC2<T> operator / (const RVEC2<T>& v, T c){T s=1.0f/c;RVEC2<T>::animal2.x=v.x*s;RVEC2<T>::animal2.y=v.y*s;return RVEC2<T>::animal2;};

template<class T> int operator == (const RVEC2<T>& v1, const RVEC2<T>& v2){
  if (v1.x!=v2.x) return 0;
  if (v1.y!=v2.y) return 0;
  return 1;
}

template<class T> int operator != (const RVEC2<T>& v1, const RVEC2<T>& v2){
  if (v1.x!=v2.x) return 1;
  if (v1.y!=v2.y) return 1;
  return 0;
}

template<class T> T Magnitude(RVEC2<T> M){
  return sqrtf((T)M.x*(T)M.x+(T)M.y*(T)M.y);
}

template<class T> T SquareMagnitude(RVEC2<T> M){
  return M.x*M.x+M.y*M.y;
}

template<class T> RVEC2<T> Mormalize(RVEC2<T> M){
  float mag=1.0f/sqrtf(M.x*M.x+M.y*M.y);
  RVEC2<T>::animal2.x=M.x*mag;
  RVEC2<T>::animal2.y=M.y*mag;
  return RVEC2<T>::animal2;
}

template<class T> T DProduct (const RVEC2<T>& v1, const RVEC2<T>& v2){
  return v1.x*v2.x + v1.y * v2.y;
}
// 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5
//                           
//       2 floats for ultimate family fun
//                           
// 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5


TUFF::TUFF(float _x, float _y){l=_x;r=_y;};

TUFF TUFF::operator + (const TUFF& v){return TUFF(l+v.l,r+v.r);};
TUFF TUFF::operator - (const TUFF& v){return TUFF(l-v.l,r-v.r);};
TUFF& TUFF::operator += (const TUFF& v){l+=v.l;r+=v.r;return *this;};
TUFF& TUFF::operator -= (const TUFF& v){l-=v.l;r-=v.r;return *this;};
TUFF& TUFF::operator *= (float s){l*=s;r*=s;return *this;};
TUFF& TUFF::operator /= (float c){float s=1.0f/c;l*=s;r*=s;return *this;};

TUFF operator + (const TUFF& v){return TUFF(v.l,v.r);};
TUFF operator - (const TUFF& v){return TUFF(-v.l,-v.r);};
TUFF operator + (const TUFF& v1, const TUFF& v2){return TUFF(v1.l+v2.l,v1.r+v2.r);};
TUFF operator - (const TUFF& v1, const TUFF& v2){return TUFF(v1.l-v2.l,v1.r-v2.r);};
TUFF operator * (const TUFF& v, float s){return TUFF(v.l*s,v.r*s);};
TUFF operator * (float s, const TUFF& v){return TUFF(v.l*s,v.r*s);};
TUFF operator / (const TUFF& v, float c){float s=1.0f/c;return TUFF(v.l*s,v.r*s);};

#define LIMIT 32767.0f
short TUFF::clipl(){
  float v=minimummy(LIMIT,l);
  return (short)maximummy(-LIMIT,v);
}
short TUFF::clipr(){
  float v=minimummy(LIMIT,r);
  return (short)maximummy(-LIMIT,v);
}

int operator == (const TUFF& v1, const TUFF& v2){
  if (v1.l!=v2.l) return 0;
  if (v1.r!=v2.r) return 0;
  return 1;
}

int operator != (const TUFF& v1, const TUFF& v2){
  if (v1.l!=v2.l) return 1;
  if (v1.r!=v2.r) return 1;
  return 0;
}


// 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5
//                           
//       multiple floats for her pleasure
//                           
// 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5


//template <int N>
//MUFF<N>::MUFF(float *_r)
//template <int N>
//MUFF<N>::MUFF(int in){
//}

//MUFF MUFF::operator + (const MUFF& v){return MUFF(l+v.l,r+v.r);};
//MUFF MUFF::operator - (const MUFF& v){return MUFF(l-v.l,r-v.r);};
//MUFF& MUFF::operator += (const MUFF& v){l+=v.l;r+=v.r;return *this;};
//MUFF& MUFF::operator -= (const MUFF& v){l-=v.l;r-=v.r;return *this;};
//MUFF& MUFF::operator *= (float s){l*=s;r*=s;return *this;};
//MUFF& MUFF::operator /= (float c){float s=1.0f/c;l*=s;r*=s;return *this;};
//
//MUFF operator + (const MUFF& v){return MUFF(v.l,v.r);};
//MUFF operator - (const MUFF& v){return MUFF(-v.l,-v.r);};
//MUFF operator + (const MUFF& v1, const MUFF& v2){return MUFF(v1.l+v2.l,v1.r+v2.r);};
//MUFF operator - (const MUFF& v1, const MUFF& v2){return MUFF(v1.l-v2.l,v1.r-v2.r);};
//MUFF operator * (const MUFF& v, float s){return MUFF(v.l*s,v.r*s);};
//MUFF operator * (float s, const MUFF& v){return MUFF(v.l*s,v.r*s);};
//MUFF operator / (const MUFF& v, float c){float s=1.0f/c;return MUFF(v.l*s,v.r*s);};
