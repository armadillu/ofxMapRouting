/*
 *  ofxMapRouting.h
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 25/02/12.
 *  Copyright 2012 uri.cat. All rights reserved.
 *
 */

#pragma once

#include "ofxTileMap.h"
#include "ofMain.h"
#include <queue>
#include <vector>
#include <set>
#include "ofxTimeMeasurements.h"

static int NodeCount = 0;

class p2 {
	public:
		p2(short x_, short y_){ x = x_; y = y_;}
		p2(){ x = y = 0;}
		short x, y;
};

class Node{

	public :
		
		short x, y;
					// f (node) = g (node) + h (node); 
		float g;	// cost of this node + it's predecessors
		float h;	// heuristic estimate of distance to goal
		float f;	// sum of cumulative cost of predecessors and self and heuristic
		Node* parent;


		Node();
		Node( short x_, short y_);
		~Node();
		
		void reset();
		
		inline bool isInSameLocation( Node* n ){
			return ( n->x == x && n->y == y );
		}
		
		inline Node& operator=(const Node& target){
			if (*this != target){
				x = target.x;
				y = target.y;
				g = target.g;
				h = target.h;
				f = target.f;
				parent = target.parent;
			}
			return *this;
		}

		inline bool operator == (const Node& target) const{
			return x == target.x && y == target.y;
		}

		inline bool operator != (const Node& target) const{
			return !(*this == target);
		}

		inline bool operator < (const Node& target) const{
			if( f < target.f ) {
				return true;
			} else if( f > target.f ) {
				return false;
			}
			//if f are equal, sort throguh space disposition
			if(x < target.x) {
				return true;
			} else if(x > target.x) {
				return false;
			} else {
				return (y < target.y);
			}
		}

		static void printNodeCount(){
			cout << "NodeCount: " << NodeCount << endl;
		}	
};


struct Compare{
	inline bool operator()(Node* a, Node* b) const{	
		return *a < *b;
	}
};


class ofxMapRouting{
	
	public:
		
		// if preAllocateAllNodes == true, all nodes for the map are preallocated
		// this will take lots of memory, but it makes the calc faster
		// set it to false to save memory, but routing will be roughly 60% slower
		ofxMapRouting( ofxTileMap * map_, bool preAllocateAllNodes = true );
		~ofxMapRouting(){
			//printf("delete ofxMapRouting\n");
			freeUnusedNodes();
			if (preAllocateNodes){
				for (int i = 0; i < nodes.size(); i++){
					delete nodes[i];
				}
			}
		}
		bool calcRoute( int originX, int originY, int destX , int destY, std::vector<p2> &solution, int maxIterations = 500);
	
		//vector<p2> getSolution(){ return solution;} //solution is returned in reverse order ( solution[0] == destination)
		ofxTileMap * getMap(){ return gameMap; }
		void set8Connected(bool conn8){ connect8 = conn8;}
		void freeUnusedNodes();
		set<Node*, Compare> open;
		set<Node*, Compare> closed;
	
		float perStepExtraCost; //add or subtract a bit of extra cost to each step, testing
		float roadCostGain; // [1 .. 255] 
		bool useSqrt;
		float eucledianDistWeight;
		
		//statistics
		float getLastSolutionPathCost(){ return lastSolutionPathCost; }
		int getLastSolutionNumSteps(){ return lastSolutionNumSteps; }
	
			
	private:
	
		vector<Node*> nodes;	// only used in preAllocate mode, holds complete list of nodes that form map
		map<int, Node*> allocatedNodes; //only used in !preAllocate mode to keep track of nodes we already allocated
	
		ofxTileMap * gameMap;	//the real map costs (might change, we always reference it)
		//std::vector <p2> solution;

		bool connect8;			//if true, diagonals are also neighbors, otherwise, only N S E W are checked
		bool preAllocateNodes;	//if true, a node per map pixel is preallocated before starting anything
								//otherwise, only walked nodes are allocated on the fly (and deallocated later)
		Node * start;
		Node * end;
	
		Node * getPreallocatedNodeAt(int x, int y); //use if preAlloc == true
	
		inline Node* getAllocatedNodeAt(int x, int y); //use this to find out (in preAlloc == false mode) if a node has been already allocated 
		inline void setAllocatedNodeAt(Node *, int x, int y); //same for storing

		bool calcRoute( Node* origin, Node* destination, std::vector<p2> &solution, int maxIterations);
		float distEstimate( Node* n, Node* dest );
		void getNeighbours( Node*, vector<Node*> &l, bool connect8 );
		bool canBeReached( Node* origin );
		void addNode(int x, int y, vector<Node*> &l); //add a Node* on that location to the supplied vector
		void reconstructPath( Node* destination, std::vector<p2> &solution ); 
	
		//statistics
		int lastSolutionNumSteps; //we store here long is the path to destination in our last calc
		float lastSolutionPathCost;

};
