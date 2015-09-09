#include <cmath>

#include "../vector3.h"
#include "../border.h"
/****************************/
/* definizione del contorno */
/****************************/

/* t va da 0 a 6\pi */
static double R=1.0;
static double r=0.4;

static vector3 border_function(double t)
{	
return vector3((R-r*cos(t/3))*cos(t),(R-r*cos(t/3))*sin(t),r*sin(t/3));
}	

static vertex* new_border_vertex(surf &S, vertex *v,vertex *w)
{
  vertex *p;
  double d;
  if (v->next_border==w || w->next_border==v)
    {
      d=(v->border+w->border)/2.0;
      if (fabs(v->border-w->border)<2.9*M_PI)
	{
	  p=S.new_vertex(border_function(d));
	  p->border=d;
	}	
      else
	{
	  d+=3*M_PI;
	  while (d>=6*M_PI) d-=6*M_PI;
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
  int i,j;
  vertex *p[9],*q[3];
  char c;
  cout<<"<S>ingolare, <N>on orientato o <O>rientato? ";
  cin>>c;
  if (c=='s')
    {
      cout<<"triplo nastro singolare";
      
      for (i=0;i<9;i++)
	{
	  p[i]=S.new_vertex(border_function(6.0*M_PI/9.0 * i));
	  p[i]->border=6*M_PI/9.0 * i;
	}
      for (i=0;i<9;i++)
	p[i]->next_border=p[(i+1)%9];
      for (i=0;i<3;i++)
	{
	  q[i]=S.new_vertex(vector3(R*cos(2.0*M_PI/3.0*i),R*sin(2.0*M_PI/3.0*i),0.0));
	}
      for (i=0;i<3;i++)
	{
	  for (j=0;j<9;j+=3)
	    {
	      S.new_triangle(p[i+j],p[(i+j+1)%9],q[(i+1)%3]);
	      S.new_triangle(q[(i+1)%3],q[i],p[i+j]);
	    }
	}
    }
  else if (c=='n')
    {
      cout<<"triplo nastro non orientato";
      
      for (i=0;i<9;i++)
	{
	  p[i]=S.new_vertex(border_function(6.0*M_PI/9.0 * i));
	  p[i]->border=6*M_PI/9.0 * i;
	}
      for (i=0;i<9;i++)
	p[i]->next_border=p[(i+1)%9];
      
      S.new_triangle(p[0],p[1],p[8]);
      S.new_triangle(p[1],p[2],p[5]);
      S.new_triangle(p[1],p[8],p[5]);
      S.new_triangle(p[5],p[7],p[8]);
      S.new_triangle(p[5],p[7],p[4]);
      S.new_triangle(p[5],p[6],p[3]);
      S.new_triangle(p[5],p[2],p[3]);
      S.new_triangle(p[3],p[6],p[7]);
      S.new_triangle(p[3],p[4],p[7]);
    }
  else
    {
      cout<<"triplo nastro orientato";
	for (i=0;i<9;i++)
	  {
	    p[i]=S.new_vertex(border_function(6.0*M_PI/9.0 * i));
	    p[i]->border=6*M_PI/9.0 * i;
	  }
      for (i=0;i<9;i++)
	p[i]->next_border=p[(i+1)%9];
      
      S.new_triangle(p[3],p[4],p[2]);
      S.new_triangle(p[4],p[2],p[1]);
      S.new_triangle(p[5],p[4],p[1]);
      S.new_triangle(p[5],p[1],p[0]);
      S.new_triangle(p[5],p[6],p[0]);
      S.new_triangle(p[6],p[0],p[8]);
      S.new_triangle(p[6],p[7],p[8]);
    }
}

static bool init() {
  Border::registry["triple"] = new Border(border_function, new_border_vertex, init_border);
  return true;
}

static bool initializer = init();
