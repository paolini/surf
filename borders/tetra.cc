#include <cmath>

#include "../vector3.h"
#include "../border.h"

class Tetra: public Border {
public:
  vertex* new_border_vertex(vertex *v,vertex *w)
  {
    vertex *p;
    p=new_vertex(0.5*(*v+*w));
    if (v->border==1 && w->border==1)
      {
	p->border=1;
	p->next_border=p;
      }
    return p;
  }
  
  vector3 border_function(double )
  {
    return vector3(0,0,0);
  }
  
  void quadr(vertex *a,vertex *b, vertex *c,vertex *d)
  {
    new_triangle(a,b,c);
    new_triangle(a,c,d);
  }
  
  void pentag(vertex *a,vertex *b, vertex *c,vertex *d, vertex *e)
  {
    new_triangle(a,b,c);
    new_triangle(a,c,d);
    new_triangle(a,d,e);
  }

  void init_border()
  {
    double tau, h;
    int i,j;
    vertex *p[16];
    vertex *q[13];
    double htau;
    double hplus;
    double hminus;
    double hin;
    char c;

    cout << "Value of h (e.g. h=6, use a negative value to get the regular tetrahedron): ";
    cin >> h;
    //h = 3;
    //h = 6;
    //h = 16;
    if (h < 0)
      {
	h = 1.0/sqrt(2.0);
	cout << "Regular tetrahedron!\n";
      }

    /*
     * l'ordinamento e' strano: la x e' ordinata in modo decrescente
     * la y in modo crescente
     * la z in modo decrescente
     * i 4 vertici del tetraedro, ordinamento lessicografico: x, y, z
     */
    p[0] = new_vertex(vector3(h, -1.0, 0.0));
    p[1] = new_vertex(vector3(h, +1.0, 0.0));
    p[2] = new_vertex(vector3(-h, 0.0, +1.0));
    p[3] = new_vertex(vector3(-h, 0.0, -1.0));

    for (i=0;i<4;i++)
      {
	p[i]->border=1.0;
	p[i]->next_border=p[i];
      }

    /* l'origine */
    q[12] = new_vertex(vector3(0.0, 0.0, 0.0));

    cout << "<C>onelike, <H>oled, <L>ong edges only? ";
    cin >> c;
    if (tolower(c) == 'c')
      {
	new_triangle (p[0], p[1], q[12]);
	new_triangle (p[0], p[2], q[12]);
	new_triangle (p[0], p[3], q[12]);
	new_triangle (p[1], p[2], q[12]);
	new_triangle (p[1], p[3], q[12]);
	new_triangle (p[2], p[3], q[12]);
	return;
      }
    if (tolower(c) == 'l')
      {
	new_triangle (p[0], p[2], q[12]);
	new_triangle (p[2], p[1], q[12]);
	new_triangle (p[1], p[3], q[12]);
	new_triangle (p[3], p[0], q[12]);
	return;
      }
    else if (tolower(c) != 'h')
      {
	cout << "Invalid choice, aborting\n";
	exit (1);
      }

    tau = 2 - sqrt(3.0);

    htau = h*tau;
    hplus = (1 + tau)/2.0;
    hminus = (1 - tau)/2.0;
    hin = hplus - hminus*3.0/(2*sqrt(2.0));

    /*
     * i vertici del quadrato centrale, ord. lessicografico: z, y
     */
    p[4] = new_vertex(vector3(0.0, -0.5, +0.5));
    p[5] = new_vertex(vector3(0.0, +0.5, +0.5));
    p[6] = new_vertex(vector3(0.0, -0.5, -0.5));
    p[7] = new_vertex(vector3(0.0, +0.5, -0.5));

    /*
     * i vertici del rettangolo destro, ord. lessicografico: z, y
     */
    p[8] = new_vertex(vector3(htau, -hplus, +hminus));
    p[9] = new_vertex(vector3(htau, +hplus, +hminus));
    p[10] = new_vertex(vector3(htau, -hplus, -hminus));
    p[11] = new_vertex(vector3(htau, +hplus, -hminus));

    /*
     * i vertici del rettangolo sinistro, ord. lessicografico: z, y
     */
    p[12] = new_vertex(vector3(-htau, -hminus, +hplus));
    p[13] = new_vertex(vector3(-htau, +hminus, +hplus));
    p[14] = new_vertex(vector3(-htau, -hminus, -hplus));
    p[15] = new_vertex(vector3(-htau, +hminus, -hplus));

    for (i=4;i<16;i++)
      {
	p[i]->border=1.0;
	p[i]->next_border=p[i];
      }

    /*
     * 4 vertici interni, angoli delle linee triple, ord less. x, y, z
     */
    q[0] = new_vertex(vector3(htau, -hin, 0.0));
    q[1] = new_vertex(vector3(htau, +hin, 0.0));
    q[2] = new_vertex(vector3(-htau, 0.0, +hin));
    q[3] = new_vertex(vector3(-htau, 0.0, -hin));

    /*
     * 4 punti medi dei lati del quadrato centrale
     */
    q[4] = new_vertex(vector3(0.0, 0.0, +0.5));
    q[5] = new_vertex(vector3(0.0, +0.5, 0.0));
    q[6] = new_vertex(vector3(0.0, 0.0, -0.5));
    q[7] = new_vertex(vector3(0.0, -0.5, 0.0));

    /*
     * 2 punti medi per x = htau e due per x = -htau
     */

    q[8] = new_vertex(vector3(htau, 0.0, +hminus));
    q[9] = new_vertex(vector3(htau, 0.0, -hminus));
    q[10] = new_vertex(vector3(-htau, -hminus, 0.0));
    q[11] = new_vertex(vector3(-htau, +hminus, 0.0));

    /* aletta orizz. di destra, poi sinistra */
    quadr (p[0], p[1], q[1], q[0]);
    quadr (p[2], p[3], q[3], q[2]);

    /* 4 triangoli di dx, poi i 4 di sx */
    new_triangle (p[0], q[0], p[8]);
    new_triangle (p[0], q[0], p[10]);
    new_triangle (p[1], q[1], p[9]);
    new_triangle (p[1], q[1], p[11]);

    new_triangle (p[2], q[2], p[12]);
    new_triangle (p[2], q[2], p[13]);
    new_triangle (p[3], q[3], p[14]);
    new_triangle (p[3], q[3], p[15]);

    /* sezione verticale x=htau: 2 trapezi */
    pentag (q[8], p[8], q[0], q[1], p[9]);
    pentag (q[9], p[10], q[0], q[1], p[11]);

    /* sezione verticale x=-htau: 2 trapezi */
    pentag (q[10], p[12], q[2], q[3], p[14]);
    pentag (q[11], p[13], q[2], q[3], p[15]);

    /*
     * 4 trapezi appoggiati alle facce del tetraedri
     * ciascuno diviso in due
     */
    quadr (p[4], q[4], q[8], p[8]); quadr (q[4], p[5], p[9], q[8]);
    quadr (p[6], q[6], q[9], p[10]); quadr (q[6], p[7], p[11], q[9]);
    quadr (p[4], q[7], q[10], p[12]); quadr (q[7], p[6], p[14], q[10]);
    quadr (p[5], q[5], q[11], p[13]); quadr (q[5], p[7], p[15], q[11]);

    /*
     * infine il quadrato verticale centrale di raccordo
     * diviso in quattro quadrati
     */
    quadr (p[4], q[4], q[12], q[7]);
    quadr (p[5], q[5], q[12], q[4]);
    quadr (p[7], q[6], q[12], q[5]);
    quadr (p[6], q[7], q[12], q[6]);
  }
};

static bool initializer = registry_function<Tetra>("tetra");
