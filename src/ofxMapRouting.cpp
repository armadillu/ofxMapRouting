/*
 *  ofxMapRouting.cpp
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 25/02/12.
 *  Copyright 2012 uri.cat. All rights reserved.
 *
 */

#include "ofxMapRouting.h"


Node::Node(){ 
	x = y = 0;
	reset();
	NodeCount++;
}

Node::Node( short x_, short y_){ 
	NodeCount++; x = x_, y = y_; g = h = f = 0; parent = NULL; 
}

void Node::reset(){ 
	g = h = f = 0;  	
	parent = NULL; 
}

Node::~Node(){ 
	NodeCount--; 
}

inline Node* ofxMapRouting::getPreallocatedNodeAt(int x, int y){ //only to be used in preAlloc mode
	
	if ( x >= 0 && x < gameMap->width && y >= 0 && y < gameMap->height ){
		return nodes[ gameMap->width * y + x ];
	}else {
		return NULL;
	}	
}


ofxMapRouting::ofxMapRouting( ofxTileMap * map_, bool preAllocateAllNodes_ ){	
	gameMap = map_;
	connect8 = false;
	perStepExtraCost = 0.0;
	roadCostGain = 1.0;
	eucledianDistWeight = 0.1;
	preAllocateNodes = preAllocateAllNodes_;

	if (preAllocateNodes){
		int w = gameMap->getWidth();
		int h = gameMap->getHeight();
		for(int j = 0; j < h; j++){
			for(int i = 0; i < w; i++){
				nodes.push_back( new Node(i, j) );
			}
		}
	}
}


bool ofxMapRouting::canBeReached(Node* origin){
	
	bool can = true;
	vector <Node*> destinationNeighborNodes;
	getNeighbours( origin, destinationNeighborNodes, connect8);
	if ( destinationNeighborNodes.empty() ){ //all neighbors are not walkable!
		can = false;
	}
	return can;
}


bool ofxMapRouting::calcRoute( int originX, int originY, int destX , int destY, std::vector<p2> &solution, int maxIterations){

	solution.clear();
	freeUnusedNodes();
	//Node::printNodeCount();
	lastSolutionNumSteps = -1;
	lastSolutionPathCost = -1;

	if (preAllocateNodes){
		start = getPreallocatedNodeAt( originX, originY );
		end = getPreallocatedNodeAt( destX, destY );
	}else{	
		std::vector <Node*> irrelevant;	
		addNode( originX, originY, irrelevant);
		addNode( destX, destY, irrelevant);
		start = getAllocatedNodeAt(originX, originY);
		end = getAllocatedNodeAt(destX, destY);
	}
	if (start == NULL || end == NULL){
		return false;
	}
	
	bool ok = calcRoute( start, end, solution, maxIterations );
	
	if ( ok ){		
		lastSolutionNumSteps = solution.size();
		lastSolutionPathCost = end->g;
		return true;
	}	
	return false;
}


template<typename Type>
void shrink_to_fit(std::vector<Type>& values){
    std::vector<Type>(values).swap(values);
}


void ofxMapRouting::freeUnusedNodes(){

	if (!preAllocateNodes){ //delete all nodes we previosuly allocated
		map<int,Node*>::iterator it;
		for (it = allocatedNodes.begin(); it != allocatedNodes.end(); ++it) {
			Node * n = it->second;
			delete n; //delete Node*
		}
		allocatedNodes.clear();
	}
	closed.clear();
	open.clear();	
}


