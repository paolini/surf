#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>

#include "vector.h"
#include "vertex.h"
#include "render.h"

#ifndef VERSION
#error La versione va` definita nel Makefile
#endif

#define MYNAME "surf"

int  print_mode=2;

double viscosity=(0.125);
double epsilon=0.00001;

int interrupt=0;

extern double linewidth;

vertex* first_vertex=NULL;
triangle* first_triangle=NULL;

void out_of_memory(void) {
cerr<<"Memoria esaurita... quitting\n";
exit(1);
}

triangle_list::triangle_list(triangle *t,triangle_list *l=NULL)
{
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

static int ntriangles=0;
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

int vertex::n_triangle(void)
{
  triangle_list *p;
  int i;
  for (i=0,p=list;p!=NULL;p=p->next,i++);
  return i;
}
vertex::vertex(void)
{
  init();
}

void vertex::init(void)
{
  list=NULL;
  border=-1;
  next_border=NULL;
  next=first_vertex;
  first_vertex=this;
}
vertex::vertex(double X,double Y,double Z)
{
  init();
  x[0]=X;x[1]=Y;x[2]=Z;
}

vertex::vertex(vector v)
{
  init();
  x[0]=v.x[0];
  x[1]=v.x[1];
  x[2]=v.x[2];
}

vertex::~vertex(void)
{
vertex **p;
 if (list!=NULL)
   {
   cerr<<"errore interno #4382\n";
   exit(1);
   }
 for(p=&first_vertex;*p!=this;p=&((*p)->next));
 *p=this->next; 
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
void check(void)
{
  int status=1;
  triangle *t;
  vertex *p;
  for (t=first_triangle;t!=NULL;t=t->next)
    if (!t->check()) status=0;
  for (p=first_vertex;p!=NULL;p=p->next)
    {
      if (p->list==NULL)
	{
	  cerr<<"Isolated vertex\n";
	  status=0;
	}
    }
  if (status)
    cerr<<"Check OK!\n";
}
vector triangle::normal(void)
{
  return ((*v[1]-*v[0])^(*v[2]-*v[0]));
}
vector triangle::bari(void)
{
  return 1.0/3.0*(*(v[0])+*(v[1])+*(v[2]));
}
int n_triangle(void)
 {
 int i;
 triangle *p;
 for (i=0,p=first_triangle;p!=NULL;i++,p=p->next);
 ntriangles=i;
 return i;
 }
ostream& operator<<(ostream& c,triangle &t)
{
return c<<"{"<<*t.v[0]<<","<<*t.v[1]<<","<<*t.v[2]<<"}";
}

triangle::triangle(vertex *A,vertex *B,vertex *C)
{
  v[0]=A;v[1]=B;v[2]=C;
  v[0]->add_triangle(this);
  v[1]->add_triangle(this);
  v[2]->add_triangle(this);
  next=first_triangle;
  first_triangle=this;
  ntriangles++;
  if (ntriangles%100==0)
    cout<<"("<<ntriangles<<")\n";
}
triangle::~triangle(void)
{
  triangle **p;
  for (p=&first_triangle;*p!=this;p=&((*p)->next));
  *p=this->next;
  v[0]->delete_triangle(this);
  v[1]->delete_triangle(this);
  v[2]->delete_triangle(this);
  ntriangles--;
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
vector vertex::grad(void)
{
  vector w(0.0,0.0,0.0);
  triangle_list *t;
  vector p1,p2;
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
  vector res(0,0,0);
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
	  (vector&)(*this)=res;
	}
      else
	cerr<<"Vertex without triangles\n";
    }
}

double area(void)
{
  triangle *p;
  double a=0;
  for (p=first_triangle;p!=NULL;p=p->next)
    a+=p->area();
  return a;
}

vertex* new_border_vertex(vertex *v,vertex *w);

int side_triangulate(vertex *v, vertex *w)
{
  triangle_list *p,*q;
  static triangle *found[10];
  int nfound=0;
  int n,i;  
  vertex *x,*z;
  
  x=new_border_vertex(v,w);
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
      delete found[n];
      if (new triangle(w,x,z)==NULL || new triangle(x,v,z)==NULL)
	cerr<<"Out of memory";
    }
  return n;
}

