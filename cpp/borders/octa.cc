#include <cmath>

#include "../vector3.h"
#include "../border.h"
#include "../surf.h"

class Octa: public Border {
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

  void init_border()
  {
    int i,j;
    vertex *p[6];
    vertex *q[7];

    string commands[][2] = {
      {"1", "brakke_1"},
      {"2", "brakke_2"},
      {"3", "brakke_3"},
      {"4", "brakke_4"},
      {"5", "brakke_5"},
      {"a", "type_a"},
      {"b", "type_b"},
      {"c", "type_c"},
      {"", ""}
    };
  
    char c = command_prompt("<1-5>: Brakke films, type <A>, type <B>, type <C>? ", commands);

    p[0]=new_vertex(vector3(1.,0.,0.));
    p[1]=new_vertex(vector3(-1.,0.,0.));
    p[2]=new_vertex(vector3(0.,1.,0.));
    p[3]=new_vertex(vector3(0.,-1.,0.));
    p[4]=new_vertex(vector3(0.,0.,1.));
    p[5]=new_vertex(vector3(0.,0.,-1.));
    for (i=0;i<6;i++)
      {
	p[i]->border=1.0;
	p[i]->next_border=p[i];
      }
  /*
     K. Brakke, Soap films and covering spaces, 1995
     There are 5 examples of minimal films over an octahedron, numbered 1-5
   */
    if (tolower (c) == '1') /* la prima di Brakke */
      {
        q[0]=new_vertex(vector3(1./3,0.,-1./3));   // (p[0]+p[5])/3
        q[1]=new_vertex(vector3(0.,1./3,-1./3));   // (p[5]+p[2])/3
        q[2]=new_vertex(vector3(-1./3,1./3,0.));   // (p[2]+p[1])/3
        q[3]=new_vertex(vector3(-1./3,0.,1./3));   // (p[1]+p[4])/3
        q[4]=new_vertex(vector3(0.,-1./3,1./3));   // (p[4]+p[3])/3
        q[5]=new_vertex(vector3(1./3,-1./3,0.));   // (p[3]+p[0])/3

        q[6]=new_vertex(vector3(0.,0.,0.));   // orig

        new_triangle (p[0], p[5], q[0]);
        new_triangle (p[5], p[2], q[1]);
        new_triangle (p[2], p[1], q[2]);
        new_triangle (p[1], p[4], q[3]);
        new_triangle (p[4], p[3], q[4]);
        new_triangle (p[3], p[0], q[5]);

	quadr(p[0],p[2],q[1],q[0]);
	quadr(p[2],p[4],q[3],q[2]);
	quadr(p[4],p[0],q[5],q[4]);

	quadr(p[3],p[5],q[0],q[5]);
	quadr(p[5],p[1],q[2],q[1]);
	quadr(p[1],p[3],q[4],q[3]);

        new_triangle (p[0], q[0], q[5]);
        new_triangle (p[2], q[2], q[1]);
        new_triangle (p[4], q[4], q[3]);
        new_triangle (p[1], q[3], q[2]);
        new_triangle (p[3], q[5], q[4]);
        new_triangle (p[5], q[1], q[0]);

        /* esagono centrale */

        new_triangle (q[5], q[0], q[6]);
        new_triangle (q[0], q[1], q[6]);
        new_triangle (q[1], q[2], q[6]);
        new_triangle (q[2], q[3], q[6]);
        new_triangle (q[3], q[4], q[6]);
        new_triangle (q[4], q[5], q[6]);
      }
    if (tolower (c) == '2') /* la seconda di Brakke, rispetto a 1 cambiano q[0] e q[1] */
      {
        q[0]=new_vertex(vector3(1./4,1./4,-1./4));   // faccia p[0,2,5]
        q[1]=new_vertex(vector3(-1./4,-1./4,-1./4)); // faccia p[1,3,5]
        q[2]=new_vertex(vector3(-1./3,1./3,0.));   // (p[2]+p[1])/3
        q[3]=new_vertex(vector3(-1./3,0.,1./3));   // (p[1]+p[4])/3
        q[4]=new_vertex(vector3(0.,-1./3,1./3));   // (p[4]+p[3])/3
        q[5]=new_vertex(vector3(1./3,-1./3,0.));   // (p[3]+p[0])/3

        q[6]=new_vertex(vector3(0.,0.,0.));   // orig

        new_triangle (p[0], p[2], q[0]);
        new_triangle (p[0], p[5], q[0]);
        new_triangle (p[5], p[2], q[0]);
        new_triangle (p[2], p[1], q[2]);
        new_triangle (p[1], p[4], q[3]);
        new_triangle (p[4], p[3], q[4]);
        new_triangle (p[3], p[0], q[5]);
        new_triangle (p[1], p[5], q[2]);
        new_triangle (p[3], p[5], q[5]);

	quadr(p[2],p[4],q[3],q[2]);
	quadr(p[4],p[0],q[5],q[4]);

	quadr(p[0],q[0],q[1],q[5]);
	quadr(p[2],q[0],q[1],q[2]);
	quadr(p[1],p[3],q[4],q[3]);

        new_triangle (p[5], q[1], q[5]);
        new_triangle (p[5], q[1], q[2]);
        new_triangle (p[4], q[4], q[3]);
        new_triangle (p[1], q[3], q[2]);
        new_triangle (p[3], q[5], q[4]);
        new_triangle (p[5], q[1], q[0]);

        /* pentagono centrale */

        new_triangle (q[5], q[1], q[6]);
        new_triangle (q[1], q[2], q[6]);
        new_triangle (q[2], q[3], q[6]);
        new_triangle (q[3], q[4], q[6]);
        new_triangle (q[4], q[5], q[6]);
      }
    if (tolower (c) == '3') /* la terza di Brakke */
      {
        q[0]=new_vertex(vector3(1./4,1./4,-1./4));   // (p[0]+p[2]+p[5])/4
        q[1]=new_vertex(vector3(1./4,-1./4,1./4));   // (p[0]+p[3]+p[4])/4
        q[2]=new_vertex(vector3(-1./4,1./4,1./4));   // (p[2]+p[4]+p[1])/4
        q[3]=new_vertex(vector3(-1./4,-1./4.,-1./4));   // (p[1]+p[3]+p[5])/4

        q[6]=new_vertex(vector3(0.,0.,0.));   // orig

        new_triangle (p[0], p[2], q[0]);
        new_triangle (p[0], p[5], q[0]);
        new_triangle (p[5], p[2], q[0]);
        new_triangle (p[0], p[4], q[1]);
        new_triangle (p[0], p[3], q[1]);
        new_triangle (p[3], p[4], q[1]);
        new_triangle (p[2], p[4], q[2]);
        new_triangle (p[1], p[2], q[2]);
        new_triangle (p[4], p[1], q[2]);
        new_triangle (p[1], p[3], q[3]);
        new_triangle (p[3], p[5], q[3]);
        new_triangle (p[5], p[1], q[3]);

        quadr(p[0],q[0],q[6],q[1]);
        quadr(p[2],q[2],q[6],q[0]);
        quadr(p[4],q[1],q[6],q[2]);
        quadr(p[1],q[2],q[6],q[3]);
        quadr(p[3],q[1],q[6],q[3]);
        quadr(p[5],q[0],q[6],q[3]);
      }
    if (tolower (c) == '4') /* la quarta di Brakke */
      {
        q[0]=new_vertex(vector3(1./4,1./4,-1./4));   // (p[0]+p[2]+p[5])/4
        q[1]=new_vertex(vector3(1./4,-1./4,1./4));   // (p[0]+p[3]+p[4])/4
        q[2]=new_vertex(vector3(-1./3,1./3,0.));   // (p[1]+p[2])/3
        q[3]=new_vertex(vector3(-1./3,0.,1./3));   // (p[1]+p[4])/3
        q[4]=new_vertex(vector3(-1./4,-1./4.,-1./3));   // (p[1]+p[3]+k*p[5])/4
        q[5]=new_vertex(vector3(-1./4,-1./3.,-1./4));   // (p[1]+k*p[3]+p[5])/4

        new_triangle (p[0], p[2], q[0]);
        new_triangle (p[0], p[4], q[1]);

	quadr(p[2],p[4],q[3],q[2]);

        new_triangle (p[0], p[5], q[0]);
        new_triangle (p[5], p[2], q[0]);
        new_triangle (p[0], p[3], q[1]);
        new_triangle (p[3], p[4], q[1]);
        new_triangle (p[1], p[2], q[2]);
        new_triangle (p[1], p[4], q[3]);

        new_triangle (p[1], q[3], q[2]);

	quadr(q[2],q[3],q[5],q[4]);
	quadr(p[2],q[2],q[4],q[0]);
	quadr(p[4],q[1],q[5],q[3]);

        new_triangle (p[0], q[0], q[4]);
        new_triangle (p[0], q[4], q[5]);
        new_triangle (p[0], q[5], q[1]);

        new_triangle (p[5], q[0], q[4]);
        new_triangle (p[3], q[1], q[5]);
        quadr(p[3],p[5],q[4],q[5]);

        quadr(p[1],q[2],q[4],p[5]);
        quadr(p[1],q[3],q[5],p[3]);
      }
    if (tolower (c) == '5') /* la quinta di Brakke */
      {
        q[0]=new_vertex(vector3(1./3,-1./3,0.));   // (p[0]+p[3])/3
        q[1]=new_vertex(vector3(-1./3,1./3,0.));   // (p[1]+p[2])/3
        q[2]=new_vertex(vector3(1./4,1./4,1./4));  // (p[0]+p[2]+p[4])/4
        q[3]=new_vertex(vector3(0.,0.,1./3));      // (2*p[4]+p[5])/3
        q[4]=new_vertex(vector3(-1./4,-1./4,-1./4));  // (p[1]+p[3]+p[5])/4
        q[5]=new_vertex(vector3(0.,0.,-1./3));     // (2*p[5]+p[4])/3

        new_triangle (p[0], p[4], q[2]);
        new_triangle (p[4], p[2], q[2]);
        new_triangle (p[0], p[2], q[2]);
        new_triangle (p[3], p[5], q[4]);
        new_triangle (p[3], p[1], q[4]);
        new_triangle (p[1], p[5], q[4]);
        new_triangle (p[0], p[3], q[0]);
        new_triangle (p[1], p[2], q[1]);

	quadr(p[0],p[5],q[5],q[0]);
	quadr(p[2],p[5],q[5],q[1]);
	quadr(p[1],p[4],q[3],q[1]);
	quadr(p[3],p[4],q[3],q[0]);

        new_triangle (p[4], q[2], q[3]);
        new_triangle (p[5], q[4], q[5]);

	quadr(p[0],q[2],q[3],q[0]);
	quadr(p[3],q[0],q[5],q[4]);
	quadr(p[1],q[4],q[5],q[1]);
	quadr(p[2],q[1],q[3],q[2]);

	quadr(q[0],q[3],q[1],q[5]);
      }
    if (tolower (c) == 'a') /* facce a scacchiera */
      {
	new_triangle (p[0], p[2], p[4]);
	new_triangle (p[0], p[3], p[5]);
	new_triangle (p[1], p[2], p[5]);
	new_triangle (p[1], p[3], p[4]);
      }
    if (tolower (c) == 'b') /* tre facce aperte */
      {
	q[0]=new_vertex(vector3(1./3,1./3,0.));   // (p[0]+p[2])/3 = aletta02
	q[1]=new_vertex(vector3(0.,1./3,1./3));   // aletta24
	q[2]=new_vertex(vector3(1./3,0.,1./3));   // aletta40

	/* alette */

	new_triangle (p[0], p[2], q[0]);
	new_triangle (p[2], p[4], q[1]);
	new_triangle (p[4], p[0], q[2]);

	/* top triangle */

	new_triangle (p[0], q[0], q[2]);
	new_triangle (p[2], q[1], q[0]);
	new_triangle (p[4], q[2], q[1]);
	new_triangle (q[0], q[1], q[2]);

	/* positive trias */

	new_triangle (p[5], p[2], q[0]);
	new_triangle (p[5], q[0], p[0]);
	new_triangle (p[1], p[4], q[1]);
	new_triangle (p[1], q[1], p[2]);
	new_triangle (p[3], p[0], q[2]);
	new_triangle (p[3], q[2], p[4]);

	/* bottom triangle */

	new_triangle (p[1], p[3], p[5]);
      }
    if (tolower (c) == 'c') /* fascia laterale */
      {
	q[0]=new_vertex(vector3(1./3,-1./3,0.));   // (p[0]+p[3])/3 = aletta03
	q[1]=new_vertex(vector3(1./3,0.,-1./3));   // aletta05
	q[2]=new_vertex(vector3(0.,1./3,-1./3));   // aletta25
	q[3]=new_vertex(vector3(-1./3,1./3,0.));   // aletta21
	q[4]=new_vertex(vector3(-1./3,0.,1./3));   // aletta41
	q[5]=new_vertex(vector3(0.,-1./3,1./3));   // aletta43

	/* alette */

	new_triangle (p[0], p[3], q[0]);
	new_triangle (p[0], p[5], q[1]);
	new_triangle (p[2], p[5], q[2]);
	new_triangle (p[2], p[1], q[3]);
	new_triangle (p[4], p[1], q[4]);
	new_triangle (p[4], p[3], q[5]);

	/* positive trias */

	new_triangle (p[0], q[0], q[1]);
	new_triangle (p[2], q[2], q[3]);
	new_triangle (p[4], q[4], q[5]);

	/* negative trias */

	new_triangle (p[1], q[3], q[4]);
	new_triangle (p[3], q[5], q[0]);
	new_triangle (p[5], q[1], q[2]);

	/* negative trapezoids */

	quadr(p[3],p[5],q[1],q[0]);
	quadr(p[5],p[1],q[3],q[2]);
	quadr(p[1],p[3],q[5],q[4]);

	/* positive trapezoids */

	quadr(p[4],p[2],q[3],q[4]);
	quadr(p[0],p[4],q[5],q[0]);
	quadr(p[2],p[0],q[1],q[2]);
      }
    if (tolower (c) == 'd') /* superficie skew */
      {
	q[0]=new_vertex(vector3(0.,0.5,0.));
	q[1]=new_vertex(vector3(0.,-0.5,0.));
	q[2]=new_vertex(vector3(0.,0.,0.5));
	q[3]=new_vertex(vector3(0.,0.,-0.5));
	new_triangle (p[0], p[2], q[0]);
	new_triangle (p[0], p[4], q[2]);
	new_triangle (p[0], q[0], q[2]);

	new_triangle (p[0], p[3], q[1]);
	new_triangle (p[0], p[5], q[3]);
	new_triangle (p[0], q[1], q[3]);

	new_triangle (p[1], p[2], q[0]);
	new_triangle (p[1], p[5], q[3]);
	new_triangle (p[1], q[0], q[3]);

	new_triangle (p[1], p[3], q[1]);
	new_triangle (p[1], p[4], q[2]);
	new_triangle (p[1], q[1], q[2]);

	quadr(p[2],p[4],q[2],q[0]);
	quadr(p[4],p[3],q[1],q[2]);
	quadr(p[3],p[5],q[3],q[1]);
	quadr(p[5],p[2],q[0],q[3]);

	quadr(q[0],q[2],q[1],q[3]);
      }
  }
};

static bool initializer = registry_function<Octa>("octa");
