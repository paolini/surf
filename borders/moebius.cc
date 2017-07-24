#include <cmath>

#include "../vector3.h"
#include "../border.h"

/****************************/
/* definizione del contorno */
/****************************/
class Moebius: public Border {
public:
  /* t va da 0 a 4\pi */
  double R;
  double r;

  Moebius(): R(1.0), r(0.4) {}
  
  vector3 border_function(double t)
  {	
    return vector3((R-r*cos(t/2.0))*cos(t),(R-r*cos(t/2.0))*sin(t),r*(sin(t/2.0)));
  }	

  vertex* new_border_vertex(vertex *v,vertex *w)
  {
    vertex *p;
    double d;
    if (v->next_border==w || w->next_border==v)
      {
	d=(v->border+w->border)/2.0;
	if (fabs(v->border-w->border)<2*M_PI)
	  {
	    p=new_vertex(border_function(d));
	    p->border=d;
	  }	
	else
	  {
	    d+=2*M_PI;
	    while (d>=4*M_PI) d-=4*M_PI;
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
      p = new_vertex(0.5*(*v+*w));
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
    vertex *p[6];
  
    for (i=0;i<6;i++)
      {
	p[i]=new_vertex(border_function(4.0*M_PI/6.0 * ((double)i+0.5)));
	p[i]->border=4*M_PI/6.0 * ((double)i+0.5);
      }
    for (i=0;i<6;i++)
      p[i]->next_border=p[(i+1)%6];
    cout<<"Vuoi il nastro di Moebius (o la sup. orient.)? (1/0))";
    cin>>mode;
    if (mode)
      {
	cout<<"Nastro di moubius\n";
	for (i=0;i<3;i++)
	  {
	    quadr(p[i],p[i+1],p[(i+4)%6],p[i+3]);
	  }
      }
    else
      {
	cout<<"Superficie orientata\n";		
	new_triangle(p[4],p[3],p[5]);
	quadr(p[0],p[2],p[3],p[5]);
	new_triangle(p[0],p[1],p[2]);
      }

  }
};


static bool initializer = registry_function<Moebius>("moebius");;
