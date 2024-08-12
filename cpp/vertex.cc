#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cassert>

#include "surf.h"

triangle_list::triangle_list(triangle *t,triangle_list *l) {
  first=t;
  next=l;
}

void triangle_list::delete_next(void)
{
  if (next!=NULL)
    {
    next->delete_next();
    delete next;
    }
}

void vertex::delete_triangle(triangle*t)
{
triangle_list** p;
triangle_list* q;
for (p=&list;(*p)!=NULL && (*p)->first!=t;p=&((*p)->next));
if (*p==NULL)
  {
  cerr<<"errore interno #4321"<<"\n";
  exit(1);
  }
q=(*p)->next;
delete *p;
*p=q;
}

int vertex::n_triangle(void) const {
  triangle_list *p;
  int i;
  for (i=0,p=list;p!=NULL;p=p->next,i++);
  return i;
}

vertex::vertex(void) {
  init();
}

void vertex::init(void) {
  list=NULL;
  border=-1;
  next_border=NULL;
  next=0;
}

vertex::vertex(double X,double Y,double Z)
{
  init();
  x[0]=X;x[1]=Y;x[2]=Z;
}

vertex::vertex(vector3 v)
{
  init();
  x[0]=v.x[0];
  x[1]=v.x[1];
  x[2]=v.x[2];
}

vertex::~vertex(void) {
  if (list!=NULL)
    {
      cerr<<"errore interno #4382\n";
      exit(1);
    }
}

void vertex::add_triangle(triangle *t)
{
  list=new triangle_list(t,list);
}

int triangle::check(void)
{
  triangle_list *t;
  int i,n,status=1;
  for (i=0;i<3;i++)
    {
      n=0;
      for (t=v[i]->list;t!=NULL;t=t->next)
	{
	  if (t->first==this) n++;
	}
      if (n!=1)
	{
	  cerr<<"Triangle occur "<<n<<" times in his vertex list\n";
	  status=0;
	}
    }
  return status;
}

vector3 triangle::normal(void)
{
  return ((*v[1]-*v[0])^(*v[2]-*v[0]));
}
vector3 triangle::bari(void)
{
  return 1.0/3.0*(*(v[0])+*(v[1])+*(v[2]));
}

triangle::triangle(vertex *A,vertex *B,vertex *C) {
  v[0]=A;v[1]=B;v[2]=C;
  v[0]->add_triangle(this);
  v[1]->add_triangle(this);
  v[2]->add_triangle(this);
  next=0;
}

triangle::~triangle(void) {
  v[0]->delete_triangle(this);
  v[1]->delete_triangle(this);
  v[2]->delete_triangle(this);
}

double triangle::area(void)
{
  return abs((*v[2]-*v[0])^(*v[1]-*v[0]))/2.0;

  //  return sqrt(sqr(*v[1]-*v[2])*sqr(*v[0]-*v[2])
  //      -sqr((*v[1]-*v[2])*(*v[0]-*v[2])))/2.0;
}
double triangle::side_length(int side)
{
  return abs(v[(side+1)%3]-v[(side+2)%3]);
}
double vertex::area(void)
{
  double a=0.0;
  triangle_list *t;
  for (t=list;t!=NULL;t=t->next)
      a+=t->first->area();
  return a;
}
vector3 vertex::grad(void)
{
  vector3 w(0.0,0.0,0.0);
  triangle_list *t;
  vector3 p1,p2;
  double a;
  for (t=list;t!=NULL;t=t->next)
    {
      if (t->first->v[0]==this)
	{
	  p1=*(t->first->v[1])-*this;
	  p2=*(t->first->v[2])-*this;
	}
      else if (t->first->v[1]==this)
	{
	  p1=*(t->first->v[2])-*this;
	  p2=*(t->first->v[0])-*this;
	}
      else
	{
	  p1=*(t->first->v[0])-*this;
	  p2=*(t->first->v[1])-*this;
	}
      if ((a=t->first->area())>0.0)
	w+=(1.0/(2.0*a))*((p1*p2)*(p1+p2)-sqr(p1)*p2-sqr(p2)*p1);
    }
  return w;
}
void vertex::evolve(double viscosity)
{
  if (next_border==NULL)
    {
      (*this)+=-viscosity*grad();
    }
}
void vertex::harmonic(void)
{
  int i,n;
  triangle_list *t;
  vector3 res(0,0,0);
  if (next_border==NULL)
    {
      for (n=0,t=list;t!=NULL;t=t->next,n++)
	{
	  for (i=0;i<3;i++)
	    res+=*(t->first->v[i]);
	}
      if (n!=0)
	{
	  res-=(double)n*(*this);
	  res/=(double)(2.0*n);
	  (vector3&)(*this)=res;
	}
      else
	cerr<<"Vertex without triangles\n";
    }
}

vertex::vertex(surf &S, const vector3 &v): vector3(v) {
  S.add(this);
};

