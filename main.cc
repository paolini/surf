#include <signal.h>
#include <dlfcn.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cassert>

#include "surf.h"
#include "border.h"

struct  sigaction Action;
void handler(int a)
{
  if (surf::interrupt)
    {
      cout<<"Interrupt: exiting...\n";
      exit(0);
    }
  surf::interrupt=1;
}

/*
Border *selected_border = 0;

void init_border(surf &S) {
  selected_border->init_border(S);
}

vector3 border_function(double x) {
  return selected_border->border_function(x);
}

vertex* surf::new_border_vertex(vertex *v, vertex *w) {
  return selected_border->new_border_vertex(*this,v,w);
}
*/

string main_commands[][2] = {
  {"c", "camera"},
  {"t", "triangulate"},
  {"r", "evolve_triangulate"},
  {"R", "evolve_traingulate_radius"},
  {"?", "print_border"},
  {"b", "triangulate_border"},
  {"h", "harmonic"},
  {"E", "evolve_viscosity"},
  {"e", "evolve"},
  {"l", "vertex_list"},
  {"L", "triangle_list"},
  {"a", "auto_zoom"},
  {"k", "check"},
  {"c", "camera"},
  {"p", "write_txt"},
  {"w", "write_border_ps"},
  {"m", "print_mode"},
  {"O", "write_pov"},
  {"P", "write_ps"},
  {"G", "write_off"},
  {"X", "quit"},
  {"", ""}
};

string camera_commands[][2] = {
  {"w", "linewidth"},
  {"p", "viewpoint"},
  {"l", "look_at"},
  {"a", "advance"},
  {"+", "zoom_in"},
  {"-", "zoom_out"},
  {"h", "light"},
  {"f", "film_size"},
  {"x", "exit"},
  {"x", "q"},
  {"", ""}
};

char command_prompt(const char* prompt, string commands[][2], bool case_sensitive) {
  string cmd;

  for(;;) {
    cout << prompt;
    getline(cin, cmd);
    cmd.erase(0, cmd.find_first_not_of(" \n\r\t"));
    cmd.erase(cmd.find_last_not_of(" \n\r\t")+1);
    if (!cin) {
      cin.clear();
      cerr<<"Error reading input!\n";
      abort();
    }
    
    if (cmd.size() == 0) continue;

    if (!case_sensitive) {
      for (int i=0; i<cmd.size(); ++i) {
	cmd[i] = tolower(cmd[i]);
      }
    }
    
    for (int i=0; commands[i][0] != ""; ++i) {
      if (cmd == commands[i][1] || cmd == commands[i][0]) {
	return commands[i][0][0];
      }
    }

    cout << "comando sconosciuto '" << cmd << "'..." << endl;
    cin.clear();
    cout << "comandi:\n" << endl;
    for (int i=0; commands[i][0]!=""; i++) {
      cout<< commands[i][0] << ": " << commands[i][1] << endl;
    }
    cout.flush();
  }
}

