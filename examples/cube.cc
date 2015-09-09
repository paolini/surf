#include <math.h>

#include "vector.h"
#include "vertex.h"

/****************************/
/* definizione del contorno */
/****************************/

vertex* new_border_vertex(vertex *v,vertex *w)
{
  vertex *p;
  p=new vertex(0.5*(*v+*w));
  if (v->border==1 && w->border==1)
    {
    p->border=1;
    p->next_border=p;
    }
  return p;
}
vector border_function(double )
{
	return vector(0,0,0);
}
void quadr(vertex *a,vertex *b, vertex *c,vertex *d)
{
  new triangle(a,b,c);
  new triangle(a,c,d);
}
void init_border(void)
{
  int i,j;
  vertex *p[8];
  vertex *q[4];
  for (i=0;i<8;i++)
    {
      p[i]=new vertex((i/4)-0.5,(i%4)/2-0.5,i%2-0.5);
      p[i]->border=1.0;
      p[i]->next_border=p[i];
    }
  for (i=0;i<4;i++)
    {
      q[i]=new vertex(0,(i/2-0.5)*(3-sqrt(3))/3.0,(i%2-0.5)*(3-sqrt(3))/3.0);
    }
  for (i=0;i<8;i+=4)
    {
      quadr(p[i],p[i+1],q[1],q[0]);
      quadr(p[i+1],p[i+3],q[3],q[1]);
      quadr(p[i+3],p[i+2],q[2],q[3]);
      quadr(p[i+2],p[i],q[0],q[2]); 
   }
  for (i=0;i<4;i++)
    new triangle(p[i],p[i+4],q[i]);
  quadr(q[0],q[1],q[3],q[2]);
}
