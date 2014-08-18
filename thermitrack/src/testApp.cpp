#include "testApp.h"
#include "isSameId.h"
//--------------------------------------------------------------
void testApp::setup(){
    ofSetWindowShape(windowWidth, windowHeight);
	ofSetFrameRate(30);
	ofSetWindowTitle("8    vladimir@scriptum.ru    MXT 2010");

	if( XML.loadFile("settings.txt") ){
		message = "settings.txt loaded!";
	}else{
		message = "unable to load settings.txt check data/ folder";
	}

	//read the colors from XML
	//if the settings file doesn't exist we assigns default values (170, 190, 240)
	comPort		= XML.getValue("port", "COM3");
	randomSignalFrequencySec= ofToInt(XML.getValue("randomSignalFrequencySec", "-1"));

	isSendingTUIO = true;

	thermitrackCam.init(160, comPort, 115200);
	//thermitrackCam.getVersion();
	thermitrackCam.setParameters(3,112);
	thermitrackCam.setParameters(4,144);
	thermitrackCam.setParameters(1,0);
	/*
		image packets every 2 frames I,2
		single tracking packets every 1 frames O,1
		multi tracking packets every 1 frames M,1
		status packets every 30 frames S,30
	*/
	
	thermitrackCam.sendCommand("TI,10,M,1,S,30");

	/* TUIO support */

	//sends to localhost, port: 3333
	tuioServer.start("default", 0);

	cursor  = NULL;

	//random drops
	lastReadSecs = ofGetElapsedTimeMillis()*0.001;
	randomX = -1;
	randomY = -1;

	//delay
	lastReadMillis = ofGetElapsedTimeMillis();
	signalDelayMillis = 500.0f;
}

//--------------------------------------------------------------
void testApp::update(){
	try {
		thermitrackCam.update();
	} catch (std::exception& e){
		printf ("Exception in update");
		return;
	}

	try {
			vector <ThermitrackPoint> trackPnts = thermitrackCam.currentPoints;
			list <ofxThermitrackPoint> newTrackedPoints;
				
			if (currentPoints.size()>0){
				//delete points that disapeared

				list <ofxThermitrackPoint> delPoints = computeDeletePoints(trackPnts);
				for (int i = 0; i < delPoints.size(); i++){
					ofxThermitrackPoint point = getListElement(delPoints,i);
					ofxPoint2f sentPoint = rotatePoint(point);
					removeTUIOCursor((int)sentPoint.x,(int)sentPoint.y);
					currentPoints.remove_if(isSameId(point));
				}
			}

			for (int i = 0; i < trackPnts.size(); i++){
				if (pointExists(trackPnts[i])){
						if (cursor != NULL){
							ofxPoint2f sentPoint = rotatePoint(trackPnts[i]);
							if (canSendNextSignal(trackPnts[i])){
								tuioServer.tuioServer->updateTuioCursor(cursor, sentPoint.x, sentPoint.y);
							}
						}
				} else {
					currentPoints.push_back(trackPnts[i]);
					ofxPoint2f sentPoint = rotatePoint(trackPnts[i]);
					cursor = tuioServer.tuioServer->addTuioCursor(sentPoint.x, sentPoint.y);
				}
			}

			randomSignal();

			tuioServer.run();
			if (currentPoints.size()>0) currentPoints.clear();
			for (int i = 0; i<trackPnts.size(); i++){
				currentPoints.push_back(trackPnts[i]);
			}
		} catch (std::exception& e) {
			printf ("Exception 2");
			return;
		}
}

bool testApp::canSendNextSignal(){
	float currentMillis = ofGetElapsedTimeMillis();

	if ((lastReadMillis+signalDelayMillis) < currentMillis){
		lastReadMillis = currentMillis;
		return true;
	}
	return false;
}

bool testApp::canSendNextSignal(ofxThermitrackPoint tp){
	ofxPoint2f delta = tp.getCurrentDelta();
	float traveledDi = delta.dot(delta);

	if (traveledDi > 500000.0f){

		//printf("yes\n");
		return true;
	}
			//printf("no\n");
	return false;
}

