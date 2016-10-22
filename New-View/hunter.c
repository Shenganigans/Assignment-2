// hunter.c
// Implementation of your "Fury of Dracula" hunter AI
// Sheng & Alli


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "Map.h"
#include <time.h>
#include "Game.h"
#include "HunterView.h"
#include "Queue.h"

#define REST 1
#define DONT_REST 0

#define FALSE 0
#define TRUE 1

// Static Functions
static int findShortestPath(HunterView gameState, LocationID from, LocationID dest, LocationID *path);
//static int canIReach(HunterView currentView, LocationID dest);
static LocationID firstMove(PlayerID player);
static LocationID RandomMove(HunterView gameState, PlayerID player, LocationID location);
static LocationID *setUpDraculaTrail(HunterView gameState);
static void initiateFirstMove(PlayerID player);

//static LocationID whereIsImmature(LocationID *dracTrail, Round currentRound, int *urgency); //TODO
/*
static LocationID *whereCouldDracBe(HunterView gameState, LocationID *trail, int *numOfLocations); //TODO
static int willItKillMe (HunterView gameState, LocationID *trail, LocationID to); //TODO
*/

void decideHunterMove(HunterView gameState)
{
  // ********************************************************************************
  // Get general information
  // ********************************************************************************
  PlayerID iAmPlayer = whoAmI(gameState);
  Round currentRound = giveMeTheRound(gameState);
  LocationID whereAmI = whereIs(gameState, iAmPlayer);

  // Check if it is the first round --initiate random moves
  if(currentRound == 0){
    initiateFirstMove(iAmPlayer);
    return;
  }

  // ********************************************************************************
  // Set up Dracula's Trail
  // ********************************************************************************
  LocationID *dracTrail = malloc(sizeof(LocationID)*EXTENDED_TRAIL_SIZE);
  dracTrail = setUpDraculaTrail(gameState);
  LocationID lastKnownDracLoc;
  int i;
  // Search trail for a known location
  for(i = 0; i < EXTENDED_TRAIL_SIZE; i++){ // iterate through trail until a known place is found
    if(dracTrail[i] <= MAX_MAP_LOCATION && dracTrail[i]>=MIN_MAP_LOCATION&& dracTrail[i]) break;
  }
  if(i == EXTENDED_TRAIL_SIZE){ // No part of Dracula's trail is known
    if(currentRound <= 6){ // Impossible to learn D location by resting
       LocationID earlyMove = RandomMove(gameState, iAmPlayer, whereAmI);
       registerBestPlay(idToAbbrev(earlyMove), "Early days...random move");
   }else { // All rest to find the sixth D move
       registerBestPlay(idToAbbrev(whereAmI), "Resting strategically");
   }
     return;
  } else { // A part of dracula's trail is known
  	int positionInTrail = i;
  	lastKnownDracLoc = dracTrail[positionInTrail];
  }

  // ********************************************************************************
  // Converge on Le Dracula
  // ********************************************************************************

    //find the shortest path to drac's last known location
    LocationID *path = malloc(71*sizeof(int));
    int pathLength = findShortestPath(gameState, whereAmI, lastKnownDracLoc, path);//find the shortest path to drac's last known location

    if (pathLength>1){
      registerBestPlay(idToAbbrev(path[1]), "Converging on Dracula");//if the path exists, make the first step there
      return;
    }

  LocationID randomMove = RandomMove(gameState, iAmPlayer, whereAmI);
  registerBestPlay(idToAbbrev(randomMove), "Default Random Move");

  // Find the location of an Immature Vampire
  // or returns UNKNOWN CITY or FALSE (if no imm. vamp has been placed)
  	//int urgency;
    //LocationID immatureLocation = whereIsImmature(dracTrail, currentRound, &urgency);
  //  immatureLocation++;//To remove werror for compilation, remove when using variable
}







// STATIC FUNCTIONS

/* For the first move of the game, spawn hunters evenly
   across the map with one player staking out Castle Dracula*/
static LocationID firstMove(PlayerID player)
	{
  LocationID move;
  switch(player){
    case PLAYER_LORD_GODALMING:
    	move = CASTLE_DRACULA;
    	break;
    case PLAYER_DR_SEWARD:
    	move = BRUSSELS;
    	break;
    case PLAYER_VAN_HELSING:
    	move = VENICE;
    	break;
  	case PLAYER_MINA_HARKER:
    	move = SARAGOSSA;
  	}
  return move;
	}

static LocationID RandomMove(HunterView gameState, PlayerID player, LocationID location)
{
  time_t t;
  srand((unsigned) time(&t));//srand is the function to set the psuedorandom seed, to create a unique random seed everytime we use the current system time
  int numLocations;
  LocationID *moveBank = whereCanIgo(gameState, &numLocations, TRUE, TRUE, TRUE);
  int randomiser = rand()%(numLocations-1);// random number is between [0, numLocations-1]
  return moveBank[randomiser];
}

