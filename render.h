#ifndef _RENDER_H_
#define _RENDER_H_

#include "vector.h"
#include "vertex.h"

class color: public vector {
public:
  color(double grayscale);
  color(double r,double g, double b);
};

/*
mypv: punto di vista
screen_z: vettore che va da mypv al centro del monitor
screen_x: vettore che va dal centro del monitor al lato destro
screen_y: vettore che va dal centro del monitor al lato sopra
abs(screen_x)=1/2*larghezza del monitor...
*/

class camera {
protected:
  vector mypv;
  vector screen_z,screen_x,screen_y;
  vector mylight;
  vector myup;
  vector look;

public:
  camera(const camera &);
  camera(void);

  void pv(const vector &);
  const vector &pv(void) const;
  void adjust(void);
  void look_at(const vector &);
  const vector &look_at(void) const;
  const vector & direction(void) const;
  void up(const vector &);
  vector up(void) const;
  void film(double width,double height,int mode=0); 
  // mode= 0:fit, 1:resize 
  double X(const vector &) const;
  double Y(const vector &) const;  
  void zoom(double factor);
  void light(const vector &);
  const vector &light(void) const;		 
  double scaling(const vector &point) const;

  friend ostream&operator<<(ostream&,camera&);
};

class bitmap_output:public camera {
protected:
  double xpix,ypix;/*dimensione dei pixel*/
  int mymaxx,mymaxy;
  int myx,myy; 
public: 
  void print(int);

  int scan_x(void);
  int scan_y(void);
  int maxx(void);
  int maxy(void);
  void maxx(int);
  void maxy(int);
  vector scan(void);
  virtual int putpixel(color c)=0;
  bitmap_output(camera &view,int maxx,int maxy,double xpix,double ypix);
  virtual int inc(void);
};

class random_bitmap_output:public bitmap_output
{
public:
  virtual int putpixel(int x,int y,color c)=0;
  virtual color getpixel(int x,int y)=0;  
  random_bitmap_output(camera &view,int maxx,int maxy,double xpix=1,double ypix=1);
  virtual int putpixel(color);
};

class text_output:public bitmap_output
{
protected:
  ostream *out;
  static char gray_scale[];
  static int scale_length;
public:
  text_output(ostream &,camera &,int lines=24,int columns=80,double xpix=1.0,double ypix =2.0);
  virtual int putpixel(color);
};

class vectorial_output: public camera {
public:
  virtual void out_triangle(vector,vector,vector,color)=0; 
  virtual void out_line(vector,vector,double width,color)=0;
  virtual void out_triangle(triangle &, int print_mode);
  vectorial_output(const camera &view);
  virtual void print(int print_mode);
};

class ps_output:public vectorial_output
{
private:
  ostream *out;
  double page_width,page_height;//in cm
  double width,height;//in cm
  double left_margin,lower_margin;

public:  
  virtual void out_triangle(vector,vector,vector,color);
  virtual void out_line(vector,vector,double,color);
  void out_color(color);
  ps_output(ostream &out,camera &view,double width=20.0, double height=26.0,
	    double page_width=21.0,double page_height=27.0);
  ~ps_output();
  double ps_x(double x);
  double ps_y(double y);
};

class pov_ray:public vectorial_output
{
private:
  ostream *out;
public:
  virtual void out_triangle(vector,vector,vector,color);
  virtual void out_triangle(triangle &, int print_mode);
  virtual void out_line(vector,vector,double,color);
  virtual void print(int print_mode);
  void out_color(color);
  void out_vector(vector);
  pov_ray(ostream &out,camera &view);
  ~pov_ray();
};



#endif
