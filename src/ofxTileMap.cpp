/*
 *  ofxTileMap.cpp
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 25/02/12.
 *  Copyright 2012 uri.cat. All rights reserved.
 *
 */

#include "ofxTileMap.h"



ofxTileMap::ofxTileMap(){
	width = height = 0;
	map = NULL;
}

ofxTileMap::ofxTileMap( int width_, int height_ ){

	width = width_;
	height = height_;
	map = (unsigned char*) malloc( sizeof(unsigned char) * width * height );
	printf("Creating ofxTileMap of dimension %d x %d\n", width, height);
}

ofxTileMap::ofxTileMap( string imagePath ){

	mapImg.loadImage(imagePath);
	
	width = mapImg.width;
	height = mapImg.height;
	map = (unsigned char*) malloc( sizeof(unsigned char) * width * height );
	printf("Creating Game ofxTileMap of dimension %d x %d\n", width, height);
	
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++)	{
			int val = MIN_COST + mapImg.getColor(i, j).getBrightness();
			if (val > NOT_WALKABLE) val = NOT_WALKABLE;
			setTile(i, j, val);
		}
	}
}

ofxTileMap::~ofxTileMap(){
	delete map;
}

void ofxTileMap::draw(){
	if( mapImg.isAllocated() ){
		mapImg.draw(0,0);
	}
}

void ofxTileMap::updateMapImage(){
	int w = width;
	int h = height;
	for(int j = 0; j < h; j++){
		for(int i = 0; i < w; i++){
			mapImg.setColor(i, j, ofColor(getTile(i, j)) );
		}
	}
	mapImg.update();
}



void ofxTileMap::randomize(int blockW, int blockH, int streetW){
	
	for (int j = 0; j < height; j++ ) {
		for (int i = 0; i < width; i++ ) {
			setTileSafe(i , j , MIN_COST );
		}
	}
	
	for (int j = 0; j < height; j+= (blockH + streetW) ) {
		for (int i = 0; i < width; i+= (blockW + streetW) ) {
			if ( rand()%100 < 97 ){	//whole block
				for (int k = streetW; k < blockW + streetW; k++ ) {
					//blockW = ofRandom(3, 5);
					//blockH = ofRandom(3, 5);
					if ( rand()%100 < 98 ){		//one row inside block
						for (int l = 1; l < blockH + streetW; l++ ) {
							//if ( rand()%100 > 95 && ( l == 1 || k == streetW || l == blockH + streetW - 1 || k == blockW + streetW - 1) )
							//	setTileSafe(i + k, j + l, 128 );
							//else
								setTileSafe(i + k, j + l, NOT_WALKABLE );
						}				
					}
				}				
			}
		}
	}
	
	//make roads a bit dirty
	for (int j = 0; j < height; j++ ) {
		for (int i = 0; i < width; i++ ) {
			if ( getTile(i, j) == 0 ) setTileSafe(i , j , (rand()%100 < 95 ) ? MIN_COST : NOT_WALKABLE);
		}
	}
	
	//clear borders
	for (int j = 0; j < height; j++ ) {
		for (int i = 0; i < width; i++ ) {
			if (i == 0 || j == 0 || i == width - 1 || j == height - 1) setTileSafe(i , j , 0);
		}
	}
}


unsigned char ofxTileMap::getTile(int x, int y){
	return map[ width * y + x ];
}

unsigned char ofxTileMap::getTileSafe(int x, int y){
	if ( x >= 0 && x < width && y >= 0 && y < height ){
		return map[ width * y + x ];
	}else {
		return NOT_WALKABLE;
	}
}

unsigned char * ofxTileMap::getTileAddressSafe(int x, int y){
	if ( x >= 0 && x < width && y >= 0 && y < height ){
		return &map[ width * y + x ];
	}else {
		return NULL;
	}	
}



void ofxTileMap::setTile(int x, int y, unsigned char val){
	map[ width * y + x ] = val;
}


void ofxTileMap::setTileSafe(int x, int y, unsigned char val){

	if ( x >= 0 && x < width && y >= 0 && y < height ){
		map[ width * y + x ] = val;
	}else {
		cout << "can't set map Tile, out of bounds: " << x << ", " << y << endl;
	}
}