/*static LocationID whereIsImmature(LocationID *dracTrail, Round currentRound, int *urgency)
{
  //TODO
  return 0;
}*/

/* FUNCTION: setUpDraculaTrail RETURNS: pointer to trail
   Retrieves Dracula's trail and modifies it so that TELEPORTs become CASTLE_DRACULA
   and takes into account HIDES and DOUBLE BACKs
*/
static LocationID *setUpDraculaTrail(HunterView gameState){
  LocationID *dracTrail = malloc(sizeof(LocationID)*EXTENDED_TRAIL_SIZE);
  giveMeTheExtendedTrail(gameState, PLAYER_DRACULA, dracTrail);
  int i;
  // Change all TELEPORTs to CASTLE_DRACULA
  for(i = 0; i < EXTENDED_TRAIL_SIZE; i++){
    if(dracTrail[i]== TELEPORT){ // converts TELEPORT to CD
      dracTrail[i] = CASTLE_DRACULA;
    }
  }
  // Takes into account DBs and HIDEs
  for(i = 0; i < EXTENDED_TRAIL_SIZE; i++){
    if(dracTrail[i] >= DOUBLE_BACK_1 && dracTrail[i] <= DOUBLE_BACK_5){
      int DBindex = dracTrail[i] - DOUBLE_BACK_1 + 1; // converts DBs to the actual city
      dracTrail[i] = dracTrail[DBindex]; 							// if known, otherwise -- C? or S?
      if(dracTrail[i] == HIDE && DBindex != (EXTENDED_TRAIL_SIZE - 1))
        dracTrail[i] = dracTrail[DBindex + 1];				// converts a DB to a HIDE to the city
      continue;																				// (not sure if this will ever occur but just in case)
    }
    if(dracTrail[i] == HIDE && i != 6)
      dracTrail[i] = dracTrail[i + 1]; 	// converts a HIDE to the preceeding city (could be S? or C? if unknown)
  }
  int k;
  printf("The extended trail is: ");
  for(k=0;k<EXTENDED_TRAIL_SIZE;k++){
    printf("[%d] ",dracTrail[k]);
  }
  printf("\n" );
  return dracTrail;
}

static void initiateFirstMove(PlayerID player){
  LocationID entry = firstMove(player);
  char *message = malloc(30);
  strcpy(message, "Just spawned in ");
  strcat(message, idToName(entry));
  registerBestPlay(idToAbbrev(entry), message);
}

static int findShortestPath(HunterView gameState, LocationID from, LocationID dest, LocationID *path)//Currently only works if From is your current location
//Path needs to be passed in
{
  int seen[71];//Setting up backtrace array
  memset(seen,-1,sizeof(seen));//Setting values to -1, marking not seen
  int curr;
  int currRail;
  int counter = 0;
  Queue nodes = newQueue();
  Queue train = newQueue();
  Map m = newMap();//Create a map to traverse
  QueueJoin(nodes,from);//Push on current location
  int travel = (whoAmI(gameState)+giveMeTheRound(gameState))%4;
  QueueJoin(train, travel);//Push on current rail length
  int numLocations;
  seen[whereIs(gameState, whoAmI(gameState))]=whereIs(gameState, whoAmI(gameState));
  while(!QueueIsEmpty(nodes)){
          curr = QueueLeave(nodes);//Pop next location off Queue
          currRail = QueueLeave(train);//Pop next rail length off Queue
          LocationID *possiblepaths = reachableLocations(m, &numLocations, curr, FALSE, currRail, TRUE, TRUE);//Get possible paths with rail length in account
          while(counter < numLocations){
                    if(seen[possiblepaths[counter]] == -1){
                            QueueJoin(nodes,possiblepaths[counter]);//Pop any new locations to Queue
                            QueueJoin(train,(currRail+1)%4);//Pop the corresponding possible rail length to rail Queue

                            seen[possiblepaths[counter]] = curr;//Set unvisited locations with values corresponding to the location we visited them from in the BST
                    }
                  counter++;
          }
          counter = 0;
  }
  if(seen[dest] == -1) return 0;//If the destination is unreached, early exit
  curr = dest;
  int pathLen = 1;
  path[0]=dest;
  while(seen[curr] != from && pathLen<=71){//Checks how long the shortest distance is (Possibly infinite loop if doesnt reach dest)
          path[pathLen] = seen[curr];//Fills the path array with the shortest path, but backwards due to the nature of a backtrace array
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
  while(countswap <= pathLen/2){//Flip the backward array
          holder = path[pathLen-countswap];
          path[pathLen-countswap] = path[countswap];
          path[countswap] = holder;
          countswap++;
  }
  return pathLen+1;//Return the length of the shortest path, the actual path is returned through the path array.
}
