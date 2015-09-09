#include <math.h>

#include "vector.h"
#include "vertex.h"


/****************************/
/* definizione del contorno */
/****************************/

// t va da 0 a 2*(3*M_PI+0.5*M_PI)=7*M_PI
double h=0.3;
double T=7.0*M_PI;
double r(double t)
{
  return sqr((t-M_PI)/(3.0/2.0*M_PI));
}
vector border_function(double t)
{
  double s,x,y,z;
  if (t>7.0/2.0*M_PI) s=7.0*M_PI-t;
  else s=t;
  if (s<5.0/2.0*M_PI)
    {
      if (s<M_PI)
	{
	  x=-cos(s);
	  y=-sin(s)-(s/M_PI)*h;
	  z=0.0;
	}
      else
	{
	  x=1.0+r(s)*cos(s-M_PI-0.5*M_PI);
	  y=-h;
	  z=2.0*r(s)*sin(s-M_PI-0.5*M_PI);
	}
    }
  else 
    {
      if (s<3.0*M_PI)
	{
	  x=0;
	  y=-h;
	  z=-(s-5.0/2.0*M_PI)/(0.5*M_PI);
	}
      else
	{
	  x=0;
	  y=-h*cos(s-3.0*M_PI);
	  z=-1.0-h*sin(s-3.0*M_PI);
	}
    }
  if (t>7.0/2.0*M_PI) y=-y;
  return vector(x,y,z);
}

vertex* new_border_vertex(vertex *v,vertex *w)
{
  vertex *p;
  double d;
  if (v->next_border==w || w->next_border==v)
    {
      d=(v->border+w->border)/2.0;
      if (fabs(v->border-w->border)<T/2.0)
	{
	  p=new vertex(border_function(d));
	  p->border=d;
	}	
      else 
	{
	  d+=T/2.0;
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
  int buco;
  vector v(0,0,0);
  vertex *p[7],*q;
  vertex *a,*b;
  for (i=0;i<7;i++)
    {
    p[i]=new vertex(border_function(T/7.0*i));
    p[i]->border=T/7*i;
    }
  for (i=0;i<7;i++)
    {
      p[i]->next_border=p[(i+1)%7];
      v+=*p[i];
    }
  
  cout<<"Vuoi il buco? 0/1 ";
  cin>>buco;
  if (buco)
    {
  a=new_border_vertex(p[2],p[3]);
  b=new_border_vertex(p[4],p[5]);

  new triangle(p[0],p[3],p[4]);
  new triangle(p[0],p[3],a);
  new triangle(p[0],p[4],b);
  new triangle(p[0],p[1],a);
  new triangle(p[0],p[6],b);
  
  new triangle(p[1],p[2],p[5]);
  new triangle(p[1],p[6],p[5]);
  new triangle(p[2],p[5],b);
  new triangle(p[2],a,b);
  new triangle(p[1],p[6],a);
  new triangle(p[6],b,a);
    }
  else
    {
  new triangle(p[0],p[6],p[1]);
  new triangle(p[6],p[1],p[5]);
  new triangle(p[1],p[2],p[5]);
  new triangle(p[2],p[5],p[3]);
  new triangle(p[5],p[3],p[4]);
    }
}