list <ofxThermitrackPoint> testApp::computeDeletePoints(vector <ofxThermitrackPoint> newPoints){
	list <ofxThermitrackPoint> ret;
	if (newPoints.size() ==0){
		ret = currentPoints;
	} else {
		for (int i = 0; i < newPoints.size(); i++){
			if (!pointExists(newPoints.at(i))){
				ret.push_back(newPoints.at(i));
			}
		}
	}
	return ret;
}

inline void testApp::randomSignal(){
	if (randomSignalFrequencySec == -1) return;

	int currentTime = ofGetElapsedTimeMillis()*0.001;

	if (!(lastReadSecs+randomSignalFrequencySec > currentTime)){
		if (randomX != -1 && randomY != -1){
			mouseReleased(randomX,randomY,1);
			randomY = randomX = -1;
		}
		randomX = ofRandom(0, myCamSize);
		randomY = ofRandom(0, myCamSize);
		mousePressed(randomX,randomY,1);

		lastReadSecs = currentTime;
	}
}

inline bool testApp::pointExists(ofxThermitrackPoint& point){
	for (int i = 0; i < currentPoints.size(); i++){
		ofxThermitrackPoint p = getListElement(currentPoints,i);
		if (p.id == point.id){
			point.update(ofxPoint2f(p.pos));
			return true;
		}
	}
	return false;
}
void testApp::removeTUIOCursor(int x, int y){
	if (getMatch(x,y)==NULL) {
		tuioServer.tuioServer->removeTuioCursor(cursor);
	}
	cursor = NULL;
}


inline ofxPoint2f testApp::rotatePoint(ofxThermitrackPoint point){
		//don't rotate
		float sendX = point.posNormalised.x;
		float sendY = point.posNormalised.y;

		//rotate 90 degrees counterclockwise
//		float sendX = point.posNormalised.y;
//		float sendY = 1-point.posNormalised.x;

		return ofxPoint2f(sendX, sendY);
}
//--------------------------------------------------------------

void testApp::draw(){
	//draw current cursors and object
	drawCam();
	drawCursors();
	//draw track points as circles
	vector <ofxThermitrackPoint> trackPnts = thermitrackCam.getTrackPoints();
    drawPoints(trackPnts);
	drawInfo(trackPnts);
}

void testApp::drawPoints(vector <ofxThermitrackPoint> trackPnts){
	ofFill();		// draw "filled shapes"
	float radius = 10.0f;
	string pointId;
	for (int i = 0; i< trackPnts.size();i++){
		ofxThermitrackPoint point = trackPnts[i];
		ofxPoint2f rotatedPoint = rotatePoint(point);
		float x = rotatedPoint.x*myCamSize; 
		float y = rotatedPoint.y*myCamSize;
		ofSetColor(255,130,0);
		ofCircle(x,y,radius);
		pointId = ofToString(point.id);
		ofSetColor(0,0,0);
		ofDrawBitmapString(pointId, x, y);
	}
	ofSetColor(0x000000);
}

void testApp::drawInfo(vector <ofxThermitrackPoint> trackPnts){
	string broadStatus = isSendingTUIO ? "Yes":"No";
	ofDrawBitmapString("Cam:"+thermitrackCam.getVerboseCamStatus()+"   Port:"+comPort+"   FPS:"+ofToString((int)ofGetFrameRate()), 20, 410);
	ofDrawBitmapString("Drop:"+ofToString(randomSignalFrequencySec)+" sec   +/- to change",20,440);
	ofDrawBitmapString("Broadcast:"+broadStatus + "   ' ' to start/stop ", 20, 460);
}

void testApp::drawCam(){
	int * camPixelVals = thermitrackCam.getPixels();

	int numPixels = sizeof(camPixelVals);
	int counter = 0;

    float gridsize = ((float)myCamSize/(float)hardwareCamSize);

    for(int i=0; i<hardwareCamSize; i++){ // y
        for(int j=0; j<hardwareCamSize; j++){ // x

            int x1 = j*gridsize;
            int y1 = i*gridsize;
			// draw a rectangle into the image
			ofSetColor(camPixelVals[counter],camPixelVals[counter],camPixelVals[counter]);
			ofRect(x1,y1,gridsize,gridsize);
			counter ++;
        }
    }
}

