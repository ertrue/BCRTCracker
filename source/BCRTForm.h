#ifndef BCRT_FORM_H
#define BCRT_FORM_H


#include <pngwriter.h>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
#include <ctime>
#include <cstdlib>


struct Color {
   double r;
   double g;
   double b;
   Color(double R, double G, double B):r(R),g(G),b(B){}
   Color():r(0),g(0),b(0){}
   Color(double grey):r(grey),g(grey),b(grey){}
   
   static Color red(){return Color(1.0,0,0);}
   static Color green(){return Color(0,1.0,0);}
   static Color blue(){return Color(0,0,1.0);}
   static Color black(){return Color(0,0,0);}
   static Color white(){return Color(1.0,1.0,1.0);}
};

struct Cell {
   char result;
   Cell():result(0){}
   Cell(char const r):result(r){}
   
   Color color() {
      if (result == 0) return Color::white();
      else if (result == 'B' || result == 'b') return Color::red();
      else if (result == 'P' || result == 'p') return Color::blue();
      else if (result == 'T' || result == 't') return Color::green();
      return Color::black();
   }
   
   bool isFilled() {
      if (result == 0) return false;
      if (result >= 'A' && result <= 'Z') return true;
      if (result >= 'a' && result <= 'z') return false;
      return false;
   }
};

class Form {
public:
   int column;
   int row;
   vector<vector<Cell> > contents;

   // for painting
public:
   int x0;
   int y0;
   int w;
   int h;
   int cellsize;

public:
   Form(int c=1, int r=1):column(c),row(r) {
      contents.assign(c, vector<Cell>(r, Cell()));
   
      x0 = 0;
      y0 = 0;
      w = 0;
      h = 0;
      cellsize = 0;
   }
 
   inline int getHeight() {return h;}
   inline int getWidth() {return w;}

   void setSize(int x, int y, int W) {
      x0 = x;
      y0 = y;
      cellsize = W/column;
      w = column*cellsize;
      h = row * cellsize;
   }

   void drawOnPNG(pngwriter & figure); 
   virtual void readHistory(string const hist) {
      contents.assign(column, vector<Cell>(row, Cell()));
   }
};


class MainForm: public Form {
public:
   MainForm():Form(66,22){}

   void readHistory(string const hist);
};



class RoadForm: public Form {
   int roadSize;
public:
   RoadForm(int road):Form(20,6),roadSize(road){}
   void readHistory(string const hist);
};

class BCRTForm {
  
   string history;

   MainForm mainForm;
   RoadForm smallRoadForm;
   RoadForm midRoadForm;
   RoadForm bigRoadForm;
  
public:
   BCRTForm():smallRoadForm(4), midRoadForm(5), bigRoadForm(6){}

   void readHistory(string const hist);
   void drawOnPNG(int W, int H, char const * name);
   void drawOnPNG(int W, int H, string name) {
      drawOnPNG(W, H, name.c_str());
   }
   
};

#endif
