#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <signal.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string>
#include <cassert>

using namespace std;

#include "vector3.h"
#include "vertex.h"
#include "render.h"
#include "border.h"

#ifndef VERSION
#error La versione va` definita nel Makefile
#endif

#define MYNAME "surf"

int  print_mode=2;

double viscosity=(0.125);
double epsilon=0.00001;

int interrupt=0;

extern double linewidth;

void out_of_memory(void) {
cerr<<"Memoria esaurita... quitting\n";
exit(1);
}

void check(surf &S)
{
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

int n_triangle(surf &S) {
 int i;
 triangle *p;
 for (i=0,p=S.first_triangle;p!=NULL;i++,p=p->next);
 assert(S.ntriangles==i);
 return i;
 }

ostream& operator<<(ostream& c,triangle &t)
{
return c<<"{"<<*t.v[0]<<","<<*t.v[1]<<","<<*t.v[2]<<"}";
}

double area(surf &S)
{
  triangle *p;
  double a=0;
  for (p=S.first_triangle;p!=NULL;p=p->next)
    a+=p->area();
  return a;
}

vertex* new_border_vertex(surf &S, vertex *v,vertex *w);

int side_triangulate(surf &S, vertex *v, vertex *w) {
  triangle_list *p,*q;
  static triangle *found[10];
  int nfound=0;
  int n,i;  
  vertex *x,*z;
  
  x=new_border_vertex(S,v,w);
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

int side_triangulate(surf &S,triangle *t,double radius)
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
	    side_triangulate(S,t->v[0],t->v[1]);
	    return 1;
	    }
	}
      else if (l0>radius)
	{
	side_triangulate(S,t->v[1],t->v[2]);
	return 1;
	}
    }
  else
    {
      if (l1>l0)
	{
	  if(l1>radius)
	    {
	      side_triangulate(S,t->v[0],t->v[2]);
	      return 1;
	    }
	}
      else if (l0>radius)
	{
	side_triangulate(S,t->v[1],t->v[2]);
	return 1;
	}
    } 
  if (l0>radius || l1>radius || l2>radius)
    cerr<<"Errore interno #2719\n";
  return 0;
}

