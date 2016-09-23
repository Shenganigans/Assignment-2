// DracView.c ... DracView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "DracView.h"
#include <string.h>
#include "Map.h"
#include <stdio.h>
#define LOCATION_NAME_ABBREV 3
#define MOST_RECENT 0
#define PREVIOUS 1
#define VAMP_MATURES 38 // should be 19 as 13 + 6 as drac leaves vamp every 13 rounds, it matures in 6 more but changed cz of calc bs
#define hasVampMatured(i) (i % VAMP_MATURES == 0)
#define TRAP_OR_VAMP 2
#define TRAP 0
#define VAMP 1
#define OLDEST_LOCATION 5

struct dracView {
  Round globalRound; // Current Game Round
  PlayerID currentPlayer; // Current Player
  int globalScore; // Current Game Score
  int globalHealth[NUM_PLAYERS]; // Array storing healths of all the players
  LocationID trailOfLocations[NUM_PLAYERS][TRAIL_SIZE]; // 2D array storing trails of every player
  LocationID location[NUM_PLAYERS];
  int minions[NUM_MAP_LOCATIONS][TRAP_OR_VAMP]; // to keep track of all traps and vamps in the game
  Map gameMap;
};

// Creates a new DracView to summarise the current state of the game
DracView newDracView(char *pastPlays, PlayerMessage messages[])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    GameView dracView = newGameView(pastPlays, messages);
    return (DracView) dracView;
}


// Frees all memory previously allocated for the DracView toBeDeleted
void disposeDracView(DracView toBeDeleted)
{
    //COMPLETE THIS IMPLEMENTATION

    free(toBeDeleted);
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round giveMeTheRound(DracView currentView)
{
    return (currentView->globalRound);
}

// Get the current score
int giveMeTheScore(DracView currentView)
{
    return (currentView->globalScore);
}

// Get the current health points for a given player
int howHealthyIs(DracView currentView, PlayerID player)
{
    return (currentView->globalHealth[player]);
}

// Get the current location id of a given player
LocationID whereIs(DracView currentView, PlayerID player)
{
    return (currentView->trailOfLocations[player][0]);
}

// Get the most recent move of a given player
void lastMove(DracView currentView, PlayerID player,
                 LocationID *start, LocationID *end)
{

     end = &currentView->trailOfLocations[player][0];
     start = &currentView->trailOfLocations[player][1];

}

// Find out what minions are placed at the specified location
void whatsThere(DracView currentView, LocationID where,
                         int *numTraps, int *numVamps)
{
    *numTraps = currentView->minions[where][0];
    *numVamps = currentView->minions[where][1];

}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(DracView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
  int i;
  for ( i= 0; i<TRAIL_SIZE; i++){
    trail[i]= currentView->trailOfLocations[player][i];
  }
}

//// Functions that query the map to find information about connectivity

// What are my (Dracula's) possible next moves (locations)
LocationID *whereCanIgo(DracView currentView, int *numLocations, int road, int sea)
{
    return connecLocations((GameView)currentView, numLocations, currentView->trailOfLocations[PLAYER_DRACULA][0], PLAYER_DRACULA, currentView->globalRound, road, FALSE, sea);

}

// What are the specified player's next possible moves
LocationID *whereCanTheyGo(DracView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
  return connecLocations((GameView)currentView, numLocations, currentView->trailOfLocations[player][0], player, currentView->globalRound, road, rail, sea);
}
