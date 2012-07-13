#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofEnableAlphaBlending();
	ofSetFrameRate(60);
	
	map = new ofxTileMap("path.png");
	routing = new ofxMapRouting( map, false );
	
	ofSetWindowShape(map->getWidth(), map->getHeight());
	ofSetWindowPosition(12, 12);
	setUIParams();
	origin.x = map->getWidth()/2;
	origin.y = map->getHeight()/2 - 5;
	
	//plot = new ofxHistoryPlot( NULL, "mem", ofGetWidth()*15, false); //NULL cos we don't want it to auto-update. confirmed by "true"
	//plot->setRange(0, ofGetHeight());
	//plot->setColor( ofColor(255,0,0) );
	//plot->setShowNumericalInfo(true);
	//plot->setRespectBorders(true);
	//plot->setLineWidth(1);	

	//mem.setup();

}

//--------------------------------------------------------------
void testApp::update(){
	
//	target.x = ofRandom(0, map->getWidth());
//	target.y = ofRandom(0, map->getHeight());;

	routing->set8Connected(connect8);

	TIME_SAMPLE_START("calcRoute");
	routing->calcRoute( origin.x, origin.y, target.x, target.y, res, maxIterations);
	TIME_SAMPLE_STOP("calcRoute");
	
	//mem.update();
	//if ( ofGetFrameNum()% == 1 ){
	//float m = mem.getProcessMemory() /1024.;
//	if (ofGetFrameNum() == 120){
//		plot->setLowerRange(m);
//	}
//	plot->update( m );

}

//--------------------------------------------------------------
void testApp::draw(){

	ofSetColor(64);
	map->draw();
	glPointSize(1);

	//closed nodes
	ofMesh m2;
	m2.setMode(OF_PRIMITIVE_POINTS);
	set<Node*, Compare>::iterator it;
	for( it =  routing->closed.begin(); it !=  routing->closed.end(); ++it ){
		Node* t = (*it); // HERE we get the vector
		if (t)
			m2.addVertex( ofVec2f( t->x, t->y) );
	}
	ofSetColor(0,0,255,200);
	m2.draw();
		
	//solution
	ofMesh m;
	m.setMode(OF_PRIMITIVE_POINTS);
	for (int i = 0; i < res.size(); i++){
		p2 p = res[i];
		m.addVertex( ofVec2f(p.x, p.y) );
	}
	ofSetColor(255,0,0);
	m.draw();
	
	//origin and destination
	ofNoFill();
	glLineWidth(1);
	ofSetColor(0, 255, 0, 128);
	ofCircle(origin.x, origin.y, 10);
	ofCircle(origin.x, origin.y, 3);
	
	ofSetColor(255, 0, 0, 128);
	ofCircle(target.x, target.y, 10);
	ofCircle(target.x, target.y, 3);
	ofFill();
	
	ofSetColor(255);
	ofDrawBitmapString("destination", target.x + 12, target.y + 3);
	ofDrawBitmapString("origin", origin.x + 12, origin.y + 3);

	//exec times
	ofPushMatrix();
		ofTranslate(ofGetWidth() - 250, 0);
		ofSetColor(0,0,0, 200);
		ofRect(20, 80, 250, 80);
		ofSetColor(255);
		TIME_SAMPLE_DRAW(20,80);
	ofPopMatrix();
	
	ofSetColor(255, 0, 0);
	int xoff = -210;
	int yoff = -30;
	int lineH = 10;
	ofDrawBitmapString( ofToString(mouseX) + " : " + ofToString(mouseY), ofGetWidth() + xoff, ofGetHeight() - 30); yoff -= lineH;
	ofDrawBitmapString( "solution num steps: " + ofToString(routing->getLastSolutionNumSteps()) , ofGetWidth()  + xoff, ofGetHeight() - 20); yoff -= lineH;
	ofDrawBitmapString( "solution path cost: " + ofToString(routing->getLastSolutionPathCost(), 1) , ofGetWidth() + xoff, ofGetHeight() - 10); yoff -= lineH;
	
	
//	int hh = 400;
//	plot->draw(0, ofGetHeight() - hh, ofGetWidth(), hh );

	gui.draw();
}

