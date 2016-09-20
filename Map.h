// Map.h ... interface to Map data type

#ifndef MAP_H
#define MAP_H

#include "Places.h"
#include "Globals.h"

typedef struct vNode *VList;
typedef struct edge{
    LocationID  start;
    LocationID  end;
    TransportID type;
} Edge;

// graph representation is hidden
typedef struct MapRep *Map;

// operations on graphs
Map  newMap();
void disposeMap(Map g);
void showMap(Map g);
int  numV(Map g);
int  numE(Map g, TransportID t);
LocationID *adjacencyChecker(VList L, LocationID v, PlayerID player, Round roundN, int *numLocations);
#endif
