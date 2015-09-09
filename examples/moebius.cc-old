#include <math.h>

#include "vector.h"
#include "vertex.h"

/****************************/
/* definizione del contorno */
/****************************/

/* t va da 0 a 4\pi */
double R=1.0;
double r=0.4;

vector border_function(double t)
{	
return vector((R-r*cos(t/2.0))*cos(t),(R-r*cos(t/2.0))*sin(t),r*(sin(t/2.0)));
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
	  p=new vertex(border_function(d));
	  p->border=d;
	}	
      else
	{
	  d+=2*M_PI;
	  while (d>=4*M_PI) d-=4*M_PI;
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
  vertex *p[6];
  
  for (i=0;i<6;i++)
    {
    p[i]=new vertex(border_function(4.0*M_PI/6.0 * ((double)i+0.5)));
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
      new triangle(p[4],p[3],p[5]);
      quadr(p[0],p[2],p[3],p[5]);
      new triangle(p[0],p[1],p[2]);
    }

}
