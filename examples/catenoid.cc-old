#include <math.h>

#include "vector.h"
#include "vertex.h"

/****************************/
/* definizione del contorno */
/****************************/

/* t va da 0 a 4\pi */
double R=1.3;
double h=1.0;

vector border_function(double t) {	
return vector(R*cos(t),R*sin(t),t>=2.0*M_PI?-h:h);
}	

vertex* new_border_vertex(vertex *v,vertex *w) {
  vertex *p;
  double d;
  if (v->next_border==w || w->next_border==v)
    {
      d=(v->border+w->border)/2.0;
      if (fabs(v->border-w->border)<M_PI)
	{
	  p=new vertex(border_function(d));
	  p->border=d;
	}	
      else 
	{
	  d+=M_PI;
	  if (v->border<2.0*M_PI)
	    while (d>=2.0*M_PI) d-=2.0*M_PI;
	  else
	    while (d>=4.0*M_PI) d-=2.0*M_PI;
	  p=new vertex(border_function(d));
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
    p=new vertex(0.5*(*v+*w));
  return p;
}

void init_border(void)
{
  int i;
  vertex *p[6];
  
  cout<<"R=";
  cin>>R;
  cout<<"h=";
  cin>>h;

  for (i=0;i<6;i++)
    {
    p[i]=new vertex(border_function(4.0*M_PI/6.0 * i));
    p[i]->border=4.0*M_PI/6.0 * i;
    }
  for (i=0;i<3;i++)
    {
      p[i]->next_border=p[(i+1)%3];
      p[i+3]->next_border=p[(i+1)%3+3];
    }

  char c;
  cout<<"Vuoi l'iperboloide (i) o i due dischi (d)? ";
  cin>>c;

  if (c=='d' || c=='D') {
    new triangle(p[0],p[1],p[2]);
    new triangle(p[3],p[4],p[5]);
  } else {
    for (i=0;i<3;i++) {
	new triangle(p[i],p[(i+1)%3],p[i+3]);
	new triangle(p[i+3],p[(i+1)%3+3],p[(i+1)%3]);
      }
  }
}
