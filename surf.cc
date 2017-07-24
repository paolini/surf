#include <map>
#include <cmath>
#include <cstdlib>
#include <string>
#include <cassert>

using namespace std;

#include "vector3.h"
#include "vertex.h"
#include "surf.h"
#include "border.h"

#ifndef VERSION
#error La versione va` definita nel Makefile
#endif

#define MYNAME "surf"

const double surf::evolve_factor = 0.85;

int surf::interrupt=0;

surf::surf(Border *border_): border(border_) {
    first_vertex=0;
    first_triangle=0;
    ntriangles=0;
    border->init_border(*this);
  }

vector3 surf::border_function(double t) {
    return border->border_function(t);
  }

vertex* surf::new_border_vertex(vertex *v, vertex *w) {
  return border->new_border_vertex(*this, v, w);
}


void surf::add(vertex *v) {
  assert(v->next==0);
  v->next=first_vertex;
  first_vertex=v;
}

void surf::remove(vertex *v) {
  vertex **p;
  for(p=&first_vertex;*p && *p!=v;p=&((*p)->next));
  assert(*p);
  *p=v->next; 
  v->next=0;
  delete v;
};

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

void surf::remove(triangle *t) {
  triangle **p;
  for (p=&first_triangle;*p && *p!=t;p=&((*p)->next));
  assert(*p);
  *p=t->next;
  delete t;
  ntriangles--;
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

/*
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
*/

int surf::n_vertices() const {
  int i=0;
  for (const vertex *p=first_vertex;p;p=p->next) ++i;
  return i;
};

void surf::check()
{
  surf &S = *this;
  int status=1;
  triangle *t;
  vertex *p;
  for (t=S.first_triangle;t!=NULL;t=t->next)
    if (!t->check()) status=0;
  for (p=S.first_vertex;p!=NULL;p=p->next) {
      if (p->list==NULL)
	{
	  cerr<<"Isolated vertex\n";
	  status=0;
	}
    }
  if (status)
    cerr<<"Check OK!\n";
}

ostream& operator<<(ostream& c,triangle &t)
{
return c<<"{"<<*t.v[0]<<","<<*t.v[1]<<","<<*t.v[2]<<"}";
}

double surf::area()
{
  surf &S = *this;
  triangle *p;
  double a=0;
  for (p=S.first_triangle;p!=NULL;p=p->next)
    a+=p->area();
  return a;
}

int surf::side_triangulate(vertex *v, vertex *w) {
  surf &S = *this; 
  triangle_list *p,*q;
  static triangle *found[10];
  int nfound=0;
  int n,i;  
  vertex *x,*z;
  
  x=S.new_border_vertex(v,w);
  for (p=v->list;p!=NULL;p=p->next)
    {
      for (q=w->list;q!=NULL;q=q->next)
	{
	  if (q->first==p->first)
	    {
	      if (nfound>=10) cerr<<"errore interno #3852\n";
	      else
		found[nfound++]=q->first;
	    }
	}
    }
  if (nfound==0)
    cerr<<"errore interno #8732";
  for (n=0;n<nfound;n++)
    {
      for (i=0;i<3 && (found[n]->v[i]==v || found[n]->v[i]==w);i++);
      if (i>=3) cerr<<"errore interno #9127\n";
      z=found[n]->v[i];
      S.remove(found[n]);
      S.new_triangle(w,x,z);
      S.new_triangle(x,v,z);
    }
  return n;
}

int surf::side_triangulate(triangle *t,double radius)
{
  surf &S = *this;
  double l0,l1,l2;
  l0=abs(*(t->v[1])-*(t->v[2]));
  l1=abs(*(t->v[0])-*(t->v[2]));
  l2=abs(*(t->v[0])-*(t->v[1]));
  if (l2>l1)
    {
      if (l2>l0)
	{
	  if (l2>radius)
	    {
	    S.side_triangulate(t->v[0],t->v[1]);
	    return 1;
	    }
	}
      else if (l0>radius)
	{
	S.side_triangulate(t->v[1],t->v[2]);
	return 1;
	}
    }
  else
    {
      if (l1>l0)
	{
	  if(l1>radius)
	    {
	      S.side_triangulate(t->v[0],t->v[2]);
	      return 1;
	    }
	}
      else if (l0>radius)
	{
	S.side_triangulate(t->v[1],t->v[2]);
	return 1;
	}
    } 
  if (l0>radius || l1>radius || l2>radius)
    cerr<<"Errore interno #2719\n";
  return 0;
}

int surf::border_side_triangulate(triangle *t,double radius) {
  surf &S = *this;
  double l0,l1,l2;
  l0=abs(*(t->v[1])-*(t->v[2]));
  l1=abs(*(t->v[0])-*(t->v[2]));
  l2=abs(*(t->v[0])-*(t->v[1]));
  if (l2>l1)
    {
      if (l2>l0)
	{
	  if (l2>radius && (t->v[0]->next_border==t->v[1] || t->v[1]->next_border==t->v[0]))
	    {
	      S.side_triangulate(t->v[0],t->v[1]);
	      return 1;
	    }
	}
      else if (l0>radius && (t->v[1]->next_border==t->v[2] || t->v[2]->next_border==t->v[1]))
	{
	S.side_triangulate(t->v[1],t->v[2]);
	return 1;
	}
    }
  else
    {
      if (l1>l0 && (t->v[0]->next_border==t->v[2] || t->v[2]->next_border==t->v[0]))
	{
	  if(l0>radius)
	    {
	      S.side_triangulate(t->v[0],t->v[2]);
	      return 1;
	    }
	}
      else if (l1>radius && (t->v[1]->next_border==t->v[2] || t->v[2]->next_border==t->v[1]))
	{
	S.side_triangulate(t->v[1],t->v[2]);
	return 1;
	}
    } 
  return 0;
}

double max(double x,double y)
{
  return x>y?x:y;
}

double surf::diameter() {
  surf &S = *this;
  double res=0.0,x;
  int i;
  triangle *t;
  for (t=S.first_triangle;t!=NULL;t=t->next)
    {
      for (i=0;i<3;i++)
	{
	  if ((x=abs(*(t->v[i])-*(t->v[(i+1)%3])))>res) res=x;
	}
    }
  return res;
}

void surf::side_triangulate(double radius,int border)
{
  surf &S = *this;
  vertex *p;
  triangle_list *t;
  double a,b,c,r;
  int done;
  for (r=max(radius,evolve_factor*S.diameter())
	 ;r>=radius && !interrupt
	 ;r=(r==radius)?-1:max(r*evolve_factor,radius))
    {    
      cout<<"radius: "<<r<<"\n";
      do
      {
	done=1;
	for (p=S.first_vertex;p!=NULL;p=p->next)
	  {
	    for (t=p->list;t!=NULL;)
	      {
		if (border?S.border_side_triangulate(t->first,r)
		    :S.side_triangulate(t->first,r))
		  {
		    t=NULL;
		    done=0;
		  }
		else
		  t=t->next;
	      }
	  }
      }  while(!(done||interrupt));
    }
if (done)
    cout<<"...done!\n";
}

triangle* surf::center_triangulate(triangle *t) {
  surf &S = *this;
  triangle *s;
  vertex *c;
  c=S.new_vertex((1.0/3.0)*(*(t->v)[0]+*(t->v)[1]+*(t->v)[2]));
  S.new_triangle((t->v)[0],(t->v)[1],c);
  S.new_triangle((t->v)[1],(t->v)[2],c);
  S.new_triangle((t->v)[2],(t->v)[0],c);
  s=t->next;
  S.remove(t);
  return s;
}	

void surf::center_triangulate() {
  surf &S = *this;
  triangle *t;
  for (t=S.first_triangle;t!=NULL;t=S.center_triangulate(t));
 }

void surf::harmonic(double eps)
{
  surf &S = *this;
  double a;
  vertex *p;
  static double epsilon=0.00001;
  if (eps>=0) epsilon=eps;
  cout<<"Harmonic... epsilon="<<epsilon<<"\n";
  do
    {
      a=S.area();
      for (p=S.first_vertex;p!=NULL;p=p->next)
	{
	  p->harmonic();
	}
    }while (fabs(a-S.area())>epsilon && !interrupt);
  cout<<" ...done\n";
}
double surf::evolve(double eps, double v)
{
  surf &S = *this;
  vertex *p;
  double a,b;
  static double viscosity=0.125;
  static double epsilon=0.000001;
  if (v>0) viscosity=v;
  else v=viscosity;
  if (eps>=0) epsilon=eps;
  cout<<"Evolving... epsilon="<<epsilon<<"visco="<<viscosity<<" ";
  b=S.area();
  do
    {
      a=b;
      for (p=S.first_vertex;p!=NULL;p=p->next)
	p->evolve(v);
      b=S.area();
      if (b>a)
	{
	  v/=2;
	  cout<<"visco="<<v<<"  "<<a<<"->"<<b<<" ("<<b-a<<")\n";
	}
    }while ((a-b>=epsilon || a<b) && !interrupt);
  if (!interrupt)
    cout<<"done.\n";
  return b;
}
void surf::evolve_triangulate(double err, double radius)
{
  surf &S = *this;
  double a,b;
  double r;
  if (radius==-1)
    r=radius=S.diameter();
  else
    r=S.diameter();
  b=S.area();
  do
    {
      r=r*evolve_factor;
      if (r<radius) r=radius;
      a=b;
      S.side_triangulate(r);
      b=S.evolve(); 
      cout<<"TOTAL ERROR: "<<a-b<<"\n\n";
    }while ((fabs(b-a)>err || S.diameter()>radius) && !interrupt);
}
void surf::print_vertex_list()
{
  surf &S = *this;
  vertex *p;
  for (p=S.first_vertex;p!=NULL;p=p->next)
    {
      cout<<*p<<" border="<<p->border;
      if (p->next_border!=NULL)
	cout<<", next="<<*(p->next_border);
      cout<<"\n";
    }
}
void surf::print_triangle_list()
{
  surf &S = *this;
  triangle *p;
  for (p=S.first_triangle;p!=NULL;p=p->next)
    {
      cout<<*p<<" (area="<<p->area()<<")\n";
    }
}

void surf::auto_zoom(camera &view) 
{
  surf &S = *this;
  double x0,x1,y0,y1,x;
  triangle* t;
  int j;
  
  x0=x1=view.X(*(t->v[0]));
  y0=y1=view.Y(*(t->v[0]));
  for (t=S.first_triangle;t!=NULL;t=t->next)
    {
      for (j=0;j<3;j++)
	{
	  x=view.X(*(t->v[j]));
	  if (x<x0) x0=x;
	  if (x>x1) x1=x;
	  x=view.Y(*(t->v[j]));
	  if (x<y0) y0=x;
	  if (x>y1) y1=x;
	}
    }
  view.film(2*(x1>-x0?x1:-x0),2*(y1>-y0?y1:-y0),1); // resize film
}

void surf::transform_vertices(transform_function transform) {
  for (vertex *v=first_vertex;v!=NULL;v=v->next)
    {
      *(vector3*)v=transform(*v);
    }
}

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

