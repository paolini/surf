#include <cmath>

#include "../vector3.h"
#include "../border.h"

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
  double tau, h;
  int i,j;
  vertex *p[16];
  vertex *q[4];
  double htau;
  double hplus;
  double hminus;
  double hin;

  tau = 2 - sqrt(3.0);
  h = 3;
  //h = 16;

  htau = h*tau;
  hplus = (1 + tau)/2.0;
  hminus = (1 - tau)/2.0;
  hin = hplus - hminus*3.0/(2*sqrt(2.0));

  /*
   * l'ordinamento e' strano: la x e' ordinata in modo decrescente
   * la y in modo crescente
   * la z in modo decrescente
   * i 4 vertici del tetraedro, ordinamento lessicografico: x, y, z
   */
  p[0] = S.new_vertex(vector3(h, -1.0, 0.0));
  p[1] = S.new_vertex(vector3(h, +1.0, 0.0));
  p[2] = S.new_vertex(vector3(-h, 0.0, -1.0));
  p[3] = S.new_vertex(vector3(-h, 0.0, +1.0));

  /*
   * i vertici del quadrato centrale, ord. lessicografico: z, y
   */
  p[4] = S.new_vertex(vector3(0.0, -0.5, +0.5));
  p[5] = S.new_vertex(vector3(0.0, +0.5, +0.5));
  p[6] = S.new_vertex(vector3(0.0, -0.5, -0.5));
  p[7] = S.new_vertex(vector3(0.0, +0.5, -0.5));

  /*
   * i vertici del rettangolo destro, ord. lessicografico: z, y
   */
  p[8] = S.new_vertex(vector3(htau, -hplus, +hminus));
  p[9] = S.new_vertex(vector3(htau, +hplus, +hminus));
  p[10] = S.new_vertex(vector3(htau, -hplus, -hminus));
  p[11] = S.new_vertex(vector3(htau, +hplus, -hminus));

  /*
   * i vertici del rettangolo sinistro, ord. lessicografico: z, y
   */
  p[12] = S.new_vertex(vector3(-htau, -hminus, +hplus));
  p[13] = S.new_vertex(vector3(-htau, +hminus, +hplus));
  p[14] = S.new_vertex(vector3(-htau, -hminus, -hplus));
  p[15] = S.new_vertex(vector3(-htau, +hminus, -hplus));

  for (i=0;i<16;i++)
    {
      p[i]->border=1.0;
      p[i]->next_border=p[i];
    }

  /*
   * 4 vertici interni, angoli delle linee triple, ord less. x, y, z
   */
  q[0] = S.new_vertex(vector3(htau, -hin, 0.0));
  q[1] = S.new_vertex(vector3(htau, +hin, 0.0));
  q[2] = S.new_vertex(vector3(-htau, 0.0, +hin));
  q[3] = S.new_vertex(vector3(-htau, 0.0, -hin));

  //for (i=0;i<4;i++)
  //  {
  //    q[i]=S.new_vertex(vector3(0,(i/2-0.5)*(3-sqrt(3))/3.0,(i%2-0.5)*(3-sqrt(3))/3.0));
  //  }

  /* aletta orizz. di destra, poi sinistra */
  quadr (S, p[0], p[1], q[1], q[0]);
  quadr (S, p[2], p[3], q[3], q[2]);

  /* 4 triangoli di dx, poi i 4 di sx */
  S.new_triangle (p[0], q[0], p[8]);
  S.new_triangle (p[0], q[0], p[10]);
  S.new_triangle (p[1], q[1], p[9]);
  S.new_triangle (p[1], q[1], p[11]);

  S.new_triangle (p[2], q[2], p[12]);
  S.new_triangle (p[2], q[2], p[13]);
  S.new_triangle (p[3], q[3], p[14]);
  S.new_triangle (p[3], q[3], p[15]);

  /* sezione verticale x=htau: 2 trapezi */
  quadr (S, p[8], q[0], q[1], p[9]);
  quadr (S, p[10], q[0], q[1], p[11]);

  /* sezione verticale x=-htau: 2 trapezi */
  quadr (S, p[12], q[2], q[3], p[14]);
  quadr (S, p[13], q[2], q[3], p[15]);

  /* 4 trapezi appoggiati alle facce del tetraedri */
  quadr (S, p[4], p[5], p[9], p[8]);
  quadr (S, p[6], p[7], p[11], p[10]);
  quadr (S, p[4], p[6], p[14], p[12]);
  quadr (S, p[5], p[7], p[15], p[13]);

  /* infine il quadrato verticale centrale di raccordo */
  quadr (S, p[4], p[5], p[7], p[6]);
}


static bool init() {
  Border::registry["tetra"] = new Border(border_function, new_border_vertex, init_border);
  return true;
}

static bool initializer = init();
