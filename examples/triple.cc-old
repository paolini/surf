#include <math.h>

#include "vector.h"
#include "vertex.h"
/****************************/
/* definizione del contorno */
/****************************/

/* t va da 0 a 6\pi */
double R=1.0;
double r=0.4;

vector border_function(double t)
{	
return vector((R-r*cos(t/3))*cos(t),(R-r*cos(t/3))*sin(t),r*sin(t/3));
}	
vertex* new_border_vertex(vertex *v,vertex *w)
{
  vertex *p;
  double d;
  if (v->next_border==w || w->next_border==v)
    {
      d=(v->border+w->border)/2.0;
      if (fabs(v->border-w->border)<2.9*M_PI)
	{
	  p=new vertex(border_function(d));
	  p->border=d;
	}	
      else
	{
	  d+=3*M_PI;
	  while (d>=6*M_PI) d-=6*M_PI;
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
	  p[i]=new vertex(border_function(6.0*M_PI/9.0 * i));
	  p[i]->border=6*M_PI/9.0 * i;
	}
      for (i=0;i<9;i++)
	p[i]->next_border=p[(i+1)%9];
      for (i=0;i<3;i++)
	{
	  q[i]=new vertex(vector(R*cos(2.0*M_PI/3.0*i),R*sin(2.0*M_PI/3.0*i),0.0));
	}
      for (i=0;i<3;i++)
	{
	  for (j=0;j<9;j+=3)
	    {
	      new triangle(p[i+j],p[(i+j+1)%9],q[(i+1)%3]);
	      new triangle(q[(i+1)%3],q[i],p[i+j]);
	    }
	}
    }
  else if (c=='n')
    {
      cout<<"triplo nastro non orientato";
      
      for (i=0;i<9;i++)
	{
	  p[i]=new vertex(border_function(6.0*M_PI/9.0 * i));
	  p[i]->border=6*M_PI/9.0 * i;
	}
      for (i=0;i<9;i++)
	p[i]->next_border=p[(i+1)%9];
      
      new triangle(p[0],p[1],p[8]);
      new triangle(p[1],p[2],p[5]);
      new triangle(p[1],p[8],p[5]);
      new triangle(p[5],p[7],p[8]);
      new triangle(p[5],p[7],p[4]);
      new triangle(p[5],p[6],p[3]);
      new triangle(p[5],p[2],p[3]);
      new triangle(p[3],p[6],p[7]);
      new triangle(p[3],p[4],p[7]);
    }
  else
    {
      cout<<"triplo nastro orientato";
	for (i=0;i<9;i++)
	  {
	    p[i]=new vertex(border_function(6.0*M_PI/9.0 * i));
	    p[i]->border=6*M_PI/9.0 * i;
	  }
      for (i=0;i<9;i++)
	p[i]->next_border=p[(i+1)%9];
      
      new triangle(p[3],p[4],p[2]);
      new triangle(p[4],p[2],p[1]);
      new triangle(p[5],p[4],p[1]);
      new triangle(p[5],p[1],p[0]);
      new triangle(p[5],p[6],p[0]);
      new triangle(p[6],p[0],p[8]);
      new triangle(p[6],p[7],p[8]);
    }
}
