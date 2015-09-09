#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "vector3.h"

class triangle_list;
class triangle;
class surf;

class vertex: public vector3 {
public:
  vertex *next;
  vertex *next_border;
  float border;
  triangle_list *list;


  void add_triangle(triangle *);
  void delete_triangle(triangle*);
  double area(void);
  vector3 grad(void);
  ~vertex(void);
  void evolve(double viscosity);
  void harmonic(void);
  int n_triangle(void) const;

  vertex(surf &S, const vector3 &v);

  vertex(void);
private:
  vertex(vector3 v);
  vertex(double x,double y,double z);

  friend class surf;
  
  void init(void);
};

class triangle
{
  public:
  vertex *v[3];
  triangle *next;

  vector3 bari(void);
  double area(void);
  vector3 normal(void);
  double side_length(int side);
  int check(void);

  triangle(): next(0){};
private:
  triangle(vertex *,vertex *,vertex *);
  ~triangle(void);
  friend class surf;
};

class triangle_list
{
public:
  triangle *first;
  triangle_list *next;

  triangle_list(triangle *,triangle_list *l=NULL);
  void delete_next(void);
};


class surf {
 public:
  vertex *first_vertex;
  triangle *first_triangle;
  int ntriangles;

  void clear() {
    
  }

  int n_vertices() const;

  int n_triangles() const {return ntriangles;};
  
  surf() {
    first_vertex=0;
    first_triangle=0;
    ntriangles=0;
  }

  void remove(vertex *);
  void add(vertex *);
  void remove(triangle *);
  void add(triangle *);

  vertex *new_vertex(const vector3 &v) {
    vertex *p=new vertex(v);
    add(p);
    return p;
  };

  triangle* new_triangle(vertex *a,vertex *b,vertex *c) {
    triangle *t=new triangle(a,b,c);
    add(t);
    return t;
  };
  
  void serialize(ostream &out) const;
  static surf *unserialize(ostream &in, surf *here=0);

  void geomview_off(ostream &out=cout) const;

};

#endif
