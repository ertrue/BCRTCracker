/********************************* PNGwriter **********************************
*
*   Website: Main:             http://pngwriter.sourceforge.net/
*            GitHub.com:       https://github.com/pngwriter/pngwriter
*            Sourceforge.net:  http://sourceforge.net/projects/pngwriter/
*
*
*    Author:                    Paul Blackburn https://github.com/individual61
*                               Axel Huebl https://github.com/ax3l
*
*    Email:                     individual61@users.sourceforge.net
*
*    Version:                   0.5.5 (August 2015)
*
*    Description:               Library that allows plotting a 48 bit
*                               PNG image pixel by pixel, which can
*                               then be opened with a graphics program.
*
*    License:                   GNU General Public License
*                               (C) 2002-2015 Paul Blackburn
*                               (C) 2013-2015 Axel Huebl
*
******************************************************************************/

/*
 * ######################################################################
 *  This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 * ######################################################################
 * */

#include <pngwriter.h>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
#include <ctime>
#include <cstdlib>

#define circleWidth 10

char CAPCHAR(char const c) {
   if (c>='a' && c<='z') return c+'A'-'a'; 
   else return c;
}


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

   void drawOnPNG(pngwriter & figure) {

      Color const black = Color::black();
      for (int i = 0; i <= column; i++) {
         figure.line(x0+i*cellsize, y0, x0+i*cellsize, y0+h, black.r, black.g, black.b);
      }
      for (int i = 0; i <= row; i++) {
         figure.line(x0, y0+i*cellsize, x0+w, y0+i*cellsize, black.r, black.g, black.b);
      }

      for (int c = 0; c < contents.size(); c++) {
         for (int r = 0; r < contents[c].size(); r++) {
            if (contents[c][r].result == 0) continue;
            Color const color = contents[c][r].color();
            if (contents[c][r].isFilled()) {
               figure.filledcircle(x0+c*cellsize+cellsize/2, y0+r*cellsize+cellsize/2-1, cellsize/2, color.r, color.g, color.b);
            } else {
               for (int i = 1; i<circleWidth; i++) {
                  figure.circle(x0+c*cellsize+cellsize/2, y0+r*cellsize+cellsize/2, cellsize/2-i, color.r, color.g, color.b);
               }
            } 
         }
      }
   }

   virtual void readHistory(string const hist) {
      contents.assign(column, vector<Cell>(row, Cell()));
   }
};


class MainForm: public Form {
public:
   MainForm():Form(66,22){}

   void readHistory(string const hist);
};

void MainForm::readHistory(string const hist)
{
   contents.assign(column, vector<Cell>(row, Cell()));
   int curCol = 0;
   int curRow = 0;
   char curRes = 'T';

   for (int i = 0; i < hist.size(); i++) {
      char now = CAPCHAR(hist.at(i));
      if (curRes != 'T' && now != curRes && (now == 'B' || now == 'P')) {
         curRow = 0;
         curCol++;
      }
      contents[curCol][curRow].result = hist.at(i);
      curRow++;
      if (now == 'B' || now == 'P')
         curRes = now;
   }

}

class RoadForm: public Form {
   int roadSize;
public:
   RoadForm(int road):Form(20,6),roadSize(road){}
   void readHistory(string const hist);
};


void RoadForm::readHistory(string const hist)
{
   contents.assign(column, vector<Cell>(row, Cell()));
   int curCol = 0;
   int curRow = 0;
   for (int i = 0; i < hist.size(); i++) {
      curCol = i/roadSize;
      curRow = i%roadSize;
      contents[curCol][curRow].result = hist.at(i);
   }
}


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
   
};

void BCRTForm::readHistory(string const hist)
{
   history = hist;
   smallRoadForm.readHistory(hist);
   midRoadForm.readHistory(hist);
   bigRoadForm.readHistory(hist);
   mainForm.readHistory(hist);
}

void BCRTForm::drawOnPNG(int W, int H, char const * name) {

   char font[] = "/Library/Fonts/Courier New Bold.ttf";
   Color fontColor = Color::black();
//int fontSize = W/(history.size()+15);
   int fontSize = 20;//W/(history.size()+15);
   
   H -= fontSize+fontSize;

   int unitx = W/(mainForm.column+4);
   int unity = H/(mainForm.row+midRoadForm.row+6);

   int const space = min(unitx, unity);
   int const margin = space*2;
   mainForm.setSize(margin, margin, W-2*margin);

   int mainH = mainForm.getHeight();
   int mainW = mainForm.getWidth();
   int roadW = (mainW-2*space)/3;
   smallRoadForm.setSize(margin,                margin+mainH+space, roadW);
   midRoadForm.setSize  (margin+roadW+space,    margin+mainH+space, roadW);
   bigRoadForm.setSize  (margin+roadW*2+space*2,margin+mainH+space, roadW);

   pngwriter figure(W, H, 1.0, name);

   mainForm.drawOnPNG(figure);
   smallRoadForm.drawOnPNG(figure);
   midRoadForm.drawOnPNG(figure);
   bigRoadForm.drawOnPNG(figure);

   char *record = new char[history.size()+20];
   strcpy(record, "history:");
   strcat(record, history.c_str());
   figure.plot_text(font, fontSize, margin, margin+mainH+space+fontSize+smallRoadForm.getHeight(), 0.0, record, fontColor.r, fontColor.g, fontColor.b);
   figure.close();
   delete [] record;
}

