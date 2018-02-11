// 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5
//                           
// bobis gumbular grannied more efficient D3Dmathz                           
//                           
// 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5 8081'5
#pragma once
#ifndef RTTVECTOR_H
#define RTTVECTOR_H

#define D3D_OVERLOADS
#include <math.h>

#define maximummy(a,b)            (((a) > (b)) ? (a) : (b))
#define minimummy(a,b)            (((a) < (b)) ? (a) : (b))

class RVEC{
public:
  float x;
  float y;
  float z;
  
  RVEC(){};
  RVEC(float f);
  RVEC(float _x, float _y, float _z);
  
  RVEC operator + (const RVEC& v);
  RVEC operator - (const RVEC& v);
  RVEC operator += (const RVEC& v);
  RVEC operator -= (const RVEC& v);
  RVEC operator *= (float s);
  RVEC operator /= (float c);
  
//  friend RVEC operator + (const RVEC& v);
  friend RVEC operator - (const RVEC& v);
  
  friend RVEC operator + (const RVEC& v1, const RVEC& v2);
  friend RVEC operator - (const RVEC& v1, const RVEC& v2);
  friend RVEC operator * (const RVEC& v, float s);
  friend RVEC operator * (float s, const RVEC& v);
  friend RVEC operator / (const RVEC& v, float c);
  friend int operator == (const RVEC& v1, const RVEC& v2);
  friend int operator != (const RVEC& v1, const RVEC& v2);
};

float SquareMagnitude(RVEC M);
float Magnitude(RVEC M);
RVEC Mormalize(RVEC M);
RVEC Xproduct(const RVEC &a,const RVEC &b);
float DProduct (const RVEC& v1, const RVEC& v2);
RVEC normalise(const RVEC &v);



class RVEC4{
public:
  float x;
  float y;
  float z;
  float w;
  
  RVEC4(){};
  RVEC4(float f);
  RVEC4(float _x, float _y, float _z, float _w);
  
  RVEC4 operator + (const RVEC4& v);
  RVEC4 operator - (const RVEC4& v);
  RVEC4 operator += (const RVEC4& v);
  RVEC4 operator -= (const RVEC4& v);
  RVEC4 operator *= (float s);
  RVEC4 operator /= (float c);
  
  friend RVEC4 operator + (const RVEC4& v);
  friend RVEC4 operator - (const RVEC4& v);
  
  friend RVEC4 operator + (const RVEC4& v1, const RVEC4& v2);
  friend RVEC4 operator - (const RVEC4& v1, const RVEC4& v2);
  friend RVEC4 operator * (const RVEC4& v, float s);
  friend RVEC4 operator * (float s, const RVEC4& v);
  friend RVEC4 operator / (const RVEC4& v, float c);
  friend int operator == (const RVEC4& v1, const RVEC4& v2);
  friend int operator != (const RVEC4& v1, const RVEC4& v2);
};

float SquareMagnitude(RVEC4 M);
float Magnitude(RVEC4 M);
RVEC4 Mormalize(RVEC4 M);
RVEC4 Xproduct(const RVEC4 &a,const RVEC4 &b);
float DProduct (const RVEC4& v1, const RVEC4& v2);
RVEC4 normalise(const RVEC4 &v);


template<class T>
class RVEC2{
public:
  T x;
  T y;

  static RVEC2 animal2;
  RVEC2(){};
  RVEC2(T f){x=f;y=f;};
  RVEC2(T _x, T _y){x=_x;y=_y;};

  RVEC2 operator + (const RVEC2& v);
  RVEC2 operator - (const RVEC2& v){return RVEC2(x-v.x,y-v.y);};
  RVEC2 operator += (const RVEC2& v);
  RVEC2 operator -= (const RVEC2& v);
  RVEC2 operator *= (T s);
  RVEC2 operator /= (T c);

  friend RVEC2 operator + (const RVEC2& v);
  friend RVEC2 operator - (const RVEC2& v);

  friend RVEC2 operator + (const RVEC2& v1, const RVEC2& v2);
  friend RVEC2 operator - (const RVEC2& v1, const RVEC2& v2);
  friend RVEC2 operator * (const RVEC2& v, float s);
  friend RVEC2 operator * (T s, const RVEC2& v);
  friend RVEC2 operator / (const RVEC2& v, float c);
  friend int operator == (const RVEC2& v1, const RVEC2& v2);
  friend int operator != (const RVEC2& v1, const RVEC2& v2);
};

template<class T>
T SquareMagnitude(RVEC2<T> M);

