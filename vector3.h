#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include <iostream>

using namespace std;

class vector3 {
 public:
  float x[3];
 public:
  
  vector3(double x,double y,double z);
  vector3 (const vector3&);
  vector3 (void){};

  friend class matrix;
  friend istream& operator>>(istream& c,vector3 &v);
  friend ostream &operator<<(ostream& c,vector3 v);
  friend vector3 &operator+=(vector3 &a,vector3 b);
  friend vector3 &operator-=(vector3 &a,vector3 b);
  friend vector3 &operator/=(vector3 &a,double k);
  friend int operator!=(vector3 a,vector3 b);
  friend vector3 operator+ (vector3 a,vector3 b);
  friend vector3 operator- (vector3 a,vector3 b);
  friend vector3 operator- (vector3 b);
  friend double operator* (vector3 a,vector3 b);
  friend vector3 operator* (double a,vector3 b);
  friend vector3 operator^(vector3 a,vector3 b);
  friend double abs(vector3 a);
  friend vector3 versor(vector3 v);
  friend double sqr(double x);
  friend double sqr(vector3 a);

  friend matrix inv(const matrix &);
  friend double det(const matrix &);
  friend vector3 operator*(const matrix &m,vector3 v);
};

class matrix {
 public:
  vector3 x[3];
  
 public:
  matrix(void);
  matrix(vector3&, vector3&, vector3&);
  matrix(matrix &);
  friend matrix inv(const matrix &);
  friend double det(const matrix &);
  friend vector3 operator*(const matrix &m,vector3 v);
};

#endif
