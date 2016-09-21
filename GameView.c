// GameView.c ... GameView ADT implementation

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h" //... if you decide to use the Map ADT

#define TRAP 0
#define VAMP 1
#define MOST_RECENT 0
#define PREVIOUS 1
#define LOCATION_NAME_ABBRV 3
#define VAMP_MATURES 19 // drac leaves vamp every 13 rounds, it matures in 6 more, hence 13+6=19?

//helper functions
static GameView currentLocation (GameView gameView, char *locationID, int currentPlayer);
static GameView locationUpdateInGV (GameView gameView, int locationID, int currentPlayer); // updating location in gameView
static GameView startingPlayerLocationAndHealth (GameView gameView);
static char *getLocationFromPastPlay(char *pastPlays, int i);
static int playerName(char *pastPlays, int i);
static GameView actions (GameView gameView, int i, int currentPlayer, char *pastPlays);
static GameView endOfRound (GameView gameView, int currentPlayer);
//--------------------------------
struct gameView {
    int globalScore; // Current game score
    Round globalRound; // Current game Round
    int currentTurn; // Current turn
    PlayerID currentPlayer; // The name says it all //changed data type
    int globalHealth[NUM_PLAYERS]; // The array for the health of the players
    //int *trailLocation; // The array for the trail
    //int *currentLocation; // The array for all the current locations of the players
    LocationID trailOfLocations[NUM_PLAYERS][TRAIL_SIZE];
    LocationID location[NUM_PLAYERS];
    int minions[NUM_MAP_LOCATIONS][2]; // to keep track of all the traps etc. 
                                       // 2 storage locations, 1 for traps and 1 for vamps
    Map gameMap; // to have a copy of game map at all times?
};


// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
    GameView gameView = malloc(sizeof(struct gameView));
    //gameView->globalHeath = malloc(sizeof(NUM_PLAYERS));// the first line of malloc automatically creates space for this, hence futile?
    //gameView->trailOfLocations = malloc(sizeof(TRAIL_SIZE));// the first line of malloc automatically creates space for this, hence futile?
    //gameView->currentLocation = malloc(sizeof(NUM_PLAYERS));// the first line of malloc automatically creates space for this, hence futile?
    gameView->globalRound = 0;
    gameView->globalScore = GAME_START_SCORE;
    gameView->currentPlayer = PLAYER_LORD_GODALMING;
    gameView->gameMap = newMap();
    
    int i, currPl; // i is very important as it is a counter of the pastplays string
                   // currPl = current player
    startingPlayerLocationAndHealth(gameView);
    for (i = 0; pastPlays[i] != '\0'; i++) {
        if (i % 8 != 0) // every turn has 8 characters 
            currPl = playerName(pastPlays, i); // setting current player until turn is completed
        i++;
        char *locationID = getLocationFromPastPlay(pastPlays, i); // local version of LocationID
        i++;
        currentLocation(gameView, locationID, currPl);
        for (; pastPlays[i] != ' '; i++)
            actions(gameView, i, currPl, pastPlays);
        endOfRound(gameView, currPl);
    } // above for loop was done by manually iterating on paper, it's most likely buggy AF LMAO
    return gameView;
}
// extracts the player's location from the pastPlays string and returns the location's ID no.
// uses the pastPlays string to get player location and returns it
static char *getLocationFromPastPlay(char *pastPlays, int i) {
    char *temp = malloc(LOCATION_NAME_ABBRV*sizeof(char)); // temp array to store the immediate location
    temp[0] = pastPlays[i];
    temp[1] = pastPlays[i+1];
    temp[2] = '\0';
    return temp;
}

// checks what player has performed the actions so we can update the game state accordingly
static int playerName(char *pastPlays, int i) {
    switch (pastPlays[i]) {
        case 'G': return PLAYER_LORD_GODALMING; 
                  break;
        case 'S': return PLAYER_DR_SEWARD; 
                  break;
        case 'H': return PLAYER_VAN_HELSING;
                  break;
        case 'M': return PLAYER_MINA_HARKER; 
                  break;
        case 'D': return PLAYER_DRACULA; 
                  break;
        default: return 0;
    }
}

// updates player's locations ("C?" and "S?" only show up in Dracula's turn)
static GameView currentLocation (GameView gameView, char *locationID, int currentPlayer) {
    if (currentPlayer == PLAYER_DRACULA) {
        if (strcmp(locationID, "C?") == 0)
            locationUpdateInGV(gameView, CITY_UNKNOWN, currentPlayer);
        else if (strcmp(locationID, "S?") == 0)
            locationUpdateInGV(gameView, SEA_UNKNOWN, currentPlayer);
        else if (strcmp(locationID, "HI") == 0)
            locationUpdateInGV(gameView, HIDE, currentPlayer);
        else if (locationID[0] == 'D') {
            switch (locationID[1]) {
                case '1': locationUpdateInGV(gameView, DOUBLE_BACK_1, currentPlayer); 
                          break;
                case '2': locationUpdateInGV(gameView, DOUBLE_BACK_2, currentPlayer);
                          break;
                case '3': locationUpdateInGV(gameView, DOUBLE_BACK_3, currentPlayer); 
                          break;
                case '4': locationUpdateInGV(gameView, DOUBLE_BACK_4, currentPlayer); 
                          break;
                case '5': locationUpdateInGV(gameView, DOUBLE_BACK_5, currentPlayer);
                          break;
                default: break;
            }
            int i = locationID[1] - '0';
            gameView->location[currentPlayer] = gameView->trailOfLocations[currentPlayer][i];
        } else if (strcmp(locationID, "TP") == 0)
            locationUpdateInGV(gameView, CASTLE_DRACULA, currentPlayer);
        else
            locationUpdateInGV(gameView, abbrevToID(locationID), currentPlayer);
    } else {
        locationUpdateInGV(gameView, abbrevToID(locationID), currentPlayer);
    }
    return gameView;
}
static GameView locationUpdateInGV  (GameView gameView, int locationID, int currentPlayer) {
    int i;
    for (i = TRAIL_SIZE; i > 0; i--) // may have to start from Trail_size -1 but can't be sure without testing
        gameView->trailOfLocations[currentPlayer][i] = gameView->trailOfLocations[currentPlayer][i-1];
        
    gameView->trailOfLocations[currentPlayer][i] = locationID;
    gameView->location[currentPlayer] = locationID;
    return gameView;
}