int side_triangulate(triangle *t,double radius)
{
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
	    side_triangulate(t->v[0],t->v[1]);
	    return 1;
	    }
	}
      else if (l0>radius)
	{
	side_triangulate(t->v[1],t->v[2]);
	return 1;
	}
    }
  else
    {
      if (l1>l0)
	{
	  if(l1>radius)
	    {
	      side_triangulate(t->v[0],t->v[2]);
	      return 1;
	    }
	}
      else if (l0>radius)
	{
	side_triangulate(t->v[1],t->v[2]);
	return 1;
	}
    } 
  if (l0>radius || l1>radius || l2>radius)
    cerr<<"Errore interno #2719\n";
  return 0;
}
int border_side_triangulate(triangle *t,double radius)
{
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
	      side_triangulate(t->v[0],t->v[1]);
	      return 1;
	    }
	}
      else if (l0>radius && (t->v[1]->next_border==t->v[2] || t->v[2]->next_border==t->v[1]))
	{
	side_triangulate(t->v[1],t->v[2]);
	return 1;
	}
    }
  else
    {
      if (l1>l0 && (t->v[0]->next_border==t->v[2] || t->v[2]->next_border==t->v[0]))
	{
	  if(l0>radius)
	    {
	      side_triangulate(t->v[0],t->v[2]);
	      return 1;
	    }
	}
      else if (l1>radius && (t->v[1]->next_border==t->v[2] || t->v[2]->next_border==t->v[1]))
	{
	side_triangulate(t->v[1],t->v[2]);
	return 1;
	}
    } 
  return 0;
}
#define evolve_factor 0.85
double max(double x,double y)
{
  return x>y?x:y;
}

double diameter(void)
{
  double res=0.0,x;
  int i;
  triangle *t;
  for (t=first_triangle;t!=NULL;t=t->next)
    {
      for (i=0;i<3;i++)
	{
	  if ((x=abs(*(t->v[i])-*(t->v[(i+1)%3])))>res) res=x;
	}
    }
  return res;
}

