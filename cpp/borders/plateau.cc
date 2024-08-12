#include <cmath>

#include "../vector3.h"
#include "../border.h"

class Plateau: public Border {
public:
  /* t va da 0 a 2\pi */
  double R;
  double h;
  int N;

  Plateau(): R(1.0), h(0.2) {}
  
  vector3 border_function(double t)
  {	
    return vector3(R*cos(t),R*sin(t),h*cos(N*t));
  }

  vertex* new_border_vertex(vertex *v,vertex *w)
  {
    vertex *p;
    double d;
    if (v->next_border==w || w->next_border==v)
      {
	d=(v->border+w->border)/2.0;
	if (fabs(v->border-w->border)<M_PI)
	  {
	    p=new_vertex(border_function(d));
	    p->border=d;
	  }	
	else
	  {
	    d+=M_PI;
	    while (d>=2*M_PI) d-=2*M_PI;
	    p=new_vertex(border_function(d));
	    p->border=d;
	  }
	if (v->next_border==w)
	  {
	    v->next_border=p;
	    p->next_border=w;
	  }
	else
	  {
	    w->next_border=p;
	    p->next_border=v;
	  }
      }	
    else
      p=new_vertex(0.5*(*v+*w));
    return p;
  }
  void quadr(vertex *a,vertex*b,vertex*c,vertex*d)
  {
    new_triangle(a,b,c);
    new_triangle(a,c,d);
  }
  
  void init_border()
  {
    int i;
    int mode;	
    vertex *p[3];
    cout<<"R= ";
    cin>>R;
    cout<<"h= ";
    cin>>h;
    cout<<"N= ";
    cin>>N;
    for (i=0;i<3;i++)
      {
	p[i]=new_vertex(border_function(2.0*M_PI/3.0 * ((double)i+0.5)));
	p[i]->border=2.0*M_PI/3.0 * ((double)i+0.5);
      }
    for (i=0;i<3;i++)
      p[i]->next_border=p[(i+1)%3];
    new_triangle(p[0],p[1],p[2]);
  }
};

static bool initializer = registry_function<Plateau>("plateau");
