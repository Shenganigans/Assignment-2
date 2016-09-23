// Map.c ... implementation of Map type
// (a specialised version of the Map ADT)
// You can change this as much as you want

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "Map.h"
#include "Places.h"

struct vNode {
   LocationID  v;    // ALICANTE, etc
   TransportID type; // ROAD, RAIL, BOAT
   VList       next; // link to next node
};

struct MapRep {
   int   nV;         // #vertices
   int   nE;         // #edges
   VList connections[NUM_MAP_LOCATIONS]; // array of lists
};


static void addSeaCities(LocationID sea, int *index, LocationID *connecs, Map m);
static void breadthSearch(int depth, LocationID station, int *index,
                          LocationID *connecs, Map m);

static int hasSeen(LocationID* connections, int index, LocationID to);
static void addConnections(Map g);

//connectedLocations() returns an array of LocationID that represent
//   all locations that are connected to the given LocationID.
// road, rail and sea are connections should only be considered
//   if the road, rail, sea parameters are TRUE.
// The size of the array is stored in the variable pointed to by numLocations
// The array can be in any order but must contain unique entries
// Your function must take into account the round and player id for rail travel
// Your function must take into account that Dracula can't move to
//   the hospital or travel by rail but need not take into account Dracula's trail
// The destination 'from' should be included in the array



// Create a new empty graph (for a map)
// #Vertices always same as NUM_PLACES
Map newMap()
{
   int i;
   Map g = malloc(sizeof(struct MapRep));
   assert(g != NULL);
   g->nV = NUM_MAP_LOCATIONS;
   for (i = 0; i < g->nV; i++){
      g->connections[i] = NULL;
   }
   g->nE = 0;
   addConnections(g);
   return g;
}

// Remove an existing graph
void disposeMap(Map g)
{
   int i;
   VList curr;
   VList next;
   assert(g != NULL);
   assert(g->connections != NULL);

   for (i = 0; i < g->nV; i++){
       curr = g->connections[i];
       while(curr != NULL){
          next = curr->next;
          free(curr);
          curr=next;
       }
   }
   free(g);
}

static VList insertVList(VList L, LocationID v, TransportID type)
{
   VList newV = malloc(sizeof(struct vNode));
   newV->v = v;
   newV->type = type;
   newV->next = L;
   return newV;
}

static int inVList(VList L, LocationID v, TransportID type)
{
	VList cur;
	for (cur = L; cur != NULL; cur = cur->next) {
		if (cur->v == v && cur->type == type) return 1;
	}
	return 0;
}

// Add a new edge to the Map/Graph
void addLink(Map g, LocationID start, LocationID end, TransportID type)
{
	assert(g != NULL);
	// don't add edges twice
	if (!inVList(g->connections[start],end,type)) {
   	g->connections[start] = insertVList(g->connections[start],end,type);
   	g->connections[end] = insertVList(g->connections[end],start,type);
   	g->nE++;
	}
}

// Display content of Map/Graph
void showMap(Map g)
{
   assert(g != NULL);
   printf("V=%d, E=%d\n", g->nV, g->nE);
   int i;
   for (i = 0; i < g->nV; i++) {
      VList n = g->connections[i];
      while (n != NULL) {
         printf("%s connects to %s ",idToName(i),idToName(n->v));
         switch (n->type) {
         case ROAD: printf("by road\n"); break;
         case RAIL: printf("by rail\n"); break;
         case BOAT: printf("by boat\n"); break;
         default:   printf("by ????\n"); break;
         }
         n = n->next;
      }
   }
}

// Return count of nodes
int numV(Map g)
{
   assert(g != NULL);
   return g->nV;
}

// Return count of edges of a particular type
int numE(Map g, TransportID type)
{
   int i, nE=0;
   assert(g != NULL);
   assert(type >= 0 && type <= ANY);
   for (i = 0; i < g->nV; i++) {
      VList n = g->connections[i];
      while (n != NULL) {
         if (n->type == type || type == ANY) nE++;
         n = n->next;
      }
    }
    return nE;
}

