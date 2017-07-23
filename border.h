#include <map>
#include <string>

#include "surf.h"

struct Border {
  typedef vector3 border_function_fun(double);
  typedef vertex* new_border_vertex_fun(surf &S, vertex *v,vertex*w);
  typedef void init_border_fun(surf &);

  border_function_fun *border_function;
  new_border_vertex_fun *new_border_vertex;
  init_border_fun *init_border;

  Border(border_function_fun *bf, new_border_vertex_fun *nbv, init_border_fun *ib)
    : border_function(bf)
    , new_border_vertex(nbv)
    , init_border(ib) {}
  
  typedef std::map<std::string, Border*> Registry;
  static Registry registry;
};


