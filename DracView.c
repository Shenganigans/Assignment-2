#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "DracView.h"


#define TOTAL_PLAY_SIZE 8
#define TOTAL_ROUND_SIZE 40
#define ROUND_SIZE 40

struct dracView {
    GameView gameView;
    int traps[NUM_MAP_LOCATIONS];
    int vampires[NUM_MAP_LOCATIONS];
    LocationID realLocation;
};

LocationID realLocation (char *pastPlays, int locationIndex);

// Creates a new DracView to summarise the current state of the game
DracView newDracView(char *pastPlays, PlayerMessage messages[])
{
    int i = 0;
    int thisPlay = 0;

    DracView dracView = malloc(sizeof(struct dracView));
    if (dracView == NULL) {
        fprintf(stderr, "Memory allocation for drac view failed.\n");
        exit(EXIT_FAILURE);
    }

    dracView->gameView = newGameView(pastPlays, messages);

    //Initialise traps
    for(i = 0; i < NUM_MAP_LOCATIONS; i++) dracView->traps[i] = 0;
    //Initialise vampires
    for(i = 0; i < NUM_MAP_LOCATIONS; i++) dracView->vampires[i] = 0;

    while (thisPlay < strlen(pastPlays)) {
       int action = thisPlay + 3;

       LocationID currentLocation = realLocation(pastPlays, thisPlay + 1);


       if (pastPlays[thisPlay] == 'D') {
           if (pastPlays[action] == 'T') {
              dracView->traps[currentLocation]++;
           }

           if (pastPlays[action + 1] == 'V') {
               dracView->vampires[currentLocation]++;
           }

           if (pastPlays[action + 2] == 'M') {
               dracView->traps[realLocation(pastPlays, (thisPlay + 1) - (TOTAL_ROUND_SIZE * 6))]--;
           }

           if (pastPlays[action + 2] == 'V') {
               dracView->vampires[realLocation(pastPlays, (thisPlay + 1) - (TOTAL_ROUND_SIZE * 6))]--;
           }
           dracView->realLocation = currentLocation;
       } else {
           while (action % TOTAL_PLAY_SIZE != 0) {
               if (pastPlays[action] == 'T') {
                   dracView->traps[currentLocation]--;
              } else if (pastPlays[action] == 'V') {
                   dracView->vampires[currentLocation]--;
                }
              action++;
           }
       }
       thisPlay += TOTAL_PLAY_SIZE;
   }

   return dracView;
}


LocationID realLocation (char *pastPlays, int locationIndex) {
    char abbrev[3] = {pastPlays[locationIndex], pastPlays[locationIndex + 1],'\0'};
     if (strcmp(abbrev, "HI") == 0 || strcmp(abbrev, "D1") == 0) {
         return realLocation(pastPlays, locationIndex - TOTAL_ROUND_SIZE);
   } else if (strcmp(abbrev, "D2") == 0) {
         return realLocation(pastPlays, locationIndex - TOTAL_ROUND_SIZE * 2);
   } else if (strcmp(abbrev, "D3") == 0) {
          return realLocation(pastPlays, locationIndex - TOTAL_ROUND_SIZE * 3);
   } else if (strcmp(abbrev, "D4") == 0) {
         return realLocation(pastPlays, locationIndex - TOTAL_ROUND_SIZE * 4);
   } else if (strcmp(abbrev, "D5") == 0) {
         return realLocation(pastPlays, locationIndex - TOTAL_ROUND_SIZE * 5);
   } else {
       return abbrevToID(abbrev);
   }
}


// Frees all memory previously allocated for the DracView toBeDeleted
void disposeDracView(DracView toBeDeleted)
{
    free(toBeDeleted->gameView);
    free(toBeDeleted);
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round giveMeTheRound(DracView currentView)
{
    return getRound(currentView->gameView);
}

// Get the current score
int giveMeTheScore(DracView currentView)
{
    return getScore(currentView->gameView);
}

// Get the current health points for a given player
int howHealthyIs(DracView currentView, PlayerID player)
{
    return getHealth(currentView->gameView, player);
}

// Get the current location id of a given player
LocationID whereIs(DracView currentView, PlayerID player)
{
    return getLocation(currentView->gameView, player);
}

// Get the most recent move of a given player
void lastMove(DracView currentView, PlayerID player,
                 LocationID *start, LocationID *end)
{
    LocationID *trailOfPlayer = NULL;
    getHistory(currentView->gameView, player, trailOfPlayer);
    *start = trailOfPlayer[0];
    *end = trailOfPlayer[1];
    return;
}

// Find out what minions are placed at the specified location
void whatsThere(DracView currentView, LocationID where,
                         int *numTraps, int *numVamps)
{
    *numTraps = currentView->traps[where];
    *numVamps = currentView->vampires[where];
    return;
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(DracView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    getHistory(currentView->gameView, player, trail);
}

//// Functions that query the map to find information about connectivity

// What are my (Dracula's) possible next moves (locations)
LocationID *whereCanIgo(DracView currentView, int *numLocations, int road, int sea)
{
    PlayerID player = getCurrentPlayer(currentView->gameView);
    LocationID from = getLocation(currentView->gameView,player);
    Round round = getRound(currentView->gameView);

    return connectedLocations(currentView->gameView, numLocations,
                               from, player, round,
                               road, FALSE, sea);
}

// What are the specified player's next possible moves
LocationID *whereCanTheyGo(DracView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
    LocationID from = getLocation(currentView->gameView,player);
    Round round = getRound(currentView->gameView);

    return connectedLocations(currentView->gameView, numLocations,
                               from, player, round,
                               road, rail, sea);
}
