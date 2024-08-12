#include <math.h>

#include "piece.h"

int piece::num_pieces=0;
double piece::r=1.0;
piece *piece::border[piece::MAXBOR];

piece::piece(void)
{
  real_t=(double)num_pieces;
  border[num_pieces++]=this;
  if (num_pieces>=MAXBOR)
    cout<<"too many arcs...\n",exit(1);
}

segment::segment(piece *f,piece *t)
{ 
  from=f->point(1);
  to=t->point(0);
}
segment::segment(vector3 f,vector3 t)
{
from=f;
to=t;
}

arc::arc(vector3 c,int from,int ver)
{
  center=c;
  radius=r;
  versus=ver;
  if (from)
    {
      alpha=M_PI;
      beta=2*M_PI;
    }
  else
    {
      alpha=0.0;
      beta=M_PI;
    }
}


double arc::angle(double t)
{
  return alpha+t*fabs(beta-alpha)*(versus?1.0:-1.0);
}

vector3 segment::point(double t)
{
  return (1.0-t)*from+t*to;
}

vector3 xy_arc::point(double t)
{
  return center+radius*vector3(cos(angle(t)),sin(angle(t)),0.0);
}
vector3 xz_arc::point(double t)
{
  return center+radius*vector3(cos(angle(t)),0.0,sin(angle(t)));
}
vector3 yz_arc::point(double t)
{
  return center+radius*vector3(0,cos(angle(t)),sin(angle(t)));
}


curve::curve(vector3 (*f)(double t),vector3 start,double t00,double t10):piece()
{
  t0=t00;t1=t10;
  fun=f;
  pos=start-fun(t0);
}

vector3 curve::point(double t)
{
  t=t*(t1-t0);
  t+=t0;
  return pos+fun(t);
}

