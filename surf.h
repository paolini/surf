#ifndef _SURF_H_
#define _SURF_H_

#include <string>
#include "vertex.h"

char command_prompt(const char *prompt, std::string commands[][2], bool case_sensitive=false);


class surf {
 public:
  vertex *first_vertex;
  triangle *first_triangle;
  int ntriangles;

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

  void check();

  int n_triangle();
  int n_vertex();

  double area();
  double diameter();

  int side_triangulate(vertex *v, vertex *w);
  int side_triangulate(triangle *t, double radius);
  void side_triangulate(double radius, int border=0);

  int border_side_triangulate(triangle *t, double radius);

  triangle* center_triangulate(triangle *t);
  void center_triangulate();
  
  void evolve_triangulate(double err, double radius=-1);

  void harmonic(double eps=-1.0);
  double evolve(double eps=-1.0, double v=-1);
  
  vertex *new_border_vertex(vertex *v, vertex *w);

  void print_vertex_list();
  void print_triangle_list();
  
  static const double evolve_factor;
  static int interrupt;
};
#endif
