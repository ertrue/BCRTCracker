#include<assert.h>
#include<vector>
#include<algorithm>
#include<stdlib.h>
#include<iostream>
#include<math.h>

#define _BANKER_   'b'
#define _PLAYER_   'p'
#define _TIE_      't'
#define _N_TIE_    'T'
#define _N_BANKER_ 'B'
#define _N_PLAYER_ 'P'

#define DEFAULT_DECK_CNT 8

void outputUsageHelp()
{
   std::cout<<"Usage of Barac Simmulator: cmd round_number [deck_cnt] outputlog"<<std::endl;
//   std::cout<<"\tdefine the rule in the format file"<<std::endl;
//   std::cout<<"\t-h to output the format file format"<<std::endl;
//   std::cout<<"\tcmd input_foramt output_analysis"<<std::endl;
}

struct card{
   int number;
   char suit;
   int operator+(card const & right) {return this->number+right.number;}
   card(int _n, char _s):number(_n), suit(_s){}
   card():number(0), suit(0){}
   int point(){return (number>=10)?0:number;}
   char numberchar() const { return (number>9)?(number-10+'A'):(number+'0');}
};

struct record {
   short unsigned int d;
   short unsigned int p;
   short unsigned int t;
   short unsigned int D_;
   short unsigned int P_;
   short unsigned int T_;
   std::string history;
   std::string cardslist;
public:
   record() {
      d=p=t=D_=P_=T_=0;
   }
};

struct baracSim {
   int deckCnt;
   std::vector<card> cards;

   int cardIndex;
   static int const cardNumPerCnt = 52;
   static float const reservedPotion = 0.8;
   std::vector<char> records; // _BANKER_: banker win, _TIE_: tie, _PLAYER_: player win;

   int endIndex;

public:
   void loadRule(std::string filename);
   void initiateNewGame();
   void processSim();
   void output(std::string output);
   void clearRecord();
   record currRecord();

public: // initialization
   baracSim():cardIndex(0),deckCnt(0),endIndex(0){}
   baracSim(int _deckCnt):deckCnt(_deckCnt),cardIndex(0) {
      for (int i = 0; i < deckCnt; i++) {
         for (int j = 0; j < cardNumPerCnt; j++) {
            cards.push_back(card(j%13+1, char(j/13+1)));
         }
      }
      endIndex = round(_deckCnt*cardNumPerCnt*reservedPotion);
   }

private:
   char checkWinner(int const banker, int const player, bool isNatural=false);
   void shuffle();
   void cut(int cutPos);
   void jumpStart();
   void play();
   bool endGame();

   static int myrandom (int i) { return std::rand()%i;}
};

record baracSim::currRecord()
{
   record res;
   for (auto it = records.begin(); it != records.end(); ++it) {
      if (*it == _BANKER_ || *it == _N_BANKER_) res.d++;
      if (*it == _PLAYER_ || *it == _N_PLAYER_) res.p++;
      if (*it == _TIE_    || *it == _N_TIE_)    res.t++;
      if (*it == _N_BANKER_) res.D_++;
      if (*it == _N_PLAYER_) res.P_++;
      if (*it == _N_TIE_)    res.T_++;
      res.history += *it;
   }
   for (auto c: cards) {
      res.cardslist += c.numberchar();
   }
   return res;
}

char baracSim::checkWinner(int const banker, int const player, bool isNatural)
{
   if (banker == player) return (isNatural)?_N_TIE_:_TIE_;
   else if (banker > player) return (isNatural)?_N_BANKER_:_BANKER_;
   else return (isNatural)?_N_PLAYER_:_PLAYER_;
}

void baracSim::shuffle()
{
   std::srand ( unsigned ( std::time(0) ) );
   std::random_shuffle(cards.begin(), cards.end(), baracSim::myrandom);
}

void baracSim::jumpStart()
{
   card & _firstCard = cards[cardIndex++];
   cardIndex += _firstCard.number;
}

void baracSim::initiateNewGame()
{
   if (deckCnt==0) return;
   cardIndex = 0;
   shuffle();
   cut(rand()%(deckCnt*cardNumPerCnt));
   jumpStart();
}

void baracSim::clearRecord()
{
   records.clear();
}

void baracSim::loadRule(std::string filename)
{
   // need to expand. Currently only default is used.

   return;
}

void baracSim::output(std::string output)
{
   // need to think a way to output the result
   FILE * fp = fopen(output.c_str(), "a");
   if (fp==NULL) {
      assert(0);
   }
   size_t size = ftell(fp);
   if (size==0) {
      fprintf(fp, "##### new game starts #####\n");
      fprintf(fp, "banker\tplayer\ttie\tdetailedhistory\tcards\n");
   }
   int cntBanker=0, cntPlayer=0, cntTie=0;
   for (auto it = records.begin(); it != records.end(); ++it) {
      if      (*it == _BANKER_ || *it == _N_BANKER_) cntBanker++;
      else if (*it == _PLAYER_ || *it == _N_PLAYER_) cntPlayer++;
      else if (*it == _TIE_    || *it == _N_TIE_)    cntTie++;
   }
   fprintf(fp, "%d\t%d\t%d\t", cntBanker, cntPlayer, cntTie);
   // win log history
   for (auto it = records.begin(); it != records.end(); ++it) {
      fprintf(fp, "%c", *it);
   }

   // card shuffle-cut result
   for (auto c:cards) {
      fprintf(fp, "%X", c.number);
   }
   fprintf(fp, "\n");
   fclose(fp);
   return;
}

