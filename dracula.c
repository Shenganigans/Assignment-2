// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Game.h"
#include "DracView.h"

char* strategy (DracView gameState);
int compareMoves (DracView gameState,LocationID draculaL,PlayerID hunter);
int avoidHunter (DracView gameState,LocationID currDrac);
int legalMove (DracView gameState, LocationID currentLocationation);
LocationID anyLegalMove (DracView gameState);
LocationID avoidSea (DracView gameState);

void decideDraculaMove(DracView gameState)
{
	char* finalMove = strategy(gameState);
	registerBestPlay(move,"Mwuhahahaha");

}


// choose all the moves which avoids hunters and/or sea
char* strategy (DracView gameState)
{
	int *numLocations = NULL; // leaving it NULL to keep compiler happy, don't know what to put LOL
	LocationID* avoidSea = whereCanIgo(gameState, numLocations, TRUE, FALSE);
	int seaAvoided = FALSE;
	//try to avoid hunters AND sea
	LocationID  bestMove;
	int i;
	LocationID currentLocation;
	//attempts to find a location not in hunter's possible moves
	for( i=0; i < strlen(*avoidSea);i++) { // can possibly be i < *numLocations
		currentLocation = avoidSea[i];
		if (legalMove(gameState,currentLocation) == TRUE){
			if (avoidHunter(gameState,currentLocation) == TRUE){
				bestMove = currentLocation;
				seaAvoided = TRUE;
				break;
			}
		}
	}
	char * finalMove = malloc(sizeof(char)*3);

	if (avoidHunter(gameState,currentLocation) == FALSE){
		//couldn't avoid hunter, avoid sea
		if (avoidSea(gameState) != UNKNOWN_LOCATION){
			finalMove = idToAbbrev(avoidSea(gameState));
		//take any move if couldn't avoid either
		} else if (anyLegalMove(gameState) != UNKNOWN_LOCATION){
			finalMove = idToAbbrev(anyLegalMove(gameState));
		//no legal moves
		} else {
			finalMove = "TP";
		}
	// both sea and hunter can be avoided
	} else {
		finalMove = idToAbbrev(bestMove);
	}
	return finalMove;
}

LocationID avoidSea (DracView gameState)
{
	int* numLocations = NULL;
	LocationID* dracPossibilities = whereCanIgo(gameState, numLocations, TRUE, FALSE);
	LocationID move = UNKNOWN_LOCATION;
	LocationID curr;
	int i;
	for( i=0; i < *numLocations; i++){
		curr = dracPossibilities[i];
		if (legalMove(gameState,curr) == TRUE){
			move = curr;
			break;
		}
	}
	return move;

}

LocationID anyLegalMove (DracView gameState)
{
	int *numLocations = NULL;
	LocationID* dracPossibilities = whereCanIgo(gameState, numLocations, TRUE, TRUE);
	LocationID legalMove = UNKNOWN_LOCATION;
	LocationID curr;
	int i;
	for( i=0; i < *numLocations; i++){
		curr = dracPossibilities[i];
		if (legalMove(gameState,curr) == TRUE){
			legalMove = curr;
			break;
		}
	}
	return legalMove;

}

// to compare ONE of dracula's possible moves to a hunter's possible moves
int compareMoves (DracView gameState,LocationID currDrac,PlayerID hunter)
{
	int numLocations = 0; // not sure what value to give
	LocationID* hunterPossibilites = whereCanTheyGo(gameState,numLocation, hunter,
		TRUE, TRUE, TRUE);
	int i;
	int match = FALSE;
	for( i=0; i < numLocations; i++){
		if (currDrac == hunterPossibilites[i]){
			match = TRUE;
			break;
		}
	}
	return match;
}

// to find if a location is in the hunter's path
int avoidHunter (DracView gameState,LocationID currentLocation)
{
	int counter = 0;
	PlayerID hunter;
	for( hunter = 0; hunter < 4; hunter++){
		if (compareMoves(gameState,currentLocation,hunter) == FALSE){
			counter++;
		}
	}
	int hunterAvoided;
	if (counter == 4){
		hunterAvoided = TRUE;
	} else {
		hunterAvoided = FALSE;
	}
	return hunterAvoided;
}


// to check if move is legal
int legalMove (DracView gameState, LocationID currentLocation)
{
	LocationID trail[TRAIL_SIZE];
	giveMeTheTrail(gameState, PLAYER_DRACULA, trail);
	int i;
	// check trail for repeated moves
	int flag = 0;
	int legal;
	for(i=0; i < TRAIL_SIZE; i++){
		if (trail[i] == currentLocation) {
			flag++;
		}
	}
	// checking for double back moves,etc. 
	if (flag == 0 && trail[0] != currentLocation){
		legal = TRUE;
	} else {
		legal = FALSE;
	}
	return legal;
}
