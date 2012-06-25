/*
 *  ofxTileMap.h
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 25/02/12.
 *  Copyright 2012 uri.cat. All rights reserved.
 *
 */

#pragma once

//some tile values
#define NOT_WALKABLE 255 
#define MIN_COST 0

#include "ofMain.h"

class ofxTileMap{
	
public:

	ofxTileMap();
	ofxTileMap( int width, int height );
	ofxTileMap( string imagePath );
	~ofxTileMap();
	
	int getWidth(){ return width; }
	int getHeight(){ return height; }
	
	void draw();

	void randomize(int blockW , int blockH, int streetW); // in case u dont use an image
	
	unsigned char getTile(int x, int y);
	unsigned char getTileSafe(int x, int y);
	unsigned char * getTileAddressSafe(int x, int y);
	
	void setTile(int x, int y, unsigned char val);
	void setTileSafe(int x, int y, unsigned char val);
	
	void updateMapImage();

	int width;
	int height;
	unsigned char * map;
	ofImage mapImg;
};
