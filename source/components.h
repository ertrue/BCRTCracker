#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_
// bacallet simulater: basic components:

#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <list>
#include <math.h>
#include <iostream>
#include <string>

#define _BANKER_   'b'
#define _PLAYER_   'p'
#define _TIE_      't'
#define _N_TIE_    'T'
#define _N_BANKER_ 'B'
#define _N_PLAYER_ 'P'

#define CARD_NUMBER_PER_DECK 52

// // winner gameSetting:
// #define TIE_GAIN 8
// #define PLAYER_GAIN 1
// #define BANKER_GAIN 0.950
// #define PLAYER_PAIR_GAIN 12
// #define BANKER_PAIR_GAIN 12

#define GENERAL_PERCENTAGE_BANKER 0.458596
#define GENERAL_PERCENTAGE_PLAYER 0.446248
#define GENERAL_PERCENTAGE_TIE    0.095156


#define RATIO_BET(p1,p2,b) {((p1)*(b)-(p2))/(b)/((p1)+(p2))}

extern double TIE_GAIN;
extern double PLAYER_GAIN;
extern double BANKER_GAIN;
extern double PLAYER_PAIR_GAIN;
extern double BANKER_PAIR_GAIN;
 

// card defintion
struct card{
   int number;
   char suit;
   int operator+(card const & right) const {return this->number+right.number;}
   card(int _n, char _s):number(_n), suit(_s){}
   card():number(0), suit(0){}
   int point() const {return (number>=10)?0:number;}
   char numberChar() const {return (number>9)?(number-10+'A'):(number+'0');}
};

// Record definition
struct Record {
   unsigned int totalCnt;
   unsigned int playerPair;
   unsigned int bankerPair;
   unsigned int banker;
   unsigned int player;
   unsigned int tie;
public:
   Record():totalCnt(0),playerPair(0),bankerPair(0),banker(0),player(0),tie(0) {}
   Record operator+=(Record const & other) {
      totalCnt += other.totalCnt;
      playerPair += other.playerPair;
      bankerPair += other.bankerPair;
      player += other.player;
      banker += other.banker;
      tie += other.tie;
      return *this;
   }
   float playerRatio () const {return (totalCnt==0)?0:(float)player/totalCnt;}
   float bankerRatio () const {return (totalCnt==0)?0:(float)banker/totalCnt;}
   float tieRatio () const {return (totalCnt==0)?0:(float)tie/totalCnt;}
   float playerPairRatio () const {return (totalCnt==0)?0:(float)playerPair/totalCnt;}
   float bankerPairRatio () const {return (totalCnt==0)?0:(float)bankerPair/totalCnt;}
};

struct GameSetting
{
   float resCardsPortion;
   int initMoney;
   int maxHand;
   int minHand;
   int testRound;
   float threshold;
private:
   std::string const resCardPerGameName;
   std::string const initMoneyName;
   std::string const maxHandName;
   std::string const minHandName;
   std::string const testPerBetName;
   std::string const betBigThresholdName;
   std::string const tieGainName;
   std::string const playerGainName;
   std::string const bankerGainName;
   std::string const playerPairGainName;
   std::string const bankerPairGainName;

public:
   GameSetting():
      resCardPerGameName("ReservedCardPortionPerGame")
      , initMoneyName("InitialMoney")
      , maxHandName("BetOnMaxHand")
      , minHandName("BetOnMinHand")
      , testPerBetName("TestRoundsPerBet")
      , betBigThresholdName("BetBigThreshold")
      , tieGainName("TIEGAIN")
      , playerGainName("PLAYERGAIN")
      , bankerGainName("BANKERGAIN")
      , playerPairGainName("PLAYERPAIRGAIN")
      , bankerPairGainName("BANKERPAIRGAIN")
   { setupDefault(); }
   void outputTemplate(std::string settingTemplate) const;
   bool load(std::string gameSettingfile="");
   void reportError() const;
   void setupDefault();
   bool checkValid() const;
   bool isAnalytical() const {return !testRound;}
   void output(FILE *fp) const {
      fprintf(fp, "#### Game Setting ####\n");
      fprintf(fp, "#%s: %.2f\n", resCardPerGameName.c_str(), resCardsPortion);
      fprintf(fp, "#%s: %d\n", initMoneyName.c_str(), initMoney);
      fprintf(fp, "#%s: %d\n", maxHandName.c_str(), maxHand);
      fprintf(fp, "#%s: %d\n", minHandName.c_str(), minHand);
      fprintf(fp, "#%s: %d\n", testPerBetName.c_str(), testRound);
      fprintf(fp, "#%s: %.3f\n", betBigThresholdName.c_str(), threshold);
      fprintf(fp, "#%s: %.3f\n", tieGainName.c_str(), TIE_GAIN);
      fprintf(fp, "#%s: %.3f\n", playerGainName.c_str(), PLAYER_GAIN);
      fprintf(fp, "#%s: %.3f\n", bankerGainName.c_str(), BANKER_GAIN);
      fprintf(fp, "#%s: %.3f\n", playerPairGainName.c_str(), PLAYER_PAIR_GAIN);
      fprintf(fp, "#%s: %.3f\n", bankerPairGainName.c_str(), BANKER_PAIR_GAIN);
   }
};



