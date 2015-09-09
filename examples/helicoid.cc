#include <math.h>

#include "vector.h"
#include "vertex.h"


/****************************/
/* definizione del contorno */
/****************************/

/* t va da 0 a 4 */
double R=1.0;
double r=0.1;
double h=1.0;
double N=5.5;

vector border_function(double t)
{	
  if (t<2.0) {
    if (t<1.0) { // t in [0,1]
      return vector(r+t*(R-r),0,-h);
    } else { // t in [1,2]
      double a=N*2.0*M_PI*(t-1.0);
      return vector(R*cos(a),R*sin(a),(t-1.5)*2.0*h);
    }
  } else {
    if (t<3.0) { // t in [2,3]
      double a=N*2.0*M_PI;
      double s=(3.0-t)*R;
      return vector(r+s*(R-r)*cos(a),r+s*(R-r)*sin(a),h);
    } else { // t in [3,4]
      double a=N*2.0*M_PI*(4.0-t);
      return vector(r*cos(a),r*sin(a),(3.5-t)*2.0*h);
    }
  }
  return vector(R*cos(t),R*sin(t),h*cos(N*t));
}	

vertex* new_border_vertex(vertex *v,vertex *w)
{
  vertex *p;
  double d;
  if (v->next_border==w || w->next_border==v) {
    d=(v->border+w->border)/2.0;

    if (fabs(v->border-w->border)<1.5) 	{
      p=new vertex(border_function(d));
      p->border=d;
    } else {
      cout<<"d="<<d<<" t="<<v->border<<" s="<<w->border<<"\n";
      d+=2.0;
      while (d>=4.0) d-=4.0;
      p=new vertex(border_function(d));
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
    p=new vertex(0.5*(*v+*w));
  return p;
}

void quadr(vertex *a,vertex*b,vertex*c,vertex*d) {
  new triangle(a,b,c);
  new triangle(a,c,d);
}

void init_border(void) {
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

  p=new (vertex *) [K];
  q=new (vertex *) [K];
  r=new (vertex *) [K];


  r[0]=new vertex(border_function(0.5));
  r[0]->border=0.5;
  r[K-1]=new vertex(border_function(2.5));
  r[K-1]->border=2.5;
  
  for (i=0;i<K;i++) {
    p[i]=new vertex(border_function(1.0+double(i)/(K-1)));
    p[i]->border=1.0+double(i)/(K-1);
    q[i]=new vertex(border_function(4.0-double(i)/(K-1)));
    q[i]->border=4.0-double(i)/(K-1);

    if (i>0 && i<K-1)
      r[i]=new vertex(0.5*(*p[i]+*q[i]));
   
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
      quadr(p[i-1],p[i],r[i],r[i-1]);
      quadr(r[i-1],r[i],q[i],q[i-1]);
      //    } else {
      // quadr(p[i],r[i],r[i-1],p[i-1]);
      // quadr(r[i],q[i],q[i-1],r[i-1]);
      //  }
  }
  
}
