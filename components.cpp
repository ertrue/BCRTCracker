#include "components.h"

#define DEBUG_CARD
// debug purpose

void printRemainingCard(std::list<card> const &cards) {
   for (auto &it:cards)
      printf("%c", it.numberChar());
}

// basic 
static int myrandom(int i) {return rand()%i;} 


gamblerSim::gamblerSim(int _deckCnt, GameSetting setting) 
                      : deckCnt(_deckCnt), moneypool(setting.initMoney)
                      , gameRounds(setting.testRound), bet_one_hand(setting.maxHand)
                      , bet_bnk_hand(setting.minHand), THRESHOLD(setting.threshold)
{
   totalCardCnt = _deckCnt * CARD_NUMBER_PER_DECK;
   initiateNewGame();
}

void gamblerSim::initiateNewGame()
{
   tieExpectedGain = playerExpectedGain = bankerExpectedGain = bankerPairExpectedGain = playerPairExpectedGain = 0.0;
   cardBench.clear();
   unknownCards.clear();
   for (int i = 0; i < deckCnt; i++) {
      for (int j = 0; j < CARD_NUMBER_PER_DECK; j++) {
         unknownCards.push_back(card(j%13+1, char(j/13+1)));
      }
   }
}

int gamblerSim::placeBet(int bet)
{
   moneypool -= bet;
   return bet;
}

void gamblerSim::gainProfit(int income)
{
   moneypool += income;
}

void gamblerSim::seeCard(card const c)
{
   for (auto it = unknownCards.begin(); it != unknownCards.end(); ++it) {
      if (it->number == c.number && it->suit == c.suit) {
         unknownCards.erase(it);
         return;
      }
   }
   printf("error in see card %d suit %d\n", c.number, c.suit);
   assert(0);
}

void gamblerSim::calculateCurrentWinningPercentage()
{
   cardBench.assign(unknownCards.begin(), unknownCards.end());
   float tie = 0;
   float player = 0;
   float banker = 0;
   int cardIndex = 0;
   
   std::srand(unsigned (std::time(0)));

   Record totalRecord;

   for (int i = 0; i < gameRounds; ++i) {
      auto it = cardBench.begin();
      shuffleFirstSixCards(cardBench.begin(), cardBench.end());
      totalRecord += playCurrentHand(it);
   }
   pP = totalRecord.playerRatio();
   bP = totalRecord.bankerRatio();
   tP = totalRecord.tieRatio();
   pPairP = totalRecord.playerPairRatio();
   bPairP = totalRecord.bankerPairRatio();
}

int gamblerSim::makeTieBet()
{
   int bet = 0;
   tieExpectedGain = tP * TIE_GAIN - 1 + tP;
   if (tieExpectedGain > THRESHOLD) bet = bet_one_hand;
#ifdef DEBUG_CARD
   if (bet) {
      printf("make *tie* bet on %f marge: ", tieExpectedGain);
      printRemainingCard(unknownCards);
      printf("\n");
   }
#endif
   return placeBet(bet);
}

int gamblerSim::makePlayerBet()
{
   int bet = 0;
   playerExpectedGain = pP * PLAYER_GAIN - bP;
   if (playerExpectedGain > THRESHOLD) bet = bet_one_hand;
#ifdef DEBUG_CARD
   if (bet) {
      printf("make *player* bet on %f marge: ", playerExpectedGain);
      printRemainingCard(unknownCards);
      printf("\n");
   }
#endif
  
   return placeBet(bet);
}

int gamblerSim::makeBankerBet()
{
   int bet = 0;
   bankerExpectedGain = bP * BANKER_GAIN - pP;
   if (bankerExpectedGain > THRESHOLD) bet = bet_one_hand;
#ifdef DEBUG_CARD
   if (bet) {
      printf("make *banker* bet on %f marge: ", bankerExpectedGain);
      printRemainingCard(unknownCards);
      printf("\n");
   }
#endif
     
   return placeBet(bet);
}

int gamblerSim::makePlayerPairBet()
{
   int bet = 0;
   playerPairExpectedGain = pPairP * PLAYER_PAIR_GAIN - 1 + pPairP;
   if (playerPairExpectedGain > THRESHOLD) bet = bet_one_hand;
#ifdef DEBUG_CARD
   if (bet) {
      printf("make *player_pair* bet on %f marge: ", playerPairExpectedGain);
      printRemainingCard(unknownCards);
      printf("\n");
   }
#endif
     
   return placeBet(bet);
}