// gambler

struct gamblerSim {
   
private:
   int gameRounds;
   int bet_one_hand;
   int bet_bnk_hand;
   float THRESHOLD;      // place the bet only if the expected gain is over two percent
   int moneypool;
   
   int totalCardCnt;
   
   std::vector<card> cardBench;
   std::list<card> unknownCards;
   int deckCnt;
public:
   float pP;
   float bP;
   float tP;
   float pPairP;
   float bPairP;
//   float pBonusP;
//   float bBonusP;

private:
   float tieExpectedGain;
   float playerExpectedGain;
   float bankerExpectedGain;
   float bankerPairExpectedGain;
   float playerPairExpectedGain;

public:
   gamblerSim(int _deckCnt, GameSetting setting = GameSetting());
   void initiateNewGame();
   void seeCard(card const c);
   int  placeBet(int bet);   
   void gainProfit(int income);
  
   float getCurrentPairRatio();
   void calculateCurrentWinningPercentage(bool isAnalytical);

   int makeTieBet();
   int makePlayerBet();
   int makeBankerBet();
   int makePlayerPairBet();
   int makeBankerPairBet();
   int makePlayerBonusBet();
   int makeBankerBonusBet();
   void makeMinimumBet(int &t, int &p, int &b, int &pp, int &bp);

   int inline money() const {return moneypool;}
};

Record playCurrentHand(std::vector<card>::iterator & cardIt);
void shuffleFirstSixCards(std::vector<card>::iterator begin, std::vector<card>::iterator end);

struct baccaratSim
{   
   float reservedPotion;
   std::vector<card> cards;
   int deckCnt;
   Record record;

   std::vector<card>::iterator currentIndex;

   int endIndex;

public:
   card initiateNewGame();
   Record playOneHand(std::vector<card> & usedCards);
   bool endGame();

public: // initialization
   baccaratSim(int _deckCnt, GameSetting setting = GameSetting()):deckCnt(_deckCnt) {
      reservedPotion = setting.resCardsPortion;
      for (int i = 0; i < deckCnt; i++) {
         for (int j = 0; j < CARD_NUMBER_PER_DECK; j++) {
            cards.push_back(card(j%13+1, char(j/13+1)));
         }
      }
      currentIndex = cards.begin();
      endIndex = round(_deckCnt*CARD_NUMBER_PER_DECK*(1-reservedPotion));
   }
   
private:
   void shuffle();
   void cut(int cutPos);
   card jumpStart();
};

struct analyzer
{
   std::vector<std::string> profitlog;
   std::vector<std::string> betlog;
   std::vector<std::string> pairbetlog;
   std::vector<std::string> gamelog;
   std::vector<std::string> pairlog;
   std::vector<int> moneylog;
   bool iswipedout;

   double totalGain;

   GameSetting gameSetting;
   
// std::vector<std::string> bRecords;
// std::vector<std::string> pRecords;
// std::vector<std::string> tRecords;
// std::vector<std::string> pPRecord;
// std::vector<std::string> bPRecord;
// std::vector<std::string> wRecords;
// std::vector<std::string> pairRecords;

public:
   analyzer():iswipedout(false), totalGain(1.0){}
   void init() {
      profitlog.clear();
      betlog.clear();
      pairbetlog.clear();
      gamelog.clear();
      pairlog.clear();
      moneylog.clear();
      totalGain = 1.0;
   }
   void play(int deckCnt, int totalRun);
   void simInBrief(int deckCnt, int totalRun, std::string const & outfile);
   void simDetailed(int deckCnt, int totalRun, std::string const & output, bool isDynamic=true);
   double betOnStaticRatio(double const ratio, gamblerSim const & gs, bool const isBanker, Record const & res);
   double betOnDynamicRatio(gamblerSim const & gs, Record const & res);
   bool outputCVS(std::string const & filename);
//   bool outputDetailedReport(std::string const & filename); 
   void outputTemplate(std::string file) const {gameSetting.outputTemplate(file);}
   bool loadSetting(std::string file) {return gameSetting.load(file);}
   void outputSetting() {gameSetting.output(stdout);}
   void reportSettingError() const {gameSetting.reportError();}
   
private:
   void wipeout() {iswipedout = true;}
};


#endif