// *************************
LocationID *connecLocations(GameView currentView, int *numLocations,
                               LocationID from, PlayerID player, Round round,
                               int road, int rail, int sea)
{
    LocationID *connecs = malloc((NUM_MAP_LOCATIONS)*(sizeof(LocationID))); //create array
    Map m = newMap();
    int depth = (player + round)%4;
    if(player == PLAYER_DRACULA) rail = FALSE; // Dracula cannot move by rail
    if(depth == 0) rail = FALSE; // no rail permitted

    VList curr = m->connections[from]; // points to beginning of from list
    int index = 0;
    connecs[index] = from;

    while(curr != NULL){ //for all immediately adjacent
      if(hasSeen(connecs, index, curr->v) == TRUE){
        // do nothing --already seen
      } else {
        if((curr->type == BOAT && sea == FALSE)||
           (curr->type == RAIL && rail == FALSE)||
           (curr->type == ROAD && road == FALSE)||
           (player == PLAYER_DRACULA && curr->v == ST_JOSEPH_AND_ST_MARYS)){
          // do nothing --transport type is disabled
        } else {
          connecs[++index] = curr->v; // add to seen/connections
          if(isSea(curr->v) && (idToType(from)!= SEA)){
            // add sea cities
            addSeaCities(curr->v, &index, connecs, m);
          }
          if(curr->type == RAIL && depth > 1){
            // breadth search for rail reacheable cities
            breadthSearch(depth, curr->v, &index, connecs, m);
          }
        }
      }
      curr = curr->next;
    }

    connecs[++index] = -1;
    *numLocations = index;
    return connecs;
}

static void addSeaCities(LocationID sea, int *index, LocationID *connecs, Map m){
  VList curr = m->connections[sea];
  while(curr!= NULL){
    if(!hasSeen(connecs, *index, curr->v)){
      if(idToType(curr->v)!=SEA) connecs[++(*index)] = curr->v; // add to seen
    }
    curr = curr->next;
  }
}

static void breadthSearch(int depth, LocationID station, int *index,
                          LocationID *connecs, Map m){
  // pre-condition that depth will be 2 or 3
  VList curr = m->connections[station];
  printf("enters with %s...", idToName(station));
  if(depth == 4) printf("recursion...");
  while(curr != NULL){
    if(curr->type == RAIL){
      if(!hasSeen(connecs, *index, curr->v)){
        connecs[++(*index)] = curr->v; //depth 2
        printf("%s... ", idToName(curr->v));
        if(depth == 3) breadthSearch(4,curr->v, index,connecs, m);
      }
    }
    curr = curr->next;
  }
}

static int hasSeen(LocationID* connections, int index, LocationID to){

  if(index == -1) return FALSE; //hasn't seen anything yet
  while(index >= 0){
    if(connections[index] == to) return TRUE;
    index--;
  }
  return FALSE;
}
//*************************

