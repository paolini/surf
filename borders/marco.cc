#include <cmath>

#include "../vector3.h"
#include "../border.h"

/****************************/
/* definizione del contorno */
/****************************/

/* t va da 0 a 6\pi */
static double r=1.0,R=1.5;
static double h=0.7;
static double osc=0.1;


static vector3 border_function(double t)
{	
if (t<2.0*M_PI)
	return vector3(r*cos(t),r*sin(t),h-osc*cos(2.0*t));
else if (t<4.0*M_PI)
	return vector3(R*cos(t),R*sin(t),osc*cos(2.0*t));
else
	return vector3(r*cos(t),r*sin(t),-h-osc*cos(2.0*t));
	
}	
static vertex* new_border_vertex(surf &S, vertex *v,vertex *w)
{
  vertex *p;
  double d;
  if (v->next_border==w || w->next_border==v)
    {
      d=(v->border+w->border)/2.0;
      if (fabs(v->border-w->border)<M_PI)
	{
	  p=S.new_vertex(border_function(d));
	  p->border=d;
	}	
      else 
	{
	  d+=M_PI;
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

static void quadr(surf &S,vertex*a,vertex*b,vertex*c,vertex*d)
{
  S.new_triangle(a,b,c);
  S.new_triangle(a,c,d);
}

static void init_border(surf &S)
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
	p[i][j]=S.new_vertex(border_function(t));
	p[i][j]->border=t;
	if (j>0)
	  p[i][j-1]->next_border=p[i][j];
      }
    p[i][3]->next_border=p[i][0];
    }
  q=S.new_vertex(vector3(0,0,0));
  for (j=0;j<4;j++)
    {
      quadr(S,p[2*((j+1)%2)][j],p[2*((j+1)%2)][(j+1)%4],p[1][(j+1)%4],p[1][j]);
      S.new_triangle(q,p[1][j],p[0][j]);
      S.new_triangle(q,p[1][j],p[2][j]);
      S.new_triangle(q,p[2*(j%2)][j],p[2*(j%2)][(j+1)%4]);
    }
}

static void init_border_bis(surf &S)
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
    p[i]=S.new_vertex(border_function(2.0*M_PI/3.0 * i));
    p[i]->border=2.0*M_PI/3.0 * i;
    }
  for (i=0;i<3;i++)
    {
      p[i]->next_border=p[(i+1)%3];
      p[i+3]->next_border=p[(i+1)%3+3];
      p[i+6]->next_border=p[(i+1)%3+6];
    }
  for (i=9;i<15;i++)
    p[i]=S.new_vertex(vector3(0,0,0));

/******************Disco 2 buchi 1 manico******>>>**************/
/*
  S.new_triangle(p[0],p[1],p[12]);
  S.new_triangle(p[0],p[12],p[9]);
  S.new_triangle(p[0],p[9],p[13]);
  S.new_triangle(p[0],p[13],p[2]);
  S.new_triangle(p[2],p[13],p[10]);
  S.new_triangle(p[2],p[10],p[11]);
  S.new_triangle(p[2],p[11],p[1]);
  S.new_triangle(p[1],p[11],p[14]);
  S.new_triangle(p[1],p[14],p[13]);
  S.new_triangle(p[1],p[13],p[12]);
  S.new_triangle(p[10],p[14],p[13]);
  S.new_triangle(p[8],p[9],p[5]);
  S.new_triangle(p[8],p[5],p[10]);
  S.new_triangle(p[8],p[10],p[7]);
  S.new_triangle(p[7],p[10],p[14]);
  S.new_triangle(p[7],p[14],p[11]);
  S.new_triangle(p[7],p[11],p[6]);
  S.new_triangle(p[6],p[11],p[4]);
  S.new_triangle(p[6],p[4],p[12]);
  S.new_triangle(p[6],p[12],p[13]);
  S.new_triangle(p[6],p[13],p[8]);
  S.new_triangle(p[9],p[13],p[8]);
  S.new_triangle(p[9],p[5],p[3]);
  S.new_triangle(p[9],p[3],p[12]);
  S.new_triangle(p[3],p[12],p[4]);
  S.new_triangle(p[4],p[11],p[5]);
  S.new_triangle(p[5],p[10],p[11]);
*/  
/***********<<*****Disco 2 buchi 1 manic0***********************/

/******************Disco 2 buchi 2 manic1******>>>**************

  S.new_triangle(p[3],p[4],p[7]);
  S.new_triangle(p[3],p[7],p[13]);
  S.new_triangle(p[3],p[13],p[14]);
  S.new_triangle(p[3],p[14],p[12]);
  S.new_triangle(p[3],p[12],p[0]);
  S.new_triangle(p[3],p[0],p[9]);
  S.new_triangle(p[3],p[9],p[11]);
  S.new_triangle(p[3],p[11],p[10]);
  S.new_triangle(p[3],p[10],p[3]);
  S.new_triangle(p[3],p[3],p[5]);
  S.new_triangle(p[5],p[3],p[1]);
  S.new_triangle(p[5],p[1],p[12]);
  S.new_triangle(p[5],p[12],p[14]);
  S.new_triangle(p[5],p[14],p[4]);
  S.new_triangle(p[4],p[14],p[13]);
  S.new_triangle(p[4],p[13],p[1]);
  S.new_triangle(p[4],p[1],p[11]);
  S.new_triangle(p[4],p[11],p[9]);
  S.new_triangle(p[4],p[9],p[8]);
  S.new_triangle(p[4],p[8],p[7]);
  S.new_triangle(p[1],p[12],p[13]);
  S.new_triangle(p[1],p[11],p[10]);
  S.new_triangle(p[1],p[10],p[9]);
  S.new_triangle(p[1],p[9],p[8]);
  S.new_triangle(p[1],p[8],p[6]);
  S.new_triangle(p[1],p[6],p[0]);
  S.new_triangle(p[0],p[6],p[13]);
  S.new_triangle(p[0],p[13],p[12]);
  S.new_triangle(p[13],p[6],p[7]);
  S.new_triangle(p[0],p[9],p[10]);
  S.new_triangle(p[0],p[10],p[3]);
  
***********<<*****Disco 2 buchi 2 manici***********************/

/******************Disco 2 buchi 1 manico****>>>****************

  S.new_triangle(p[0],p[11],p[2]);
  S.new_triangle(p[10],p[11],p[0]);
  S.new_triangle(p[0],p[10],p[7]);
  S.new_triangle(p[1],p[7],p[0]);
  S.new_triangle(p[2],p[11],p[9]);
  S.new_triangle(p[2],p[9],p[3]);
  S.new_triangle(p[2],p[3],p[5]);
  S.new_triangle(p[2],p[5],p[1]);
  S.new_triangle(p[1],p[5],p[11]);
  S.new_triangle(p[1],p[11],p[10]);
  S.new_triangle(p[1],p[10],p[8]);
  S.new_triangle(p[1],p[8],p[7]);
  S.new_triangle(p[4],p[5],p[11]);
  S.new_triangle(p[4],p[11],p[9]);
  S.new_triangle(p[4],p[9],p[8]);
  S.new_triangle(p[4],p[8],p[6]);
  S.new_triangle(p[4],p[6],p[3]);
  S.new_triangle(p[3],p[9],p[6]);
  S.new_triangle(p[8],p[9],p[10]);
  S.new_triangle(p[6],p[7],p[10]);
  S.new_triangle(p[6],p[9],p[10]);

*********<<*******Disco 2 buchi 1 manico***********************/

} 

static bool init() {
  Border::registry["marco"] = new Border(border_function, new_border_vertex, init_border);
  return true;
}

static bool initializer = init();
