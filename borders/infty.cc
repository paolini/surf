#include <cmath>

#include "../vector3.h"
#include "../border.h"
#include "../piece.h"

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
/*
static vertex *mynew(surf &S,double t, vertex *previous=NULL)
{
  static vertex *prev=NULL;
  vertex *p;
  if (t<0) prev=previous;
  else
    {
      p=S.new_vertex(border_function(t));
      p->border=t;
      if (previous!=NULL)
	previous->next_border=p;
      else if (prev!=NULL)
	prev->next_border=p;
      prev=p;
    }
}
*/
static void quadr(surf &S,vertex *a,vertex *b,vertex *c,vertex *d)
{
  S.new_triangle(a,b,c);
  S.new_triangle(a,c,d);
}

static double h=0.3;



static vertex *init_border(surf &S,int n,vertex *first,double acca,double erre)
{
  double x1,x2;
  piece* p[12];
  vertex* next;
  vertex *last;
  vertex* v[12];
  int i;
  double &r = piece::r;
  h=acca;
  piece::r=erre;
  x1=first->x[0]+piece::r;
  x2=x1+2.0*piece::r;
  v[0]=first;
  p[0]=new xz_arc(vector3(x1,0,0),1,1);
  p[1]=new xy_arc(vector3(x2,0,0),1,1);
  next=S.new_vertex(vector3(x2+piece::r+piece::r,0,0));
  p[2]=new segment(p[1]->point(1.0),*next);
  next->border=p[2]->real_t+1.0;
  if (n>1)
    v[3]=init_border(S,n-1,next,acca/2.0,erre/2.0);
  else
    {
      v[3]=S.new_vertex(vector3(x2+r+r,h,0));
      new segment(*next,*v[3]);
      next->next_border=v[3];
    }
  h=acca;
  r=erre;
  p[3]=new segment(*(v[3]),vector3(x2+r,h,0.0));
  p[4]=new xy_arc(vector3(x2,h,0),0,1);
  p[5]=new xz_arc(vector3(x1,h,0),0,0);
  p[6]=new segment(p[5]->point(1.0),vector3(x1-r,h,h));
  p[7]=new xz_arc(vector3(x1,h,h),1,0);
  p[8]=new xy_arc(vector3(x2,h,h),1,0);
  p[9]=new segment(vector3(x2+r,h,h),vector3(x2+r,0.0,h));
  p[10]=new xy_arc(vector3(x2,0,h),0,0);
  p[11]=new xz_arc(vector3(x1,0,h),0,1);
  for (i=0;i<12;i++)
    {
      if (i!=3 && i!=0)
	{
	  v[i]=S.new_vertex(p[i]->point(0.0));
	}
      v[i]->border=p[i]->real_t;
      if (i!=0)
	v[i-1]->next_border=v[i];
    }
  v[2]->next_border=next;
  last=S.new_vertex(p[11]->point(1.0));
  last->border=p[11]->real_t+1.0;
  v[11]->next_border=last;
  quadr(S,v[0],v[6],v[5],v[1]);
  quadr(S,v[1],v[5],v[8],v[11]);
  quadr(S,v[11],v[8],v[7],last);
  quadr(S,v[0],v[6],v[7],last);
  quadr(S,v[1],v[2],v[10],v[11]);
  quadr(S,v[2],v[4],v[9],v[10]);
  quadr(S,v[4],v[9],v[8],v[5]);
  quadr(S,v[2],next,v[3],v[4]);
  return last;
}

static void init_border(surf &S)
{
  int n;
  vertex* first,*last;
  first=S.new_vertex(vector3(0,0,0));
  first->border=0.0;
  cout<<"Quanti anelli vuoi?";
  cin>>n;
  last=init_border(S,n,first,0.9,1.0);
  last->next_border=first;
  new segment(*last,*first);
  cout<<piece::num_pieces<<" archi...\n";
}


static bool init() {
  Border::registry["infty"] = new Border(border_function, new_border_vertex, init_border);
  return true;
}

static bool initializer = init();