// initialises player's health and locations at the beginning of the game
static GameView startingPlayerLocationAndHealth (GameView gameView) {
    int i, j;
    for (i = 0; i < NUM_PLAYERS; i++) {
        if (i == PLAYER_DRACULA)
            gameView->globalHealth[i] = GAME_START_BLOOD_POINTS;
        else 
            gameView->globalHealth[i] = GAME_START_HUNTER_LIFE_POINTS;
        for (j = 0; j < TRAIL_SIZE; j++)
            gameView->trailOfLocations[i][j] = UNKNOWN_LOCATION;
    }
    return gameView;
}

// changes game state according to actions performed in the pastPlays string
static GameView actions (GameView gameView, int i, int currentPlayer, char *pastPlays) {
    LocationID cpLocation = UNKNOWN_LOCATION;
    if (currentPlayer != PLAYER_DRACULA)
        //LocationID 
        cpLocation = getLocation(gameView, currentPlayer); // current player location
    else
        //LocationID
        cpLocation = gameView->location[PLAYER_DRACULA];
	
    switch (pastPlays[i]) {
        case 'T':
            if (currentPlayer != PLAYER_DRACULA) {
                gameView->globalHealth[currentPlayer] -= LIFE_LOSS_TRAP_ENCOUNTER;
                gameView->minions[cpLocation][TRAP]--;
                if (gameView->minions[cpLocation][TRAP] < 0)
                    gameView->minions[cpLocation][TRAP] = 0;
            } else {
                gameView->minions[cpLocation][TRAP]++;
            }
            break;
        case 'D':
            gameView->globalHealth[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
            gameView->globalHealth[currentPlayer] -= LIFE_LOSS_DRACULA_ENCOUNTER;
            break;
        case 'V' :
            if (currentPlayer == PLAYER_DRACULA && (i%VAMP_MATURES!=0)) {
                gameView->minions[cpLocation][VAMP]++;
            } 
			if (currentPlayer != PLAYER_DRACULA) {
                gameView->minions[cpLocation][VAMP]--;
                if (gameView->minions[cpLocation][VAMP] < 0)
                    gameView->minions[cpLocation][VAMP] = 0;
            }
            if (i%VAMP_MATURES==0){
                LocationID lastDracLocation = gameView->trailOfLocations[PLAYER_DRACULA][5]; 
                gameView->globalScore -= SCORE_LOSS_VAMPIRE_MATURES;
                gameView->minions[lastDracLocation][VAMP]--;
                    if (gameView->minions[lastDracLocation][VAMP] < 0)
                        gameView->minions[lastDracLocation][VAMP] = 0;
            }
            break;
        case '.': // nothing has occured
            break;
        default:
            break;
    }
    return gameView;
}

// takes care of updating all values at the end of every round (mainly end of Dracula's turn)
static GameView endOfRound (GameView gameView, int currentPlayer) {
    int locationID = gameView->location[currentPlayer];
    if (currentPlayer == PLAYER_DRACULA) { 
        gameView->globalScore -= SCORE_LOSS_DRACULA_TURN; // score decreases by 1 each time Dracula finishes a turn
        if (gameView->globalScore < 0) // score cannot be less than 0
            gameView->globalScore = 0;
        if ((validPlace(locationID) && idToType(locationID) == SEA) || locationID == SEA_UNKNOWN)
            gameView->globalHealth[currentPlayer] -= LIFE_LOSS_SEA; // Dracula loses 2 blood pts if he ends his turn at sea
        else if (locationID == CASTLE_DRACULA)
            gameView->globalHealth[currentPlayer] += LIFE_GAIN_CASTLE_DRACULA; // Dracula gains 10 blood pts if he ends his turn at Castle Dracula
               
        gameView->globalRound++; //Dracula finishes his turn, increment the round no.
        gameView->currentPlayer = PLAYER_LORD_GODALMING;
    } else {
        gameView->currentPlayer++; // Post increment so current player gets used ones before it changes
                                   // Incrementing before loop so that even though it gets used, it gets incremented for next time?
        if (locationID == gameView->trailOfLocations[currentPlayer][PREVIOUS]) {
            gameView->globalHealth[currentPlayer] += LIFE_GAIN_REST;  // Hunter gains 4 life pts if they end their current turn in the location of their previous turn
            if (gameView->globalHealth[currentPlayer] > GAME_START_HUNTER_LIFE_POINTS) // Hunter's health is not permitted to exceed 9 life pts
                gameView->globalHealth[currentPlayer] = GAME_START_HUNTER_LIFE_POINTS;
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
    free(toBeDeleted); // we don't need the above three lines, just this will free everything LOL
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
    return currentView->globalHealth[player];
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
    return connecLocations(currentView, numLocations, from, player, round,
                           road, rail, sea);
}
