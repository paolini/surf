#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cassert>

#include "vertex.h"

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

void surf::add(vertex *v) {
  assert(v->next==0);
  v->next=first_vertex;
  first_vertex=v;
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

void surf::remove(vertex *v) {
  vertex **p;
  for(p=&first_vertex;*p && *p!=v;p=&((*p)->next));
  assert(*p);
  *p=v->next; 
  v->next=0;
  delete v;
};

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

void surf::add(triangle *t) {
  static int max=0;
  assert(t->next==0);
  t->next=first_triangle;
  first_triangle=t;
  ntriangles++;
  if (ntriangles%250==0 && ntriangles>max) {
    max=ntriangles;
    cout<<"("<<ntriangles<<")\n";
  }
}

triangle::triangle(vertex *A,vertex *B,vertex *C) {
  v[0]=A;v[1]=B;v[2]=C;
  v[0]->add_triangle(this);
  v[1]->add_triangle(this);
  v[2]->add_triangle(this);
  next=0;
}

void surf::remove(triangle *t) {
  triangle **p;
  for (p=&first_triangle;*p && *p!=t;p=&((*p)->next));
  assert(*p);
  *p=t->next;
  delete t;
  ntriangles--;
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

void surf::serialize(ostream &out) const {
  out<<"SURF 1\n";
  
  map<const vertex *,int> vmap;
  map<const triangle *,int> tmap;
  int i;
  
  i=0;
  for (const vertex *p=first_vertex;p;p=p->next) 
    vmap[p]=i++;

  assert(vmap.size()==n_vertices());
  
  i=0;
  for (const triangle *p=first_triangle;p;p=p->next) 
    tmap[p]=i++;
  
  assert(tmap.size()==ntriangles);

  out<<vmap.size()<<" "<<tmap.size()<<"\n";

  // serialize vertices

  out<<"VERTICES\n";
  for (const vertex *p=first_vertex;p;p=p->next) {
    out<<vmap[p]<<" "<<p->x[0]<<" "<<p->x[1]<<" "<<p->x[2]<<"\n";
    out<<p->border<<" "<<vmap[p->next_border]<<"\n";
    out<<p->n_triangle()<<" ";
    for (const triangle_list* q=p->list;q;q=q->next)
      out<<tmap[q->first]<<" ";
    out<<"\n";
  }

  //serialize triangles
  out<<"TRIANGLES\n";
  
  for (const triangle *p=first_triangle;p;p=p->next) {
    out<<tmap[p]<<" "
       <<vmap[p->v[0]]<<" "<<vmap[p->v[1]]<<" "<<vmap[p->v[2]]<<"\n";
  }
}


surf *unserialize(istream &in, surf *S) {
  if (S==0) {
    S=new surf;
  } else {
    // s andrebbe ripulito
  }
  
  string s;
  
  in>>s;
  if (s!="SURF") throw runtime_error("SURF expected unserializing");

  int version;
  in>>version;
  if (version!=1) throw runtime_error("unknown SURF version");
  
  int nvertices,ntriangles;
  
  in>>nvertices>>ntriangles;
  
  vector<vertex *> vmap(nvertices);
  vector<triangle *> tmap(ntriangles);

  for (int i=0;i<nvertices;++i)
    vmap[i]=new vertex;
  
  for (int i=0;i<ntriangles;++i) 
    tmap[i]=new triangle;

  for (int i=0;i<nvertices;++i) {
    int n;
    cin>>n;
    if (n!=i) throw runtime_error("unserialize error (1843)");
    
    in>> vmap[i]->x[0] >> vmap[i]->x[1] >> vmap[i]->x[2];
    in>> vmap[i]->border>>n; //
    vmap[i]->next_border=vmap[n];
    
    in>>n; //ntriangles
    for (int j=0;j<n;j++) {
      
    }
  }
  
}

int surf::n_vertices() const {
  int i=0;
  for (const vertex *p=first_vertex;p;p=p->next) ++i;
  return i;
};

vertex::vertex(surf &S, const vector3 &v): vector3(v) {
  S.add(this);
};

void surf::geomview_off(ostream &out) const {
  map<vertex *,int> hash;
  out<<"{ OFF\n";
  out<<"# surf "<<VERSION<<" manu-fatto\n";
  out<<n_vertices()<<" "<<n_triangles()<<" "<<n_triangles()*3<<"\n";
  int n=0;
  for (vertex *p=first_vertex;p;p=p->next) {
    out<<p->x[0]<<" "<<p->x[1]<<" "<<p->x[2]<<"\n";
    hash[p]=n++;
  }
  out<<"#\n";
  for (triangle *t=first_triangle;t;t=t->next)
    out<<"3 "<<hash[t->v[0]]<<" "<<hash[t->v[1]]<<" "<<hash[t->v[2]]<<"\n";
  out<<"}\n";
};

