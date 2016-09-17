// GameView.c ... GameView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h" //... if you decide to use the Map ADT

#define VAMP_OR_TRAP 2
#define TRAP 0
#define VAMP 1
#define MOST_RECENT 0
#define PREVIOUS 1

//helper functions
static GameView endOfRound (GameView gameView, int currentPlayer);


//--------------------------------
struct gameView {
    int globalScore; //Current game score
    Round globalRound; //Current game Round
    int currentTurn; //Current turn
    PlayerID currentPlayer; //The name says it all //changed data type
    int *globalHeath; //The array for the health of the players
    //int *trailLocation; //The array for the trail
    //int *currentLocation; //The array for all the current locations of the players
    LocationID trailOfLocations[NUM_PLAYERS][TRAIL_SIZE];
    LocationID location[NUM_PLAYERS];
    int minions[NUM_MAP_LOCATIONS][VAMP_OR_TRAP]; //to keep track of all the traps etc. 
    Map gameMap; //to have a copy of game map at all times?
};


// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    GameView gameView = malloc(sizeof(struct gameView));
    //gameView->globalHeath = malloc(sizeof(NUM_PLAYERS));// the first line of malloc automatically creates space for this, hence futile?
    //gameView->trailOfLocations = malloc(sizeof(TRAIL_SIZE));// the first line of malloc automatically creates space for this, hence futile?
    //gameView->currentLocation = malloc(sizeof(NUM_PLAYERS));// the first line of malloc automatically creates space for this, hence futile?
    // new stuff here
    gameView->globalRound = 0;
    gameView->globalScore = GAME_START_SCORE;
    gameView->currentPlayer = PLAYER_LORD_GODALMING;
    gameView->gameMap = newMap();
    //-------------
    return gameView;
}

// takes care of updating all values at the end of every round (mainly end of Dracula's turn)
static GameView endOfRound (GameView gameView, int currentPlayer) {
    int locationID = gameView->location[currentPlayer];
    if (curreentPlayer == PLAYER_DRACULA) { 
        gameView->globalScore -= SCORE_LOSS_DRACULA_TURN; // score decreases by 1 each time Dracula finishes a turn
        if (gameView->globalScore < 0) // score cannot be less than 0
            gameView->globalScore = 0;
        if ((validPlace(locationID) && idToType(locationID) == SEA) || locationID == SEA_UNKNOWN)
            gameView->health[currPlayer] -= LIFE_LOSS_SEA; // Dracula loses 2 blood pts if he ends his turn at sea
        else if (locationID == CASTLE_DRACULA)
            gameView->health[currPlayer] += LIFE_GAIN_CASTLE_DRACULA; // Dracula gains 10 blood pts if he ends his turn at Castle Dracula
               
        gameView->round++; //Dracula finishes his turn, increment the round no.
        gameView->currentPlayer = PLAYER_LORD_GODALMING;
    } else {
        gameView->currentPlayer++; // Post increment so current player gets used ones before it changes
                                   // Incrementing before loop so that even though it gets used, it gets incremented for next time?
        if (locationID == gameView->trail[currentPlayer][PREVIOUS]) {
            gameView->health[currentPlayer] += LIFE_GAIN_REST;  // Hunter gains 4 life pts if they end their current turn in the location of their previous turn
            if (gameView->health[currentPlayer] > GAME_START_HUNTER_LIFE_POINTS) // Hunter's health is not permitted to exceed 9 life pts
                gameView->health[currentPlayer] = GAME_START_HUNTER_LIFE_POINTS;
        }
    }
    return gameView; // Return final changes to the game after every round
}

// Frees all memory previously allocated for the GameView toBeDeleted
void disposeGameView(GameView toBeDeleted)
{
    //COMPLETE THIS IMPLEMENTATION
    /*free(toBeDeleted->globalHeath); 
    free(toBeDeleted->trailLocation);
    free(toBeDeleted->currentLocation);*/
    free(toBeDeleted); // we don't need the above three lines, just this will free everything lol
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
    return currentView->currentPlayer;
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
     return currentView->trailOfLocations[player][MOST_RECENT];
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void getHistory(GameView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) 
        trail[i] = currentView->trailOfLocations[player][i];
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
