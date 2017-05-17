#include <stdlib.h>
#include <math.h>

#include "render.h"

extern int n_triangle(surf &);
//extern triangle* S.first_triangle;
extern int out_of_memory(void);
extern double diameter(surf &);

double linewidth=15;

// COLOR

color::color(double r, double g, double b) {
  x[0]=r;  x[1]=g;  x[2]=b;
}

color::color(double c) {
  x[0]=x[1]=x[2]=c;
}

// CAMERA

double camera::scaling(const vector3 &v) const {
  return abs(screen_z)/((v-mypv)*screen_z);
}

camera::camera(void) {
  mypv=vector3(0,0,0);
  screen_z=vector3(0,1,0);
  screen_y=vector3(0,0,1);
  screen_x=vector3(1,0,0);
  mylight=vector3(0,0,1);
  myup=vector3(0,0,1);
}

camera::camera(const camera &c) {
  mypv=c.mypv;
  screen_z=c.screen_z;
  screen_x=c.screen_x;
  screen_y=c.screen_y;
  mylight=c.mylight;
  look=c.look;
}

void camera::light(const vector3 &v) {
  mylight=v;
  if (abs(mylight)>1.0) mylight=versor(mylight);
}

const vector3 &camera::light(void) const {
  return mylight;
}
const vector3 &camera::direction(void) const {
  return screen_z;
}

void camera::pv(const vector3 &v) {
  mypv=v;
  look_at(look);
}

const vector3 &camera::pv(void) const {
  return mypv;
}

void camera::film(double w,double h,int mode) {
  double k;
  if (mode==0) //fit
    {
      if (w/abs(screen_x) > h/abs(screen_y))
	k=h/2/abs(screen_y);
      else
	k=w/2/abs(screen_x);
      screen_x=k*screen_x;
      screen_y=k*screen_y;
      screen_z=k*screen_z;
    }
  else //resize
    {
      screen_x=w/2.0*versor(screen_x);
      screen_y=h/2.0*versor(screen_y);
    }
}

double camera::X(const vector3 &v) const {
  return ((v-mypv)*versor(screen_x))*sqr(screen_z)
    /((v-mypv)*screen_z);
}

double camera::Y(const vector3 &v) const {
  return ((v-mypv)*versor(screen_y))*sqr(screen_z)
    /((v-mypv)*screen_z);
}

void camera::zoom(double k) {
  screen_z=k*screen_z;
}

void camera::look_at(const vector3 &v) {
  screen_z=abs(screen_z)*versor(v-mypv);
  look=v;
  adjust();
}

const vector3 &camera::look_at(void) const {
  return look;
}

void camera::up(const vector3 &v) {
  myup=v;
  adjust();
}

void camera::adjust(void) {
  double f;
  if (abs(screen_z)==0.0)
    screen_z=vector3(0,1,0);
  if (abs(screen_x)==0.0)
    screen_x=vector3(1,0,0);
  if (abs(screen_y)==0.0)
    screen_y=vector3(0,0,1);
  
  if (abs(screen_z^myup)!=0.0)
    screen_x=abs(screen_x)*versor(screen_z^myup);
  screen_y=abs(screen_y)*versor(screen_x^screen_z);
}

//BITMAP_OUTPUT

int bitmap_output::maxx(void)
{return mymaxx;}

int bitmap_output::maxy(void)
{return mymaxy;}

void bitmap_output::maxx(int x)
{mymaxx=x;}

void bitmap_output::maxy(int y)
{mymaxy=y;}

random_bitmap_output::random_bitmap_output(camera &view,int maxx,int maxy,double xpix,double ypix)
:bitmap_output(view,maxx,maxy,xpix,ypix)
{}

vector3 bitmap_output::scan(void)
{
  //  return screen_z
  //  +xpix*(myx-mymaxx/2)*versor(screen_x)
  //  +ypix*(myy-mymaxy/2)*versor(screen_y);
  return screen_z
    +(2.0*myx/mymaxx-1.0)*screen_x
    +(2.0*myy/mymaxy-1.0)*screen_y;
}

int bitmap_output::inc(void)
{
  if (++myx==mymaxx)
    {
      myx=0;
      if (++myy==mymaxy)
	myy=0;
    }
 return (myy!=0 || myx!=0);
}
bitmap_output::bitmap_output(camera &view,int x,int y,double xx,double yy)
  :camera(view)
{
  myy=myx=0;
  mymaxx=x;
  mymaxy=y;
  xpix=xx;
  ypix=yy;
}
int bitmap_output::scan_x(void)
{
  return myx;
}
int bitmap_output::scan_y(void)
{
  return myy;
}

