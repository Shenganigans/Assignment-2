// GameView.c ... GameView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
// #include "Map.h" ... if you decide to use the Map ADT

struct gameView {
    int globalScore; //Current game score
    Round globalRound; //Current game Round
    int currentTurn; //Current turn
    int currentPlayer; //The name says it all
    int *globalHeath; //The array for the health of the players
    int *trailLocation; //The array for the trail
    int *currentLocation; //The array for all the current locations of the players
};


// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    GameView gameView = malloc(sizeof(struct gameView));
    gameView->globalHeath = malloc(sizeof(NUM_PLAYERS));
    gameView->trailLocation = malloc(sizeof(TRAIL_SIZE));
    gameView->currentLocation = malloc(sizeof(NUM_PLAYERS));
    //todo
    return gameView;
}


// Frees all memory previously allocated for the GameView toBeDeleted
void disposeGameView(GameView toBeDeleted)
{
    //COMPLETE THIS IMPLEMENTATION
    free(toBeDeleted->globalHeath);
    free(toBeDeleted->trailLocation);
    free(toBeDeleted->currentLocation);
    free(toBeDeleted);
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round getRound(GameView currentView)
{
    return currentView->globalRound;
}

// Get the id of current player - ie whose turn is it?
PlayerID getCurrentPlayer(GameView currentView)
{
    return currentView->currentTurn;
}

// Get the current score
int getScore(GameView currentView)
{
    return currentView->globalScore;
}

// Get the current health points for a given player
int getHealth(GameView currentView, PlayerID player)
{
    return currentView->globalHeath[player];
}

// Get the current location id of a given player
LocationID getLocation(GameView currentView, PlayerID player)
{
    return currentView->currentLocation[player];
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void getHistory(GameView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

//// Functions that query the map to find information about connectivity

// Returns an array of LocationIDs for all directly connected locations

LocationID *connectedLocations(GameView currentView, int *numLocations,
                               LocationID from, PlayerID player, Round round,
                               int road, int rail, int sea)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return NULL;
}