template<class T>
T Magnitude(RVEC2<T> M);

template<class T>
RVEC2<T> Mormalize(RVEC2<T> M);

template<class T>
T DProduct (const RVEC2<T>& v1, const RVEC2<T>& v2);

template<class T>
RVEC2<T> normalise(const RVEC2<T> &v);




// i choose to be hard & tuff (TUFF=Two Floats)
class TUFF{
public:
  float l;
  float r;

  TUFF(){};
  TUFF(float _x, float _y);

  TUFF operator + (const TUFF& v);
  TUFF operator - (const TUFF& v);
  TUFF& operator += (const TUFF& v);
  TUFF& operator -= (const TUFF& v);
  TUFF& operator *= (float s);
  TUFF& operator /= (float c);

  friend TUFF operator + (const TUFF& v);
  friend TUFF operator - (const TUFF& v);

  friend TUFF operator + (const TUFF& v1, const TUFF& v2);
  friend TUFF operator - (const TUFF& v1, const TUFF& v2);
  friend TUFF operator * (const TUFF& v, float s);
  friend TUFF operator * (float s, const TUFF& v);
  friend TUFF operator / (const TUFF& v, float c);
  friend int operator == (const TUFF& v1, const TUFF& v2);
  friend int operator != (const TUFF& v1, const TUFF& v2);
  short clipl();
  short clipr();
};



// i choose to be hard & muff (MUFF=Multiple Floats)
template<int N>
class MUFF{
public:
  MUFF(){};
  MUFF(float *_r){
    for(int i=0;i<N;i++){
      r[i]=_r[i];
    }
  }
  float r[N];

  MUFF operator + (const MUFF& v){
    MUFF temp;
    for(int i=0;i<N;i++){
      temp.r[i]=r[i]+v.r[i];
    }
    return temp;
  };
  MUFF operator - (const MUFF& v){
    MUFF temp;
    for(int i=0;i<N;i++){
      temp.r[i]=r[i]-v.r[i];
    }
    return temp;
  };
  MUFF operator += (const MUFF& v){
    for(int i=0;i<N;i++){
      r[i]+=v.r[i];
    }
    return *this;
  };
  MUFF operator -= (const MUFF& v){
    for(int i=0;i<N;i++){
      r[i]+=v.r[i];
    }
    return *this;
  };
  MUFF operator *= (const MUFF& v){
    for(int i=0;i<N;i++){
      r[i]*=v.r[i];
    }
    return *this;
  };
  MUFF operator /= (float c){
    float s=1.0f/c;
    for(int i=0;i<N;i++){
      r[i]*=s;
    }
    return *this;
  };
  
  friend MUFF operator + (const MUFF& v){
    return v;
  };
  friend MUFF operator - (const MUFF& v){
    MUFF temp;
    for(int i=0;i<N;i++){
      temp.r[i]=-v.r[i];
    }
    return temp;
  };
  friend MUFF operator + (const MUFF& v1,const MUFF& v2){
    MUFF temp;
    for(int i=0;i<N;i++){
      temp.r[i]=v1.r[i]+v2.r[i];
    }
    return temp;
  };
  friend MUFF operator - (const MUFF& v1,const MUFF& v2){
    MUFF temp;
    for(int i=0;i<N;i++){
      temp.r[i]=v1.r[i]-v2.r[i];
    }
    return temp;
  };
  friend MUFF operator * (const MUFF& v, float s){
    MUFF temp;
    for(int i=0;i<N;i++){
      temp.r[i]=v.r[i]*s;
    }
    return temp;
  };
  friend MUFF operator / (const MUFF& v, float c){
    MUFF temp;
    float s=1.0f/c;
    for(int i=0;i<N;i++){
      temp.r[i]=v.r[i]*c;
    }
    return temp;
  };

////  MUFF operator + (const MUFF& v);
////  MUFF operator - (const MUFF& v);
////  MUFF& operator += (const MUFF& v);
////  MUFF& operator -= (const MUFF& v);
////  MUFF& operator *= (float s);
////  MUFF& operator /= (float c);
////
////  friend MUFF operator + (const MUFF& v);
////  friend MUFF operator - (const MUFF& v);
////
////  friend MUFF operator + (const MUFF& v1, const MUFF& v2);
////  friend MUFF operator - (const MUFF& v1, const MUFF& v2);
//////  friend MUFF operator * (const MUFF& v, float s);
////  friend MUFF operator * (float s, const MUFF& v);
//  friend MUFF operator / (const MUFF& v, float c);
};

#endif
