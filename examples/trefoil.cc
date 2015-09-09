#include <math.h>

#include "vector.h"
#include "vertex.h"

/****************************/
/* definizione del contorno */
/****************************/

/* t va da 0 a 4\pi */
double R=1.0; //raggio grande
double r=0.2; //raggio piccolo
double r1=0.4; //altezza?
double mysqrt(double x)
{
	return x;
//  return x>0?sqrt(x):-sqrt(-x);
}	
vector border_function(double t)
{	
return vector((R-r*cos(3.0*t/2.0))*cos(t),(R-r*cos(3.0*t/2.0))*sin(t),r1*mysqrt(sin(3.0*t/2.0)));
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
void init_border(void)
{
  int i;
  int mode;	
  vertex *p[6];

  cout<<"R= "; cin>>R;
  cout<<"r= "; cin>>r;
  cout<<"h= "; cin>>r1;
  
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
	  new triangle(p[i],p[i+1],p[(i+4)%6]);
	  new triangle(p[i],p[(i+3)%6],p[(i+4)%6]);
	}
    }
  else
    {
      vertex *a[2];
      a[0]= new vertex(vector(0,0,1));
      a[1]= new vertex(vector(0,0,-1));
      cout<<"Superficie orientata\n";
      for (i=0;i<6;i++)
	{
	  new triangle(p[i],p[(i+1)%6],a[i%2]);
	  new triangle(p[i],p[(i+3)%6],a[(i+1)%2]);
	}
    }

}
