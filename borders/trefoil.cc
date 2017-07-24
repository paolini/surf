#include <cmath>

#include "../vector3.h"
#include "../border.h"

class Trefoil: public Border {
public:
  /* t va da 0 a 4\pi */
  double R; //raggio grande
  double r; //raggio piccolo
  double r1; //altezza?

  Trefoil(): R(1.0), r(0.2), r1(0.4) {}
  
  double mysqrt(double x)
  {
    return x;
    //  return x>0?sqrt(x):-sqrt(-x);
  }

  vector3 border_function(double t)
  {	
    return vector3((R-r*cos(3.0*t/2.0))*cos(t),(R-r*cos(3.0*t/2.0))*sin(t),r1*mysqrt(sin(3.0*t/2.0)));
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
      p=new_vertex(0.5*(*v+*w));
    return p;
  }

  void init_border()
  {
    int i;
    int mode;	
    vertex *p[6];

    cout<<"R= "; cin>>R;
    cout<<"r= "; cin>>r;
    cout<<"h= "; cin>>r1;
  
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
	cout<<"Nastro di moebius\n";
	for (i=0;i<3;i++)
	  {
	    new_triangle(p[i],p[i+1],p[(i+4)%6]);
	    new_triangle(p[i],p[(i+3)%6],p[(i+4)%6]);
	  }
      }
    else
      {
	vertex *a[2];
	a[0]= new_vertex(vector3(0,0,1));
	a[1]= new_vertex(vector3(0,0,-1));
	cout<<"Superficie orientata\n";
	for (i=0;i<6;i++)
	  {
	    new_triangle(p[i],p[(i+1)%6],a[i%2]);
	    new_triangle(p[i],p[(i+3)%6],a[(i+1)%2]);
	  }
      }

  }
};

static bool initializer = registry_function<Trefoil>("trefoil");
