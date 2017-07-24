#include <cmath>

#include "../vector3.h"
#include "../border.h"
#include "../surf.h"


static double depth=0.5;


static vector3 border_function(double t)
{
  return vector3(0,0,0);
}
static vertex* new_border_vertex(surf &S, vertex *v,vertex *w)
{
  vertex *p;
  p=S.new_vertex(0.5*(*v+*w));
  return p;
}


static vertex *p[50][2];
static int nv=0;

static void double_vertex(surf &S,double x,double y)
{
  cout<<"vertex("<<x<<","<<y<<")\n";
  p[nv][0]=S.new_vertex(vector3(x,y,-depth));
  p[nv][1]=S.new_vertex(vector3(x,y,depth));
  nv++;
}

static void lato(surf &S,int a, int b)
{
  cout<<"lato("<<a<<","<<b<<")\n";
  S.new_triangle(p[a][0],p[b][0],p[b][1]);
  S.new_triangle(p[a][0],p[b][1],p[a][1]);
}
static void triangolo(surf &S,int a,int b ,int c)
{
  S.new_triangle(p[a][0],p[b][0],p[c][0]);
  S.new_triangle(p[a][1],p[b][1],p[c][1]);
}
static void quadrato(surf &S,int a, int b, int c, int d)
{
  triangolo(S,a,b,c);
  triangolo(S,a,c,d);
}
static void perimetro(surf &S)
{
  int i;
  for(i=0;i<nv;i++)
    lato(S,i,(i+1)%nv);
}
static double step=0.1;
static double make(surf &S,const char *s)	
{
  double x;
  double theta;
  for(x=0;*s!='\0';s++,x+=6)
    {
      cout<<"Writing: "<<*s<<"\n";
      nv=0;
      switch(*s)
	{
	case 'E':
	  double_vertex(S,x+0,0);
	  double_vertex(S,x+5,0);
	  double_vertex(S,x+5,1);
	  double_vertex(S,x+1,1);
	  double_vertex(S,x+1,2);
	  double_vertex(S,x+3,2);
	  double_vertex(S,x+3,3);
	  double_vertex(S,x+1,3);
	  double_vertex(S,x+1,4);
	  double_vertex(S,x+5,4);
	  double_vertex(S,x+5,5);
	  double_vertex(S,x+0,5);
	  perimetro(S);
	  cout<< "ciao\n";
	  quadrato(S,0,1,2,3);
	  quadrato(S,4,5,6,7);
	  quadrato(S,0,3,8,11);
	  quadrato(S,8,9,10,11);
	  break;
	case 'M':
	  double_vertex(S,x+0,0);
	  double_vertex(S,x+1,0);
	  double_vertex(S,x+1,3);
	  double_vertex(S,x+2.5,2.5);
	  double_vertex(S,x+4,3);
	  double_vertex(S,x+4,0);
	  double_vertex(S,x+5,0);
	  double_vertex(S,x+5,5);
	  double_vertex(S,x+2.5,4);
	  double_vertex(S,x+0,5);
	  perimetro(S);
	  quadrato(S,0,1,2,9);
	  quadrato(S,2,3,8,9);
	  quadrato(S,3,4,7,8);
	  quadrato(S,4,5,6,7);
	  break;
	case 'A':
	  double_vertex(S,x+0,0);
	  double_vertex(S,x+1,0);
	  double_vertex(S,x+1.4,1);
	  double_vertex(S,x+3.6,1);
	  double_vertex(S,x+4,0);
	  double_vertex(S,x+5,0);
	  double_vertex(S,x+3,5);
	  double_vertex(S,x+2,5);
	  perimetro(S);
	  double_vertex(S,x+1.8,2);
	  double_vertex(S,x+3.2,2);
	  double_vertex(S,x+2.5,5.0-1.25);
	  lato(S,8,9);
	  lato(S,9,10);
	  lato(S,10,8);
	  quadrato(S,0,1,10,7);
	  quadrato(S,2,3,9,8);
	  quadrato(S,4,5,6,10);
	  triangolo(S,10,6,7);
	  break;
	case 'N':
	  double_vertex(S,x+0,0);
	  double_vertex(S,x+1,0);
	  double_vertex(S,x+1,3.5);
	  double_vertex(S,x+4,0);
	  double_vertex(S,x+5,0);
	  double_vertex(S,x+5,5);
	  double_vertex(S,x+4,5);
	  double_vertex(S,x+4,1.5);
	  double_vertex(S,x+1,5);
	  double_vertex(S,x+0,5);
	  perimetro(S);
	  quadrato(S,0,1,8,9);
	  quadrato(S,2,3,7,8);
	  quadrato(S,3,4,5,6);
	  break;
	case 'U':
	  double_vertex(S,x+1,2.5);
	  double_vertex(S,x+1,5);
	  double_vertex(S,x+0,5);
	  double_vertex(S,x+0,2.5);
	  double_vertex(S,x+4,2.5);
	  double_vertex(S,x+4,5);
	  double_vertex(S,x+5,5);
	  double_vertex(S,x+5,2.5);
	  quadrato(S,0,1,2,3);
	  quadrato(S,4,5,6,7);
	  lato(S,0,1);lato(S,1,2);lato(S,2,3);
	  lato(S,4,5);lato(S,5,6);lato(S,6,7);
	  for (theta=M_PI+step;theta<2.0*M_PI;theta+=step)
	    {
	      double_vertex(S,x+2.5+2.5*cos(theta),2.5+2.5*sin(theta));
	      double_vertex(S,x+2.5+1.5*cos(theta),2.5+1.5*sin(theta));
	      lato(S,3,8);lato(S,9,0);
	      quadrato(S,0,3,8,9);
	      p[3][0]=p[8][0];p[3][1]=p[8][1];
	      p[0][0]=p[9][0];p[0][1]=p[9][1];
	      nv-=2;
	    }
	  lato(S,0,4);lato(S,3,7);
	  quadrato(S,0,4,7,3);
	  break;
	case 'L':
	  double_vertex(S,x+0,0);
	  double_vertex(S,x+5,0);
	  double_vertex(S,x+5,1);
	  double_vertex(S,x+1,1);
	  double_vertex(S,x+1,5);
	  double_vertex(S,x+0,5);

	  perimetro(S);
	  quadrato(S,0,1,2,3);
	  quadrato(S,3,4,5,0);
	  break;
	}
    }
  return x;
}


static double l;
static double R=1.0,r=0.4,h=0.1;
static vector3 transform(vector3 v)
{
  double a,b;
  v.x[0]*=2.0*M_PI/l;
  v.x[1]=(v.x[1]-2.5)/5.0;
  a=r*v.x[1]*cos(v.x[0]/2.0)+h*v.x[2]*sin(v.x[0]/2.0);
  b=h*v.x[2]*cos(v.x[0]/2.0)-r*v.x[1]*sin(v.x[0]/2.0);
  return vector3((R+a)*cos(v.x[0]),(R+a)*sin(v.x[0]),b);
}
  
static void init_border(surf &S)
{
  double radius;
  cout<<"radius=";
  cin>>radius;
  cout<<"step=";
  cin>>step;
  cout<<"r=";
  cin>>r;
  cout<<"h=";
  cin>>h;
  l=make(S,"EMANUEL");
  S.side_triangulate(radius);
  S.transform_vertices(transform);
}


static bool init() {
  Border::registry["manu"] = new Border(border_function, new_border_vertex, init_border);
  return true;
}

static bool initializer = init();
