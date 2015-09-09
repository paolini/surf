#include <math.h>

#include "vector.h"
#include "vertex.h"

extern int side_triangulate(vertex *v, vertex *w);
extern void side_triangulate(double radius,int border=0);
extern vertex* first_vertex;
extern triangle* first_triangle;



/****************************/
/* definizione del contorno */
/****************************/

/* non funzioneranno le triangolazioni... vabbe'... */

int N_SPINE=1;
double *x,*y;
double r;
int N;

// parametrizzo con t=0...2*(2*N_SPINE+1)*M_PI
// t=0...2*M_PI e' il cerchio grosso
// t=2*(2*k+1)*M_PI e' il cerchio in basso della spina k
// t=2*(2*k+2)*M_PI e' il cerchio in alto della spina k

vector border_function(double t)
{	
  static int k,h;
  if (t>=2*M_PI)
    {
      h=(int)((t/(2.0*M_PI)-1.0))%2;
      k=(int)((t/(2.0*M_PI)-1.0)/2.0);
      return vector(r*cos(t)+x[k],r*sin(t)+y[k],h?1.0:0.0);
    }
  else
    return vector(cos(t),sin(t),0);
}	

vertex* new_border_vertex(vertex *v,vertex *w)
{
  vertex *p;
  double d;
  int k;
  if (v->next_border==w || w->next_border==v)
    {
      d=(v->border+w->border)/2.0;
      if (fabs(v->border-w->border)<M_PI)
	{
	  p=new vertex(border_function(d));
	  p->border=d;
	}	
      else 
	{
	  d+=M_PI;
	  k=(int)(v->border/(2.0*M_PI));
	  while (d>=(k+1)*2.0*M_PI) d-=2.0*M_PI;
	  p=new vertex(border_function(d));
	  p->border=d;
	}
      if (v->next_border==w)
	{
	  v->next_border=p;
	  p->next_border=w;
	}
      else
	{
	  w->next_border=p;
	  p->next_border=v;
	}
    }	
  else
    p=new vertex(0.5*(*v+*w));
  return p;
}
void quadr(vertex *a,vertex *b, vertex *c,vertex *d)
{
  new triangle(a,b,c);
  new triangle(a,c,d);
}
void init_border(void)
{
  int i,j,k;
  double d,l,t,t0;
  double xx,yy;
  vertex *lastp,*firstp,*p,*p0;
  vertex *lastq,*firstq,*q;

  cout<<"Numero lati: ";
  cin>>N;
  cout<<"Numero spine: ";
  cin>>N_SPINE;

  t=0.0;
  p0=new vertex(vector(0,0,0));
  for (i=0;i<10*N;i++)
    {  
    p=new vertex(border_function(t));
    p->border=t;
    if (i>0)
      {
	lastp->next_border=p;
	new triangle(p0,p,lastp);
      }
    else
      firstp=p;
    t+=2.0*M_PI/(10*N);
    lastp=p;
    }
  p->next_border=firstp;
  new triangle(p0,p,firstp);
  cout<<"triangulate radius: ";
  cin>>r;
  
  side_triangulate(r);


  x=new double[N_SPINE];
  y=new double[N_SPINE];
  l=0.5;
  d=sqrt(l*l*4.0/N_SPINE);
  r=d*d*d;
  cout<<"r="<<r<<" ";
  cin>>r;

  xx=-l+(d/2.0);yy=-l+(d/2.0);
  for (k=0;k<N_SPINE;k++)
    {
      x[k]=xx;
      y[k]=yy;
      t0=2.0*(2.0*k+1)*M_PI;
      p0=new vertex(vector(x[k],y[k],1));
      for (i=0;i<N;i++)
	{
	  t=2.0*M_PI/N*i;
	  q=new vertex(vector(r*cos(t)+x[k],r*sin(t)+y[k],0));
	  p=new vertex(vector(r*cos(t)+x[k],r*sin(t)+y[k],1.0));
	  q->border=t0+t;
	  p->border=t0+t+2.0*M_PI;
	  if (i>0)
	    {
	      lastp->next_border=p;
	      lastq->next_border=q;
	      new triangle(p0,p,lastp);
	      quadr(lastp,p,q,lastq);
	    }
	  else
	    {
	      firstp=p;
	      firstq=q;
	    }
	  lastp=p;
	  lastq=q;
	}
      p->next_border=firstp;
      q->next_border=firstq;
      new triangle(p0,p,firstp);
      quadr(firstp,p,q,firstq);
      xx+=d;
      if (xx>l)
	{
	  xx=-l+(d/2.0);
	  yy+=d;
	}
    }
}

