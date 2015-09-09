#include <cmath>

#include "../vector3.h"
#include "../border.h"
#include "../piece.h"

static double h=0.4;
/****************************/
/* definizione del contorno */
/****************************/


static vector3 border_function(double t)
{
  int n;
  n=int(t);
  return piece::border[n]->point(t-(double)n);
}	

static vertex* new_border_vertex(surf &S, vertex *v,vertex *w)
{
  vertex *p;
  double d;
  if (v->next_border==w || w->next_border==v)
    {
      d=(v->border+w->border)/2.0;
      if (fabs(v->border-w->border)<((double)piece::num_pieces)/2.0)
	{
	  p=S.new_vertex(border_function(d));
	  p->border=d;
	}	
      else 
	{
	  d+=((double)piece::num_pieces)/2.0;
	  while(d>(double)piece::num_pieces) d-=(double)piece::num_pieces;
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


static void quadr(surf &S,vertex *a,vertex *b,vertex *c,vertex *d)
{
  S.new_triangle(a,b,c);
  S.new_triangle(a,c,d);
}

double &r = piece::r;

static vector3 moebius(double t)
{
  return vector3((r-h*cos(t/2.0))*cos(t),(r-h*cos(t/2.0))*sin(t),h*(sin(t/2.0)));
}
static vector3 triplo(double t)
{
return vector3((r-h*cos(t/3))*cos(t),(r-h*cos(t/3))*sin(t),h*sin(t/3));
}

static void init_border(surf &S)
{
  int i;
  vertex *p[7],*q[10],*s[3];
  piece *b[4];
  cout<<"Superficie che si retrae sul bordo\n";
  b[0]=new curve(moebius,vector3(-r/2.0,0,0),h/3-2.0*M_PI,2*M_PI-h/3);
  b[1]=new segment(b[0]->point(1.0),vector3(r/2,0,0));
  b[2]=new curve(triplo,b[1]->point(1.0),-3.0*M_PI+h/3,3.0*M_PI-h/3);
  b[3]=new segment(b[2]->point(1.0),b[0]->point(0.0));
  
  for (i=0;i<6;i++)
    {
      p[i]=S.new_vertex (b[0]->point(1.0/6.0*i));
      p[i]->border=1.0/6.0*i;
      if (i>0)
	p[i-1]->next_border=p[i];
    }
  p[6]=S.new_vertex (b[0]->point(1.0));
  p[6]->border=1.0;
  p[5]->next_border=p[6];
  for (i=0;i<3;i++)
    quadr(S,p[i],p[i+1],p[i+4],p[i+3]);
  S.new_triangle(p[0],p[3],p[6]);

  for (i=0;i<3;i++)
    s[i]=S.new_vertex(vector3(0,0,0));
  for (i=0;i<9;i++)
    {
      q[i]=S.new_vertex(b[2]->point(1.0/9.0*i));
      q[i]->border=2.0+1.0/9.0*i;
      if (i>0)
	q[i-1]->next_border=q[i];
    }
  q[9]=S.new_vertex (b[2]->point(1.0));
  q[9]->border=3.0;
  q[9]->next_border=p[0];
  q[8]->next_border=q[9];
  p[6]->next_border=q[0];
  
  for (i=0;i<9;i++)
    {
      quadr(S,q[i],q[i+1],s[(i+1)%3],s[i%3]);
    }
  S.new_triangle(q[0],q[9],s[0]);
  quadr(S,p[0],p[6],q[0],q[9]);
}

static bool init() {
  Border::registry["retract"] = new Border(border_function, new_border_vertex, init_border);
  return true;
}

static bool initializer = init();
