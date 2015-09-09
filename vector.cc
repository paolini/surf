#include "vector.h"

matrix::matrix(void) {}

matrix::matrix(vector &a,vector &b,vector &c) {
 x[0]=a;
 x[1]=b;
 x[2]=c;
}

matrix::matrix(matrix &m) {
  x[0]=m.x[0];
  x[1]=m.x[1];
  x[2]=m.x[2];
}

double det(const matrix &m) {
  return 
    m.x[0].x[0]*(m.x[1].x[1]*m.x[2].x[2]-m.x[2].x[1]*m.x[1].x[2])-
    m.x[1].x[0]*(m.x[0].x[1]*m.x[2].x[2]-m.x[2].x[1]*m.x[0].x[2])+
    m.x[2].x[0]*(m.x[1].x[1]*m.x[1].x[2]-m.x[1].x[1]*m.x[1].x[2]);
}

matrix inv(const matrix &a) {
#define COMPL_ALG(i,j) (a.x[(i+1)%3].x[(j+1)%3]*a.x[(i+2)%3].x[(j+2)%3]-a.x[(i+1)%3].x[(j+2)%3]*a.x[(i+2)%3].x[(j+1)%3])
  double d=det(a);
  matrix r;
  if (d==0)
    {
      cerr<<"matrice non invertibile \n\n";
      return(r);
    }
  for (int i=0;i<3;i++)
    for (int j=0;j<3;j++)
      r.x[i].x[j]=(1-2*((i+j)%2))*COMPL_ALG(j,i)/d;
  return(r);
}

vector::vector(const vector &v) {
  x[0]=v.x[0];
  x[1]=v.x[1];
  x[2]=v.x[2];
}

vector::vector(double X,double Y,double Z) {
  x[0]=X;x[1]=Y;x[2]=Z;
}

istream& operator>>(istream& c,vector &v) {
  return c>>v.x[0]>>v.x[1]>>v.x[2];
}

ostream& operator<<(ostream& c,vector v) {
return c<<"("<<v.x[0]<<","<<v.x[1]<<","<<v.x[2]<<")";
}

vector &operator+=(vector &a,vector b) {
  a.x[0]+=b.x[0];a.x[1]+=b.x[1];a.x[2]+=b.x[2];
  return a;
}

vector &operator-=(vector &a,vector b) {
  a.x[0]-=b.x[0];a.x[1]-=b.x[1];a.x[2]-=b.x[2];
  return a;
}

vector &operator/=(vector &a,double k) {
  a.x[0]/=k;a.x[1]/=k;a.x[2]/=k;
  return a;
}

int operator!=(vector a,vector b) {
  return !(a.x[0]==b.x[0] && a.x[1]==b.x[1] && a.x[2]==b.x[2]);
}

vector operator+ (vector a,vector b) {
  return vector (a.x[0]+b.x[0],a.x[1]+b.x[1],a.x[2]+b.x[2]);
}

vector operator- (vector a,vector b) {
  return vector (a.x[0]-b.x[0],a.x[1]-b.x[1],a.x[2]-b.x[2]);
}

vector operator- (vector b) {
  return vector (-b.x[0],-b.x[1],-b.x[2]);
}

double operator* (vector a,vector b) {
  return a.x[0]*b.x[0]+a.x[1]*b.x[1]+a.x[2]*b.x[2];
}

vector operator* (double a,vector b) {
  return vector(a*b.x[0],a*b.x[1],a*b.x[2]);
}

vector operator*(const matrix &m,vector v) {
  return v.x[0]*m.x[0]+v.x[1]*m.x[1]+v.x[2]*m.x[2];
}

vector operator^(vector a,vector b) {
  return vector (a.x[1]*b.x[2]-a.x[2]*b.x[1],a.x[2]*b.x[0]-
		 a.x[0]*b.x[2],a.x[0]*b.x[1]-a.x[1]*b.x[0]);
}

double abs(vector a) {
  return sqrt(a.x[0]*a.x[0]+a.x[1]*a.x[1]+a.x[2]*a.x[2]);
}

vector versor(vector v) {
  double f;
  f=abs(v);
  if (f>0.0)
    return 1.0/abs(v)*v;
  else return v;
}

double sqr(double x)
{return x*x;};

double sqr(vector a) {
  return a.x[0]*a.x[0]+a.x[1]*a.x[1]+a.x[2]*a.x[2];
}
