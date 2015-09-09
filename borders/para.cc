#include <cmath>

#include "../vector3.h"
#include "../border.h"

using namespace std;

/****************************/
/* definizione del contorno */
/****************************/

// t va da 0 a 2*(3*M_PI+0.5*M_PI)=7*M_PI
static double h=0.3;
static double T=7.0*M_PI;
static double my_sqr(double x) {return x*x;}
static double r(double t)
{
  return my_sqr((t-M_PI)/(3.0/2.0*M_PI));
}

static vector3 border_function(double t)
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
  return vector3(x,y,z);
}

static vertex* new_border_vertex(surf &S, vertex *v,vertex *w)
{
  vertex *p;
  double d;
  if (v->next_border==w || w->next_border==v)
    {
      d=(v->border+w->border)/2.0;
      if (fabs(v->border-w->border)<T/2.0)
	{
	  p=S.new_vertex(border_function(d));
	  p->border=d;
	}	
      else 
	{
	  d+=T/2.0;
	  p=S.new_vertex(border_function(d));
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
    p=S.new_vertex(0.5*(*v+*w));
  return p;
}

static void init_border(surf &S)
{
  int i;
  int buco;
  vector3 v(0,0,0);
  vertex *p[7],*q;
  vertex *a,*b;
  for (i=0;i<7;i++)
    {
    p[i]=S.new_vertex(border_function(T/7.0*i));
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
      a=new_border_vertex(S,p[2],p[3]);
      b=new_border_vertex(S,p[4],p[5]);

  S.new_triangle(p[0],p[3],p[4]);
  S.new_triangle(p[0],p[3],a);
  S.new_triangle(p[0],p[4],b);
  S.new_triangle(p[0],p[1],a);
  S.new_triangle(p[0],p[6],b);
  
  S.new_triangle(p[1],p[2],p[5]);
  S.new_triangle(p[1],p[6],p[5]);
  S.new_triangle(p[2],p[5],b);
  S.new_triangle(p[2],a,b);
  S.new_triangle(p[1],p[6],a);
  S.new_triangle(p[6],b,a);
    }
  else
    {
  S.new_triangle(p[0],p[6],p[1]);
  S.new_triangle(p[6],p[1],p[5]);
  S.new_triangle(p[1],p[2],p[5]);
  S.new_triangle(p[2],p[5],p[3]);
  S.new_triangle(p[5],p[3],p[4]);
    }
}


static bool init() {
  Border::registry["para"] = new Border(border_function, new_border_vertex, init_border);
  return true;
}

static bool initializer = init();