int random_bitmap_output::putpixel(color c)
{
  putpixel(myx,myy,c);
  return inc();
}


text_output::text_output(ostream &o, camera &view,int l,int c,double xpix,double ypix)
  :bitmap_output(view,c,l,xpix,ypix)
{
  film(xpix*c,ypix*l,0);
  mymaxx=(int)(2.0*abs(screen_x)/xpix);
  mymaxy=(int)(2.0*abs(screen_y)/ypix);
  out=&o;
}
int text_output::putpixel(color c)
{
  *out<<gray_scale[(int)(abs(c)*0.99*scale_length)];
  if (myx==mymaxx-1) *out<<"\n";
  return inc();
}

vectorial_output::vectorial_output(const camera &view)
  :camera(view) {
}

color triangle_color(triangle &t,vector3 light)
{
  return color(fabs(versor(((*(t.v[2])-*(t.v[0]))^(*(t.v[1])-*(t.v[0]))))*light));
}

double ps_output::ps_x(double x) {
  return (x+width/2.0+left_margin)*(72.0/2.54);
}

double ps_output::ps_y(double y) {
  return (y+height/2.0+lower_margin)*(72.0/2.54);
}

ps_output::ps_output(ostream &o,camera &view,double w,double h,
		     double pw,double ph):vectorial_output(view) {
  double k;
  out=&o;
  width=w;height=h;
  film(width,height,0);
  width=2.0*abs(screen_x);
  height=2.0*abs(screen_y);
  page_width=pw;page_height=ph;
  left_margin=(page_width-width)/2;
  lower_margin=(page_height-height)/2;
  (*out)<<"%!PS-Adobe-2.0 EPSF-2.0\n"
	<<"%%Title: Vector3ial Output\n"
	<<"%%Creator: sup.cpp, Manu-fatto (Emanuele Paolini)\n"
	<<"%%BoundingBox: "<<ps_x(-width/2.0)<<" "<<ps_y(-height/2.0)<<" "
	<<ps_x(width/2.0)<<" "<<ps_y(height/2.0)<<"\n"
	<<"%%Pages: 1\n"
	<<"%%DocumentFonts:\n"
	<<"%%EndComments\n";
  (*out)<<"%% View: "<<view<<"\n"
	<<"%%EndProlog\n";
  (*out)<<"\n%% Definitions:\n";
  (*out)<<"/SupDict 10 dict def SupDict begin\n";
  (*out)<<"/C /setrgbcolor load def\n"
	<<"/N /newpath load def\n"
	<<"/M /moveto load def\n"
	<<"/L /lineto load def\n"
	<<"/K /closepath load def\n"
	<<"/W /setlinewidth load def\n"
	<<"/S /stroke load def\n"
	<<"/F /fill load def\n";
    
  (*out)<<"1 setlinecap 1 setlinejoin \n";

  // disegna un rettangolo intorno alla vista
  *out<<"\n%newpath "<<ps_x(-width/2.0)<<" "<<ps_y(-height/2.0)<<" moveto\n%";
  *out<<ps_x(width/2.0)<<" "<<ps_y(-height/2.0)<<" lineto\n%";
  *out<<ps_x(width/2.0)<<" "<<ps_y(height/2.0)<<" lineto\n%";
  *out<<ps_x(-width/2.0)<<" "<<ps_y(height/2.0)<<" lineto\n%";
  *out<<ps_x(-width/2.0)<<" "<<ps_y(-height/2.0)<<" lineto closepath 0 1 0 setrgbcolor stroke\n";
}
ps_output::~ps_output(void)
{
*out<<"end showpage\n";
}
void ps_output::out_color(color c)
{
  *out<<c.x[0]<<" "<<c.x[1]<<" "<<c.x[2]<<" C\n";
}
void ps_output::out_line(vector3 p1,vector3 p2,double width,color c)
{
  *out<<"N ";
  *out<<ps_x(X(p1))<<" "<<ps_y(Y(p1))<<" M ";
  *out<<ps_x(X(p2))<<" "<<ps_y(Y(p2))<<" L ";
  *out<<" "<<width<<" W\n";
  out_color(c);
  *out<<"S\n";
}
void ps_output::out_triangle(vector3 p1,vector3 p2,vector3 p3,color c)
{
  *out<<"N ";
  *out<<ps_x(X(p1))<<" "<<ps_y(Y(p1))<<" M ";
  *out<<ps_x(X(p2))<<" "<<ps_y(Y(p2))<<" L ";
  *out<<ps_x(X(p3))<<" "<<ps_y(Y(p3))<<" L ";
  *out<<"K ";
  out_color(c);
  *out<<"F\n";
}
ostream &operator<<(ostream &o,camera &c)
{
  return o<<"{PV="<<c.mypv<<", Look="<<c.look
   <<", ScrY="<<c.screen_y<<", Light="<<c.light()
   <<", Dir="<<c.screen_z<<", ScrX="<<c.screen_x
   <<"}";
}

