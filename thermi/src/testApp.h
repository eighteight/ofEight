#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include "ofxThermitrackCam.h"
//#include "ofxTuioServer.h"
#include "ofxXmlSettings.h"
#include "ofxThermitrackTypes.h"
#include "ofxThermitrackPoint.h"


class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

		int 				threshold;

		int static const myCamSize = 400;
		int static const hardwareCamSize = 16;
		void updateTUIOCursor(int, int, int);
//		TuioCursor* getMatch(int,int);
		void drawCursors();
		void drawCam();
		void drawPoints(vector <ofxThermitrackPoint> trackPnts);
		void drawInfo();

		///////// thermitrack //////
		ofxThermitrackCam thermitrackCam;
		bool pointExists(ofxThermitrackPoint&);
		list <ofxThermitrackPoint> currentPoints;
		list <ofxThermitrackPoint> computeDeletePoints(vector <ofxThermitrackPoint>);
		ofxThermitrackPoint getListElement(list<ofxThermitrackPoint> alist, int index);
		ofVec2f rotatePoint(ofxThermitrackPoint);

		/* TUIO support */
//		bool isSendingTUIO;
//		ofxTuioServer tuioServer;
//		TuioCursor * cursor;
//		std::list<TuioCursor*> stickyCursorList;
//		void removeTUIOCursor(int, int);

		//xml
		ofxXmlSettings XML;
		string xmlStructure;
		string message;
		string comPort;
private:
		void randomSignal();
		static int const windowWidth = 400;
		static int const windowHeight = 490;
		int randomSignalFrequencySec;
		float signalDelayMillis;
		float lastReadMillis;
		int lastReadSecs;
		int randomX;
		int randomY;

		bool canSendNextSignal(ofxThermitrackPoint);
		bool canSendNextSignal();
};

#endif
