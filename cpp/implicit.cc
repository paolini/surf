/********* Superfici in forma implicita **********/

double i_f(vector3);
vector3 i_grad(vector3);

vertex *find_vertex(vector3 p,double err=0)
{
  vertex *v;
  for (v=S.first_vertex;v!=NULL;v=v->next)
    {
      if (abs(*v-p)<=err)
	return v;
    }
  return new vertex(p);
}

void make_triangolo(vector3 p1,vector3 p2,vector3 p3,double err=0)
{
  vertex *v1;vertex *v2;vertex *v3;
  v1=find_vertex(p1,err);
  v2=find_vertex(p2,err);
  v3=find_vertex(p3,err);
  new triangle(v1,v2,v3);
}
void make_quadrato(vector3 centro,double lato,int i)
{
  vector3 x,y;
  x=vector3(0,0,0);y=vector3(0,0,0);
  x.x[(i+1)%3]=lato/2.0;
  y.x[(i+2)%3]=lato/2.0;
  make_triangolo(centro-x-y,centro+x-y,centro+x+y,lato/10.0);
  make_triangolo(centro+x+y,centro-x+y,centro-x-y,lato/10.0); 
}
void implicit(vector3 from, vector3 to,double step,double r=0.0)
{
  double x,y;
  vector3 p;
  vector3 dir;
  int i;
  double X,Y,Z;
  
  for (i=0;i<3;i++)
    {
      dir=vector3(0,0,0);
      dir.x[i]=step;
      for (p.x[0]=from.x[0];p.x[0]<to.x[0];p.x[0]+=step)
	for (p.x[1]=from.x[1];p.x[1]<to.x[1];p.x[1]+=step)
	  for (p.x[2]=from.x[2];p.x[2]<to.x[2];p.x[2]+=step)
	    {
	      if (r!=0.0 || abs(p-0.5*(to-from))<=r) 
	      x=i_f(p);
	      y=i_f(p+dir);
	      if (x*y<0.0 || (x==0.0 && y!=0.0))
		{
		  make_quadrato(p+0.5*dir,step,i);
		}
	    }
    }
}
vector3 zero(vector3 v)
{
  return v-0.01*i_f(v)*i_grad(v);
}
double implicit_error(void)
{
  double err=0;
  vertex *v;
  for (v=S.first_vertex;v!=NULL;v=v->next)
    {
      if (fabs(i_f(*v))>err) err=fabs(i_f(*v));
    }
  return err;
}
void implicit_evolve(double eps=0.0001)
{
  vertex *v;
  while (implicit_error()>eps && !interrupt)
    {
      cout<<"ERROR: "<<implicit_error()<<"\n";
      for (v=S.first_vertex;v!=NULL;v=v->next)
	{
	  *(vector3 *)v=zero(*v);
	}
    }
}

// #define IMPLICIT

#ifdef IMPLICIT

vector3 border_function(double t)
{
  return vector3(0,0,0);
}

vertex *new_border_vertex(vertex *v,vertex *w) {
  return new vertex(0.5*(*v+*w));
}
#else
double i_f(vector3 v)
{return 0;};
vector3 i_grad(vector3 v)
{return vector3 (0,0,0);};
#endif