int main(int argc, char ** argv)
{

   BCRTForm form1;

   string history = "BPpbPbPpbpPptPPPbPbpPpPbBPPpppBbPbpPbbtbptBpbBpppBpbbbbBPpbPppBBpPp";
   form1.readHistory(history);
   form1.drawOnPNG(1920, 1080, "form1.png");
   history = "TtpPbbtbptBpbBpppBpbbbbBPpbPppBBpPp";
   form1.readHistory(history);
   form1.drawOnPNG(1920, 1080, "form2.png");

   exit(0);
   //////////////////////////////////////////////////////////////////////
   
   /* one.png
    * This will be a 300x300 image with a black background, called one.png
    * */
   pngwriter one(300,300,0,"one.png");
   
   std::cout << "Generating one.png...";
   
   /* Purple rectangle (filled)
    * Make a purple filled retangle. Notice we are using
    * int colour levels, e.g. 65535 is the max level of green.
    * */ 
   one.filledsquare(30,5,45,295,65535,0,65535);
   
   /* Blue rectangle
    * Make a blue rectangle over the green one.
    * Notice that we are using colour coefficcients of type double. 
    * */
   one.square(20,10,40,290,0.0,0.0,1.0);
   
   /* Yellow circle (filled) and Grey Circle
    * Using colour coefficcients of type double.
    * */
   one.filledcircle(250,250,40,1.0,1.0,0.0);
   one.circle(250,70,30,20000,20000,50000);
   
   /* Lines
    * Draw 16 lines to test all general directions of line().
    * All start from the centre and radiate outwards.
    * The colour is varied to be able to distinguish each line.
    * */
   //Top row
   one.line(150,150,0,300,0,65535,4000);
   one.line(150,150,75,300,0,65535,8000);
   one.line(150,150,150,300,0,65535,12000);
   one.line(150,150,225,300,0,65535,16000);
   one.line(150,150,300,300,0,65535,20000);
   //Right side
   one.line(150,150,300,225,0,65535,24000);
   one.line(150,150,300,150,0,65535,28000);
   one.line(150,150,300,75,0,65535,32000);
   one.line(150,150,300,0,0,65535,36000);
   //Bottom row
   one.line(150,150,225,0,0,65535,40000);
   one.line(150,150,150,0,0,65535,44000);
   one.line(150,150,75,0,0,65535,48000);
   one.line(150,150,0,0,0,65535,52000);
   //Left side
   one.line(150,150,0,75,0,65535,56000);
   one.line(150,150,0,150,0,65535,60000);
   one.line(150,150,0,225,0,65535,64000);

   /*Change the text information in the PNG header
    * */
   one.settext("one.png", "John Cleese", "Test PNG", "pngtest");
    
   std::cout << " done. Writing to disk...";
   one.close();
   std::cout << " done." << std::endl;
  
  
  ////////////////////////////////////////////////////////////////////////////////////
   /*  two.png
    *  This will be a 300x300 image with a black background, it will be called two.png.
    * Note that we are using 0.0 as the background colour and that we are using a string
    * type object as the filename, which can convert itself into a const char * with 
    * filename.c_str().
    * */
   std::cout << "Generating two.png...";
   string filename = "two.png";
   pngwriter two(300,300,0.0,filename.c_str()); 
 
   /* Gradients
    * Here we will draw a few gradients, which are nothing more than drawing a filledsquare 
    * or line or filledcircle repeatedly, changing the colour as we go.
    * */
   
   /* Draw the gradient in the lower left corner.
    * */
   for(int iter = 1;iter< 300;iter++) 
     {	
	two.line(1,300,iter,1,65535-int(65535*((double)iter)/300.0), 0,65535);
     }
   
   /* Draw the green circles.
    * */
   for(int iter2 = 0; iter2 < 8; iter2++)
     {
	two.filledcircle(280-25*iter2,250,10,0,65535 - int(65535*((double)iter2/7.0)),0);
     }
   
   /* Draw the other central gradient.
    * Notice that it is drawing a filled square, and moving the upper right corner down
    * and left, while changind the colour.
    * */
   for(int iter3 = 120; iter3>0;iter3--)
     {
	two.filledsquare(70,70,70+iter3,70+iter3,0,0,65535 -int(65535*((double)iter3/120.0 )))	;	
     }

   /* Draw the orange circle gradient.
    * */
   for(int iter4 = 0; iter4<31; iter4++)
     {
	two.filledcircle(250,200,30-iter4,1.0,1.0 - double(iter4)/30.0,0.0);
     }
   
   
   /* Now, just as an example, we will use a lower compression on this image.
    * The default is 6 (from 0 to 9) and we will set it to 3. The lower the compression used
    * the faster the image will be close()d. Complex images will take longer to 
    * close() than simple ones.
    * */
   two.setcompressionlevel(3);
   std::cout << " done. Writing to disk...";
   two.close();
   std::cout << " done." << std::endl;
    
   
   ///////////////////////////////////////////////////////////////////
  
   /* copiaburro.png
    * This section opens burro.png and places it in the pngwriter instance called burro.
    * The PNGwriter instance burro is 1x1 pixels in size because we will use it
    * as a container for another PNG image that we will
    * soon read in. No point in grabbing memory now, so we'll set the width and height to 1.
    * Notice that the original is not altered in any way.
    * */
   pngwriter burro(1,1,0,"copiaburro.png"); 
  
   /* readfromfile()
    * Now we specify the path to the file we wish to place into 
    * the PNGwriter instance "burro":
    * */
   std::cout << "Opening burro.png...";   
   burro.readfromfile("burro.png"); //It really is that easy.
   std::cout << " done." << std::endl;
   
   /* getwidth() and getheight()
    * We may not know the size of the image we just read in. 
    * We find out like this:
    * */
   int burrowidth = burro.getwidth();
   int burroheight = burro.getheight();
  
   
   std::cout << "The image that has just been read from disk (burro.png) is " << burro.getheight();
   std::cout << " pixels high and " << burro.getwidth()<< " pixels wide." << std::endl;
   std::cout << "Bit depth is " << burro.getbitdepth() << std::endl;
   std::cout << "Image gamma is: " << burro.getgamma() << std::endl; 
  
   /* Cover the Donkey
    * We now draw a red circle over the donkey, just to prove that the image was read in correctly,
    * and that it was manipulated.
    * */
   burro.filledcircle((int)(burrowidth/3.0),(int)(2.0*burroheight/3.0),(int)(burrowidth/5),65535,0,0);

   /* close()
    * Must close this instance to have it written to disk.
    * */
   std::cout << "Writing to disk...";
   burro.close();
   std::cout << " done." << std::endl; 
 
   /////////////////////////////////////////////////////////////////////
   
   /* Rainbow
    * To demonstrate the use and usefulness of plotHSV(), we will
    * generate a rainbow.
    * We will use the function plotHSV that
    * plots a pixel at x, y with the colour given in the HSV colourspace
    * (hue, saturation, value), instead of the traditional RGB.
    * */ 
   
   std::cout << "\nCreating arcoiris.png...";
   pngwriter arcoiris(300,300,0,"arcoiris.png");
   std::cout << " done." <<endl;
   std::cout << "Plotting...";
   for(int a= 1;a<301;a++)
      for(int b= 1;b<301;b++)
       arcoiris.plotHSV(a,b,double(a)/300.0,double(b)/300.0,1.0);

   std::cout << " done." << endl;
   
   arcoiris.setgamma(0.5);
   std::cout << "Writing to disk...";
   arcoiris.close();
   std::cout << " done." << std::endl; 
   /////////////////////////////////////////////////////////////////
   
   /* Random filled blended triangles.*/
   
   srand ( time(NULL) );
   
   std::cout << "\nCreating triangles.png...";
   pngwriter triangles(400,400,0,"triangles.png");
   std::cout << " done.";
   double opacityrange = 0.3;
   
   std::cout << "\nPlotting triangles...";
   for(int jj = 0; jj < 10;  jj++)
     {
	
	//This creates the blue triangle outlines.
	triangles.triangle(
				       ((int) rand()%500) - 50,
				       ((int) rand()%500) - 50, 
				       ((int) rand()%500) - 50, 
				       ((int) rand()%500) - 50,
				       ((int) rand()%500) - 50, 
				       ((int) rand()%500) - 50, 
				       0.0, 0.0, 1.0);
     }
   
   for(int ii = 0; ii < 20;  ii++)
     {
	//This creates the red blended triangles
	triangles.filledtriangle_blend(
				       ((int) rand()%500) - 50,
				       ((int) rand()%500) - 50, 
				       ((int) rand()%500) - 50, 
				       ((int) rand()%500) - 50,
				       ((int) rand()%500) - 50, 
				       ((int) rand()%500) - 50, 
				       0.4 + ((double)(rand()/RAND_MAX))*opacityrange, 
				       1.0, 0.0, 0.0);
     }
   std::cout << " done.";   
   std::cout << "\nWriting to disk...";
   triangles.close();
   std::cout << " done.";
   std::cout << "\n\npngtest has finished. Take a look at the PNG images that have been created!\n";
   return 0;
}
