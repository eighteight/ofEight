#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

//#include "ofxThermitrackCam.h"
#include "ofxTuioServer.h"
#include "ofxXmlSettings.h"
#include "ofxThermitrack.h"


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

public:
		void updateTUIOCursor(int, int, int);
		TuioCursor* getMatch(int,int);
		void drawCursors();
		void drawCam();
		void drawPoints(vector <ThermitrackPoint> trackPnts);
		void drawInfo(vector <ThermitrackPoint> trackPnts);

		///////// thermitrack //////
		ofxThermitrack thermitrackCam;
		bool pointExists(ThermitrackPoint&);
		list <ThermitrackPoint> currentPoints;
		list <ThermitrackPoint> computeDeletePoints(vector <ThermitrackPoint>);
		ThermitrackPoint getListElement(list<ThermitrackPoint> alist, int index);
		ofPoint rotatePoint(ThermitrackPoint);

		/* TUIO support */
		bool isSendingTUIO;
		ofxTuioServer tuioServer;
		TuioCursor * cursor;
		std::list<TuioCursor*> stickyCursorList;
		void removeTUIOCursor(int, int);

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

		bool canSendNextSignal(ThermitrackPoint);
		bool canSendNextSignal();
};

#endif