// Add edges to Graph representing map of Europe
static void addConnections(Map g)
{
   //### ROAD Connections ###

   addLink(g, ALICANTE, GRANADA, ROAD);
   addLink(g, ALICANTE, MADRID, ROAD);
   addLink(g, ALICANTE, SARAGOSSA, ROAD);
   addLink(g, AMSTERDAM, BRUSSELS, ROAD);
   addLink(g, AMSTERDAM, COLOGNE, ROAD);
   addLink(g, ATHENS, VALONA, ROAD);
   addLink(g, BARCELONA, SARAGOSSA, ROAD);
   addLink(g, BARCELONA, TOULOUSE, ROAD);
   addLink(g, BARI, NAPLES, ROAD);
   addLink(g, BARI, ROME, ROAD);
   addLink(g, BELGRADE, BUCHAREST, ROAD);
   addLink(g, BELGRADE, KLAUSENBURG, ROAD);
   addLink(g, BELGRADE, SARAJEVO, ROAD);
   addLink(g, BELGRADE, SOFIA, ROAD);
   addLink(g, BELGRADE, ST_JOSEPH_AND_ST_MARYS, ROAD);
   addLink(g, BELGRADE, SZEGED, ROAD);
   addLink(g, BERLIN, HAMBURG, ROAD);
   addLink(g, BERLIN, LEIPZIG, ROAD);
   addLink(g, BERLIN, PRAGUE, ROAD);
   addLink(g, BORDEAUX, CLERMONT_FERRAND, ROAD);
   addLink(g, BORDEAUX, NANTES, ROAD);
   addLink(g, BORDEAUX, SARAGOSSA, ROAD);
   addLink(g, BORDEAUX, TOULOUSE, ROAD);
   addLink(g, BRUSSELS, AMSTERDAM, ROAD);
   addLink(g, BRUSSELS, COLOGNE, ROAD);
   addLink(g, BRUSSELS, LE_HAVRE, ROAD);
   addLink(g, BRUSSELS, PARIS, ROAD);
   addLink(g, BRUSSELS, STRASBOURG, ROAD);
   addLink(g, BUCHAREST, CONSTANTA, ROAD);
   addLink(g, BUCHAREST, GALATZ, ROAD);
   addLink(g, BUCHAREST, KLAUSENBURG, ROAD);
   addLink(g, BUCHAREST, SOFIA, ROAD);
   addLink(g, BUCHAREST, BELGRADE, ROAD);
   addLink(g, BUDAPEST, KLAUSENBURG, ROAD);
   addLink(g, BUDAPEST, SZEGED, ROAD);
   addLink(g, BUDAPEST, VIENNA, ROAD);
   addLink(g, BUDAPEST, ZAGREB, ROAD);
   addLink(g, CADIZ, GRANADA, ROAD);
   addLink(g, CADIZ, LISBON, ROAD);
   addLink(g, CADIZ, MADRID, ROAD);
   addLink(g, CASTLE_DRACULA, GALATZ, ROAD);
   addLink(g, CASTLE_DRACULA, KLAUSENBURG, ROAD);
   addLink(g, CLERMONT_FERRAND, BORDEAUX, ROAD);
   addLink(g, CLERMONT_FERRAND, GENEVA, ROAD);
   addLink(g, CLERMONT_FERRAND, MARSEILLES, ROAD);
   addLink(g, CLERMONT_FERRAND, NANTES, ROAD);
   addLink(g, CLERMONT_FERRAND, PARIS, ROAD);
   addLink(g, CLERMONT_FERRAND, TOULOUSE, ROAD);
   addLink(g, COLOGNE, AMSTERDAM, ROAD);
   addLink(g, COLOGNE, BRUSSELS, ROAD);
   addLink(g, COLOGNE, FRANKFURT, ROAD);
   addLink(g, COLOGNE, HAMBURG, ROAD);
   addLink(g, COLOGNE, LEIPZIG, ROAD);
   addLink(g, COLOGNE, STRASBOURG, ROAD);
   addLink(g, CONSTANTA, BUCHAREST, ROAD);
   addLink(g, CONSTANTA, GALATZ, ROAD);
   addLink(g, CONSTANTA, VARNA, ROAD);
   addLink(g, DUBLIN, GALWAY, ROAD);
   addLink(g, EDINBURGH, MANCHESTER, ROAD);
   addLink(g, FLORENCE, GENOA, ROAD);
   addLink(g, FLORENCE, ROME, ROAD);
   addLink(g, FLORENCE, VENICE, ROAD);
   addLink(g, FRANKFURT, COLOGNE, ROAD);
   addLink(g, FRANKFURT, LEIPZIG, ROAD);
   addLink(g, FRANKFURT, NUREMBURG, ROAD);
   addLink(g, FRANKFURT, STRASBOURG, ROAD);
   addLink(g, GALATZ, KLAUSENBURG, ROAD);
   addLink(g, GALATZ, BUCHAREST, ROAD);
   addLink(g, GALATZ, CASTLE_DRACULA, ROAD);
   addLink(g, GALATZ, CONSTANTA, ROAD);
   addLink(g, GALWAY, DUBLIN, ROAD);
   addLink(g, GENEVA, CLERMONT_FERRAND, ROAD);
   addLink(g, GENEVA, MARSEILLES, ROAD);
   addLink(g, GENEVA, PARIS, ROAD);
   addLink(g, GENEVA, STRASBOURG, ROAD);
   addLink(g, GENEVA, ZURICH, ROAD);
   addLink(g, GENOA, FLORENCE, ROAD);
   addLink(g, GENOA, MARSEILLES, ROAD);
   addLink(g, GENOA, MILAN, ROAD);
   addLink(g, GENOA, VENICE, ROAD);
   addLink(g, GRANADA, ALICANTE, ROAD);
   addLink(g, GRANADA, CADIZ, ROAD);
   addLink(g, GRANADA, MADRID, ROAD);
   addLink(g, HAMBURG, BERLIN, ROAD);
   addLink(g, HAMBURG, COLOGNE, ROAD);
   addLink(g, HAMBURG, LEIPZIG, ROAD);
   addLink(g, KLAUSENBURG, BELGRADE, ROAD);
   addLink(g, KLAUSENBURG, BUCHAREST, ROAD);
   addLink(g, KLAUSENBURG, BUDAPEST, ROAD);
   addLink(g, KLAUSENBURG, CASTLE_DRACULA, ROAD);
   addLink(g, KLAUSENBURG, GALATZ, ROAD);
   addLink(g, KLAUSENBURG, SZEGED, ROAD);
   addLink(g, LEIPZIG, BERLIN, ROAD);
   addLink(g, LEIPZIG, HAMBURG, ROAD);
   addLink(g, LEIPZIG, FRANKFURT, ROAD);
   addLink(g, LEIPZIG, COLOGNE, ROAD);
   addLink(g, LEIPZIG, NUREMBURG, ROAD);
   addLink(g, LE_HAVRE, BRUSSELS, ROAD);
   addLink(g, LE_HAVRE, NANTES, ROAD);
   addLink(g, LE_HAVRE, PARIS, ROAD);
   addLink(g, LISBON, CADIZ, ROAD);
   addLink(g, LISBON, MADRID, ROAD);
   addLink(g, LISBON, SANTANDER, ROAD);
   addLink(g, LIVERPOOL, MANCHESTER, ROAD);
   addLink(g, LIVERPOOL, SWANSEA, ROAD);
   addLink(g, LONDON, MANCHESTER, ROAD);
   addLink(g, LONDON, PLYMOUTH, ROAD);
   addLink(g, LONDON, SWANSEA, ROAD);
   addLink(g, MADRID, ALICANTE, ROAD);
   addLink(g, MADRID, GRANADA, ROAD);
   addLink(g, MADRID, CADIZ, ROAD);
   addLink(g, MADRID, LISBON, ROAD);
   addLink(g, MADRID, SANTANDER, ROAD);
   addLink(g, MADRID, SARAGOSSA, ROAD);
   addLink(g, MARSEILLES, CLERMONT_FERRAND, ROAD);
   addLink(g, MARSEILLES, GENEVA, ROAD);
   addLink(g, MARSEILLES, GENOA, ROAD);
   addLink(g, MARSEILLES, MILAN, ROAD);
   addLink(g, MARSEILLES, TOULOUSE, ROAD);
   addLink(g, MARSEILLES, ZURICH, ROAD);
   addLink(g, MILAN, GENOA, ROAD);
   addLink(g, MILAN, MARSEILLES, ROAD);
   addLink(g, MILAN, MUNICH, ROAD);
   addLink(g, MILAN, VENICE, ROAD);
   addLink(g, MILAN, ZURICH, ROAD);
   addLink(g, MUNICH, MILAN, ROAD);
   addLink(g, MUNICH, NUREMBURG, ROAD);
   addLink(g, MUNICH, STRASBOURG, ROAD);
   addLink(g, MUNICH, VENICE, ROAD);
   addLink(g, MUNICH, VIENNA, ROAD);
   addLink(g, MUNICH, ZAGREB, ROAD);
   addLink(g, MUNICH, ZURICH, ROAD);
   addLink(g, NANTES, LE_HAVRE, ROAD);
   addLink(g, NANTES, CLERMONT_FERRAND, ROAD);
   addLink(g, NANTES, BORDEAUX, ROAD);
   addLink(g, NANTES, PARIS, ROAD);
   addLink(g, NAPLES, ROME, ROAD);
   addLink(g, NAPLES, BARI, ROAD);
   addLink(g, NUREMBURG, LEIPZIG, ROAD);
   addLink(g, NUREMBURG, MUNICH, ROAD);
   addLink(g, NUREMBURG, FRANKFURT, ROAD);
   addLink(g, NUREMBURG, PRAGUE, ROAD);
   addLink(g, NUREMBURG, STRASBOURG, ROAD);
   addLink(g, PARIS, BRUSSELS, ROAD);
   addLink(g, PARIS, GENEVA, ROAD);
   addLink(g, PARIS, CLERMONT_FERRAND, ROAD);
   addLink(g, PARIS, NANTES, ROAD);
   addLink(g, PARIS, LE_HAVRE, ROAD);
   addLink(g, PARIS, STRASBOURG, ROAD);
   addLink(g, PRAGUE, VIENNA, ROAD);
   addLink(g, PRAGUE, NUREMBURG, ROAD);
   addLink(g, PRAGUE, BERLIN, ROAD);
   addLink(g, ROME, FLORENCE, ROAD);
   addLink(g, ROME, NAPLES, ROAD);
   addLink(g, ROME, BARI, ROAD);
   addLink(g, PLYMOUTH, LONDON, ROAD);
   addLink(g, SALONICA, SOFIA, ROAD);
   addLink(g, SALONICA, VALONA, ROAD);
   addLink(g, SANTANDER, SARAGOSSA, ROAD);
   addLink(g, SANTANDER, MADRID, ROAD);
   addLink(g, SANTANDER, LISBON, ROAD);
   addLink(g, SARAGOSSA, TOULOUSE, ROAD);
   addLink(g, SARAGOSSA, BORDEAUX, ROAD);
   addLink(g, SARAGOSSA, BARCELONA, ROAD);
   addLink(g, SARAGOSSA, ALICANTE, ROAD);
   addLink(g, SARAGOSSA, MADRID, ROAD);
   addLink(g, SARAGOSSA, SANTANDER, ROAD);
   addLink(g, SARAJEVO, SOFIA, ROAD);
   addLink(g, SARAJEVO, ST_JOSEPH_AND_ST_MARYS, ROAD);
   addLink(g, SARAJEVO, VALONA, ROAD);
   addLink(g, SARAJEVO, ZAGREB, ROAD);
   addLink(g, SARAJEVO, BELGRADE, ROAD);
   addLink(g, SOFIA, VALONA, ROAD);
   addLink(g, SOFIA, VARNA, ROAD);
   addLink(g, SOFIA, BUCHAREST, ROAD);
   addLink(g, SOFIA, SALONICA, ROAD);
   addLink(g, SOFIA, SARAJEVO, ROAD);
   addLink(g, SOFIA, BELGRADE, ROAD);
   addLink(g, STRASBOURG, ZURICH, ROAD);
   addLink(g, STRASBOURG, MUNICH, ROAD);
   addLink(g, STRASBOURG, NUREMBURG, ROAD);
   addLink(g, STRASBOURG, FRANKFURT, ROAD);
   addLink(g, STRASBOURG, COLOGNE, ROAD);
   addLink(g, STRASBOURG, BRUSSELS, ROAD);
   addLink(g, STRASBOURG, PARIS, ROAD);
   addLink(g, STRASBOURG, GENEVA, ROAD);
   addLink(g, ST_JOSEPH_AND_ST_MARYS, BELGRADE, ROAD);
   addLink(g, ST_JOSEPH_AND_ST_MARYS, SARAJEVO, ROAD);
   addLink(g, ST_JOSEPH_AND_ST_MARYS, SZEGED, ROAD);
   addLink(g, ST_JOSEPH_AND_ST_MARYS, ZAGREB, ROAD);
   addLink(g, SZEGED, ZAGREB, ROAD);
   addLink(g, SZEGED, BUDAPEST, ROAD);
   addLink(g, SZEGED, KLAUSENBURG, ROAD);
   addLink(g, SZEGED, BELGRADE, ROAD);
   addLink(g, SZEGED, ST_JOSEPH_AND_ST_MARYS, ROAD);
   addLink(g, TOULOUSE, BORDEAUX, ROAD);
   addLink(g, TOULOUSE, CLERMONT_FERRAND, ROAD);
   addLink(g, TOULOUSE, MARSEILLES, ROAD);
   addLink(g, TOULOUSE, BARCELONA, ROAD);
   addLink(g, TOULOUSE, SARAGOSSA, ROAD);
   addLink(g, VIENNA, ZAGREB, ROAD);
   addLink(g, VIENNA, PRAGUE, ROAD);
   addLink(g, VIENNA, BUDAPEST, ROAD);
   addLink(g, VIENNA, MUNICH, ROAD);
   addLink(g, MANCHESTER, EDINBURGH, ROAD);
   addLink(g, MANCHESTER, LIVERPOOL, ROAD);
   addLink(g, MANCHESTER, LONDON, ROAD);
   addLink(g, MANCHESTER, LONDON, ROAD);
   addLink(g, SWANSEA, LONDON, ROAD);
   addLink(g, SWANSEA, LIVERPOOL, ROAD);
   addLink(g, ZURICH, STRASBOURG, ROAD);
   addLink(g, ZURICH, MUNICH, ROAD);
   addLink(g, ZURICH, MILAN, ROAD);
   addLink(g, ZURICH, MARSEILLES, ROAD);
   addLink(g, ZURICH, GENEVA, ROAD);
   addLink(g, VENICE, MUNICH, ROAD);
   addLink(g, VENICE, MILAN, ROAD);
   addLink(g, VENICE, GENOA, ROAD);
   addLink(g, VENICE, FLORENCE, ROAD);
   addLink(g, VARNA, CONSTANTA, ROAD);
   addLink(g, VARNA, SOFIA, ROAD);
   addLink(g, VALONA, ATHENS, ROAD);
   addLink(g, ZAGREB, MUNICH, ROAD);
   addLink(g, ZAGREB, VIENNA, ROAD);
   addLink(g, ZAGREB, BUDAPEST, ROAD);
   addLink(g, ZAGREB, SZEGED, ROAD);
   addLink(g, ZAGREB, ST_JOSEPH_AND_ST_MARYS, ROAD);
   addLink(g, ZAGREB, SARAJEVO, ROAD);

   //### RAIL Connections ###

   addLink(g, ALICANTE, BARCELONA, RAIL);
   addLink(g, ALICANTE, MADRID, RAIL);
   addLink(g, BARCELONA, SARAGOSSA, RAIL);
   addLink(g, BARCELONA, ALICANTE, RAIL);
   addLink(g, BARI, NAPLES, RAIL);
   addLink(g, BELGRADE, SOFIA, RAIL);
   addLink(g, BELGRADE, SZEGED, RAIL);
   addLink(g, BERLIN, HAMBURG, RAIL);
   addLink(g, BERLIN, LEIPZIG, RAIL);
   addLink(g, BERLIN, PRAGUE, RAIL);
   addLink(g, BORDEAUX, PARIS, RAIL);
   addLink(g, BORDEAUX, SARAGOSSA, RAIL);
   addLink(g, BRUSSELS, COLOGNE, RAIL);
   addLink(g, BRUSSELS, PARIS, RAIL);
   addLink(g, BUCHAREST, CONSTANTA, RAIL);
   addLink(g, BUCHAREST, GALATZ, RAIL);
   addLink(g, BUCHAREST, SZEGED, RAIL);
   addLink(g, BUDAPEST, VIENNA, RAIL);
   addLink(g, COLOGNE, FRANKFURT, RAIL);
   addLink(g, COLOGNE, BRUSSELS, RAIL);
   addLink(g, CONSTANTA, BUCHAREST, RAIL);
   addLink(g, EDINBURGH, MANCHESTER, RAIL);
   addLink(g, FLORENCE, MILAN, RAIL);
   addLink(g, FLORENCE, ROME, RAIL);
   addLink(g, FRANKFURT, LEIPZIG, RAIL);
   addLink(g, FRANKFURT, STRASBOURG, RAIL);
   addLink(g, FRANKFURT, COLOGNE, RAIL);
   addLink(g, GALATZ, BUCHAREST, RAIL);
   addLink(g, GENEVA, MILAN, RAIL);
   addLink(g, GENOA, MILAN, RAIL);
   addLink(g, HAMBURG, BERLIN, RAIL);
   addLink(g, LEIPZIG, NUREMBURG, RAIL);
   addLink(g, LEIPZIG, BERLIN, RAIL);
   addLink(g, LEIPZIG, FRANKFURT, RAIL);
   addLink(g, LE_HAVRE, PARIS, RAIL);
   addLink(g, LISBON, MADRID, RAIL);
   addLink(g, LIVERPOOL, MANCHESTER, RAIL);
   addLink(g, LONDON, MANCHESTER, RAIL);
   addLink(g, LONDON, SWANSEA, RAIL);
   addLink(g, MADRID, ALICANTE, RAIL);
   addLink(g, MADRID, SANTANDER, RAIL);
   addLink(g, MADRID, SARAGOSSA, RAIL);
   addLink(g, MADRID, LISBON, RAIL);
   addLink(g, MANCHESTER, EDINBURGH, RAIL);
   addLink(g, MANCHESTER, LIVERPOOL, RAIL);
   addLink(g, MANCHESTER, LONDON, RAIL);
   addLink(g, MARSEILLES, PARIS, RAIL);
   addLink(g, MILAN, FLORENCE, RAIL);
   addLink(g, MILAN, GENOA, RAIL);
   addLink(g, MILAN, GENEVA, RAIL);
   addLink(g, MILAN, ZURICH, RAIL);
   addLink(g, MUNICH, NUREMBURG, RAIL);
   addLink(g, NAPLES, ROME, RAIL);
   addLink(g, NAPLES, BARI, RAIL);
   addLink(g, NUREMBURG, LEIPZIG, RAIL);
   addLink(g, NUREMBURG, MUNICH, RAIL);
   addLink(g, PARIS, BORDEAUX, RAIL);
   addLink(g, PARIS, BRUSSELS, RAIL);
   addLink(g, PARIS, LE_HAVRE, RAIL);
   addLink(g, PARIS, MARSEILLES, RAIL);
   addLink(g, PRAGUE, VIENNA, RAIL);
   addLink(g, PRAGUE, BERLIN, RAIL);
   addLink(g, ROME, FLORENCE, RAIL);
   addLink(g, ROME, NAPLES, RAIL);
   addLink(g, SALONICA, SOFIA, RAIL);
   addLink(g, SANTANDER, MADRID, RAIL);
   addLink(g, SARAGOSSA, BARCELONA, RAIL);
   addLink(g, SARAGOSSA, MADRID, RAIL);
   addLink(g, SARAGOSSA, BORDEAUX, RAIL);
   addLink(g, SOFIA, VARNA, RAIL);
   addLink(g, SOFIA, BELGRADE, RAIL);
   addLink(g, SOFIA, SALONICA, RAIL);
   addLink(g, STRASBOURG, ZURICH, RAIL);
   addLink(g, STRASBOURG, FRANKFURT, RAIL);
   addLink(g, SWANSEA, LONDON, RAIL);
   addLink(g, SZEGED, BELGRADE, RAIL);
   addLink(g, SZEGED, BUCHAREST, RAIL);
   addLink(g, SZEGED, BUDAPEST, RAIL);
   addLink(g, VARNA, SOFIA, RAIL);
   addLink(g, VENICE, VIENNA, RAIL);
   addLink(g, VIENNA, PRAGUE, RAIL);
   addLink(g, VIENNA, VENICE, RAIL);
   addLink(g, ZURICH, STRASBOURG, RAIL);
   addLink(g, ZURICH, MILAN, RAIL);

   //### BOAT Connections ###

   addLink(g, ADRIATIC_SEA, BARI, BOAT);
   addLink(g, ADRIATIC_SEA, IONIAN_SEA, BOAT);
   addLink(g, ADRIATIC_SEA, VENICE, BOAT);
   addLink(g, ALICANTE, MEDITERRANEAN_SEA, BOAT);
   addLink(g, AMSTERDAM, NORTH_SEA, BOAT);
   addLink(g, ATHENS, IONIAN_SEA, BOAT);
   addLink(g, ATLANTIC_OCEAN, BAY_OF_BISCAY, BOAT);
   addLink(g, ATLANTIC_OCEAN, CADIZ, BOAT);
   addLink(g, ATLANTIC_OCEAN, ENGLISH_CHANNEL, BOAT);
   addLink(g, ATLANTIC_OCEAN, GALWAY, BOAT);
   addLink(g, ATLANTIC_OCEAN, IRISH_SEA, BOAT);
   addLink(g, ATLANTIC_OCEAN, LISBON, BOAT);
   addLink(g, ATLANTIC_OCEAN, MEDITERRANEAN_SEA, BOAT);
   addLink(g, ATLANTIC_OCEAN, NORTH_SEA, BOAT);
   addLink(g, BARCELONA, MEDITERRANEAN_SEA, BOAT);
   addLink(g, BARI, ADRIATIC_SEA, BOAT);
   addLink(g, BAY_OF_BISCAY, BORDEAUX, BOAT);
   addLink(g, BAY_OF_BISCAY, CADIZ, BOAT);
   addLink(g, BAY_OF_BISCAY, NANTES, BOAT);
   addLink(g, BAY_OF_BISCAY, SANTANDER, BOAT);
   addLink(g, BAY_OF_BISCAY, ATLANTIC_OCEAN, BOAT);
   addLink(g, BLACK_SEA, CONSTANTA, BOAT);
   addLink(g, BLACK_SEA, IONIAN_SEA, BOAT);
   addLink(g, BLACK_SEA, VARNA, BOAT);
   addLink(g, BORDEAUX, BAY_OF_BISCAY, BOAT);
   addLink(g, CADIZ, BAY_OF_BISCAY, BOAT);
   addLink(g, CAGLIARI, MEDITERRANEAN_SEA, BOAT);
   addLink(g, CAGLIARI, TYRRHENIAN_SEA, BOAT);
   addLink(g, CONSTANTA, BLACK_SEA, BOAT);
   addLink(g, DUBLIN, IRISH_SEA, BOAT);
   addLink(g, EDINBURGH, NORTH_SEA, BOAT);
   addLink(g, ENGLISH_CHANNEL, LE_HAVRE, BOAT);
   addLink(g, ENGLISH_CHANNEL, ATLANTIC_OCEAN, BOAT);
   addLink(g, ENGLISH_CHANNEL, LONDON, BOAT);
   addLink(g, ENGLISH_CHANNEL, NORTH_SEA, BOAT);
   addLink(g, ENGLISH_CHANNEL, PLYMOUTH, BOAT);
   addLink(g, GALWAY, ATLANTIC_OCEAN, BOAT);
   addLink(g, GENOA, TYRRHENIAN_SEA, BOAT);
   addLink(g, HAMBURG, NORTH_SEA, BOAT);
   addLink(g, IONIAN_SEA, SALONICA, BOAT);
   addLink(g, IONIAN_SEA, BLACK_SEA, BOAT);
   addLink(g, IONIAN_SEA, TYRRHENIAN_SEA, BOAT);
   addLink(g, IONIAN_SEA, VALONA, BOAT);
   addLink(g, IONIAN_SEA, ADRIATIC_SEA, BOAT);
   addLink(g, IONIAN_SEA, ATHENS, BOAT);
   addLink(g, IRISH_SEA, LIVERPOOL, BOAT);
   addLink(g, IRISH_SEA, SWANSEA, BOAT);
   addLink(g, IRISH_SEA, DUBLIN, BOAT);
   addLink(g, IRISH_SEA, ATLANTIC_OCEAN, BOAT);
   addLink(g, LE_HAVRE, ENGLISH_CHANNEL, BOAT);
   addLink(g, LISBON, ATLANTIC_OCEAN, BOAT);
   addLink(g, LIVERPOOL, IRISH_SEA, BOAT);
   addLink(g, LONDON, ENGLISH_CHANNEL, BOAT);
   addLink(g, MARSEILLES, MEDITERRANEAN_SEA, BOAT);
   addLink(g, MEDITERRANEAN_SEA, MARSEILLES, BOAT);
   addLink(g, MEDITERRANEAN_SEA, TYRRHENIAN_SEA, BOAT);
   addLink(g, MEDITERRANEAN_SEA, ALICANTE, BOAT);
   addLink(g, MEDITERRANEAN_SEA, ATLANTIC_OCEAN, BOAT);
   addLink(g, MEDITERRANEAN_SEA, BARCELONA, BOAT);
   addLink(g, MEDITERRANEAN_SEA, CAGLIARI, BOAT);
   addLink(g, NANTES, BAY_OF_BISCAY, BOAT);
   addLink(g, NAPLES, TYRRHENIAN_SEA, BOAT);
   addLink(g, NORTH_SEA, AMSTERDAM, BOAT);
   addLink(g, NORTH_SEA, EDINBURGH, BOAT);
   addLink(g, NORTH_SEA, ATLANTIC_OCEAN, BOAT);
   addLink(g, NORTH_SEA, HAMBURG, BOAT);
   addLink(g, NORTH_SEA, ENGLISH_CHANNEL, BOAT);
   addLink(g, PLYMOUTH, ENGLISH_CHANNEL, BOAT);
   addLink(g, ROME, TYRRHENIAN_SEA, BOAT);
   addLink(g, SALONICA, IONIAN_SEA, BOAT);
   addLink(g, SANTANDER, BAY_OF_BISCAY, BOAT);
   addLink(g, SWANSEA, IRISH_SEA, BOAT);
   addLink(g, TYRRHENIAN_SEA, ROME, BOAT);
   addLink(g, TYRRHENIAN_SEA, NANTES, BOAT);
   addLink(g, TYRRHENIAN_SEA, MEDITERRANEAN_SEA, BOAT);
   addLink(g, TYRRHENIAN_SEA, CAGLIARI, BOAT);
   addLink(g, TYRRHENIAN_SEA, GENOA, BOAT);
   addLink(g, TYRRHENIAN_SEA, IONIAN_SEA, BOAT);
   addLink(g, VALONA, IONIAN_SEA, BOAT);
   addLink(g, VARNA, BLACK_SEA, BOAT);
   addLink(g, VENICE, ADRIATIC_SEA, BOAT);
}
