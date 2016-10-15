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
int shouldWeRest(HunterView gameState, LocationID *lastKnownDracLoc, LocationID *trail);

void decideHunterMove(HunterView gameState)
{
  PlayerID me = whoAmI(gameState);
  LocationID whereIAm = whereIs(gameState, me);
  LocationID lastKnownDracLoc = -1;
  LocationID *trail=malloc(sizeof(LocationID)*TRAIL_SIZE);
  int rest = shouldWeRest(gameState, &lastKnownDracLoc, trail);
  if(rest == REST){
    registerBestPlay(idToAbbrev(whereIAm),"Resting");
    return;
  }
  time_t t;
  srand((unsigned) time(&t));
  int numLocations;
//When the game just starts
  if(giveMeTheRound(gameState)==0){
    if(whoAmI(gameState)==0){
      registerBestPlay("KL","This is spawn");//spawn near Castle Dracula
    }else{//spawn randomly otherwise (could be changed to better fixed spawn)
      int randSpawn = rand()%(70-1);
      char* spawn = idToAbbrev(randSpawn);
      registerBestPlay(spawn,"This is random spawn");
    }
    return;
  }
  //random moves
  LocationID *possiblePlaces = whereCanIgo(gameState, &numLocations,TRUE, TRUE, TRUE);
  int randomiser = rand()%(numLocations-1);
  LocationID place = possiblePlaces[randomiser];
  char* randomPlace = idToAbbrev(place);
  registerBestPlay(randomPlace, "Random Move");
}


int shouldWeRest(HunterView gameState, LocationID *lastKnownDracLoc, LocationID *trail){//Checks when we last saw Vamp and where
  //Also returns REST when we havent seen Cullen in 6 turns
  int roundCount = 0;
  giveMeTheTrail(gameState, PLAYER_DRACULA, trail);
  while (*lastKnownDracLoc == -1 && roundCount<TRAIL_SIZE){
    if((trail[roundCount]>=0 && trail[roundCount]<=70)){
      *lastKnownDracLoc = trail[roundCount];
      break;
    }
    if(trail[roundCount]==102){
      *lastKnownDracLoc = trail[roundCount-1];
      break;
    }
    if(trail[roundCount]>102&&trail[roundCount]<108){
      int backtrack = trail[roundCount]-102;
      *lastKnownDracLoc = trail[roundCount-backtrack];
      break;
    }
    if(trail[roundCount]==108){
      *lastKnownDracLoc = CASTLE_DRACULA;
      break;
    }
    roundCount++;
  }
  int currRound = giveMeTheRound(gameState);
  if(roundCount < 6||currRound<6){
    //*lastKnownDracRound=currRound-roundCount;
    return DONT_REST;
  }
  return REST;
}
