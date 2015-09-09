#include <math.h>

#include "vector.h"
#include "vertex.h"

double r=1.0;
double h=0.4;
/****************************/
/* definizione del contorno */
/****************************/

class piece
{
public:
  virtual vector point(double t)=0;
  double real_t;
  piece(void);
};
#define MAXBOR 100
piece *border[MAXBOR];
int num_pieces=0;


class arc:public piece
{
public:
  vector center;
  double radius;
  double alpha;
  double beta;
  int versus; /*antiorario?*/

  virtual vector point(double)=0;
  double angle(double t);

  arc(vector,int,int);
};
class xy_arc:public arc
{
public:
  virtual vector point(double t);
  xy_arc(vector v,int f,int t):arc(v,f,t){};
};
class xz_arc:public arc
{
public:
  virtual vector point(double t);
  xz_arc(vector v,int f,int t):arc(v,f,t){};
};
class yz_arc:public arc
{
public:
  virtual vector point(double t);
  yz_arc(vector v,int f,int t):arc(v,f,t){};

};

class segment:public piece
{
public:
  vector from;
  vector to;
  
  virtual vector point(double t);
  segment(vector f,vector t);
  segment(piece *f,piece *t);
};

class curve:public piece
{
public:
  vector pos;
  double t0,t1;
  vector (*fun)(double t);
  
  virtual vector point(double t);
  curve(vector (*fun)(double t),vector start,double t0=0,double t1=1);
};

curve::curve(vector (*f)(double t),vector start,double t00=0,double t10=1):piece()
{
  t0=t00;t1=t10;
  fun=f;
  pos=start-fun(t0);
}

vector curve::point(double t)
{
  t=t*(t1-t0);
  t+=t0;
  return pos+fun(t);
}


piece::piece(void)
{
  real_t=(double)num_pieces;
  border[num_pieces++]=this;
  if (num_pieces>=MAXBOR)
    cout<<"too many arcs...\n",exit(1);
}

segment::segment(piece *f,piece *t)
{ 
  from=f->point(1);
  to=t->point(0);
}
segment::segment(vector f,vector t)
{
from=f;
to=t;
}

arc::arc(vector c,int from,int ver)
{
  center=c;
  radius=r;
  versus=ver;
  if (from)
    {
      alpha=M_PI;
      beta=2*M_PI;
    }
  else
    {
      alpha=0.0;
      beta=M_PI;
    }
}


double arc::angle(double t)
{
  return alpha+t*fabs(beta-alpha)*(versus?1.0:-1.0);
}

vector segment::point(double t)
{
  return (1.0-t)*from+t*to;
}

vector xy_arc::point(double t)
{
  return center+radius*vector(cos(angle(t)),sin(angle(t)),0.0);
}
vector xz_arc::point(double t)
{
  return center+radius*vector(cos(angle(t)),0.0,sin(angle(t)));
}
vector yz_arc::point(double t)
{
  return center+radius*vector(0,cos(angle(t)),sin(angle(t)));
}


vector border_function(double t)
{
  int n;
  n=int(t);
  return border[n]->point(t-(double)n);
}	

vertex* new_border_vertex(vertex *v,vertex *w)
{
  vertex *p;
  double d;
  if (v->next_border==w || w->next_border==v)
    {
      d=(v->border+w->border)/2.0;
      if (fabs(v->border-w->border)<((double)num_pieces)/2.0)
	{
	  p=new vertex(border_function(d));
	  p->border=d;
	}	
      else 
	{
	  d+=((double)num_pieces)/2.0;
	  while(d>(double)num_pieces) d-=(double)num_pieces;
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


void quadr(vertex *a,vertex *b,vertex *c,vertex *d)
{
  new triangle(a,b,c);
  new triangle(a,c,d);
}


vector moebius(double t)
{
return vector((r-h*cos(t/2.0))*cos(t),(r-h*cos(t/2.0))*sin(t),h*(sin(t/2.0)));
}
vector triplo(double t)
{
return vector((r-h*cos(t/3))*cos(t),(r-h*cos(t/3))*sin(t),h*sin(t/3));
}

void init_border(void)
{
  int i;
  vertex *p[7],*q[10],*s[3];
  piece *b[4];
  cout<<"Superficie che si retrae sul bordo\n";
  b[0]=new curve(moebius,vector(-r/2.0,0,0),h/3-2.0*M_PI,2*M_PI-h/3);
  b[1]=new segment(b[0]->point(1.0),vector(r/2,0,0));
  b[2]=new curve(triplo,b[1]->point(1.0),-3.0*M_PI+h/3,3.0*M_PI-h/3);
  b[3]=new segment(b[2]->point(1.0),b[0]->point(0.0));
  
  for (i=0;i<6;i++)
    {
      p[i]=new vertex (b[0]->point(1.0/6.0*i));
      p[i]->border=1.0/6.0*i;
      if (i>0)
	p[i-1]->next_border=p[i];
    }
  p[6]=new vertex (b[0]->point(1.0));
  p[6]->border=1.0;
  p[5]->next_border=p[6];
  for (i=0;i<3;i++)
    quadr(p[i],p[i+1],p[i+4],p[i+3]);
  new triangle(p[0],p[3],p[6]);

  for (i=0;i<3;i++)
    s[i]=new vertex(vector(0,0,0));
  for (i=0;i<9;i++)
    {
      q[i]=new vertex(b[2]->point(1.0/9.0*i));
      q[i]->border=2.0+1.0/9.0*i;
      if (i>0)
	q[i-1]->next_border=q[i];
    }
  q[9]=new vertex (b[2]->point(1.0));
  q[9]->border=3.0;
  q[9]->next_border=p[0];
  q[8]->next_border=q[9];
  p[6]->next_border=q[0];
  
  for (i=0;i<9;i++)
    {
      quadr(q[i],q[i+1],s[(i+1)%3],s[i%3]);
    }
  new triangle(q[0],q[9],s[0]);
  quadr(p[0],p[6],q[0],q[9]);
}

