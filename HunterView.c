// HunterView.c ... HunterView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Globals.h"
#include "Game.h"
#include "HunterView.h"
#include "GameView.h"
#include "Map.h" // if you decide to use the Map ADT

#define STARTING_ROUND 0
#define NO_OF_ACTIONS_PER_TURN 8
//366 is MAX Dracula turns
#define MAX_TURNS (366*5)
#define MAX_LENGTH ( MAX_TURNS * (NO_OF_ACTIONS_PER_TURN + 1) )

struct hunterView {
     GameView hunter;
     LocationID trailLocation[TRAIL_SIZE];
};


// Creates a new HunterView to summarise the current state of the game
HunterView newHunterView(char *pastPlays, PlayerMessage messages[])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    HunterView hunterView = malloc(sizeof(struct hunterView));
    hunterView->hunter = newGameView(pastPlays, messages);

    int i;

    for( i = 0 ; i < TRAIL_SIZE ; i++ ){
        hunterView->trailLocation[i] = UNKNOWN_LOCATION;
    }

    int past = strnlen(pastPlays, MAX_LENGTH);

    for( i = 0 ; i < past ; i += NO_OF_ACTIONS_PER_TURN){
        PlayerID currPlayer = ( (i/(NO_OF_ACTIONS_PER_TURN)) % NUM_PLAYERS);

        if(currPlayer == PLAYER_DRACULA){ // Getting Dracula Information
            LocationID currentLocation = abbrevToID(pastPlays + i + 1);

            if(!validPlace(currentLocation)){
                if(pastPlays[i+1] == 'H'){ // Hide Move
                    currentLocation = hunterView->trailLocation[0];
                } else if(pastPlays[i+1] == 'D'){ // Double Back Move
                    int back = (int)(pastPlays[i+1] - '0');
                    currentLocation = hunterView->trailLocation[back];
                } else if(pastPlays[i+1] == 'T'){ // TP to Castle Dracula
                    currentLocation = CASTLE_DRACULA;
                } else if(pastPlays[i+1] == 'C'){ // City Move
                    currentLocation = CITY_UNKNOWN;
                } else if(pastPlays[i+1] == 'S'){ // Sea Move
                    currentLocation = SEA_UNKNOWN;
                } else {
                    currentLocation = UNKNOWN_LOCATION;
                }
            }

            int j;
            for(j = (TRAIL_SIZE - 1) ; j >= 1 ; j--) {
                hunterView->trailLocation[j] = hunterView->trailLocation[j-1]; // Moving hunter along the trail
            }
            hunterView->trailLocation[0] = currentLocation;
        }
    }
    return hunterView;
}


// Frees all memory previously allocated for the HunterView toBeDeleted
void disposeHunterView(HunterView toBeDeleted)
{
    //COMPLETE THIS IMPLEMENTATION
    free( toBeDeleted );
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round giveMeTheRound(HunterView currentView)
{
    return getRound(currentView->hunter);
}

// Get the id of current player
PlayerID whoAmI(HunterView currentView)
{
    return getCurrentPlayer(currentView->hunter);
}

// Get the current score
int giveMeTheScore(HunterView currentView)
{
    return getScore(currentView->hunter);
}

// Get the current health points for a given player
int howHealthyIs(HunterView currentView, PlayerID player)
{
    return getHealth(currentView->hunter, player);
}

// Get the current location id of a given player
LocationID whereIs(HunterView currentView, PlayerID player)
{
    return getLocation(currentView->hunter, player);
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(HunterView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    getHistory(currentView->hunter, player, trail);
}


//// Functions that query the map to find information about connectivity

// What are my possible neit moves (locations)
LocationID *whereCanIgo(HunterView currentView, int *numLocations, int road, int rail, int sea)
{
    LocationID *possibilites;

    if(giveMeTheRound(currentView) == STARTING_ROUND){
        possibilites = (LocationID *)(malloc(sizeof(LocationID)*NUM_MAP_LOCATIONS));
        *numLocations = 0;

        int i;
        for( i = 0 ; i<NUM_MAP_LOCATIONS ; i++ ){
            possibilites[i] = i;
        }
    } else {
        possibilites = connectedLocations(currentView->hunter, numLocations,
                                        whereIs(currentView,
                                                getCurrentPlayer(currentView->hunter)),
                                        getCurrentPlayer(currentView->hunter),
                                        giveMeTheRound(currentView), road , rail,
                                        sea);
    }


    return possibilites;
}

// What are the specified player's neit possible moves
LocationID *whereCanTheyGo(HunterView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
    Round turn;

    if( whoAmI(currentView) <= player){
        turn = giveMeTheRound(currentView);
    } else {
        turn = (giveMeTheRound(currentView) + 1) ;
    }

    LocationID *possibilites;

    if( turn == STARTING_ROUND){
        possibilites = (LocationID *)(malloc(sizeof(LocationID)*NUM_MAP_LOCATIONS));

        int i;
        for( i = 0 ; i < NUM_MAP_LOCATIONS ; i++ ) {
            if( player != PLAYER_DRACULA || i != ST_JOSEPH_AND_ST_MARYS){
                possibilites[i] = i;
            }
        }
    } else {
        if( PLAYER_DRACULA == player){
            LocationID dracLocation = whereIs( currentView, PLAYER_DRACULA);

            if(validPlace(dracLocation)){
                possibilites = connectedLocations(currentView->hunter, numLocations,
                                                whereIs(currentView, PLAYER_DRACULA),
                                                turn, player, road,
                                                FALSE, sea); // can't travel by rail
            } else {
                possibilites = NULL;
            }
        } else {
            possibilites = connectedLocations( currentView->hunter, numLocations,
                                            whereIs(currentView, player),
                                            turn, player, road, rail,
                                            sea);
        }
    }

    return possibilites;
}
