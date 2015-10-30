#include<assert.h>
#include<vector>
#include<algorithm>
#include<stdlib.h>
#include<iostream>
#include<math.h>

#define _DEALER_   'd'
#define _PLAYER_   'p'
#define _TIE_      't'
#define _N_TIE_    'T'
#define _N_DEALER_ 'D'
#define _N_PLAYER_ 'P'

#define DEFAULT_DECK_CNT 8

void outputUsageHelp()
{
   std::cout<<"Usage of Barac Simmulator: cmd round_number [deck_cnt] outputlog"<<std::endl;
//   std::cout<<"\tdefine the rule in the format file"<<std::endl;
//   std::cout<<"\t-h to output the format file format"<<std::endl;
//   std::cout<<"\tcmd input_foramt output_analysis"<<std::endl;
}

void getPercentageofSimWithCurrentCardBench(float & _t, float & _p, float & _d, 
      std::vector<card> & cardBench)
{
   int totalRunTimes = 1e6;
   baracSimBasic _basicSim(cardBench, totalRunTimes);
   _basicSim.run();
   _t = _basicSim.tiePercentage();
   _p = _basicSim.playerPercentage();
   _d = _basicSim.dealerPercentage();
}

struct baracSimBasic {
   std::vector<card> cards;
   int totalRunTimes;

   int tieCnt;
   int dealerCnt;
   int playerCnt;
public:
   baracSimBasic(std::vector<card> const & _cards, int const runTimes):cards(_cards),totalRunTimes(runTimes),tieCnt(0),dealerCnt(0),playerCnt(0) {}
   char processOneHand();
   void run();
   float tiePercentage() const {return (float)tieCnt/totalRunTimes;}
   float playerPercentage() const {return (float)playerCnt/totalRunTimes;}
   float dealerPercentage() const {return (float)dealerCnt/totalRunTimes;}

private:
   char checkWinner(int const dealer, int const player);
   void shuffle();
   static int myrandom(int i) {return std::rand()%i;}
};

char baracSimBasic::checkWinner(int const dealer, int const player)
{
   if (dealer == player) return _TIE_;
   else if (dealer > player) return _DEALER_;
   else return _PLAYER_;
}

void baracSimBasic::run()
{
   for (int i = 0; i<totalRunTimes; ++i) {
      shuffle();
      char res = processOneHand();
      if (res == _DEALER_ || res == _N_DEALER_) dealerCnt++;
      if (res == _PLAYER_ || res == _N_PLAYER_) playerCnt++;
      if (res == _TIE_    || res == _N_TIE_)    tieCnt++;
   }
}

char processOneHand(std::vector<cards> & cards, int & cardIndex)
{
   // according to wikipedia Chinese version
   int dealer = 0;
   int player = 0;
   player = (player+cards[cardIndex++].point())%10;
   dealer = (dealer+cards[cardIndex++].point())%10;
   player = (player+cards[cardIndex++].point())%10;
   dealer = (dealer+cards[cardIndex++].point())%10;
   if (dealer >= 8 && player >= 8) { // Natural Win;
      return _N_TIE_;
   } 
   else if (dealer >= 8) {
      return _N_DEALER_;
   }
   else if (player >= 8) {
      return _N_PLAYER_;
   } 
   else { 
      // player make decision first and then dealer make the following decision
      card playerCard;
      bool isPlayerFill = false;
      if (player <= 5) {
         playerCard = cards[cardIndex++];
         player = (player+playerCard.point())%10;
         isPlayerFill = true;
      }
      if (  (dealer <= 2) 
            || (dealer == 3 && (!isPlayerFill || playerCard.point()!= 8))
            || (dealer == 4 && (!isPlayerFill || (playerCard.point()>=2 && playerCard.point()<=7)))
            || (dealer == 5 && (!isPlayerFill || (playerCard.point()>=4 && playerCard.point()<=7)))
            || (dealer == 6 && isPlayerFill && playerCard.point()>=6 && playerCard.point()<=7)) 
      {
         dealer = (dealer+cards[cardIndex++].point())%10;
      }
      // check winner
      if (dealer == player) return _TIE_;
      else if (dealer > player) return _DEALER_;
      else return _PLAYER_;
   } 
}

struct baracSim {
   int deckCnt;
   std::vector<card> cards;

   int cardIndex;
   static int const cardNumPerCnt = 52;
   static float const reservedPotion = 0.8;
   std::vector<char> records; // _DEALER_: dealer win, _TIE_: tie, _PLAYER_: player win;

