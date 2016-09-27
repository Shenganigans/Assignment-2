// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "HunterView.h"
#include <time.h>
#include <math.h>

void decideHunterMove(HunterView gameState)
{
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
  char* randomPlace = idToName(place);
  registerBestPlay(randomPlace,"This is random AI");
}