void vectorial_output::out_triangle(triangle &t, int print_mode) {
  int i;
  if (print_mode!=0 && print_mode!=-1)
    out_triangle(*(t.v[0]),*(t.v[1]),*(t.v[2]),triangle_color(t,light()));
  if (print_mode!=1)
    {
      for (i=0;i<3;i++)
	{
	  if (t.v[i]->next_border!=NULL)
	    {
	      vertex *a=t.v[i];vertex *b;
	      if (t.v[i]->next_border==t.v[(i+1)%3])
		b=t.v[(i+1)%3];
	      else if (t.v[i]->next_border==t.v[(i+2)%3])
		b=t.v[(i+2)%3];
	      else b=a;
	      if (b!=a || print_mode!=-1)
		out_line(*a,*b,(print_mode==-1?2.0:1.0)*linewidth*scaling(*a),
			 color(print_mode==-1?1.0:0.0));
	    }
	}
    }
}

void pov_ray::out_triangle(triangle &t, int print_mode) {
  int i,k;
  class triangle_list *l;
  vector3 n,N,n1;
  double d;
  
  if (print_mode!=0) {
    *out<<"smooth_triangle {";
    for (i=0;i<3;i++)
      {
	n=vector3(0,0,0);
	N=versor(t.normal());
	for (k=0,l=t.v[i]->list;l!=NULL;l=l->next,k++) {
	    n1=versor(l->first->normal());
	    d=n1*N;
	    if (d>0.8)
	      n+=n1;
	    else if (d<-0.8)
	      n+=-n1;
	    else 
	      k--;
	  }
	if (k==0)
	  {
	    cerr<<"Errore interno #9178\n";
	  }
	out_vector3(*(t.v[i]));
	*out<<" ";
	out_vector3(1.0/(double)k*n);
	*out<<" ";
      }
    // *out<<" texture {Color}}\n";
    *out<<"}\n";
  }
  
  if (print_mode!=1) {
    for (i=0;i<3;i++) {
      if (t.v[i]->next_border!=NULL) {
	vertex *a=t.v[i];vertex *b;
	if (t.v[i]->next_border==t.v[(i+1)%3])
	  b=t.v[(i+1)%3];
	else if (t.v[i]->next_border==t.v[(i+2)%3])
	  b=t.v[(i+2)%3];
	else b=a;
	if (b!=a)
	  out_line(*a,*b,linewidth,color(1.0));
      }
    }
  }
}

void pov_ray::out_line(vector3 a,vector3 b,double r,color) {
  const double r_scale=0.001;
  *out<<"cylinder { ";
  out_vector3(a);
  *out<<", ";
  out_vector3(b);
  *out<<", "<<r*r_scale<<" open ";  
  *out<<" texture {Wire}}\n";

  *out<<"sphere { ";
  out_vector3(a);
  *out<<", "<<r*r_scale<<" texture {Wire}}\n";
}

void pov_ray::out_vector3(vector3 v)
{
  *out<<"<"<<v.x[0]<<","<<v.x[1]<<","<<v.x[2]<<">";
}
void pov_ray::out_color(color x)
{
  *out<<"color rgb ";
  out_vector3(x);
  *out<<"";
}

void pov_ray::out_triangle(vector3 a, vector3 b,vector3 c,color x)
{
  *out<<"triangle{";
  out_vector3(a);
  out_vector3(b);
  out_vector3(c);
  *out<<" texture {Color}}\n";
}

