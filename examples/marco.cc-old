#include <math.h>

#include "vector.h"
#include "vertex.h"

/****************************/
/* definizione del contorno */
/****************************/

/* t va da 0 a 6\pi */
double r=1.0,R=1.5;
double h=0.7;
double osc=0.1;


vector border_function(double t)
{	
if (t<2.0*M_PI)
	return vector(r*cos(t),r*sin(t),h-osc*cos(2.0*t));
else if (t<4.0*M_PI)
	return vector(R*cos(t),R*sin(t),osc*cos(2.0*t));
else
	return vector(r*cos(t),r*sin(t),-h-osc*cos(2.0*t));
	
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

void quadr(vertex*a,vertex*b,vertex*c,vertex*d)
{
  new triangle(a,b,c);
  new triangle(a,c,d);
}

void init_border(void)
{
  int i,j;
  vertex *p[3][4];
  vertex *q;
  double t;

  cout<<"Marco sup. ";
  cout<<"R=";
  cin>>R;
  cout<<"h=";
  cin>>h;
  cout<<"osc=";
  cin>>osc;
	  
  for (i=0;i<3;i++)
    {
    for (j=0;j<4;j++)
      {
	t=2.0*M_PI*i+M_PI/2.0*j;
	p[i][j]=new vertex(border_function(t));
	p[i][j]->border=t;
	if (j>0)
	  p[i][j-1]->next_border=p[i][j];
      }
    p[i][3]->next_border=p[i][0];
    }
  q=new vertex(vector(0,0,0));
  for (j=0;j<4;j++)
    {
      quadr(p[2*((j+1)%2)][j],p[2*((j+1)%2)][(j+1)%4],p[1][(j+1)%4],p[1][j]);
      new triangle(q,p[1][j],p[0][j]);
      new triangle(q,p[1][j],p[2][j]);
      new triangle(q,p[2*(j%2)][j],p[2*(j%2)][(j+1)%4]);
    }
}

void init_border_bis(void)
{
  int i,j;
  vertex *p[15];

  cout<<"Marco sup. ";
  cout<<"R=";
  cin>>R;
  cout<<"h=";
  cin>>h;
  cout<<"osc=";
  cin>>osc;
	  
  for (i=0;i<9;i++)
    {
    p[i]=new vertex(border_function(2.0*M_PI/3.0 * i));
    p[i]->border=2.0*M_PI/3.0 * i;
    }
  for (i=0;i<3;i++)
    {
      p[i]->next_border=p[(i+1)%3];
      p[i+3]->next_border=p[(i+1)%3+3];
      p[i+6]->next_border=p[(i+1)%3+6];
    }
  for (i=9;i<15;i++)
    p[i]=new vertex(vector(0,0,0));

/******************Disco 2 buchi 1 manico******>>>**************/
/*
  new triangle(p[0],p[1],p[12]);
  new triangle(p[0],p[12],p[9]);
  new triangle(p[0],p[9],p[13]);
  new triangle(p[0],p[13],p[2]);
  new triangle(p[2],p[13],p[10]);
  new triangle(p[2],p[10],p[11]);
  new triangle(p[2],p[11],p[1]);
  new triangle(p[1],p[11],p[14]);
  new triangle(p[1],p[14],p[13]);
  new triangle(p[1],p[13],p[12]);
  new triangle(p[10],p[14],p[13]);
  new triangle(p[8],p[9],p[5]);
  new triangle(p[8],p[5],p[10]);
  new triangle(p[8],p[10],p[7]);
  new triangle(p[7],p[10],p[14]);
  new triangle(p[7],p[14],p[11]);
  new triangle(p[7],p[11],p[6]);
  new triangle(p[6],p[11],p[4]);
  new triangle(p[6],p[4],p[12]);
  new triangle(p[6],p[12],p[13]);
  new triangle(p[6],p[13],p[8]);
  new triangle(p[9],p[13],p[8]);
  new triangle(p[9],p[5],p[3]);
  new triangle(p[9],p[3],p[12]);
  new triangle(p[3],p[12],p[4]);
  new triangle(p[4],p[11],p[5]);
  new triangle(p[5],p[10],p[11]);
*/  
/***********<<*****Disco 2 buchi 1 manic0***********************/

/******************Disco 2 buchi 2 manic1******>>>**************

  new triangle(p[3],p[4],p[7]);
  new triangle(p[3],p[7],p[13]);
  new triangle(p[3],p[13],p[14]);
  new triangle(p[3],p[14],p[12]);
  new triangle(p[3],p[12],p[0]);
  new triangle(p[3],p[0],p[9]);
  new triangle(p[3],p[9],p[11]);
  new triangle(p[3],p[11],p[10]);
  new triangle(p[3],p[10],p[3]);
  new triangle(p[3],p[3],p[5]);
  new triangle(p[5],p[3],p[1]);
  new triangle(p[5],p[1],p[12]);
  new triangle(p[5],p[12],p[14]);
  new triangle(p[5],p[14],p[4]);
  new triangle(p[4],p[14],p[13]);
  new triangle(p[4],p[13],p[1]);
  new triangle(p[4],p[1],p[11]);
  new triangle(p[4],p[11],p[9]);
  new triangle(p[4],p[9],p[8]);
  new triangle(p[4],p[8],p[7]);
  new triangle(p[1],p[12],p[13]);
  new triangle(p[1],p[11],p[10]);
  new triangle(p[1],p[10],p[9]);
  new triangle(p[1],p[9],p[8]);
  new triangle(p[1],p[8],p[6]);
  new triangle(p[1],p[6],p[0]);
  new triangle(p[0],p[6],p[13]);
  new triangle(p[0],p[13],p[12]);
  new triangle(p[13],p[6],p[7]);
  new triangle(p[0],p[9],p[10]);
  new triangle(p[0],p[10],p[3]);
  
***********<<*****Disco 2 buchi 2 manici***********************/

/******************Disco 2 buchi 1 manico****>>>****************

  new triangle(p[0],p[11],p[2]);
  new triangle(p[10],p[11],p[0]);
  new triangle(p[0],p[10],p[7]);
  new triangle(p[1],p[7],p[0]);
  new triangle(p[2],p[11],p[9]);
  new triangle(p[2],p[9],p[3]);
  new triangle(p[2],p[3],p[5]);
  new triangle(p[2],p[5],p[1]);
  new triangle(p[1],p[5],p[11]);
  new triangle(p[1],p[11],p[10]);
  new triangle(p[1],p[10],p[8]);
  new triangle(p[1],p[8],p[7]);
  new triangle(p[4],p[5],p[11]);
  new triangle(p[4],p[11],p[9]);
  new triangle(p[4],p[9],p[8]);
  new triangle(p[4],p[8],p[6]);
  new triangle(p[4],p[6],p[3]);
  new triangle(p[3],p[9],p[6]);
  new triangle(p[8],p[9],p[10]);
  new triangle(p[6],p[7],p[10]);
  new triangle(p[6],p[9],p[10]);

*********<<*******Disco 2 buchi 1 manico***********************/

} 