int gamblerSim::makeBankerPairBet()
{
   int bet = 0;
   bankerPairExpectedGain = bPairP * BANKER_PAIR_GAIN - 1 + bPairP;
   if (bankerPairExpectedGain > THRESHOLD) bet = bet_one_hand;
#ifdef DEBUG_CARD
   if (bet) {
      printf("make *banker_pair* bet on %f marge: ", bankerPairExpectedGain);
      printRemainingCard(unknownCards);
      printf("\n");
   }
#endif
     
   return placeBet(bet);
}

int gamblerSim::makePlayerBonusBet()
{
   int bet = 0;
//   if (pBonusP > PLAYER_BONUS_THREHOLD) bet = bet_one_hand;
      
   return placeBet(bet);
}

int gamblerSim::makeBankerBonusBet()
{
   int bet = 0;
//   if (bBonusP > BANKER_BONUS_THREHOLD) bet = bet_one_hand;
      
   return placeBet(bet);
}

void gamblerSim::makeMinimumBet(int &t, int &p, int &b, int &pp, int &bp)
{
   int i = 0;
   float maxExp= -1;
   if (maxExp < tieExpectedGain) { i = 1; maxExp = tieExpectedGain;}
   if (maxExp < playerExpectedGain) { i = 2; maxExp = playerExpectedGain;}
   if (maxExp < bankerExpectedGain) { i = 3; maxExp = bankerExpectedGain;}
   if (maxExp < playerPairExpectedGain) { i = 4; maxExp = playerPairExpectedGain;}
   if (maxExp < bankerPairExpectedGain) { i = 5; maxExp = bankerPairExpectedGain;}
   
   int bet = placeBet(bet_bnk_hand);

   if (i == 1) t += bet;
   else if (i == 2) p += bet;
   else if (i == 3) b += bet;
   else if (i == 4) pp += bet;
   else if (i == 5) bp += bet;
}

void shuffleFirstSixCards(std::vector<card>::iterator begin, std::vector<card>::iterator end)
{
   int length = end-begin;
   if (length <=6)
      std::random_shuffle(begin, end, myrandom);
   else {
      for (int i = 0; i<6; ++i) {
         int targetIndex = std::rand()%(length-i);
         std::swap(*(begin+i), *(begin+i+targetIndex));
      }
   }     
}

Record playCurrentHand(std::vector<card>::iterator & cardIt)
{
   Record res;
   int banker = 0;
   int player = 0;

   if (cardIt->number == (cardIt+2)->number) res.playerPair++;
   if ((cardIt+1)->number == (cardIt+3)->number) res.bankerPair++;

   player = (player+(cardIt++)->point())%10;
   banker = (banker+(cardIt++)->point())%10;
   player = (player+(cardIt++)->point())%10;
   banker = (banker+(cardIt++)->point())%10;
   if (banker < 8 && player < 8) { // Check Non-Natural Win;
      // player make decision first and then banker make the following decision
      card playerCard;
      bool isPlayerFill = false;
      if (player <= 5) {
         playerCard = *(cardIt++);
         player = (player+playerCard.point())%10;
         isPlayerFill = true;
      }
      if (  (banker <= 2) 
            || (banker == 3 && (!isPlayerFill || playerCard.point()!= 8))
            || (banker == 4 && (!isPlayerFill || (playerCard.point()>=2 && playerCard.point()<=7)))
            || (banker == 5 && (!isPlayerFill || (playerCard.point()>=4 && playerCard.point()<=7)))
            || (banker == 6 && isPlayerFill && playerCard.point()>=6 && playerCard.point()<=7)) 
      {
         banker = (banker+(cardIt++)->point())%10;
      }
   }

   if (banker > player) res.banker++;
   else if (banker < player) res.player++;
   else res.tie++;

   res.totalCnt++;

   return res;
}

//// There goes the baccaratSim

card baccaratSim::initiateNewGame()
{
   assert(deckCnt!=0);
   shuffle();
   cut(rand()%(deckCnt*CARD_NUMBER_PER_DECK));
   return jumpStart();
}