//draw them for debug purposes
void testApp::drawCursors() {
	char id[3];
	// draw the cursors
	std::list<TuioCursor*> cursorList = tuioServer.tuioServer->getTuioCursors();
	for (std::list<TuioCursor*>::iterator tuioCursor = cursorList.begin(); tuioCursor!=cursorList.end(); tuioCursor++) {
		TuioCursor * tcur = (*tuioCursor); 
		std::list<TuioPoint> path = tcur->getPath();
		if (path.size()>0) {
			
			TuioPoint last_point = path.front();
			glBegin(GL_LINES);
			glColor3f(0.0, 0.0, 1.0);
			
			for (std::list<TuioPoint>::iterator point = path.begin(); point!=path.end(); point++) {
				glVertex3f(last_point.getX()*myCamSize, last_point.getY()*myCamSize, 0.0f);
				glVertex3f(point->getX()*myCamSize, point->getY()*myCamSize, 0.0f);
				last_point.update(point->getX(),point->getY());
			}
			glEnd();
			
			// draw the finger tip
			glColor3f(0.0, 0.75, 0.75);
			ofCircle(tcur->getX()*myCamSize, tcur->getY()*myCamSize, 5);
		}
	}
}

void testApp::updateTUIOCursor(int x, int y, int type){
	if (!isSendingTUIO) return;
	TuioCursor * match = getMatch(x,y);
	if(type ==  2){
		if (match!=NULL) {
			tuioServer.tuioServer->removeTuioCursor(match);
			stickyCursorList.remove(match);
			cursor = NULL;
		} else {
			cursor = tuioServer.tuioServer->addTuioCursor((float)x/(float)myCamSize, (float)y/(float)myCamSize);
			stickyCursorList.push_back(cursor);
		}
	} else {
		if (match!=NULL) {
			cursor=match;
		} else {
			cursor = tuioServer.tuioServer->addTuioCursor((float)(x)/(float)myCamSize, (float)(y)/(float)myCamSize);
		}
	}	
}
//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	switch (key){
		case ' ':
			isSendingTUIO = !isSendingTUIO;
			break;
		case '+':
			if (randomSignalFrequencySec != 100){
				randomSignalFrequencySec == -1? randomSignalFrequencySec = 100:	randomSignalFrequencySec++;
			}
			break;
		case '-':
			if (randomSignalFrequencySec!=-1){
				randomSignalFrequencySec --;
			}
			break;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	if (!isSendingTUIO || !(x>0 && x < myCamSize && y> 0 && y<myCamSize)) return;

	if (cursor==NULL) return;
	if (cursor->getTuioTime()==tuioServer.getCurrentTime()) return;
	tuioServer.tuioServer->updateTuioCursor(cursor,(float)x/(float)myCamSize,(float)y/(float)myCamSize);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	if (!isSendingTUIO || x>myCamSize || y>myCamSize) return;
    updateTUIOCursor(x,y,button);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	if (cursor==NULL) return;    
	if (NULL == getMatch(x,y)) {
		tuioServer.tuioServer->removeTuioCursor(cursor);
	}
	cursor = NULL;
	drawCam();
}

TuioCursor* testApp::getMatch(int x, int y){
	TuioCursor *match = NULL;
	std::list<TuioCursor*> cursorList = tuioServer.tuioServer->getTuioCursors();
	for (std::list<TuioCursor*>::iterator tuioCursor = stickyCursorList.begin(); tuioCursor!=stickyCursorList.end(); tuioCursor++) {
		float d = (*tuioCursor)->getDistance((float)x, (float)y);
		printf("MM %i %i %f\n",x,y,d);
			
		if ( d < 10.0f) {
			match = (*tuioCursor);
			break;
		}
	}
	return match;
}

ofxThermitrackPoint testApp::getListElement(list<ofxThermitrackPoint> alist, int index){
	list<ofxThermitrackPoint>::iterator itr = alist.begin();
	std::advance(itr, index);
	ofxThermitrackPoint point = *itr;
	return point;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	ofSetWindowShape(windowWidth, windowHeight);
}