   int endIndex;

public:
   void loadRule(std::string filename);
   void initiateNewGame();
   void processSim();
   void output(std::string output);
   void clearRecord();
   gameRecord currRecord();

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
   char checkWinner(int const dealer, int const player);
   void shuffle();
   void cut(int cutPos);
   void jumpStart();
   void play();
   bool endGame();

   static int myrandom (int i) { return std::rand()%i;}
};

gameRecord baracSim::currRecord()
{
   gameRecord res;
   for (auto it = records.begin(); it != records.end(); ++it) {
      if (*it == _DEALER_ || *it == _N_DEALER_) res.d++;
      if (*it == _PLAYER_ || *it == _N_PLAYER_) res.p++;
      if (*it == _TIE_    || *it == _N_TIE_)    res.t++;
      if (*it == _N_DEALER_) res.D_++;
      if (*it == _N_PLAYER_) res.P_++;
      if (*it == _N_TIE_)    res.T_++;
      res.history += *it;
   }
   return res;
}

char baracSim::checkWinner(int const dealer, int const player)
{
   if (dealer == player) return _TIE_;
   else if (dealer > player) return _DEALER_;
   else return _PLAYER_;
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
      fprintf(fp, "dealer\tplayer\ttie\tdetailedhistory\n");
   }
   int cntDealer=0, cntPlayer=0, cntTie=0;
   for (auto it = records.begin(); it != records.end(); ++it) {
      if      (*it == _DEALER_ || *it == _N_DEALER_) cntDealer++;
      else if (*it == _PLAYER_ || *it == _N_PLAYER_) cntPlayer++;
      else if (*it == _TIE_    || *it == _N_TIE_)    cntTie++;
   }
   fprintf(fp, "%d\t%d\t%d\t", cntDealer, cntPlayer, cntTie);
   for (auto it = records.begin(); it != records.end(); ++it) {
      fprintf(fp, "%c", *it);
   }
   fprintf(fp, "\n");
   fclose(fp);
   return;
}

void baracSim::processSim()
{
   while (!endGame()) {
      int pastIndex = cardIndex;
      char res = processOneHand(cards, cardIndex);
      records.push_back(res);
      for (int i = pastIndex; i < cardIndex; ++i) {
         gamer.seeCard(cards[i]);
      }:e

/*      // according to wikipedia Chinese version
      int dealer = 0;
      int player = 0;
      player = (player+cards[cardIndex++].point())%10;
      dealer = (dealer+cards[cardIndex++].point())%10;
      player = (player+cards[cardIndex++].point())%10;
      dealer = (dealer+cards[cardIndex++].point())%10;
      if (dealer >= 8 && player >= 8) { // Natural Win;
         records.push_back(_N_TIE_);
      } 
      else if (dealer >= 8) {
         records.push_back(_N_DEALER_);
      }
      else if (player >= 8) {
         records.push_back(_N_PLAYER_);
      } 
      else { 
         // player make decision first and then dealer make the following decision
         card playerCard;
         bool isPlayerFill = false;
         if (player <= 5) {
            playerCard = cards[cardIndex++];
            player = (player+playerCard.point())%10;
            isPlayerFill = true;
         }
         if (  (dealer <= 2) 
            || (dealer == 3 && (!isPlayerFill || playerCard.point()!= 8))
            || (dealer == 4 && (!isPlayerFill || (playerCard.point()>=2 && playerCard.point()<=7)))
            || (dealer == 5 && (!isPlayerFill || (playerCard.point()>=4 && playerCard.point()<=7)))
            || (dealer == 6 && isPlayerFill && playerCard.point()>=6 && playerCard.point()<=7)) 
         {
               dealer = (dealer+cards[cardIndex++].point())%10;
         }

         records.push_back(checkWinner(dealer, player));
      }
*/
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

void analyzeRecords(std::vector<gameRecord> const & records, int const deckCnt, std::string const outputfile)
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
   fprintf(fp, "percentage of dealer: %lf%%\n", double(d)/double(d+p+t)*100);
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

void outputRecords(std::vector<gameRecord> const & records, std::string const outputfile)
{
   FILE *fp = fopen(outputfile.c_str(), "w");
   fprintf(fp, "##### new game starts #####\n");
   fprintf(fp, "dealer\tplayer\ttie\t_Dealer\t_Player\t_Tie\tdetailedhistory\n");
   for (auto it = records.begin(); it != records.end(); ++it) {
      fprintf(fp, "%d\t%d\t%d\t%d\t%d\t%d\t%s\n", it->d, it->p, it->t, it->D_, it->P_, it->T_, it->history.c_str());
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

   std::vector<gameRecord> records;

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
