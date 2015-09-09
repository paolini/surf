#include <cmath>

#include "../vector3.h"
#include "../border.h"

/****************************/
/* definizione del contorno */
/****************************/

/* t va da 0 a 4\pi */
static double R=1.0;
static double r=0.4;

static vector3 border_function(double t)
{	
return vector3((R-r*cos(t/2.0))*cos(t),(R-r*cos(t/2.0))*sin(t),r*(sin(t/2.0)));
}	

static vertex* new_border_vertex(surf &S, vertex *v,vertex *w)
{
  vertex *p;
  double d;
  if (v->next_border==w || w->next_border==v)
    {
      d=(v->border+w->border)/2.0;
      if (fabs(v->border-w->border)<2*M_PI)
	{
	  p=S.new_vertex(border_function(d));
	  p->border=d;
	}	
      else
	{
	  d+=2*M_PI;
	  while (d>=4*M_PI) d-=4*M_PI;
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

static void quadr(surf &S,vertex *a,vertex*b,vertex*c,vertex*d)
{
  S.new_triangle(a,b,c);
  S.new_triangle(a,c,d);
}

static void init_border(surf &S)
{
  int i;
  int mode;	
  vertex *p[6];
  
  for (i=0;i<6;i++)
    {
    p[i]=S.new_vertex(border_function(4.0*M_PI/6.0 * ((double)i+0.5)));
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
	  quadr(S,p[i],p[i+1],p[(i+4)%6],p[i+3]);
	}
    }
  else
    {
      cout<<"Superficie orientata\n";		
      S.new_triangle(p[4],p[3],p[5]);
      quadr(S,p[0],p[2],p[3],p[5]);
      S.new_triangle(p[0],p[1],p[2]);
    }

}


static bool init() {
  Border::registry["moebius"] = new Border(border_function, new_border_vertex, init_border);
  return true;
}

static bool initializer = init();