void baracSim::processSim()
{
   while (!endGame()) {
      // according to wikipedia Chinese version
      int banker = 0;
      int player = 0;
      player = (player+cards[cardIndex++].point())%10;
      banker = (banker+cards[cardIndex++].point())%10;
      player = (player+cards[cardIndex++].point())%10;
      banker = (banker+cards[cardIndex++].point())%10;
      if (banker >= 8 || player >= 8) { // Natural Win;
         records.push_back(checkWinner(banker, player, true));
      } 
      else { 
         // player make decision first and then banker make the following decision
         card playerCard;
         bool isPlayerFill = false;
         if (player <= 5) {
            playerCard = cards[cardIndex++];
            player = (player+playerCard.point())%10;
            isPlayerFill = true;
         }
         if (  (banker <= 2) 
            || (banker == 3 && (!isPlayerFill || playerCard.point()!= 8))
            || (banker == 4 && (!isPlayerFill || (playerCard.point()>=2 && playerCard.point()<=7)))
            || (banker == 5 && (!isPlayerFill || (playerCard.point()>=4 && playerCard.point()<=7)))
            || (banker == 6 && isPlayerFill && playerCard.point()>=6 && playerCard.point()<=7)) 
         {
               banker = (banker+cards[cardIndex++].point())%10;
         }

         records.push_back(checkWinner(banker, player));
      }
   }
}

void baracSim::cut(int cutPos)
{
   if (cutPos == 0) return;
   
   std::vector<card> tempcards(cards.begin(), cards.begin()+cutPos);
   int i = cutPos, j=0;
   for (; i<cards.size(); ++j,++i) {
      cards[j] = cards[i];
   }
   for (i=0; i<cutPos; ++i, ++j) {
      cards[j] = tempcards[i];
   }
}

bool baracSim::endGame()
{
   return (cardIndex >= endIndex);
}

void analyzeRecords(std::vector<record> const & records, int const deckCnt, std::string const outputfile)
{
   // average
   // std_dev
   // min
   // max
   // tie

   FILE *fp = fopen(outputfile.c_str(), "a");
   if (fp == NULL) return;

   long long unsigned int d,p,t;
   d=p=t=0;
   int _max=INT_MIN, _min=INT_MAX;
   
   int const bound = 50;

   std::vector<int> statistic(2*bound+1, 0); // [-bound~bound]

   for (auto it = records.begin(); it != records.end(); ++it) {
      d+=it->d;
      p+=it->p;
      t+=it->t;
      int diff = it->d-it->p;
      _max = std::max(_max, diff);
      _min = std::min(_min, diff);
      if (diff<-bound) statistic.front()++;
      else if (diff > bound) statistic.back()++;
      else statistic[diff+bound]++;
   }
   
   fprintf(fp, "##### statistic data here: %d Decks per round ######\n", deckCnt);
   fprintf(fp, "total play:%llu @ round: %lu\n", d+p+t, records.size());
   fprintf(fp, "average bets per round:%lf\n", double(d+p+t)/records.size());
   fprintf(fp, "percentage of banker: %lf%%\n", double(d)/double(d+p+t)*100);
   fprintf(fp, "percentage of player: %lf%%\n", double(p)/double(d+p+t)*100);
   fprintf(fp, "percentage of tie: %lf%%\n", double(t)/double(d+p+t)*100);
   fprintf(fp, "frequency\t Count\n");
   for (int i = 0; i < statistic.size(); ++i) {
      fprintf(fp, "%d\t%d\n", i-bound, statistic[i]);
   }
   fprintf(fp, "#================================\n");
   fclose(fp);
   return;
}

void outputRecords(std::vector<record> const & records, std::string const outputfile)
{
   FILE *fp = fopen(outputfile.c_str(), "w");
   fprintf(fp, "##### new game starts #####\n");
   fprintf(fp, "banker\tplayer\ttie\t_Banker\t_Player\t_Tie\tdetailedhistory\tcardslist\n");
   for (auto it = records.begin(); it != records.end(); ++it) {
      fprintf(fp, "%d\t%d\t%d\t%d\t%d\t%d\t%s\t%s\n", it->d, it->p, it->t, it->D_, it->P_, it->T_, it->history.c_str(), it->cardslist.c_str());
   }
   fclose(fp);
}

int main(int argc, char ** argv)
{
   if (argc != 4 && argc != 3) {
      outputUsageHelp();
      return 0;
   }

   int roundTotalCnt = atoi(argv[1]);
   std::string outputfile = std::string(argv[argc-1]);
   int deckCnt = (argc==4)?atoi(argv[2]):DEFAULT_DECK_CNT;
   
   printf("total round %d simulating...\n", roundTotalCnt);

   baracSim _bs(deckCnt); // use DEFAULT_DECK_CNT decks for playing. Default

   // skip _bs.loadRule() for now...

   std::vector<record> records;

   for (int i = 0; i < roundTotalCnt; i++) {
      _bs.initiateNewGame();
      _bs.processSim();
      records.push_back(_bs.currRecord());
      _bs.clearRecord();
   }
   
   analyzeRecords(records, deckCnt, outputfile+".log");
   
   if (getenv("DETAILED_LOG")) {
      std::cout<<"Outputing detailed log..."<<std::endl;
      outputRecords(records, outputfile+".cvs");
   }
   
   return 0;
}