pov_ray::pov_ray(ostream &o,camera &v):vectorial_output(v) {
  out=&o;
  *out<<"// POVRAY file generated by surf manu-fatto\n";
  *out<<"// ratio: X->"<<abs(screen_x)<<" Y->"<<abs(screen_y)<<"\n";

  *out<<"camera{location ";
  out_vector3(pv());
  *out<<"\nsky ";
  out_vector3(screen_y);
  *out<<"\nlook_at ";
  out_vector3(look_at());
  *out<<"}\n\n";

  *out<<"light_source{ ";
  out_vector3(look_at()-10.0*light());
  *out<<" ";
  out_color(color(1.0));
  *out<<"}\n";

  *out<<"light_source{<0,0,10> color rgb <1,1,1>}\n";

  // *out<<"#include \"metals.inc\"\n";
  *out<<"#declare P_Chrome2   = color rgb <0.39, 0.41, 0.43>;\n\n";
  
  *out<<
#ifdef THIS_IS_OLD
    "#declare Color = texture {\n"
    "pigment{color rgbf <0.8,0.8,1,0.3>}\n"
    "finish{irid{0.25 thickness 0.7 turbulence 0.3}\n"
    "  ambient .4 phong 0.75}\n"
    "}\n\n"
    "#declare Wire = texture {\n"
    "pigment{P_Chrome2}\n"
    "finish{ambient .4 phong 0.75}\n}\n\n"
#else
    "#declare Color = texture {\n"
    "pigment{color rgbf <0.75,0.8,1.0,0.3>}\n"
    "finish{irid{0.2 thickness 0.7 turbulence 0.6}\n"
    "  ambient .6 phong 0.75}\n"
    "}\n\n"
    "#declare Wire = texture {\n"
    "pigment{P_Chrome2}\n"
    "finish{ambient 0 phong 0.75}\n}\n\n"
#endif
    ;
  *out<<"union{\n";
  *out<<"// plane { <0,0,1>, -5 pigment {color rgb <1,1,1>}\n"
    "// finish {ambient 0.5}}\n";
}
pov_ray::~pov_ray()
{
  *out<<"texture {Color}\n";
  *out<<"}\n\n//END\n";
}

char text_output::gray_scale[12]={' ','.',',',':',';','i','|','[','I','X','W','@'};
int text_output::scale_length=12;

/*************** 3d vettoriale *********/
vector3 pv;
vector3 dir;
int triangle_compare(const void *T1,const void *T2)
{
  triangle **t1, **t2;
  t1=(triangle **) T1;
  t2=(triangle **) T2;
  return ((((*t1)->bari()-pv)*dir-((*t2)->bari()-pv)*dir)>0.0)?-1:1;
}

void vectorial_output::print(surf &S,int print_mode)
{
  int n,i,j;
  triangle **list; /*array con i puntatori ai triangoli da ordinare*/
  triangle *t;
  double x0,x1,y0,y1,x,d;
  cout<<"printing symplex...\n";
  n=n_triangle(S);
  ::pv=pv(); 
  cout <<"pv="<<::pv<<"\n";
  dir=direction();
  cout <<"direction="<<dir<<"\n";
  typedef triangle *triangle_ptr;
  list=new triangle_ptr[n];
  for (t=S.first_triangle,i=0;i<n;t=t->next,i++)
    list[i]=t;
  if (list==NULL) out_of_memory();
  cout<<"Sorting triangles...\n";
  qsort(list,n,sizeof(triangle *),triangle_compare);
  cout<<" ...done.\n";

  d=diameter(S);
  for (i=0,j=0;i<n;i++) {
    if (print_mode==0) {
      for (;j<n && i<n 
	     && ((list[j]->bari()-list[i]->bari())*dir>-2.2*d*abs(dir));
	   j++)
	out_triangle(*(list[j]),-1);
    }
    if (i%250==0)
      cout<<"("<<i<<")\n";
    out_triangle(*(list[i]),print_mode);
  }
  cout<<"\n...done.\n";
}

void pov_ray::print(surf &S,int print_mode) {
  triangle *t;
  for (t=S.first_triangle; t; t=t->next) {
    out_triangle(*t,print_mode);
  }
}

/************** ray-tracer ****************/
vector3 light;

double intersection(triangle *t,vector3 ray) /*torna -1.0 se non interseca*/
{
  static matrix m;
  static vector3 alpha;
  ray-=pv;
  m.x[0]=*(t->v[0])-pv;
  m.x[1]=*(t->v[1])-pv;
  m.x[2]=*(t->v[2])-pv;
  alpha=(inv(m))*(ray);
  if (alpha.x[0]>=0 && alpha.x[1]>=0 && alpha.x[2]>=0)
    return (abs(ray)/(alpha.x[0]+alpha.x[1]+alpha.x[2]));
  else
    return -1.0;
}
color ray_color(surf &S,vector3 v)
{
  triangle *t;
  triangle *found;
  double dist=-1.0;
  double d;
  for (t=S.first_triangle;t!=NULL;t=t->next)
    {
      d=intersection(t,v);
      if (d>=0 && (d<dist || dist==-1.0))
	{
	  found=t;
	  dist=d;
	}
    }
  if (dist>=0.0)
    return color(1.0);
  //triangle_color(*found,light);
  else 
    return color(0);
}

void bitmap_output::print(surf &S,int print_mode) {
  int cont;
  cout<<"printing bitmap "<<maxx()<<"x"<<maxy()<<"...\n";
  ::pv=pv();
  ::light=light();
  do
    {
      cont=putpixel(ray_color(S,scan()+::pv));
    }while (cont);
}
