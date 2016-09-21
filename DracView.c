// DracView.c ... DracView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "DracView.h"
// #include "Map.h" ... if you decide to use the Map ADT

struct dracView {
    GameView draculaView;
};


// Creates a new DracView to summarise the current state of the game
DracView newDracView(char *pastPlays, PlayerMessage messages[])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    DracView dracView = malloc(sizeof(struct dracView));
    dracView->draculaView = newGameView(pastPlays,messages);
    return dracView;
}


// Frees all memory previously allocated for the DracView toBeDeleted
void disposeDracView(DracView toBeDeleted)
{
    //COMPLETE THIS IMPLEMENTATION
    free(toBeDeleted->draculaView);
    free(toBeDeleted);
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round giveMeTheRound(DracView currentView)
{
    return (getRound(currentView->draculaView));
}

// Get the current score
int giveMeTheScore(DracView currentView)
{
    return (getScore(currentView->draculaView));
}

// Get the current health points for a given player
int howHealthyIs(DracView currentView, PlayerID player)
{
    return (getHealth(currentView->draculaView, player));
}

// Get the current location id of a given player
LocationID whereIs(DracView currentView, PlayerID player)
{
    return (getLocation(currentView->draculaView, player));
}

// Get the most recent move of a given player
void lastMove(DracView currentView, PlayerID player,
                 LocationID *start, LocationID *end)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATIO
     char trail[TRAIL_SIZE];
     getHistory(currentView->draculaView, player, trail[TRAIL_SIZE]);
     end = trail[0];
     start = trail[1];//REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    
}

// Find out what minions are placed at the specified location
void whatsThere(DracView currentView, LocationID where,
                         int *numTraps, int *numVamps)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return;
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(DracView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    getHistory(currentView->draculaView, player, &trail[TRAIL_SIZE]);
}

//// Functions that query the map to find information about connectivity

// What are my (Dracula's) possible next moves (locations)
LocationID *whereCanIgo(DracView currentView, int *numLocations, int road, int sea)
{
    return (connectedLocations(currentView->draculaView, numLocations,
                                 (getLocation(currentView->draculaView, PLAYER_DRACULA)), PLAYER_DRACULA , (getRound(currentView->draculaView)),
                                 road, FALSE, sea));

}

// What are the specified player's next possible moves
LocationID *whereCanTheyGo(DracView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
  return (connectedLocations(currentView->draculaView, numLocations,
                               (getLocation(currentView->draculaView, player)), player , (getRound(currentView->draculaView)),
                               road, rail, sea));
}
