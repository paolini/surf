#include <math.h>

#include "vector.h"
#include "vertex.h"


double r=1.0;

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
vertex *mynew(double t, vertex *previous=NULL)
{
  static vertex *prev=NULL;
  vertex *p;
  if (t<0) prev=previous;
  else
    {
      p=new vertex(border_function(t));
      p->border=t;
      if (previous!=NULL)
	previous->next_border=p;
      else if (prev!=NULL)
	prev->next_border=p;
      prev=p;
    }
}
void quadr(vertex *a,vertex *b,vertex *c,vertex *d)
{
  new triangle(a,b,c);
  new triangle(a,c,d);
}

double h=0.3;



vertex *init_border(int n,vertex *first,double acca,double erre)
{
  double x1,x2;
  piece* p[12];
  vertex* next;
  vertex *last;
  vertex* v[12];
  int i;
  h=acca;
  r=erre;
  x1=first->x[0]+r;
  x2=x1+2.0*r;
  v[0]=first;
  p[0]=new xz_arc(vector(x1,0,0),1,1);
  p[1]=new xy_arc(vector(x2,0,0),1,1);
  next=new vertex(vector(x2+r+r,0,0));
  p[2]=new segment(p[1]->point(1.0),*next);
  next->border=p[2]->real_t+1.0;
  if (n>1)
    v[3]=init_border(n-1,next,acca/2.0,erre/2.0);
  else
    {
      v[3]=new vertex(vector(x2+r+r,h,0));
      new segment(*next,*v[3]);
      next->next_border=v[3];
    }
  h=acca;
  r=erre;
  p[3]=new segment(*(v[3]),vector(x2+r,h,0.0));
  p[4]=new xy_arc(vector(x2,h,0),0,1);
  p[5]=new xz_arc(vector(x1,h,0),0,0);
  p[6]=new segment(p[5]->point(1.0),vector(x1-r,h,h));
  p[7]=new xz_arc(vector(x1,h,h),1,0);
  p[8]=new xy_arc(vector(x2,h,h),1,0);
  p[9]=new segment(vector(x2+r,h,h),vector(x2+r,0.0,h));
  p[10]=new xy_arc(vector(x2,0,h),0,0);
  p[11]=new xz_arc(vector(x1,0,h),0,1);
  for (i=0;i<12;i++)
    {
      if (i!=3 && i!=0)
	{
	  v[i]=new vertex(p[i]->point(0.0));
	}
      v[i]->border=p[i]->real_t;
      if (i!=0)
	v[i-1]->next_border=v[i];
    }
  v[2]->next_border=next;
  last=new vertex(p[11]->point(1.0));
  last->border=p[11]->real_t+1.0;
  v[11]->next_border=last;
  quadr(v[0],v[6],v[5],v[1]);
  quadr(v[1],v[5],v[8],v[11]);
  quadr(v[11],v[8],v[7],last);
  quadr(v[0],v[6],v[7],last);
  quadr(v[1],v[2],v[10],v[11]);
  quadr(v[2],v[4],v[9],v[10]);
  quadr(v[4],v[9],v[8],v[5]);
  quadr(v[2],next,v[3],v[4]);
  return last;
}

void init_border(void)
{
  int n;
  vertex* first,*last;
  first=new vertex(vector(0,0,0));
  first->border=0.0;
  cout<<"Quanti anelli vuoi?";
  cin>>n;
  last=init_border(n,first,0.9,1.0);
  last->next_border=first;
  new segment(*last,*first);
  cout<<num_pieces<<" archi...\n";
}

