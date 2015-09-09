#include <cmath>

#include "../border.h"


/****************************/
/* definizione del contorno */
/****************************/

/* t va da 0 a 4 */
static double R=1.0;
static double r=0.1;
static double h=1.0;
static double N=5.5;

static vector3 border_function(double t) {	
  if (t<2.0) {
    if (t<1.0) { // t in [0,1]
      return vector3(r+t*(R-r),0,-h);
    } else { // t in [1,2]
      double a=N*2.0*M_PI*(t-1.0);
      return vector3(R*cos(a),R*sin(a),(t-1.5)*2.0*h);
    }
  } else {
    if (t<3.0) { // t in [2,3]
      double a=N*2.0*M_PI;
      double s=(3.0-t)*R;
      return vector3(r+s*(R-r)*cos(a),r+s*(R-r)*sin(a),h);
    } else { // t in [3,4]
      double a=N*2.0*M_PI*(4.0-t);
      return vector3(r*cos(a),r*sin(a),(3.5-t)*2.0*h);
    }
  }
  return vector3(R*cos(t),R*sin(t),h*cos(N*t));
}	

static vertex* new_border_vertex(surf &S, vertex *v,vertex *w)
{
  vertex *p;
  double d;
  if (v->next_border==w || w->next_border==v) {
    d=(v->border+w->border)/2.0;

    if (fabs(v->border-w->border)<1.5) 	{
      p=S.new_vertex(border_function(d));
      p->border=d;
    } else {
      cout<<"d="<<d<<" t="<<v->border<<" s="<<w->border<<"\n";
      d+=2.0;
      while (d>=4.0) d-=4.0;
      p=S.new_vertex(border_function(d));
      p->border=d;
      cout<<"-> d="<<d<<" t="<<v->border<<" s="<<w->border<<"\n";
    }
 
    if (v->next_border==w) {
      v->next_border=p;
      p->next_border=w;
    } else {
      w->next_border=p;
      p->next_border=v;
    }
    
  } else
    p=S.new_vertex(0.5*(*v+*w));
  return p;
}

static void quadr(surf &S, vertex *a,vertex*b,vertex*c,vertex*d) {
  S.new_triangle(a,b,c);
  S.new_triangle(a,c,d);
}

static void init_border(surf &S) {
  int i;

  cout<<"R= ";
  cin>>R;
  cout<<"r= ";
  cin>>r;
  cout<<"h= ";
  cin>>h;
  cout<<"N= ";
  cin>>N;

  int K=7*int(N+1);
  vertex **p;
  vertex **q;
  vertex **r;

  typedef vertex *vertex_ptr;
  
  p=new vertex_ptr [K];
  q=new vertex_ptr [K];
  r=new vertex_ptr [K];


  r[0]=S.new_vertex(border_function(0.5));
  r[0]->border=0.5;
  r[K-1]=S.new_vertex(border_function(2.5));
  r[K-1]->border=2.5;
  
  for (i=0;i<K;i++) {
    p[i]=S.new_vertex(border_function(1.0+double(i)/(K-1)));
    p[i]->border=1.0+double(i)/(K-1);
    q[i]=S.new_vertex(border_function(4.0-double(i)/(K-1)));
    q[i]->border=4.0-double(i)/(K-1);

    if (i>0 && i<K-1)
      r[i]=S.new_vertex(0.5*(*p[i]+*q[i]));
   
    if (i) {
      p[i-1]->next_border=p[i];
      q[i]->next_border=q[i-1];
    }
  }
  
  p[K-1]->next_border=r[K-1];
  r[K-1]->next_border=q[K-1];

  q[0]->next_border=r[0];
  r[0]->next_border=p[0];

  for (i=1;i<K;++i) {
    //    if (i<K/2) {
      quadr(S,p[i-1],p[i],r[i],r[i-1]);
      quadr(S,r[i-1],r[i],q[i],q[i-1]);
      //    } else {
      // quadr(p[i],r[i],r[i-1],p[i-1]);
      // quadr(r[i],q[i],q[i-1],r[i-1]);
      //  }
  }
  
}


static bool init() {
  Border::registry["helicoid"] = new Border(border_function, new_border_vertex, init_border);
  return true;
}

static bool initializer = init();
