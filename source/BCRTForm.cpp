#include "BCRTForm.h"

char CAPCHAR(char const c) {
   if (c>='a' && c<='z') return c+'A'-'a'; 
   else return c;
}

void Form::drawOnPNG(pngwriter & figure) {
   int circleWidth = max(cellsize/4,1);
   Color const black = Color::black();
   for (int i = 0; i <= column; i++) {
      figure.line(x0+i*cellsize, y0, x0+i*cellsize, y0+h, black.r, black.g, black.b);
   }
   for (int i = 0; i <= row; i++) {
      figure.line(x0, y0+i*cellsize, x0+w, y0+i*cellsize, black.r, black.g, black.b);
   }

   int xorg = x0+cellsize/2;
   int yorg = y0+row*cellsize-cellsize/2;
   for (int c = 0; c < contents.size(); c++) {
      for (int r = 0; r < contents[c].size(); r++) {
         if (contents[c][r].result == 0) continue;
         Color const color = contents[c][r].color();
         if (contents[c][r].isFilled()) {
            figure.filledcircle(xorg+c*cellsize, yorg-r*cellsize, cellsize/2, color.r, color.g, color.b);
         } else {
            for (int i = 1; i<circleWidth; i++) {
               figure.circle(xorg+c*cellsize, yorg-r*cellsize, cellsize/2-i, color.r, color.g, color.b);
            }
         } 
      }
   }
}

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
   int fontSize = 20;
   
   H -= (fontSize+fontSize)*2;

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

   char record[100];
   strcpy(record, "history:");
   strcat(record, history.c_str());
   figure.plot_text(font, fontSize, margin, margin+mainH+space+fontSize+smallRoadForm.getHeight(), 0.0, record, fontColor.r, fontColor.g, fontColor.b);

   int Banker=0,Player=0,Tie=0;
   for (auto c:history) {
      if (c=='B'|| c=='b') Banker++;
      else if (c=='P'|| c=='p') Player++;
      else if (c=='T'|| c=='t') Tie++;
   }
   sprintf(record, "Banker: %d, Player: %d, Tie: %d", Banker, Player, Tie);
   figure.plot_text(font, fontSize, margin, margin+mainH+space*2+fontSize*2+smallRoadForm.getHeight(), 0.0, record, fontColor.r, fontColor.g, fontColor.b);
   
   figure.close();
}