bool ofxMapRouting::calcRoute( Node* origin, Node* destination, std::vector<p2> &solution, int maxIterations){
		
	if ( !canBeReached(origin) ){
		return false;
	}

	if ( !canBeReached(destination) ){
		return false;
	}
	
	int iterations = 0;
	
	origin->g = 0;
	origin->h = distEstimate( origin, destination );
	origin->f = origin->g + origin->h;
	origin->parent = NULL;
	
	open.insert( origin );
	
	while ( !open.empty() ){
		
		if (iterations > maxIterations){
			cout << "too many iterations!" << endl;
			return false;
		}
		
		Node* current = *(open.begin()); //get the node in open list with lowest f score 		
		
		if ( current->isInSameLocation( destination ) ){	//we made it to the destionation!			
			reconstructPath( destination, solution );			
			//printf("iterations: %d, open: %d, closed: %d\n", iterations, (int)open.size(), (int)closed.size());		
			return true;
		}
		open.erase(current);	//remove current from open
		closed.insert(current);	//add current to closed
		//printf("open: %d  closed: %d\n", open.size(), closed.size());

		vector <Node*> neighborNodes;	
		getNeighbours( current, neighborNodes, connect8); //this already ignores not walkable neighbor nodes (not added to neighborNodes list)
		
		for ( int i = 0; i < neighborNodes.size(); i++ ){
			
			Node * neighbor = neighborNodes[i];	//get current neighbor
			
			if ( 0 == closed.count(neighbor) ){ // neighbor is not in the closed list
								
				float updatedCost = current->g + roadCostGain * gameMap->getTile(neighbor->x, neighbor->y) + 0.00f + perStepExtraCost;
				
				if ( 1 == open.count(neighbor) ){ //neighbor is in the open set
					
					if ( updatedCost < neighbor->g ){
						open.erase( neighbor );	//put in front of list by removing it from open and re-adding it ?
						neighbor->parent = current;
						neighbor->g = updatedCost; 
						open.insert( neighbor );
					}
					
				}else{ //neighbor not found in the open set
					
					neighbor->parent = current;
					neighbor->h = distEstimate( neighbor, destination );					
					neighbor->g = updatedCost ;
					neighbor->f = neighbor->g + neighbor->h;
					open.insert( neighbor );
				}
			}
		}
		iterations++;
	}
	//cout << "No Solution" << endl;
	return false;
}


void ofxMapRouting::reconstructPath( Node* destination, std::vector<p2> &solution ){

	while (destination != NULL){
		p2 p;
		p.x = destination->x;
		p.y = destination->y;
		solution.push_back( p );
		destination = destination->parent;
	}
}


Node* ofxMapRouting::getAllocatedNodeAt(int x, int y){
	int index = gameMap->getWidth() * y + x;
	map<int,Node*>::iterator it;	
	it = allocatedNodes.find(index);
	
	if ( it == allocatedNodes.end() ){	//not found!		
		return NULL;
	}else{
		return it->second;
	}
}


inline void ofxMapRouting::setAllocatedNodeAt(Node *n, int x, int y){
	int index = gameMap->getWidth() * y + x;
	allocatedNodes[index] = n;
}


void ofxMapRouting::addNode(int xx, int yy, vector<Node*> &l){
	
	if ( gameMap->getTileSafe(xx, yy) != NOT_WALKABLE ){
		
		Node * n = NULL;
		if (preAllocateNodes){
			n = getPreallocatedNodeAt(xx, yy);
		}else{
			n = getAllocatedNodeAt(xx, yy);
			if (n == NULL){ //we havent allocated that one yet, lets do so
				n = new Node( xx, yy );
				setAllocatedNodeAt(n, xx, yy);
			}
		}
		if ( n != NULL) l.push_back( n );
	}
}


void ofxMapRouting::getNeighbours(Node* n,	vector<Node*> &l, bool connect8){
	
	int x = n->x;
	int y = n->y;
	int s = 1;
	
	addNode( x, y + s, l );
	addNode( x, y - s, l );
	addNode( x + s, y, l );	
	addNode( x - s, y, l );
	
	if (connect8){
		addNode( x + s, y + s, l );
		addNode( x + s, y - s, l );
		addNode( x - s, y + s, l );
		addNode( x - s, y - s, l );
	}
}


float ofxMapRouting::distEstimate( Node* n, Node* dest ){
	
	float a = dest->x - n->x;
	float b = dest->y - n->y;
	
	float d = ( a * a + b * b);
	if (useSqrt){
		d = sqrtf( d ) ; // leaving sqrtf out to speed up things
	}
	
	//doing a combination of real distance + 4/8 connected distances to obtain better results here
	//especially on 4-connected mode, we obtain diagonals by doing this, otherwise only get "elbow" paths
	
	if (connect8){
		return eucledianDistWeight * d + (1.0f - eucledianDistWeight) * MAX(fabs(a), fabs(b)) / (roadCostGain+1); 
	}else{
		
		return eucledianDistWeight * d + (1.0f - eucledianDistWeight) * (fabs(a) + fabs(b) ) / (roadCostGain+1) ;
	}
}

