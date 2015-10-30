#include "components.h"

#define DEFAULT_DECK_CNT 8

void outputUsageHelp(char * cmdName)
{
   fprintf(stderr, "Usage: %s outputfile [-g game_number] [-d deck_cnt] [-s gameSetting_file]\n", cmdName);
   fprintf(stderr, "\nOther uage:\n");
   fprintf(stderr, "    -T game_setting_file_template:\n\tOutput gameSettingfile_template. User should fill the gameSetting_file template if default game setting file is not preferred. Delete the line if user does not know what to put or just want to use default setting.\n");
//   std::cout<<"\tdefine the game setting in the format file"<<std::endl;
//   std::cout<<"\t-h to output the format file format"<<std::endl;
//   std::cout<<"\tcmd input_foramt output_analysis"<<std::endl;
}

std::string getGameSettingTemplate(int argc, char ** argv) {
   std::string res;
   for (int i = 1; i < argc-1; ++i) {
      if (strcmp(argv[i], "-T") == 0) {
         res = std::string(argv[i+1]);
      }
   }
   return res;
}

int getRoundTotalCnt(int argc, char ** argv)
{
   int res = 1000;
   for (int i = 2; i < argc-1; ++i) {
      if (strcmp(argv[i], "-g") == 0) {
         res = atoi(argv[i+1]);
         break;
      }
   }
   if (res == 0) {
      res = 1000;
      outputUsageHelp(argv[0]);
   }
   return res;
}

std::string getOutputFile(int argc, char ** argv)
{
   if (argc < 2) {
      outputUsageHelp(argv[0]);
      exit(0);
   }
   return std::string(argv[1]);
}

std::string getGameSettingFile(int argc, char ** argv)
{
   std::string res;
   for (int i = 2; i < argc-1; i++) {
      if (strcmp(argv[i], "-s") == 0) {
         res = std::string(argv[i+1]);
         break;
      }
   }
   return res;
}

int getDeckCnt(int argc, char ** argv)
{
   int res = DEFAULT_DECK_CNT;
   for (int i = 2; i < argc-1; i++) {
      if (strcmp(argv[i], "-d") == 0) {
         res = atoi(argv[i+1]);
         break;
      }
   }
   if (res == 0) {
      res = DEFAULT_DECK_CNT;
      outputUsageHelp(argv[0]);
   }
   return res;

}


int main(int argc, char ** argv)
{
   if (argc < 2) {
      outputUsageHelp(argv[0]);
      return 0;
   }

   int roundTotalCnt = getRoundTotalCnt(argc, argv);
   std::string outputfile = getOutputFile(argc, argv);
   int deckCnt = getDeckCnt(argc, argv);
   std::string gameSettingfile = getGameSettingFile(argc, argv);
   std::string gameSettingtemplate = getGameSettingTemplate(argc, argv);

//   int deckCnt = (argc==4)?atoi(argv[2]):DEFAULT_DECK_CNT;

   analyzer _analyzer;
   
   if (gameSettingtemplate.size()) {
      _analyzer.outputTemplate(gameSettingtemplate);
      exit(0);
   }
   
   if (_analyzer.loadSetting(gameSettingfile)==false) { // error in loading gameSettings
      _analyzer.reportSettingError();
      exit(0);
   }
 
   printf("total round %d simulating...\n", roundTotalCnt);
   _analyzer.outputSetting();
   _analyzer.play(deckCnt, roundTotalCnt);
   return _analyzer.outputCVS(outputfile);
}
