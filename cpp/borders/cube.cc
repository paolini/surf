#include <cmath>

#include "../vector3.h"
#include "../border.h"
#include "../surf.h"

/****************************/
/* definizione del contorno */
/****************************/

class Cube: public Border {
public:
  
  vertex* new_border_vertex(vertex *v,vertex *w)
  {
    surf &S = *this;
    vertex *p;
    p=S.new_vertex(0.5*(*v+*w));
    if (v->border==1 && w->border==1)
      {
	p->border=1;
	p->next_border=p;
      }
    return p;
  }
  
  vector3 border_function(double )
  {
    return vector3(0,0,0);
  }
  
  void quadr(vertex *a,vertex *b, vertex *c,vertex *d)
  {
    surf &S = *this;
    S.new_triangle(a,b,c);
    S.new_triangle(a,c,d);
  }
  
  void init_border()
  {
    surf &S = *this;
    int i,j;
    vertex *p[8];
    vertex *q[4];
    
    string commands[][2] = {
      {"s", "simply_connected"},
      {"n", "non_simply_connected"},
      {"c", "custom_sizes"},
      {"", ""}
    };
    string commands2[][2] = {
      {"s", "simply_connected"},
      {"n", "non_simply_connected"},
      {"", ""}
    };
    
    char c = command_prompt("<S>imply connected, <N>on-simply connected? <C>ustomize size?", commands);
    double sx=1.0, sy=1.0, sz=1.0;
    if (c=='c') {
      cout << "Enter sizes x, y, z: ";
      cin >> sx >> sy >> sz;
      c = command_prompt("<S>imply connected, <N>on-simply connected?", commands2);
    }

    for (i=0;i<8;i++)
      {
	p[i]=S.new_vertex(vector3(sx*((i/4)-0.5),sy*((i%4)/2-0.5),sz*(i%2-0.5)));
	p[i]->border=1.0;
	p[i]->next_border=p[i];
      }
    for (i=0;i<4;i++)
      {
	q[i]=S.new_vertex(vector3(sx*0,sy*(i/2-0.5)*(3-sqrt(3))/3.0,sz*(i%2-0.5)*(3-sqrt(3))/3.0));
      }
    for (i=0;i<8;i+=4)
      {
	quadr(p[i],p[i+1],q[1],q[0]);
	quadr(p[i+1],p[i+3],q[3],q[1]);
	quadr(p[i+3],p[i+2],q[2],q[3]);
	quadr(p[i+2],p[i],q[0],q[2]); 
      }
    for (i=0;i<4;i++)
      S.new_triangle(p[i],p[i+4],q[i]);
    if (c == 's')
      quadr(q[0],q[1],q[3],q[2]);
  }
};

static bool initializer = registry_function<Cube>("cube");
