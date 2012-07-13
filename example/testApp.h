#pragma once

#include "ofMain.h"
#include "ofxMapRouting.h"
#include "ofxTileMap.h"
#include "ofxTimeMeasurements.h"
#include "ofxSimpleGuiToo.h"
//#include "ofxHistoryPlot.h"
//#include "ofxMemoryUsage.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void keyPressed( int key ){
			
			if (key == '\t') gui.toggleDraw();	
			if (key == ' ') ofToggleFullscreen();			
		}
	
		void mouseDragged(int x, int y, int button){
			mousePressed(x,y,button);
		}
	
		void mousePressed(int x, int y, int button){
			if (!gui.isOn()){
				if (button==0){
					target.x = x;
					target.y = y;
				}else{
					origin.x = x;
					origin.y = y;					
				}
			}
		}
	
		void setUIParams(){
			
			defaultSimpleGuiConfig.gridSize.x = 351;
			
			gui.addToggle("8 Connected", connect8 );			
			gui.addToggle("use sqrtf", routing->useSqrt );				
			
			gui.addSlider("Eucledian Dist weight", routing->eucledianDistWeight, 0.0, 1.0);	
			gui.addSlider("road Cost Gain", routing->roadCostGain, 0, 300);	
			gui.addSlider("extra cost per step", routing->perStepExtraCost, -1, 1);	
			gui.addSlider("max Iterations", maxIterations, 100, 100000);

			gui.loadFromXML();
		};
	
	
		ofxMapRouting * routing;
		ofxTileMap * map;
		std::vector <p2> res;
	
		int maxIterations;
		bool connect8;
	
		ofVec2f origin;
		ofVec2f target;
	
		ofxSimpleGuiToo gui;
	
		//ofxHistoryPlot * plot;
		//ofxMemoryUsage mem;

};
