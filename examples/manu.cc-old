#include <math.h>

#include "vector.h"
#include "vertex.h"

extern int side_triangulate(vertex *v, vertex *w);
extern void side_triangulate(double radius,int border=0);
extern vertex* first_vertex;
extern triangle* first_triangle;

double depth=0.5;


vector border_function(double t)
{
  return vector(0,0,0);
}
vertex* new_border_vertex(vertex *v,vertex *w)
{
  vertex *p;
  p=new vertex(0.5*(*v+*w));
  return p;
}


vertex *p[50][2];
int nv=0;

int double_vertex(double x,double y)
{
  cout<<"vertex("<<x<<","<<y<<")\n";
  p[nv][0]=new vertex(vector(x,y,-depth));
  p[nv][1]=new vertex(vector(x,y,depth));
  nv++;
}

void lato(int a, int b)
{
  cout<<"lato("<<a<<","<<b<<")\n";
  new triangle(p[a][0],p[b][0],p[b][1]);
  new triangle(p[a][0],p[b][1],p[a][1]);
}
void triangolo(int a,int b ,int c)
{
  new triangle(p[a][0],p[b][0],p[c][0]);
  new triangle(p[a][1],p[b][1],p[c][1]);
}
void quadrato(int a, int b, int c, int d)
{
  triangolo(a,b,c);
  triangolo(a,c,d);
}
void perimetro(void)
{
  int i;
  for(i=0;i<nv;i++)
    lato(i,(i+1)%nv);
}
double step=0.1;
double make(char *s)	
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
	  double_vertex(x+0,0);
	  double_vertex(x+5,0);
	  double_vertex(x+5,1);
	  double_vertex(x+1,1);
	  double_vertex(x+1,2);
	  double_vertex(x+3,2);
	  double_vertex(x+3,3);
	  double_vertex(x+1,3);
	  double_vertex(x+1,4);
	  double_vertex(x+5,4);
	  double_vertex(x+5,5);
	  double_vertex(x+0,5);
	  perimetro();
	  cout<< "ciao\n";
	  quadrato(0,1,2,3);
	  quadrato(4,5,6,7);
	  quadrato(0,3,8,11);
	  quadrato(8,9,10,11);
	  break;
	case 'M':
	  double_vertex(x+0,0);
	  double_vertex(x+1,0);
	  double_vertex(x+1,3);
	  double_vertex(x+2.5,2.5);
	  double_vertex(x+4,3);
	  double_vertex(x+4,0);
	  double_vertex(x+5,0);
	  double_vertex(x+5,5);
	  double_vertex(x+2.5,4);
	  double_vertex(x+0,5);
	  perimetro();
	  quadrato(0,1,2,9);
	  quadrato(2,3,8,9);
	  quadrato(3,4,7,8);
	  quadrato(4,5,6,7);
	  break;
	case 'A':
	  double_vertex(x+0,0);
	  double_vertex(x+1,0);
	  double_vertex(x+1.4,1);
	  double_vertex(x+3.6,1);
	  double_vertex(x+4,0);
	  double_vertex(x+5,0);
	  double_vertex(x+3,5);
	  double_vertex(x+2,5);
	  perimetro();
	  double_vertex(x+1.8,2);
	  double_vertex(x+3.2,2);
	  double_vertex(x+2.5,5.0-1.25);
	  lato(8,9);
	  lato(9,10);
	  lato(10,8);
	  quadrato(0,1,10,7);
	  quadrato(2,3,9,8);
	  quadrato(4,5,6,10);
	  triangolo(10,6,7);
	  break;
	case 'N':
	  double_vertex(x+0,0);
	  double_vertex(x+1,0);
	  double_vertex(x+1,3.5);
	  double_vertex(x+4,0);
	  double_vertex(x+5,0);
	  double_vertex(x+5,5);
	  double_vertex(x+4,5);
	  double_vertex(x+4,1.5);
	  double_vertex(x+1,5);
	  double_vertex(x+0,5);
	  perimetro();
	  quadrato(0,1,8,9);
	  quadrato(2,3,7,8);
	  quadrato(3,4,5,6);
	  break;
	case 'U':
	  double_vertex(x+1,2.5);
	  double_vertex(x+1,5);
	  double_vertex(x+0,5);
	  double_vertex(x+0,2.5);
	  double_vertex(x+4,2.5);
	  double_vertex(x+4,5);
	  double_vertex(x+5,5);
	  double_vertex(x+5,2.5);
	  quadrato(0,1,2,3);
	  quadrato(4,5,6,7);
	  lato(0,1);lato(1,2);lato(2,3);
	  lato(4,5);lato(5,6);lato(6,7);
	  for (theta=M_PI+step;theta<2.0*M_PI;theta+=step)
	    {
	      double_vertex(x+2.5+2.5*cos(theta),2.5+2.5*sin(theta));
	      double_vertex(x+2.5+1.5*cos(theta),2.5+1.5*sin(theta));
	      lato(3,8);lato(9,0);
	      quadrato(0,3,8,9);
	      p[3][0]=p[8][0];p[3][1]=p[8][1];
	      p[0][0]=p[9][0];p[0][1]=p[9][1];
	      nv-=2;
	    }
	  lato(0,4);lato(3,7);
	  quadrato(0,4,7,3);
	  break;
	case 'L':
	  double_vertex(x+0,0);
	  double_vertex(x+5,0);
	  double_vertex(x+5,1);
	  double_vertex(x+1,1);
	  double_vertex(x+1,5);
	  double_vertex(x+0,5);

	  perimetro();
	  quadrato(0,1,2,3);
	  quadrato(3,4,5,0);
	  break;
	}
    }
  return x;
}


double l;
double R=1.0,r=0.4,h=0.1;
vector transform(vector v)
{
  double a,b;
  v.x[0]*=2.0*M_PI/l;
  v.x[1]=(v.x[1]-2.5)/5.0;
  a=r*v.x[1]*cos(v.x[0]/2.0)+h*v.x[2]*sin(v.x[0]/2.0);
  b=h*v.x[2]*cos(v.x[0]/2.0)-r*v.x[1]*sin(v.x[0]/2.0);
  return vector((R+a)*cos(v.x[0]),(R+a)*sin(v.x[0]),b);
}
  
void init_border(void)
{
  vertex *v;
  double radius;
  cout<<"radius=";
  cin>>radius;
  cout<<"step=";
  cin>>step;
  cout<<"r=";
  cin>>r;
  cout<<"h=";
  cin>>h;
  l=make("EMANUEL");
  side_triangulate(radius);
  for (v=first_vertex;v!=NULL;v=v->next)
    {
      *(vector*)v=transform(*v);
    }
}
