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

// winner gameSetting:
#define TIE_GAIN 8
#define PLAYER_GAIN 1
#define BANKER_GAIN 0.95
#define PLAYER_PAIR_GAIN 12
#define BANKER_PAIR_GAIN 12
 

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

public:
   GameSetting():
      resCardPerGameName("ReservedCardPortionPerGame")
      , initMoneyName("InitialMoney")
      , maxHandName("BetOnMaxHand")
      , minHandName("BetOnMinHand")
      , testPerBetName("TestRoundsPerBet")
      , betBigThresholdName("BetBigThreshold")
   { setupDefault(); }
   void outputTemplate(std::string settingTemplate) const;
   bool load(std::string gameSettingfile="");
   void reportError() const;
   void setupDefault();
   bool checkValid() const;
   void output(FILE *fp) const {
      fprintf(fp, "#### Game Setting ####\n");
      fprintf(fp, "#%s: %.2f\n", resCardPerGameName.c_str(), resCardsPortion);
      fprintf(fp, "#%s: %d\n", initMoneyName.c_str(), initMoney);
      fprintf(fp, "#%s: %d\n", maxHandName.c_str(), maxHand);
      fprintf(fp, "#%s: %d\n", minHandName.c_str(), minHand);
      fprintf(fp, "#%s: %d\n", testPerBetName.c_str(), testRound);
      fprintf(fp, "#%s: %.3f\n", betBigThresholdName.c_str(), threshold);
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

private:
   float pP;
   float bP;
   float tP;
   float pPairP;
   float bPairP;
//   float pBonusP;
//   float bBonusP;
  
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
   
   void calculateCurrentWinningPercentage();

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

   GameSetting gameSetting;

public:
   analyzer():iswipedout(false){}
   void init() {
      profitlog.clear();
      betlog.clear();
      pairbetlog.clear();
      gamelog.clear();
      pairlog.clear();
      moneylog.clear();
   }
   void play(int deckCnt, int totalRun);
   bool outputCVS(std::string const & filename);
   void outputTemplate(std::string file) const {gameSetting.outputTemplate(file);}
   bool loadSetting(std::string file) {return gameSetting.load(file);}
   void outputSetting() {gameSetting.output(stdout);}
   void reportSettingError() const {gameSetting.reportError();}
   
private:
   void wipeout() {iswipedout = true;}
};


#endif
