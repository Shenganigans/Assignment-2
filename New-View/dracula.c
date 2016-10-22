// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
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
	registerBestPlay(finalMove,"Mwuhahahaha");
}

// choose all the moves which avoids hunters and/or sea
char* strategy (DracView gameState)
{
	int numLocations = 0;
	char *finalMove = malloc(sizeof(char)*3);
	int seaAvoided = FALSE;
	//try to avoid hunters AND sea
	LocationID bestMove;
	LocationID currentLocation;
	//giveMeTheTrail(gameState, PLAYER_DRACULA, myTrail);
	//attempts to find a location not in hunter's possible moves

	//Pick starting location: Start in Zurich
	int currRound = giveMeTheRound(gameState);
	if (currRound == 0) {
		finalMove = "ZU";
		return finalMove;
	}
	//Dracula's available moves
//	printf("Round 2 onwards\n");
	LocationID* notSea = whereCanIgo(gameState, &numLocations, TRUE, FALSE);
	int i;
	printf("%d numlocations\n", numLocations);
	for (i = 0; i < numLocations; i++) {
		currentLocation = notSea[i];


		char *string1 = idToAbbrev(notSea[i]);
	       printf("%s ", string1);
	       printf("\ni is %d\n", i);




//		printf("notSea[i] passes\n");
		if (legalMove(gameState,currentLocation) == TRUE){
			if (avoidHunter(gameState,currentLocation) == TRUE){
				bestMove = currentLocation;
				seaAvoided = TRUE;
				break;
			}
		}
	}
//	printf("Sea avoided bit onwards\n");
	if (seaAvoided == FALSE){
		//couldn't avoid hunter, avoid sea
		if (avoidSea(gameState) != UNKNOWN_LOCATION){
			finalMove = idToAbbrev(avoidSea(gameState));
		//take any move if couldn't avoid either
		} else if (anyLegalMove(gameState) != UNKNOWN_LOCATION){
			finalMove = idToAbbrev(anyLegalMove(gameState));
		//no legal moves
		} else {
			finalMove = "HI";
		}
	// both sea and hunter can be avoided
	} else {
		finalMove = idToAbbrev(bestMove);
	}
//	printf("End final moves\n");
	return finalMove;
}

LocationID avoidSea (DracView gameState)
{
	int numLocations = 0;
	LocationID* dracPossibilities = whereCanIgo(gameState, &numLocations, TRUE, FALSE);
	LocationID move = UNKNOWN_LOCATION;
	LocationID curr;

	int i;

	printf("%d numlocations\n", numLocations);
	printf("Avoid sea drac Possibilities\n");
	for(i = 0; i < numLocations; i++){

	        curr = dracPossibilities[i];
	       char *string = idToAbbrev(dracPossibilities[i]);
	       printf("%s ", string);
	       printf("\ni is %d\n", i);

		if (legalMove(gameState, curr) == TRUE){
			move = curr;
			printf("i is %d\n", i);
			break;
		}
	}
	printf("\n");
	return move;
}

LocationID anyLegalMove (DracView gameState)
{
//	printf("Start any legalMove\n");
	int numLocations = 0;
	LocationID* dracPossibilities = whereCanIgo(gameState, &numLocations, TRUE, TRUE);
	LocationID legMove = UNKNOWN_LOCATION; // any legal move
	LocationID curr;
	int i;
	for(i = 0; i < numLocations; i++){
		curr = dracPossibilities[i];
		if (legalMove(gameState,curr) == TRUE){
			legMove = curr;
			break;
		}
	}
//	printf("End any legalMove\n");
	return legMove;
}

// to compare ONE of dracula's possible moves to a hunter's possible moves
// Checks if one of the hunter's moves will move onto dracula's position
int compareMoves (DracView gameState,LocationID currDrac,PlayerID hunter)
{
//	printf("Start any compareMoves\n");
	int numLocations = 0; // not sure what value to give
	LocationID* hunterPossibilites = whereCanTheyGo(gameState, &numLocations, hunter,
		TRUE, TRUE, TRUE);
	int i;
	int match = FALSE;
	for (i = 0; i < numLocations; i++){
		if (currDrac == hunterPossibilites[i]){
			match = TRUE;
			break;
		}
	}
//	printf("End any compareMoves\n");
	return match;
}

// to find if a location is in the hunter's path
int avoidHunter (DracView gameState,LocationID currentLocation)
{
//	printf("Start avoidHunter\n");
	int counter = 0;
	PlayerID hunter;
	for (hunter = 0; hunter < 4; hunter++){
		if (compareMoves(gameState, currentLocation, hunter) == FALSE) {
			counter++;
		}
	}
	int hunterAvoided;
	if (counter == 4){
		hunterAvoided = TRUE;
	} else {
		hunterAvoided = FALSE;
	}
//	printf("End avoidHunter\n");
	return hunterAvoided;
}

// to check if move is legal
int legalMove (DracView gameState, LocationID currentLocation)
{
//	printf("legalMove start\n");
	LocationID trail[TRAIL_SIZE];
	giveMeTheTrail(gameState, PLAYER_DRACULA, trail);
	int i;
	// check trail for repeated moves
	int flag = 0;
	int legal;

	for (i = 0; i < TRAIL_SIZE; i++){
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
//	printf("legalMove passes\n");
	return legal;
}
