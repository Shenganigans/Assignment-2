// DracView.c ... DracView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "DracView.h"
#include <string.h>
// #include "Map.h" ... if you decide to use the Map ADT
#define CHARS_PER_TURN 8
#define LOCATION_ABBREVIATION 1
#define TRAP 3
#define VAMP 4
#define DRACULA_ACTION 5
#define START_ENCOUNTER 3
#define MAX_ENCOUNTERS 3
static void insertdracTrailLoc (DracView dracView, LocationID placeID);
struct dracView {
    GameView draculaView;
    LocationID dracTrail[TRAIL_SIZE];
    int traps[NUM_MAP_LOCATIONS];
    int vamps[NUM_MAP_LOCATIONS];
    int location;
};

// Creates a new DracView to summarise the current state of the game
DracView newDracView(char *pastPlays, PlayerMessage messages[])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    DracView dracView = malloc(sizeof(struct dracView));
    dracView->draculaView = newGameView(pastPlays, messages);

    int i, j;
    for(i = 0; i <8; i++) {
        dracView->dracTrail[i] = UNKNOWN_LOCATION;
    }

    for(i = 0;i < NUM_MAP_LOCATIONS; i++) {
        dracView->traps[i] = 0;
    }
    dracView->location = NOWHERE;

    int past = strnlen(pastPlays, 7);

    for(i = 0; i < past; i += CHARS_PER_TURN) {
        PlayerID currPlayer = ( (i/CHARS_PER_TURN) % NUM_PLAYERS);
        LocationID currLoc = abbrevToID(pastPlays + i + LOCATION_ABBREVIATION);
        if(!validPlace(currLoc)) {
            switch (pastPlays[i + LOCATION_ABBREVIATION]) {
					 case 'H':
						 currLoc = dracView->dracTrail[0];
					    break;
					 case 'T':
					    currLoc = CASTLE_DRACULA;
						 break;
					 case 'D':
						 currLoc = dracView->dracTrail[pastPlays[i + LOCATION_ABBREVIATION + 1]-'0'];
						 break;
					 default:
						 assert(TRUE == FALSE);
				}
        }

        assert(validPlace(currLoc));

        if(currPlayer == PLAYER_DRACULA) {
            if(pastPlays[i + TRAP] == 'T') {
                dracView->traps[currLoc]++;
            }
            if(pastPlays[i + VAMP] == 'V') {
                dracView->location = currLoc;
            }
            if(pastPlays[i + DRACULA_ACTION] == 'M') {
                dracView->traps[dracView->dracTrail[8 - 1]]--;
            } else if(pastPlays[i + DRACULA_ACTION] == 'V') {
                dracView->location = NOWHERE;
            }
            insertdracTrailLoc(dracView, currLoc);
        } else {
            for(j = 0; j < MAX_ENCOUNTERS; j++) {
                char encounter = pastPlays[i + START_ENCOUNTER + j];

                if(encounter == 'T') {
                    dracView->traps[currLoc]--;
                } else if(encounter == 'V') {
                    dracView->location= NOWHERE;
                }
            }
        }
    }

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

     int trail[TRAIL_SIZE];
     getHistory(currentView->draculaView, player, &trail[TRAIL_SIZE]);
     end = trail;
     start = trail+1;

}

// Find out what minions are placed at the specified location
void whatsThere(DracView currentView, LocationID where,
                         int *numTraps, int *numVamps)
{
    assert(currentView != NULL);
    assert(validPlace(where));

    if(where == currentView->location) {
        (*numVamps) = 1;
    } else {
        (*numVamps) = 0;
    }

    (*numTraps) = currentView->traps[where];
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

static void insertdracTrailLoc (DracView dracView, LocationID placeID) {
    assert(dracView != NULL);
    assert(validPlace(placeID));

    int i;
    for(i = TRAIL_SIZE - 1; i >= 1; i--) {
        dracView->dracTrail[i] = dracView->dracTrail[i-1];
    }
    dracView->dracTrail[0] = placeID;
    return;
}
