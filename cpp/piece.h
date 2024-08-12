#include "vector3.h"

class piece
{
public:
  virtual vector3 point(double t)=0;
  double real_t;
  piece(void);

  static double r;
  static const int MAXBOR=100;
  static piece *border[MAXBOR];
  static int num_pieces;
};

class arc:public piece
{
public:
  vector3 center;
  double radius;
  double alpha;
  double beta;
  int versus; /*antiorario?*/

  virtual vector3 point(double)=0;
  double angle(double t);

  arc(vector3,int,int);
};

class xy_arc:public arc
{
public:
  virtual vector3 point(double t);
  xy_arc(vector3 v,int f,int t):arc(v,f,t){};
};
class xz_arc:public arc
{
public:
  virtual vector3 point(double t);
  xz_arc(vector3 v,int f,int t):arc(v,f,t){};
};
class yz_arc:public arc
{
public:
  virtual vector3 point(double t);
  yz_arc(vector3 v,int f,int t):arc(v,f,t){};

};

class segment:public piece
{
public:
  vector3 from;
  vector3 to;
  
  virtual vector3 point(double t);
  segment(vector3 f,vector3 t);
  segment(piece *f,piece *t);
};

class curve: public piece {
public:
  vector3 pos;
  double t0,t1;
  vector3 (*fun)(double t);
  
  virtual vector3 point(double t);
  curve(vector3 (*fun)(double t),vector3 start,double t0=0,double t1=1);
};

template<class T>
class t_curve: public piece {
public:
  vector3 pos;
  double t0,t1;
  T *ptr;
  typedef vector3 (T::*fun_t)(double t);
  fun_t fun;
  
  vector3 point(double t) {return (ptr->*fun)(t);}
  
  t_curve(T *ptr_, fun_t fun_, vector3 start, double t0_=0, double t1_=1)
    : piece(), ptr(ptr_), t0(t0_), t1(t1_), fun(fun_) {
    pos = start - (ptr->*fun)(t0);
  }
};

