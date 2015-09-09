#include <math.h>

#include "vector.h"
#include "vertex.h"


/****************************/
/* definizione del contorno */
/****************************/

/* t va da 0 a 2\pi */
double R=1.0;
double h=0.2;
int N;

vector border_function(double t)
{	
return vector(R*cos(t),R*sin(t),h*cos(N*t));
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
	  p=new vertex(border_function(d));
	  p->border=d;
	}	
      else
	{
	  d+=M_PI;
	  while (d>=2*M_PI) d-=2*M_PI;
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
void quadr(vertex *a,vertex*b,vertex*c,vertex*d)
{
  new triangle(a,b,c);
  new triangle(a,c,d);
}
void init_border(void)
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
    p[i]=new vertex(border_function(2.0*M_PI/3.0 * ((double)i+0.5)));
    p[i]->border=2.0*M_PI/3.0 * ((double)i+0.5);
    }
  for (i=0;i<3;i++)
      p[i]->next_border=p[(i+1)%3];
  new triangle(p[0],p[1],p[2]);
}
