// GameView.c ... GameView ADT implementation

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"

#define LOCATION_NAME_ABBREV 3
#define MOST_RECENT 0
#define PREVIOUS 1
#define VAMP_MATURES 38 // should be 19 as 13 + 6 as drac leaves vamp every 13 rounds, it matures in 6 more but changed cz of calc bs
#define hasVampMatured(i) (i % VAMP_MATURES == 0)
#define TRAP_OR_VAMP 2
#define TRAP 0
#define VAMP 1
#define OLDEST_LOCATION 5

struct gameView {
    Round globalRound; // Current Game Round
    PlayerID currentPlayer; // Current Player
    int globalScore; // Current Game Score
    int globalHealth[NUM_PLAYERS]; // Array storing healths of all the players
    LocationID trailOfLocations[NUM_PLAYERS][TRAIL_SIZE]; // 2D array storing trails of every player
    LocationID location[NUM_PLAYERS]; 
    int minions[NUM_MAP_LOCATIONS][TRAP_OR_VAMP]; // to keep track of all traps and vamps in the game
    Map gameMap; 
};

// helper functions
static char *getLocationFromPastPlay (char *pastPlays, int i);
static int playerName(char *pastPlays, int i);
static GameView startingPlayerLocationAndHealth (GameView currentView);
static GameView currentLocation (GameView currentView, char *locationID, int currentPlayer);
static GameView actions (GameView currentView, int i, int currentPlayer, char *pastPlays);
static GameView endOfRound (GameView currentView, int currentPlayer);
static GameView locationUpdateInGV (GameView gameView, int locationID, int currentPlayer); // updating location in GameView

// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
    GameView gameView = malloc(sizeof(struct gameView));
    gameView->globalRound = 0;
    gameView->globalScore = GAME_START_SCORE;
    gameView->currentPlayer = PLAYER_LORD_GODALMING;
    gameView->gameMap = newMap();
    int i, currentPlayer;
    startingPlayerLocationAndHealth(gameView);
    for (i = 0; pastPlays[i] != '\0'; i++) {
        if (i % 8 == 0) // every turn has 8 characters  
            currentPlayer = playerName(pastPlays, i); // setting current player until turn is complete
        i++;
        char *locationID = getLocationFromPastPlay(pastPlays, i); // local version of LocationID
        i++;
        currentLocation(gameView, locationID, currentPlayer);
        for (i++; pastPlays[i] != ' ' && pastPlays[i+1] != '\0'; i++)
            actions(gameView, i, currentPlayer, pastPlays);
        endOfRound(gameView, currentPlayer);
    }
    return gameView;
}

// takes care of end of turn housekeeping (mainly end of Dracula's turn)
static GameView endOfRound (GameView gameView, int currentPlayer) {
    int locationID = gameView->location[currentPlayer];
    if (currentPlayer == PLAYER_DRACULA) {
        // globalScore decreases by 1 each time Dracula finishes a turn
        gameView->globalScore -= SCORE_LOSS_DRACULA_TURN;
        // globalScore cannot be less than 0
        if (gameView->globalScore < 0) gameView->globalScore = 0;
        // Dracula loses 2 blood pts if he ends his turn at sea
        if ((validPlace(locationID) && idToType(locationID) == SEA) || locationID == SEA_UNKNOWN)
            gameView->globalHealth[currentPlayer] -= LIFE_LOSS_SEA;
        // Dracula gains 10 blood pts if he ends his turn at Castle Dracula
        else if (locationID == CASTLE_DRACULA)
            gameView->globalHealth[currentPlayer] += LIFE_GAIN_CASTLE_DRACULA;
        gameView->currentPlayer = PLAYER_LORD_GODALMING;
        // After 5 turns, or when Dracula finishes his turn, increment the globalRound no.
        gameView->globalRound++;
    } else {
        gameView->currentPlayer++;
        // Hunter gains 4 life pts if they end their current turn in the location of their previous turn
        if (locationID == gameView->trailOfLocations[currentPlayer][PREVIOUS]) {
            gameView->globalHealth[currentPlayer] += LIFE_GAIN_REST;
            // Hunter's globalHealth is not permitted to exceed 9 life pts
            if (gameView->globalHealth[currentPlayer] > GAME_START_HUNTER_LIFE_POINTS)
                gameView->globalHealth[currentPlayer] = GAME_START_HUNTER_LIFE_POINTS;
        }
    }
    return gameView; // Return final changes to the game after every round
}