int main(int argc, char *argv[]) {
  int i;
  double vel=0.5;
  double radius;
  double a,b;
  char c=' ';
  int  print_mode=2;


  Action.sa_handler=handler;
  sigaction(SIGINT,&Action,NULL);

  if (argc!=2) {
    cerr<<"no surface given\n";
    abort();
  }

  string name = argv[1];
  if (name == "--ask") {
    cout<<"Enter border name:";
    cin>>name;
  }
  if (Border::registry.find(name) == Border::registry.end()) {
    cerr<<"invalid border name "<<name<<"\n";
    cerr<<"registered borders:\n";
    for (Border::Registry::const_iterator i=Border::registry.begin();i!=Border::registry.end();++i) {
      cerr<< (i->first) << "\n";
    }
    abort();
  }
  surf S(Border::registry[name]);

  camera view;
  view.adjust();
  view.light(versor(vector3(-3,-2,5)));
  view.pv(vector3(0,-6,3));
  view.look_at(vector3(0,0,0));
  view.up(vector3(0,0,1));
  view.zoom(4.0);
  for (i=0;c!='X';i++)
    {
      if (surf::interrupt)
      {
	int n;
	cout<<"Interrupt: press <X><Enter> to quit or Ctrl-C again.\n";
	cin.clear();
      }
    cout<<"Passo n. "<<i<<"\n";
    cout<<"Triangoli: "<< S.n_triangles() <<", vertici: "<<S.n_vertices();
    cout<<". L'area totale e':"<< S.area() <<", ";
    cout<<"Diametro triangolazione: "<<S.diameter()<<"\n";
    cout<<"Camera: "<<view<<"\n";

    c = command_prompt("surf> ", main_commands, true);

    surf::interrupt = 0;

    switch(c)
      {
      case 't':
	cout<<"Enter radius: ";
	cin>>radius;
	cout<<"\n";
	S.side_triangulate(radius);
	break;
      case 'r':
	cout<<"Enter error: ";
	cin>>a;
	S.evolve_triangulate(a);
	break;
      case 'R':
	cout<<"Enter error: ";
	cin>>a;
	cout<<"Enter radius: ";
	cin >>radius;
	S.evolve_triangulate(a,radius);
	break;
      case '?':
	cout<<"Enter border parameter: ";
	cin>>radius;
	cout<<"Vector3 ("<<radius<<") = "<<S.border_function(radius)<<"\n";
	break;
      case 'b':
	cout<<"Enter radius: ";
	cin>>radius;
	cout<<"\n";
	S.side_triangulate(radius,1);
	break;
      case -1:
	S.center_triangulate();
	break;
      case 'H':
	cout<<"Error: ";
	cin>>a;
	S.harmonic(a);
	break;
      case 'h':
	S.harmonic();
	break;
      case 'E':
	cout<<"Viscosity: ";
	cin>>a;
	cout<<"Error: ";
	cin>>b;
	S.evolve(b,a);
	break;
      case 'e':
	S.evolve();
	break;
      case 'l':
	S.print_vertex_list();
	break;
      case 'L':
	S.print_triangle_list();
	break;
      case 'a':
	cout<<"Automatic zoom... \n";
	S.auto_zoom(view);
	break;
      case 'k':
	S.check();
	break;
      case 'c':
	cout<<"Move camera...\n";
	{
	  vector3 v;
	  do
	    {
	      c = command_prompt("surf camera> ", camera_commands); 
	      switch(c)
		{
		case 'w':
		  cout<<"linewidth=" << vectorial_output::linewidth <<"\n";
		  cout<<"Enter linewidth: ";
		  cin>> vectorial_output::linewidth;
		  break;
		case 'p':
		  cout<<"Enter PV: ";
		  cin>>v;
		  view.pv(v);
		  break;
		case 'l':
		  cout<<"Look at: ";
		  cin>>v;
		  view.look_at(v);
		  break;
		case 'a':
		  view.pv(view.pv()+vel*view.direction());
		  break;
		case '+':
		  view.zoom(1.5);
		  break;
		case '-':
		  view.zoom(1.0/1.5);
		  break;
		case 'h':
		  cout<<"Light direction: ";
		  cin>>v;
		  view.light(v);
		  break;
		case 'f':
		  double w,h;
		  cout<<"width: "; cin>>w;
		  cout<<"height: "; cin>>h;
		  view.film(w,h);
		  break;
		}
	    } while (c != 'x');
	}
	c='z';
	break;
      case 'p': {
	  text_output txt(cout,view);
	  cout<<"text bitmap: "<<txt<<"\n";
	  txt.print(S,print_mode);
	}
      break;
      case 'w':
	cout<<"Printing border...\n";
	cout<<"End : ";
	cin>>b;
	cout<<"Step : ";
	cin>>radius;
	{
	  ofstream file("film.ps",ios::out);
	  ps_output ps(file,view,20.0,27.0);
	  vector3 v;
	  for (a=radius;a<b;a+=radius)
	    {
	      ps.out_line(S.border_function(a),S.border_function(a-radius),
			  ps.linewidth*ps.scaling(S.border_function(a)),color(1,0,0));
	    }
	  cout<<"done!";
	}
	break;
      case 'm':
	{
	  cout<<"Print mode: (0=border, 1=sup, 2=both)";
	  cin>>print_mode;
	}
      break;
      case 'O':
	{
	  const char *filename="film.pov";
	  ofstream file (filename,ios::out);
	  pov_ray ray(file,view);
	  cout<<"Pov-ray: "<<ray<<"\n";
	  ray.print(S,print_mode);
	  cout<<"POV written on file "<<filename<<"\n";
	}
      break;
      case 'P':
	{
	  const char *filename="film.ps";
	  ofstream file(filename,ios::out);
	  ps_output ps(file,view,20.0,27.0);
	  cout<<"Postscript: "<<ps<<"\n";
	  ps.print(S,print_mode);
	  cout<<"PS written on file "<<filename<<"\n";
	}
	break;
      case 'G':
	{
	  string filename("film.off");
	  ofstream file(filename.c_str());
	  S.geomview_off(file);
	  cout<<"OFF written on file "<<filename<<"\n";
	}
	break;
      case 'X':
	cout<<"Exiting...\n";
	break;
      default:
	cout << "comando sconosciuto [" << c << "] " << int(c) << endl; 
	assert(false);
      }
  }
  return 0;
}