void side_triangulate(double radius,int border=0)
{
  vertex *p;
  triangle_list *t;
  double a,b,c,r;
  int done;
  for (r=max(radius,evolve_factor*diameter())
	 ;r>=radius && !interrupt
	 ;r=(r==radius)?-1:max(r*evolve_factor,radius))
    {    
      cout<<"radius: "<<r<<"\n";
      do
      {
	done=1;
	for (p=first_vertex;p!=NULL;p=p->next)
	  {
	    for (t=p->list;t!=NULL;)
	      {
		if (border?border_side_triangulate(t->first,r)
		    :side_triangulate(t->first,r))
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
triangle* center_triangulate(triangle *t)
{
  triangle *s;
  vertex *c;
  c=new vertex((1.0/3.0)*(*(t->v)[0]+*(t->v)[1]+*(t->v)[2]));
  new triangle((t->v)[0],(t->v)[1],c);
  new triangle((t->v)[1],(t->v)[2],c);
  new triangle((t->v)[2],(t->v)[0],c);
  s=t->next;
  delete t;
  return s;
}	
int n_vertex(void)
 {
 int i;
 vertex *p;
 for (i=0,p=first_vertex;p!=NULL;i++,p=p->next);
 return i;
 }
void center_triangulate(void)
 {
 triangle *t;
 for (t=first_triangle;t!=NULL;t=center_triangulate(t));
 }
void harmonic(double eps=-1.0)
{
  double a;
  vertex *p;
  static double epsilon=0.00001;
  if (eps>=0) epsilon=eps;
  cout<<"Harmonic... epsilon="<<epsilon<<"\n";
  do
    {
      a=area();
      for (p=first_vertex;p!=NULL;p=p->next)
	{
	  p->harmonic();
	}
    }while (fabs(a-area())>epsilon && !interrupt);
  cout<<" ...done\n";
}
double evolve(double eps=-1.0,double v=-1)
{
  vertex *p;
  double a,b;
  static double viscosity=0.125,epsilon=0.000001;
  if (v>0) viscosity=v;
  else v=viscosity;
  if (eps>=0) epsilon=eps;
  cout<<"Evolving... epsilon="<<epsilon<<"visco="<<viscosity<<" ";
  b=area();
  do
    {
      a=b;
      for (p=first_vertex;p!=NULL;p=p->next)
	p->evolve(v);
      b=area();
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
void evolve_triangulate(double err,double radius=-1)
{
  double a,b;
  double r;
  if (radius==-1)
    r=radius=diameter();
  else
    r=diameter();
  b=area();
  do
    {
      r=r*evolve_factor;
      if (r<radius) r=radius;
      a=b;
      side_triangulate(r);
      b=evolve();      
      cout<<"TOTAL ERROR: "<<a-b<<"\n\n";
    }while ((fabs(b-a)>err || diameter()>radius) && !interrupt);
}
void vertex_list(void)
{
  vertex *p;
  for (p=first_vertex;p!=NULL;p=p->next)
    {
      cout<<*p<<" border="<<p->border;
      if (p->next_border!=NULL)
	cout<<", next="<<*(p->next_border);
      cout<<"\n";
    }
}
void triangle_list(void)
{
triangle *p;
for (p=first_triangle;p!=NULL;p=p->next)
  {
    cout<<*p<<" (area="<<p->area()<<")\n";
  }
}
/********* Superfici in forma implicita **********/

double i_f(vector);
vector i_grad(vector);

vertex *find_vertex(vector p,double err=0)
{
  vertex *v;
  for (v=first_vertex;v!=NULL;v=v->next)
    {
      if (abs(*v-p)<=err)
	return v;
    }
  return new vertex(p);
}

void make_triangolo(vector p1,vector p2,vector p3,double err=0)
{
  vertex *v1;vertex *v2;vertex *v3;
  v1=find_vertex(p1,err);
  v2=find_vertex(p2,err);
  v3=find_vertex(p3,err);
  new triangle(v1,v2,v3);
}
void make_quadrato(vector centro,double lato,int i)
{
  vector x,y;
  x=vector(0,0,0);y=vector(0,0,0);
  x.x[(i+1)%3]=lato/2.0;
  y.x[(i+2)%3]=lato/2.0;
  make_triangolo(centro-x-y,centro+x-y,centro+x+y,lato/10.0);
  make_triangolo(centro+x+y,centro-x+y,centro-x-y,lato/10.0); 
}
void implicit(vector from, vector to,double step,double r=0.0)
{
  double x,y;
  vector p;
  vector dir;
  int i;
  double X,Y,Z;
  
  for (i=0;i<3;i++)
    {
      dir=vector(0,0,0);
      dir.x[i]=step;
      for (p.x[0]=from.x[0];p.x[0]<to.x[0];p.x[0]+=step)
	for (p.x[1]=from.x[1];p.x[1]<to.x[1];p.x[1]+=step)
	  for (p.x[2]=from.x[2];p.x[2]<to.x[2];p.x[2]+=step)
	    {
	      if (r!=0.0 || abs(p-0.5*(to-from))<=r) 
	      x=i_f(p);
	      y=i_f(p+dir);
	      if (x*y<0.0 || (x==0.0 && y!=0.0))
		{
		  make_quadrato(p+0.5*dir,step,i);
		}
	    }
    }
}
vector zero(vector v)
{
  return v-0.01*i_f(v)*i_grad(v);
}
double implicit_error(void)
{
  double err=0;
  vertex *v;
  for (v=first_vertex;v!=NULL;v=v->next)
    {
      if (fabs(i_f(*v))>err) err=fabs(i_f(*v));
    }
  return err;
}
void implicit_evolve(double eps=0.0001)
{
  vertex *v;
  while (implicit_error()>eps && !interrupt)
    {
      cout<<"ERROR: "<<implicit_error()<<"\n";
      for (v=first_vertex;v!=NULL;v=v->next)
	{
	  *(vector *)v=zero(*v);
	}
    }
}

/*********Gestione della grafica**************/


/***************** End *****************/
void auto_zoom(camera &view) 
{
  double x0,x1,y0,y1,x;
  triangle* t;
  int j;
  
  x0=x1=view.X(*(t->v[0]));
  y0=y1=view.Y(*(t->v[0]));
  for (t=first_triangle;t!=NULL;t=t->next)
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


/****************************/
/* definizione del contorno */
/****************************/
// #define IMPLICIT

#ifdef IMPLICIT

vector border_function(double t)
{
  return vector(0,0,0);
}
vertex *new_border_vertex(vertex *v,vertex *w)
{
  return new vertex(0.5*(*v+*w));
}
#else
double i_f(vector v)
{return 0;};
vector i_grad(vector v)
{return vector (0,0,0);};
#endif


// #include "tenda.sup"
// #include "moebius.sup"

// #include "triplo.sup"

// #define DUE_DISCHI

#ifdef IPER
#include "iper.sup"
#endif 

// #include "cubo.sup"

// #include "spine.sup"

// #include "para.sup"

// #include "marco.sup"

// #include "infty.sup"

// #include "trif.sup"

// #include "retrae.sup"

// #include "plateau.sup"

// #include "manu.sup"

#ifdef DEGIORGI
#include "degiorgi.sup"
#endif

#ifdef COMP2
#include "comp2.sup"
#endif
// #include "giaq.sup"

/********** end ************/


struct  sigaction Action;
void handler(int a)
{
  if (interrupt)
    {
      cout<<"Interrupt: exiting...\n";
      exit(0);
    }
  interrupt=1;
}


vector (*dl_border_function)(double);
vertex* (*dl_new_border_vertex)(vertex *v,vertex*w);
void (*dl_init_border)(void);

void init_border() {
  (*dl_init_border)();
}

vector border_function(double x) {
  return (*dl_border_function)(x);
}

vertex* new_border_vertex(vertex *v, vertex *w) {
  return (*dl_new_border_vertex)(v,w);
}

int dl_init(char *name) {
  void *dl;
  char *err;
  char filename[256];

  if (name[0]=='/') {
    strcpy(filename,"");
  } else {
    getcwd(filename,sizeof(filename));
    if (strlen(name)+strlen(filename)+2>= sizeof(filename)) {
      cerr<<"filename too long\n";
      abort();
    }
    strcat(filename,"/");
  }
  strcat(filename,name);
  
  if (!(dl=dlopen(filename,RTLD_NOW))) goto error;
  if (!((void *)dl_border_function=dlsym(dl,"border_function__Fd"))) goto error;
  if (!((void *)dl_new_border_vertex=dlsym(dl,"new_border_vertex__FP6vertexT0"))) 
    goto error;
  if (!((void *)dl_init_border=dlsym(dl,"init_border__Fv"))) goto error;
  
  return 1;
 error:
  cerr<<"dlerror: "<<dlerror()<<"\n";
  cerr<<"opening file: "<<filename<<"\n";
  abort();
}


int main(int argc, char *argv[]) {
  int i;
  double vel=0.5;
  double radius;
  double a,b;
  char c=' ';

  Action.sa_handler=handler;
  sigaction(SIGINT,&Action,NULL);

  if (argc==2)
    dl_init(argv[1]);
  else {
    cerr<<"no surface given\n";
    abort();
  }

  camera view;
  init_border();
  view.adjust();
  view.light(versor(vector(-3,-2,5)));
  view.pv(vector(0,-6,3));
  view.look_at(vector(0,0,0));
  view.up(vector(0,0,1));
  view.zoom(4.0);
  for (i=0;c!='X';i++)
  {
    if (interrupt)
      {
	cout<<"Interrupt: press <X><Enter> to quit or Ctrl-C again.\n";
      }
    cout<<"Passo n. "<<i<<"\n";
    cout<<"Triangoli: "<<n_triangle()<<", vertici: "<<n_vertex();
    cout<<". L'area totale e':"<<area()<<"\n";
    cout<<"Camera: "<<view<<"\n";
    cin>>c;
    interrupt=0;
    switch(c)
      {
      case 't':
	cout<<"Enter radius: ";
	cin>>radius;
	cout<<"\n";
	side_triangulate(radius);
	break;
      case 'r':
	cout<<"Enter error: ";
	cin>>a;
	evolve_triangulate(a);
	break;
      case 'R':
	cout<<"Enter error: ";
	cin>>a;
	cout<<"Enter radius: ";
	cin >>radius;
	evolve_triangulate(a,radius);
	break;
      case '?':
	cout<<"Enter border parameter: ";
	cin>>radius;
	cout<<"Vector ("<<radius<<") = "<<border_function(radius)<<"\n";
	break;
      case 'b':
	cout<<"Enter radius: ";
	cin>>radius;
	cout<<"\n";
	side_triangulate(radius,1);
	break;
      case -1:
	center_triangulate();
	break;
      case 'H':
	cout<<"Error: ";
	cin>>a;
	harmonic(a);
	break;
      case 'h':
	harmonic();
	break;
      case 'E':
	cout<<"Viscosity: ";
	cin>>a;
	cout<<"Error: ";
	cin>>b;
	evolve(b,a);
	break;
      case 'd':
	cout<<"Diameter: "<<diameter()<<"\n";
	break;
      case 'i':
	implicit_evolve(1000);
	break;
      case 'I':
	implicit_evolve(0.001);
	break;
      case 'e':
	evolve();
	break;
      case 'l':
	vertex_list();
	break;
      case 'L':
	triangle_list();
	break;
      case 'a':
	cout<<"Automatic zoom... \n";
	auto_zoom(view);
	break;
      case 'k':
	check();
	break;
      case 'c':
	cout<<"Move camera...\n";
	{
	  vector v;
	  do
	    {
	      cout<<"Camera: "<<view<<"\n";
	      cin>>c;
	      switch(c)
		{
		case 'w':
		  cout<<"linewidth="<<linewidth<<"\n";
		  cout<<"Enter linewidth: ";
		  cin>>linewidth;
		  break;
		case 'p':
		  cout<<"Enter PV: ";
		  cin>>v;
		  view.pv(v);
		  break;
		case 'l':
		  cout<<"Look at: ";
		  cin>>v;
		  view.look_at(v);
		  break;
		case 'a':
		  view.pv(view.pv()+vel*view.direction());
		  break;
		case '+':
		  view.zoom(1.5);
		  break;
		case '-':
		  view.zoom(1.0/1.5);
		  break;
		case 'h':
		  cout<<"Light direction: ";
		  cin>>v;
		  view.light(v);
		  break;
		case 'f':
		  double w,h;
		  cout<<"width: "; cin>>w;
		  cout<<"height: "; cin>>h;
		  view.film(w,h);
		  break;
		}
	    }while (c!='q' && c!='x' && c!='X');
	}
	c='z';
	break;
      case 'p': {
	  text_output txt(cout,view);
	  cout<<"text bitmap: "<<txt<<"\n";
	  txt.print(print_mode);
	}
      break;
      case 'w':
	cout<<"Printing border...\n";
	cout<<"End : ";
	cin>>b;
	cout<<"Step : ";
	cin>>radius;
	{
	  ofstream file("film.ps",ios::out);
	  ps_output ps(file,view,20.0,27.0);
	  vector v;
	  for (a=radius;a<b;a+=radius)
	    {
	      ps.out_line(border_function(a),border_function(a-radius),linewidth*ps.scaling(border_function(a)),color(1,0,0));
	    }
	  cout<<"done!";
	}
	break;
      case 'm':
	{
	  cout<<"Print mode: (0=border, 1=sup, 2=both)";
	  cin>>print_mode;
	}
      break;
      case 'O':
	{
	  char *filename="film.pov";
	  ofstream file (filename,ios::out);
	  pov_ray ray(file,view);
	  cout<<"Pov-ray: "<<ray<<"\n";
	  ray.print(print_mode);
	  cout<<"POV written on file "<<filename<<"\n";
	}
      break;
      case 'P':
	{
	  char *filename="film.ps";
	  ofstream file(filename,ios::out);
	  ps_output ps(file,view,20.0,27.0);
	  cout<<"Postscript: "<<ps<<"\n";
	  ps.print(print_mode);
	  cout<<"PS written on file "<<filename<<"\n";
	}
      break;
      case 'X':
	cout<<"Exiting...\n";
	break;
      default:
	cout<<"comando sconosciuto...";
	cout.flush();
	break;
      }
  }
  return 0;
}