void baccaratSim::shuffle()
{
   std::srand ( unsigned ( std::time(0) ) );
   std::random_shuffle(cards.begin(), cards.end(), myrandom);
}

void baccaratSim::cut(int cutPos)
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

card baccaratSim::jumpStart()
{
   currentIndex = cards.begin();
   card _firstCard = *(currentIndex++);
   currentIndex += _firstCard.number;
   return _firstCard;
}

Record baccaratSim::playOneHand(std::vector<card> & usedCards)
{
   auto before = currentIndex;
   Record rec = playCurrentHand(currentIndex);
   usedCards.assign(before, currentIndex);
   return rec;
}

bool baccaratSim::endGame()
{
   return ( (currentIndex - cards.begin()) > endIndex);
}


void analyzer::play(int deckCnt, int totalRun)
{
   baccaratSim _bs(deckCnt, gameSetting);
   gamblerSim gambler(deckCnt, gameSetting);
   
   std::vector<card> usedCards;
   std::string profithistory, gamehistory, pairhistory, bethistory, pairbethistory;

   for (int i=0; i<totalRun; ++i) {
      gambler.initiateNewGame();
      gambler.seeCard(_bs.initiateNewGame());
      while (!_bs.endGame() && gambler.money()>0) {
         gambler.calculateCurrentWinningPercentage();

         int before = gambler.money();
        
         // place the bet
         int tieBet = gambler.makeTieBet();
         int playerBet = gambler.makePlayerBet();
         int bankerBet = gambler.makeBankerBet();
         int playerPairBet = gambler.makePlayerPairBet();
         int bankerPairBet = gambler.makeBankerPairBet();
         if (gambler.money() == before) {
            // if no clear bet can be make then we need to adopt a minimum bet on
            gambler.makeMinimumBet(tieBet, playerBet, bankerBet, playerPairBet, bankerPairBet);
         }
   
         usedCards.clear();
         Record res = _bs.playOneHand(usedCards);
         
         for (auto it = usedCards.begin(); it != usedCards.end(); ++it)
            gambler.seeCard(*it);

         // calculate Gain
         if (res.player)      gambler.gainProfit(round(playerBet*(1+PLAYER_GAIN)));
         if (res.banker)      gambler.gainProfit(round(bankerBet*(1+BANKER_GAIN)));
         if (res.tie)         gambler.gainProfit(round(tieBet*(1+TIE_GAIN)));
         if (res.playerPair)  gambler.gainProfit(round(playerPairBet*(1+PLAYER_PAIR_GAIN)));
         if (res.bankerPair)  gambler.gainProfit(round(bankerPairBet*(1+BANKER_PAIR_GAIN)));
         
         // record history
         int gain = gambler.money() - before;

         if (gain > gameSetting.maxHand/2) profithistory += 'W';
         else if (gain > 0 ) profithistory += 'w';
         else if (gain == 0) profithistory += 'E';
         else if (gain >= -gameSetting.maxHand/2) profithistory += 'l';
         else  profithistory += 'L';

         // game history
         if (res.player)   gamehistory += 'p';
         else if (res.banker) gamehistory += 'b';
         else gamehistory += 't';
         
         // pair history
         if (res.playerPair && res.bankerPair) pairhistory += '2';
         else if (res.playerPair ) pairhistory += 'P';
         else if (res.bankerPair ) pairhistory += 'B';
         else pairhistory += '.';

         // bet on player history
         if (tieBet != 0) bethistory+='t';
         else if (playerBet != 0) bethistory += 'p';
         else if (bankerBet != 0) bethistory += 'b';
         else bethistory += '.';

         // bet on pair history
         if (playerPairBet != 0 || bankerPairBet != 0) pairbethistory += 'y';
         else pairbethistory += '.';

      }
      profitlog.push_back(profithistory);
      gamelog.push_back(gamehistory);
      pairlog.push_back(pairhistory);
      betlog.push_back(bethistory);
      pairbetlog.push_back(pairbethistory);

      moneylog.push_back(gambler.money());
     
      profithistory.clear();
      gamehistory.clear();
      pairhistory.clear();
      bethistory.clear();
      pairbethistory.clear();
      
      // print progress::
      fprintf(stderr, "\rcurrent progress: %3.2f%%", 100.0*i/totalRun);

      if (gambler.money()<=0) {
         wipeout();
         break;
      }
   }
   fprintf(stderr, "\rcurrent progress: 100.00%%\n");
}



