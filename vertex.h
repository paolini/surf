#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "vector.h"

class triangle_list;
class triangle;

class vertex: public vector
{
public:
  triangle_list *list;
  vertex *next;
  vertex *next_border;
  float border;

  void add_triangle(triangle *);
  void delete_triangle(triangle*);
  double area(void);
  vector grad(void);
  vertex(void);
  vertex(vector v);
  vertex(double x,double y,double z);
  ~vertex(void);
  void evolve(double viscosity);
  void harmonic(void);
  int n_triangle(void);
private:
  void init(void);
};

class triangle
{
  public:
  vertex *v[3];
  triangle *next;

  vector bari(void);
  double area(void);
  vector normal(void);
  double side_length(int side);
  triangle(vertex *,vertex *,vertex *);
  ~triangle(void);
  int check(void);
};

class triangle_list
{
public:
  triangle *first;
  triangle_list *next;

  triangle_list(triangle *,triangle_list *l=NULL);
  void delete_next(void);
};

#endif
