#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <iostream.h>



class vector {
 public:
  float x[3];
 public:
  
  vector(double x,double y,double z);
  vector (const vector&);
  vector (void){};

  friend class matrix;
  friend istream& operator>>(istream& c,vector &v);
  friend ostream &operator<<(ostream& c,vector v);
  friend vector &operator+=(vector &a,vector b);
  friend vector &operator-=(vector &a,vector b);
  friend vector &operator/=(vector &a,double k);
  friend int operator!=(vector a,vector b);
  friend vector operator+ (vector a,vector b);
  friend vector operator- (vector a,vector b);
  friend vector operator- (vector b);
  friend double operator* (vector a,vector b);
  friend vector operator* (double a,vector b);
  friend vector operator^(vector a,vector b);
  friend double abs(vector a);
  friend vector versor(vector v);
  friend double sqr(double x);
  friend double sqr(vector a);

  friend matrix inv(const matrix &);
  friend double det(const matrix &);
  friend vector operator*(const matrix &m,vector v);
};

class matrix {
 public:
  vector x[3];
  
 public:
  matrix(void);
  matrix(vector&, vector&, vector&);
  matrix(matrix &);
  friend matrix inv(const matrix &);
  friend double det(const matrix &);
  friend vector operator*(const matrix &m,vector v);
};

#endif
