#include <cmath>

#include "../vector3.h"
#include "../border.h"
#include "../surf.h"

class Manu: public Border {
public:
  
  vertex *p[50][2];
  int nv;
  double step;
  double depth;
  double l;
  double R, r, h;

  
  Manu(): depth(0.5), nv(0), step(0.1), R(1.0), r(0.4), h(0.1) {}

  vector3 border_function(double t)
  {
    return vector3(0,0,0);
  }
  
  vertex* new_border_vertex(vertex *v,vertex *w)
  {
    vertex *p;
    p=new_vertex(0.5*(*v+*w));
    return p;
  }
  
  void double_vertex(double x,double y)
  {
    cout<<"vertex("<<x<<","<<y<<")\n";
    p[nv][0]=new_vertex(vector3(x,y,-depth));
    p[nv][1]=new_vertex(vector3(x,y,depth));
    nv++;
  }

  void lato(int a, int b)
  {
    cout<<"lato("<<a<<","<<b<<")\n";
    new_triangle(p[a][0],p[b][0],p[b][1]);
    new_triangle(p[a][0],p[b][1],p[a][1]);
  }
  
  void triangolo(int a,int b ,int c)
  {
    new_triangle(p[a][0],p[b][0],p[c][0]);
    new_triangle(p[a][1],p[b][1],p[c][1]);
  }

  void quadrato(int a, int b, int c, int d)
  {
    triangolo(a,b,c);
    triangolo(a,c,d);
  }
  
  void perimetro()
  {
    int i;
    for(i=0;i<nv;i++)
      lato(i,(i+1)%nv);
  }

  double make(const char *s)	
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

  vector3 transform(vector3 v)
  {
    double a,b;
    v.x[0]*=2.0*M_PI/l;
    v.x[1]=(v.x[1]-2.5)/5.0;
    a=r*v.x[1]*cos(v.x[0]/2.0)+h*v.x[2]*sin(v.x[0]/2.0);
    b=h*v.x[2]*cos(v.x[0]/2.0)-r*v.x[1]*sin(v.x[0]/2.0);
    return vector3((R+a)*cos(v.x[0]),(R+a)*sin(v.x[0]),b);
  }
  
  void init_border()
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
    l=make("EMANUEL");
    side_triangulate(radius);
    for (vertex_iterator i=this->vertex_begin(); i!=this->vertex_end(); ++i) {
      *static_cast<vector3 *>(&*i) = transform(*i);
    }
  }
};

static bool initializer = registry_function<Manu>("manu");
