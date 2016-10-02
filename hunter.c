// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "HunterView.h"
#include <time.h>
#include <math.h>
#define REST 1
#define DONT_REST 0
int shouldWeRest(LocationID lastKnownDracLoc, Round lastKnowDracRound, LocationID trail[TRAIL_SIZE]);
int canIReach(HunterView currentView, LocationID dest);
void decideHunterMove(HunterView gameState)
{
  LocationID lastKnownDracLoc = -1;
  Round lastKnowDracRound;
  LocationID trail[TRAIL_SIZE];
  int rest = shouldWeRest(&lastKnownDracLoc, &lastKnowDracRound, LocationID trail[TRAIL_SIZE])
//---------------------------If we decide to use the rest function--------------------------------------------------
  time_t t;
  srand((unsigned) time(&t));
  int numLocations;
  if(giveMeTheRound(gameState)==0){
    registerBestPlay("PA","This is random AI");
    return;
  }
  LocationID *possiblePlaces = whereCanIgo(gameState, &numLocations,TRUE, TRUE, TRUE);
  int randomiser = rand()%(numLocations-1);
  LocationID place = possiblePlaces[randomiser];
  char* randomPlace = idToAbbrev(place);
  registerBestPlay(randomPlace,"This is random AI");
}

int canIReach(HunterView currentView, LocationID dest){//checks if player can reach a specific dest from current location
  Player me = whoAmI(currentView);
  int numLocations;
  LocationID *places;
  int found = 0;
  places = whereCanIgo(currentView, *numLocations, TRUE, TRUE, TRUE);
  for(i = 0 ; i<numLocations ; i++ ){
          if(places[i] == dest){
            found = 1;
          }
  }
  return found;
}

int shouldWeRest(LocationID *lastKnownDracLoc, Round *lastKnowDracRound, LocationID trail[TRAIL_SIZE]){//Checks when we last saw Vampy McVamp and where
  //Also returns 1 when we havent seen Edward Cullen in 5 turns but is subject to change
  int roundCount = 0;
  giveMeTheTrail(gameState, PLAYER_DRACULA, trail[TRAIL_SIZE]);
  while (*lastKnownDracLoc = -1 && *roundCount<TRAIL_SIZE){
    if((trail[roundCount]>=0 && trail[roundCount]=<70)){
      *lastKnownDracLoc = trail[roundCount];
      break;
    }
    if(trail[roundCount]==102){
      *lastKnownDracLoc = trail[roundCount+1];
      break;
    }
    if(trail[roundCount]>102&&trail[roundCount]<108){
      int backtrack = trail[roundCount]-102;
      *lastKnownDracLoc = trail[roundCount+backtrack];
      break;
    }
    if(trail[roundCount]==108){
      *lastKnownDracLoc = CASTLE_DRACULA;
      break;
    }
    roundCount++;
  }
  *lastKnowDracRound = roundCount;
  int currRound = GiveMeTheRound(gameState);
  if(currRound - *lastKnowDracRound >=5){
    return REST;
  }
  return DONT_REST;
}
