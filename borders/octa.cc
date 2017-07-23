#include <cmath>

#include "../vector3.h"
#include "../border.h"
#include "../surf.h"

/****************************/
/* definizione del contorno */
/****************************/

static vertex* new_border_vertex(surf &S, vertex *v,vertex *w)
{
  vertex *p;
  p=S.new_vertex(0.5*(*v+*w));
  if (v->border==1 && w->border==1)
    {
    p->border=1;
    p->next_border=p;
    }
  return p;
}
static vector3 border_function(double )
{
	return vector3(0,0,0);
}
static void quadr(surf &S, vertex *a,vertex *b, vertex *c,vertex *d)
{
  S.new_triangle(a,b,c);
  S.new_triangle(a,c,d);
}
static void init_border(surf &S)
{
  int i,j;
  vertex *p[6];
  vertex *q[6];

  string commands[][2] = {
    {"s", "simply_connected"},
    {"a", "type_a"},
    {"b", "type_b"},
    {"c", "type_c"},
    {"", ""}
  };
  
  char c = command_prompt("<S>imply connected, type <A>, type <B>, type <C>? ", commands);

  p[0]=S.new_vertex(vector3(1.,0.,0.));
  p[1]=S.new_vertex(vector3(-1.,0.,0.));
  p[2]=S.new_vertex(vector3(0.,1.,0.));
  p[3]=S.new_vertex(vector3(0.,-1.,0.));
  p[4]=S.new_vertex(vector3(0.,0.,1.));
  p[5]=S.new_vertex(vector3(0.,0.,-1.));
  for (i=0;i<6;i++)
    {
      p[i]->border=1.0;
      p[i]->next_border=p[i];
    }
  if (tolower (c) == 's') /* una di quelle di Brakke */
  {
    printf ("Non gestito!\n");
    exit (1);
  }
  if (tolower (c) == 'a') /* facce a scacchiera */
  {
    S.new_triangle (p[0], p[2], p[4]);
    S.new_triangle (p[0], p[3], p[5]);
    S.new_triangle (p[1], p[2], p[5]);
    S.new_triangle (p[1], p[3], p[4]);
  }
  if (tolower (c) == 'b') /* tre facce aperte */
  {
    q[0]=S.new_vertex(vector3(1./3,1./3,0.));   // (p[0]+p[2])/3 = aletta02
    q[1]=S.new_vertex(vector3(0.,1./3,1./3));   // aletta24
    q[2]=S.new_vertex(vector3(1./3,0.,1./3));   // aletta40

    /* alette */

    S.new_triangle (p[0], p[2], q[0]);
    S.new_triangle (p[2], p[4], q[1]);
    S.new_triangle (p[4], p[0], q[2]);

    /* top triangle */

    S.new_triangle (p[0], q[0], q[2]);
    S.new_triangle (p[2], q[1], q[0]);
    S.new_triangle (p[4], q[2], q[1]);
    S.new_triangle (q[0], q[1], q[2]);

    /* positive trias */

    S. new_triangle (p[5], p[2], q[0]);
    S. new_triangle (p[5], q[0], p[0]);
    S. new_triangle (p[1], p[4], q[1]);
    S. new_triangle (p[1], q[1], p[2]);
    S. new_triangle (p[3], p[0], q[2]);
    S. new_triangle (p[3], q[2], p[4]);

    /* bottom triangle */

    S.new_triangle (p[1], p[3], p[5]);
  }
  if (tolower (c) == 'c') /* fascia laterale */
  {
    q[0]=S.new_vertex(vector3(1./3,-1./3,0.));   // (p[0]+p[3])/3 = aletta03
    q[1]=S.new_vertex(vector3(1./3,0.,-1./3));   // aletta05
    q[2]=S.new_vertex(vector3(0.,1./3,-1./3));   // aletta25
    q[3]=S.new_vertex(vector3(-1./3,1./3,0.));   // aletta21
    q[4]=S.new_vertex(vector3(-1./3,0.,1./3));   // aletta41
    q[5]=S.new_vertex(vector3(0.,-1./3,1./3));   // aletta43

    /* alette */

    S.new_triangle (p[0], p[3], q[0]);
    S.new_triangle (p[0], p[5], q[1]);
    S.new_triangle (p[2], p[5], q[2]);
    S.new_triangle (p[2], p[1], q[3]);
    S.new_triangle (p[4], p[1], q[4]);
    S.new_triangle (p[4], p[3], q[5]);

    /* positive trias */

    S.new_triangle (p[0], q[0], q[1]);
    S.new_triangle (p[2], q[2], q[3]);
    S.new_triangle (p[4], q[4], q[5]);

    /* negative trias */

    S. new_triangle (p[1], q[3], q[4]);
    S. new_triangle (p[3], q[5], q[0]);
    S. new_triangle (p[5], q[1], q[2]);

    /* negative trapezoids */

    quadr(S,p[3],p[5],q[1],q[0]);
    quadr(S,p[5],p[1],q[3],q[2]);
    quadr(S,p[1],p[3],q[5],q[4]);

    /* positive trapezoids */

    quadr(S,p[4],p[2],q[3],q[4]);
    quadr(S,p[0],p[4],q[5],q[0]);
    quadr(S,p[2],p[0],q[1],q[2]);
  }
  if (tolower (c) == 'd') /* superficie skew */
  {
    q[0]=S.new_vertex(vector3(0.,0.5,0.));
    q[1]=S.new_vertex(vector3(0.,-0.5,0.));
    q[2]=S.new_vertex(vector3(0.,0.,0.5));
    q[3]=S.new_vertex(vector3(0.,0.,-0.5));
    S.new_triangle (p[0], p[2], q[0]);
    S.new_triangle (p[0], p[4], q[2]);
    S.new_triangle (p[0], q[0], q[2]);

    S.new_triangle (p[0], p[3], q[1]);
    S.new_triangle (p[0], p[5], q[3]);
    S.new_triangle (p[0], q[1], q[3]);

    S.new_triangle (p[1], p[2], q[0]);
    S.new_triangle (p[1], p[5], q[3]);
    S.new_triangle (p[1], q[0], q[3]);

    S.new_triangle (p[1], p[3], q[1]);
    S.new_triangle (p[1], p[4], q[2]);
    S.new_triangle (p[1], q[1], q[2]);

    quadr(S,p[2],p[4],q[2],q[0]);
    quadr(S,p[4],p[3],q[1],q[2]);
    quadr(S,p[3],p[5],q[3],q[1]);
    quadr(S,p[5],p[2],q[0],q[3]);

    quadr(S,q[0],q[2],q[1],q[3]);
  }
}


static bool init() {
  Border::registry["octa"] = new Border(border_function, new_border_vertex, init_border);
  return true;
}

static bool initializer = init();
