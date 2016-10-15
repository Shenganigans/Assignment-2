// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "HunterView.h"
#include "Map.h"
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
  int roundCount = 5;
  giveMeTheTrail(gameState, PLAYER_DRACULA, trail);
  while (*lastKnownDracLoc == -1 && roundCount>=0){
    if((trail[roundCount]>=0 && trail[roundCount]<=70)){
      *lastKnownDracLoc = trail[roundCount];
      return DONT_REST;
    }
    if(trail[roundCount]==102){
      *lastKnownDracLoc = trail[roundCount-1];
      if(*lastKnownDracLoc>=0 && *lastKnownDracLoc<=70)return DONT_REST;
    }
    if(trail[roundCount]>102&&trail[roundCount]<108){
      int backtrack = trail[roundCount]-102;
      *lastKnownDracLoc = trail[roundCount-backtrack];
      if(*lastKnownDracLoc>=0 && *lastKnownDracLoc<=70)return DONT_REST;
    }
    if(trail[roundCount]==108){
      *lastKnownDracLoc = CASTLE_DRACULA;
      return DONT_REST;
    }
    roundCount--;
  }
  int currRound = giveMeTheRound(gameState);
  if(roundCount>=0||currRound<=6){
    //*lastKnownDracRound=currRound-roundCount;
    return DONT_REST;
  }
  return REST;
}
int findShortestPath(HunterView gameState, LocationID from, LocationID dest, LocationID *path)//untested, need queue.c.h from week 9 lab
{
        int seen[71];
        memset(seen,-1,sizeof(seen));
        int curr;
        int counter = 0;
        Queue nodes = newQueue();
        Queue train = newQueue();
        Map m = newMap();
        QueueJoin(nodes,from);
        int travel = (whoAmI(gameState)+giveMeTheRound(gameState))%4
        QueueJoin(train, travel);
        int numLocations
        while(!QueueIsEmpty(q)){
                curr = QueueLeave(nodes);
                currRail = QueueLeave(train);
                LocationID *possiblepaths = reachableLocations(map, &numLocations, curr, FALSE, currRail, TRUE, TRUE);
                while(counter < 71){
                        if(seen[counter] == -1){
                                  QueueJoin(nodes,counter);
                                  QueueJoin(train,(currRail+1)%4)
                                  seen[counter] = curr;
                        }

                        counter++;
                }
                counter = 0;
        }
        if(seen[dest] == -1) return 0;
        curr = dest;
        int pathLen = 1;
        path[0]=dest;
        while(seen[curr] != from){
                path[pathLen] = seen[curr];
                pathLen++;
                curr = seen[curr];
        }
        if(from != dest){
                path[pathLen]=from;
        }else{
                pathLen--;
        }
        int countswap = 0;
        int holder = 0;
        while(countswap <= pathLen/2){
                holder = path[pathLen-countswap];
                path[pathLen-countswap] = path[countswap];
                path[countswap] = holder;
                countswap++;
        }
        return pathLen+1;
}