// changes game state according to actions performed in the pastPlays string
static GameView actions (GameView gameView, int i, int currentPlayer, char *pastPlays) {
    LocationID playerLocation = (currentPlayer != PLAYER_DRACULA) ? getLocation(gameView, currentPlayer) : gameView->location[PLAYER_DRACULA];
    switch (pastPlays[i]) {
        case 'T':
            if (currentPlayer != PLAYER_DRACULA) {
                gameView->globalHealth[currentPlayer] -= LIFE_LOSS_TRAP_ENCOUNTER;
                gameView->minions[playerLocation][TRAP]--;
                if (gameView->minions[playerLocation][TRAP] < 0)
                    gameView->minions[playerLocation][TRAP] = 0;
            } else {
                gameView->minions[playerLocation][TRAP]++;
            }
            break;
        case 'D':
            gameView->globalHealth[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
            gameView->globalHealth[currentPlayer] -= LIFE_LOSS_DRACULA_ENCOUNTER;
            break;
        case 'V':
            if (currentPlayer == PLAYER_DRACULA && !hasVampMatured(i)) {
                gameView->minions[playerLocation][VAMP]++;
            } else if (currentPlayer != PLAYER_DRACULA) {
                gameView->minions[playerLocation][VAMP]--;
                if (gameView->minions[playerLocation][VAMP] < 0)
                    gameView->minions[playerLocation][VAMP] = 0;
            }
            break;
        case '.':
            break;
        default:
            break;
    }
    if (hasVampMatured(i) && pastPlays[i] == 'V') {
        LocationID oldestDracTrailLocation = gameView->trailOfLocations[PLAYER_DRACULA][OLDEST_LOCATION];
        gameView->globalScore -= SCORE_LOSS_VAMPIRE_MATURES;
        gameView->minions[oldestDracTrailLocation][VAMP]--;
        if (gameView->minions[oldestDracTrailLocation][VAMP] < 0)
            gameView->minions[oldestDracTrailLocation][VAMP] = 0;
    }
    return gameView;
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
                case '1': locationUpdateInGV(gameView, DOUBLE_BACK_1, currentPlayer); break;
                case '2': locationUpdateInGV(gameView, DOUBLE_BACK_2, currentPlayer); break;
                case '3': locationUpdateInGV(gameView, DOUBLE_BACK_3, currentPlayer); break;
                case '4': locationUpdateInGV(gameView, DOUBLE_BACK_4, currentPlayer); break;
                case '5': locationUpdateInGV(gameView, DOUBLE_BACK_5, currentPlayer); break;
                default: break;
            }
            int index = locationID[1] - '0';
            gameView->location[currentPlayer] = gameView->trailOfLocations[currentPlayer][index];
        } else if (strcmp(locationID, "TP") == 0)
            locationUpdateInGV(gameView, CASTLE_DRACULA, currentPlayer);
        else
            locationUpdateInGV(gameView, abbrevToID(locationID), currentPlayer);
    } else {
        locationUpdateInGV(gameView, abbrevToID(locationID), currentPlayer);
    }
    return gameView;
}

static GameView locationUpdateInGV (GameView gameView, int locationID, int currentPlayer) {
    int i;
    for (i = TRAIL_SIZE-1; i > 0; i--)
        gameView->trailOfLocations[currentPlayer][i] = gameView->trailOfLocations[currentPlayer][i-1];
    gameView->trailOfLocations[currentPlayer][i] = locationID;
    gameView->location[currentPlayer] = locationID;
    return gameView;
}

// initialises player's globalHealth and locations at the beginning of the game
static GameView startingPlayerLocationAndHealth (GameView gameView) {
    int i, j;
    for (i = 0; i < NUM_PLAYERS; i++) {
        gameView->globalHealth[i] = (i == PLAYER_DRACULA) ? GAME_START_BLOOD_POINTS : GAME_START_HUNTER_LIFE_POINTS;
        for (j = 0; j < TRAIL_SIZE; j++)
            gameView->trailOfLocations[i][j] = UNKNOWN_LOCATION;
    }
    return gameView;
}

// extracts the player's location from the pastPlays string and returns the location's ID no.
// uses the pastPlays string to get player location and return it
static char *getLocationFromPastPlay(char *pastPlays, int i) {
    char *locationID = malloc(LOCATION_NAME_ABBREV*sizeof(char));
    locationID[0] = pastPlays[i];
    locationID[1] = pastPlays[i+1];
    locationID[2] = '\0';
    return locationID;
}

// checks what player has performed the actions so we can update the game state accordingly
static int playerName(char *pastPlays, int i) {
    switch (pastPlays[i]) {
        case 'G': return PLAYER_LORD_GODALMING; break;
        case 'S': return PLAYER_DR_SEWARD; break;
        case 'H': return PLAYER_VAN_HELSING; break;
        case 'M': return PLAYER_MINA_HARKER; break;
        case 'D': return PLAYER_DRACULA; break;
        default: return PLAYER_LORD_GODALMING;
    }
}

// Frees all memory previously allocated for the GameView toBeDeleted
void disposeGameView(GameView toBeDeleted)
{
    free(toBeDeleted);
}


//// Functions to return simple information about the current state of the game

// Get the current globalRound
Round getRound(GameView currentView)
{
    return currentView->globalRound;
}

// Get the id of current player - ie whose turn is it?
PlayerID getCurrentPlayer(GameView currentView)
{
    return currentView->currentPlayer;
}

// Get the current globalScore
int getScore(GameView currentView)
{
    return currentView->globalScore;
}

// Get the current globalHealth points for a given player
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

// Fills the trailOfLocations array with the location ids of the last 6 turns
void getHistory(GameView currentView, PlayerID player,
                            LocationID trailOfLocations[TRAIL_SIZE])
{
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) 
        trailOfLocations[i] = currentView->trailOfLocations[player][i];
}

//// Functions that query the map to find information about connectivity

// connectedLocations() returns an array of LocationID that represent
//   all locations that are connected to the given LocationID.
// road, rail and sea are connections should only be considered
//   if the road, rail, sea parameters are TRUE.
// The size of the array is stored in the variable pointed to by numLocations
// The array can be in any order but must contain unique entries
// Your function must take into account the round and player id for rail travel
// Your function must take into account that Dracula can't move to
//   the hospital or travel by rail but need not take into account Dracula's trailOfLocations
// The destination 'from' should be included in the array

LocationID *connectedLocations(GameView currentView, int *numLocations,
                               LocationID from, PlayerID player, Round round,
                               int road, int rail, int sea)
{
   
    return connecLocations(currentView, numLocations, from, player, round, road, rail, sea);
}