int border_side_triangulate(surf &S, triangle *t,double radius) {
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
	      side_triangulate(S,t->v[0],t->v[1]);
	      return 1;
	    }
	}
      else if (l0>radius && (t->v[1]->next_border==t->v[2] || t->v[2]->next_border==t->v[1]))
	{
	side_triangulate(S,t->v[1],t->v[2]);
	return 1;
	}
    }
  else
    {
      if (l1>l0 && (t->v[0]->next_border==t->v[2] || t->v[2]->next_border==t->v[0]))
	{
	  if(l0>radius)
	    {
	      side_triangulate(S,t->v[0],t->v[2]);
	      return 1;
	    }
	}
      else if (l1>radius && (t->v[1]->next_border==t->v[2] || t->v[2]->next_border==t->v[1]))
	{
	side_triangulate(S,t->v[1],t->v[2]);
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

double diameter(surf &S) {
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

void side_triangulate(surf &S, double radius,int border=0)
{
  vertex *p;
  triangle_list *t;
  double a,b,c,r;
  int done;
  for (r=max(radius,evolve_factor*diameter(S))
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
		if (border?border_side_triangulate(S,t->first,r)
		    :side_triangulate(S,t->first,r))
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

triangle* center_triangulate(surf &S, triangle *t) {
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
int n_vertex(surf &S)
 {
 int i;
 vertex *p;
 for (i=0,p=S.first_vertex;p!=NULL;i++,p=p->next);
 return i;
 }
void center_triangulate(surf &S) {
 triangle *t;
 for (t=S.first_triangle;t!=NULL;t=center_triangulate(S,t));
 }

void harmonic(surf &S,double eps=-1.0)
{
  double a;
  vertex *p;
  static double epsilon=0.00001;
  if (eps>=0) epsilon=eps;
  cout<<"Harmonic... epsilon="<<epsilon<<"\n";
  do
    {
      a=area(S);
      for (p=S.first_vertex;p!=NULL;p=p->next)
	{
	  p->harmonic();
	}
    }while (fabs(a-area(S))>epsilon && !interrupt);
  cout<<" ...done\n";
}
double evolve(surf &S,double eps=-1.0,double v=-1)
{
  vertex *p;
  double a,b;
  static double viscosity=0.125,epsilon=0.000001;
  if (v>0) viscosity=v;
  else v=viscosity;
  if (eps>=0) epsilon=eps;
  cout<<"Evolving... epsilon="<<epsilon<<"visco="<<viscosity<<" ";
  b=area(S);
  do
    {
      a=b;
      for (p=S.first_vertex;p!=NULL;p=p->next)
	p->evolve(v);
      b=area(S);
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
void evolve_triangulate(surf &S, double err,double radius=-1)
{
  double a,b;
  double r;
  if (radius==-1)
    r=radius=diameter(S);
  else
    r=diameter(S);
  b=area(S);
  do
    {
      r=r*evolve_factor;
      if (r<radius) r=radius;
      a=b;
      side_triangulate(S,r);
      b=evolve(S);      
      cout<<"TOTAL ERROR: "<<a-b<<"\n\n";
    }while ((fabs(b-a)>err || diameter(S)>radius) && !interrupt);
}
void vertex_list(surf &S)
{
  vertex *p;
  for (p=S.first_vertex;p!=NULL;p=p->next)
    {
      cout<<*p<<" border="<<p->border;
      if (p->next_border!=NULL)
	cout<<", next="<<*(p->next_border);
      cout<<"\n";
    }
}
void triangle_list(surf &S)
{
triangle *p;
for (p=S.first_triangle;p!=NULL;p=p->next)
  {
    cout<<*p<<" (area="<<p->area()<<")\n";
  }
}

/*********Gestione della grafica**************/


/***************** End *****************/
void auto_zoom(surf &S,camera &view) 
{
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

vector3 (*dl_border_function)(double);
vertex* (*dl_new_border_vertex)(surf &S, vertex *v,vertex*w);
void (*dl_init_border)(surf &);

Border *selected_border = 0;

void init_border(surf &S) {
  selected_border->init_border(S);
}

vector3 border_function(double x) {
  return selected_border->border_function(x);
}

vertex* new_border_vertex(surf &S,vertex *v, vertex *w) {
  return selected_border->new_border_vertex(S,v,w);
}

/*
int dl_init(char *name) {
  void *dl;
  char *err;
  char filename[256];

  void (*dl_surf_get)(void (**ib)(surf &),
		      vector3 (**bf)(double),
		      vertex * (**nbv)(surf &,vertex*, vertex*));

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
  
  if ( (void *) dl_surf_get=dlsym(dl,"dl_surf_get")) {
    (*dl_surf_get)(&dl_init_border,
		   &dl_border_function,
		   &dl_new_border_vertex);
    return 1;
  } else { // vecchio metodo...
    if (!((void *)dl_border_function=dlsym(dl,"border_function__Fd"))) 
      goto error;
    if (!((void *)dl_new_border_vertex=dlsym(dl,"new_border_vertex__FP6vertexT0"))) 
    goto error;
    if (!((void *)dl_init_border=dlsym(dl,"init_border__Fv"))) goto error;
  
    return 1;
  }
 error:
  cerr<<"dlerror: "<<dlerror()<<"\n";
  cerr<<"opening file: "<<filename<<"\n";
  abort();
}
*/

int main(int argc, char *argv[]) {
  surf S;
  int i;
  double vel=0.5;
  double radius;
  double a,b;
  char c=' ';

  Action.sa_handler=handler;
  sigaction(SIGINT,&Action,NULL);

  if (argc==2) {
    string name = argv[1];
    if (name == "--ask") {
      cout<<"Enter border name:";
      cin>>name;
    }
    if (Border::registry.find(name) != Border::registry.end()) {
      selected_border = Border::registry[name];
    } else {
      cerr<<"invalid border name "<<name<<"\n";
      cerr<<"registered borders:\n";
      for (Border::Registry::const_iterator i=Border::registry.begin();i!=Border::registry.end();++i) {
	cerr<< (i->first) << "\n";
      }
      abort();
    }
  } else {
    cerr<<"no surface given\n";
    abort();
  }

  camera view;
  init_border(S);
  view.adjust();
  view.light(versor(vector3(-3,-2,5)));
  view.pv(vector3(0,-6,3));
  view.look_at(vector3(0,0,0));
  view.up(vector3(0,0,1));
  view.zoom(4.0);
  for (i=0;c!='X';i++)
    {
    if (interrupt)
      {
	int n;
	cout<<"Interrupt: press <X><Enter> to quit or Ctrl-C again.\n";
	cin.clear();
      }
    cout<<"Passo n. "<<i<<"\n";
    cout<<"Triangoli: "<<n_triangle(S)<<", vertici: "<<n_vertex(S);
    cout<<". L'area totale e':"<<area(S)<<", ";
    cout<<"Diametro triangolazione: "<<diameter(S)<<"\n";
    cout<<"Camera: "<<view<<"\n";

    string cmd;
    
    cin>>cmd;
    
    if (!cin) {
      cin.clear();
      cerr<<"Error reading input!\n";
      continue;
    }

    c=cmd[0];

    interrupt=0;

    switch(c)
      {
      case 't':
	cout<<"Enter radius: ";
	cin>>radius;
	cout<<"\n";
	side_triangulate(S,radius);
	break;
      case 'r':
	cout<<"Enter error: ";
	cin>>a;
	evolve_triangulate(S,a);
	break;
      case 'R':
	cout<<"Enter error: ";
	cin>>a;
	cout<<"Enter radius: ";
	cin >>radius;
	evolve_triangulate(S,a,radius);
	break;
      case '?':
	cout<<"Enter border parameter: ";
	cin>>radius;
	cout<<"Vector3 ("<<radius<<") = "<<border_function(radius)<<"\n";
	break;
      case 'b':
	cout<<"Enter radius: ";
	cin>>radius;
	cout<<"\n";
	side_triangulate(S,radius,1);
	break;
      case -1:
	center_triangulate(S);
	break;
      case 'H':
	cout<<"Error: ";
	cin>>a;
	harmonic(S,a);
	break;
      case 'h':
	harmonic(S);
	break;
      case 'E':
	cout<<"Viscosity: ";
	cin>>a;
	cout<<"Error: ";
	cin>>b;
	evolve(S,b,a);
	break;
	/*
	  case 'i':
	  implicit_evolve(1000);
	  break;
	  case 'I':
	  implicit_evolve(0.001);
	  break;
	*/
      case 'e':
	evolve(S);
	break;
      case 'l':
	vertex_list(S);
	break;
      case 'L':
	triangle_list(S);
	break;
      case 'a':
	cout<<"Automatic zoom... \n";
	auto_zoom(S,view);
	break;
      case 'k':
	check(S);
	break;
      case 'c':
	cout<<"Move camera...\n";
	{
	  vector3 v;
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
	  txt.print(S,print_mode);
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
	  vector3 v;
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
	  const char *filename="film.pov";
	  ofstream file (filename,ios::out);
	  pov_ray ray(file,view);
	  cout<<"Pov-ray: "<<ray<<"\n";
	  ray.print(S,print_mode);
	  cout<<"POV written on file "<<filename<<"\n";
	}
      break;
      case 'P':
	{
	  const char *filename="film.ps";
	  ofstream file(filename,ios::out);
	  ps_output ps(file,view,20.0,27.0);
	  cout<<"Postscript: "<<ps<<"\n";
	  ps.print(S,print_mode);
	  cout<<"PS written on file "<<filename<<"\n";
	}
	break;
      case 'G':
	{
	  string filename("film.off");
	  ofstream file(filename.c_str());
	  S.geomview_off(file);
	  cout<<"OFF written on file "<<filename<<"\n";
	}
	break;
      case 'X':
	cout<<"Exiting...\n";
	break;
      default:
	cout<<"comando sconosciuto '"<<cmd<<"'...";
	cout.flush();
	break;
      }
  }
  return 0;
}
