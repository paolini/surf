#ifndef _SURF_H_
#define _SURF_H_

#include <string>
#include "vertex.h"
#include "render.h"

char command_prompt(const char *prompt, std::string commands[][2], bool case_sensitive=false);

template<class T> class next_iterator {
private:
  T *p;
public:
  next_iterator(T *p_): p(p_) {};
  bool operator!=(const next_iterator &other) const {return other.p != p;}
  bool operator==(const next_iterator &other) const {return other.p == p;}
  T &operator*() {return *p;}
  next_iterator &operator++() {p = p->next; return *this;}
};

typedef next_iterator<triangle> triangle_iterator;
typedef next_iterator<vertex> vertex_iterator;

class Border;

class surf {
public:
  
  int n_vertices() const;
  int n_triangles() const {return ntriangles;};
  
  surf(Border *border_);
  
  vector3 border_function(double t);
  vertex* new_border_vertex(vertex *v, vertex *w);
    
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
  
  void print_vertex_list();
  void print_triangle_list();

  typedef vector3 transform_function(vector3);
  
  void transform_vertices(transform_function transform);
  
  void auto_zoom(camera &view);
  
  static const double evolve_factor;
  static int interrupt;

  triangle_iterator triangle_begin() {return first_triangle;}
  triangle_iterator triangle_end() {return 0;}

  vertex_iterator vertex_begin() {return first_vertex;}
  vertex_iterator vertex_end() {return 0;}
  
private:
  Border *border;
  triangle *first_triangle;
  int ntriangles;
  vertex *first_vertex;
};
#endif