bool analyzer::outputCVS(std::string const & filename)
{
   std::string tabfile = filename + ".cvs";
   FILE * fp = fopen(tabfile.c_str(), "w");
   if (fp == NULL) return false;
   if (iswipedout) fprintf(fp, "#####    wiped out in the end! #####\n");
   else            fprintf(fp, "#####    Has %d money left! #####\n", moneylog.back());
   gameSetting.output(fp);
   fprintf(fp, "money_history\tw/lhistory\tbet_history\tpair_bet_history\tgamehistory\tpairhistory\n");
   for (size_t i = 0; i<profitlog.size(); ++i) {
      fprintf(fp, "%d\t%s\t%s\t%s\t%s\t%s\n", moneylog[i], profitlog[i].c_str(), betlog[i].c_str(), pairbetlog[i].c_str(), gamelog[i].c_str(), pairlog[i].c_str());
   }
   fclose(fp);
   return true;
}

void GameSetting::outputTemplate(std::string settingtemplate) const
{
   if (settingtemplate.size()==0) {
      fprintf(stderr, "GameSetting Template file name is not set up correctly\n");
      return;
   }
   FILE * fp = fopen(settingtemplate.c_str(), "w");
   if (fp == NULL) {
      fprintf(stderr, "Failed to create game setting template file %s\n", settingtemplate.c_str());
      return;
   }

   fprintf(fp, "%s: [0.1 ~ 0.4]\n", resCardPerGameName.c_str());
   fprintf(fp, "%s: xxxx\n", initMoneyName.c_str());
   fprintf(fp, "%s: xxxx\n", maxHandName.c_str());
   fprintf(fp, "%s: xxxx\n", minHandName.c_str());
   fprintf(fp, "%s: [100~100k]\n", testPerBetName.c_str());
   fprintf(fp, "%s: [0 ~ 0.1]\n", betBigThresholdName.c_str());
   fclose(fp);
}

bool GameSetting::load(std::string gameSetting)
{
   setupDefault();
   if (gameSetting=="") {
      // allow load nothing means default gameSetting;
      return true;
   }
   std::ifstream file(gameSetting);
   if (!file) {
      fprintf(stderr, "Failed to open game setting file %s\n", gameSetting.c_str());
      return false;
   }
   std::string line;
   while (getline(file, line)) {
      if (line.compare(0, resCardPerGameName.length(), resCardPerGameName) == 0) {
         resCardsPortion = atof(line.substr(resCardPerGameName.length()+1).c_str());
      } 
      else if (line.compare(0, initMoneyName.length(), initMoneyName) == 0) {
         initMoney = atoi(line.substr(initMoneyName.length()+1).c_str());
      }
      else if (line.compare(0, maxHandName.length(), maxHandName) == 0) {
         maxHand = atoi(line.substr(maxHandName.length()+1).c_str());
      }
      else if (line.compare(0, minHandName.length(), minHandName) == 0) {
         minHand = atoi(line.substr(minHandName.length()+1).c_str());
      }
      else if (line.compare(0, testPerBetName.length(), testPerBetName) == 0) {
         testRound = atoi(line.substr(testPerBetName.length()+1).c_str());
      }
      else if (line.compare(0, betBigThresholdName.length(), betBigThresholdName) == 0) {
         threshold = atof(line.substr(betBigThresholdName.length()+1).c_str());
      }
   }
   file.close();
   return checkValid();
}

void GameSetting::reportError() const
{
   fprintf(stderr, "Please run -T template_file to follow the game setting definition.\n");
}

void GameSetting::setupDefault()
{
   resCardsPortion = 0.2;
   initMoney = 1e6;
   maxHand = 1e3;
   minHand = 20;
   testRound = 1e4;
   threshold = 0.02;
}

bool GameSetting::checkValid() const
{
   return ( (resCardsPortion >= 0.1 && resCardsPortion <= 0.4)
         && (minHand >= 0)
         && (maxHand >= minHand)
         && (initMoney >= maxHand)
         && (testRound >= 1e2 && testRound <= 1e5)
         && (threshold >= 0.0 && threshold <= 0.1) );
}
