#include <iostream>
#include "vertex.h"

static void init_border(surf &S);
static vector3 border_function(double);
static vertex *new_border_vertex(surf &S, vertex *, vertex*);

extern "C" void dl_surf_get(void (**ib)(surf &),
			     vector3 (**bf)(double),
			     vertex * (**nbv)(surf&, vertex*, vertex*)) {
  *ib=&init_border;
  *bf=&border_function;
  *nbv=&new_border_vertex;
  //  cout<<"dl_surf_get: "<<(void *)(*ib)<<"\n";
}
