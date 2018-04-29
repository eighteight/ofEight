#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	// listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";

	//FreeConsole();
	receiver.setup(PORT);

	mouseX = 0;
	mouseY = 0;

	ofBackground(0);

	//initialize the variable so it's off at the beginning
	usecamera = false;

	isMouseDraw = false;

}

//--------------------------------------------------------------
void ofApp::update(){

	// check for waiting messages
	while(receiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(m);
        
		// check for mouse moved message
		if(m.getAddress() == "/mousemove/1"){
			// both the arguments are int32's
			mouseX = m.getArgAsInt32(0);
			mouseY = m.getArgAsInt32(1);
			mouseZ = m.getArgAsInt(2);
			cout << mouseZ << endl;
 			collectPoints(mouseX*8, mouseY*4, mouseZ);
			mMovement = 0.01;
			usecamera = false;
		}

		if (m.getAddress() == "/stop/1") {
			mMovement = 4.0;
			if (points.size() > 10000) {
				points.clear();
			}
			///usecamera = true;
			ofResetElapsedTimeCounter();
		}
	}

	if (!usecamera) {
		ofVec3f sumOfAllPoints(0, 0, 0);
		for (unsigned int i = 0; i < points.size(); i++) {
			points[i].z -= mMovement;
			sumOfAllPoints += points[i];
		}
		center = sumOfAllPoints / points.size();
	}


	if (usecamera) {
		float rotateAmount = ofMap(ofGetElapsedTimef()*0.1, 0, ofGetWidth(), 0, 360);
		cout << "MAP " << rotateAmount << endl;
		//ofMap(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp)
		ofVec3f furthestPoint;
		if (points.size() > 0) {
			furthestPoint = points[0];
			ofVec3f directionToFurthestPoint = (furthestPoint - center);
			ofVec3f directionToFurthestPointRotated = directionToFurthestPoint.getRotated(rotateAmount, ofVec3f(0, 1, 0));
			camera.setPosition(center + directionToFurthestPointRotated);
			camera.lookAt(center);
		}
	}
}


//--------------------------------------------------------------
void ofApp::draw(){

	//if we're using the camera, start it.
	//everything that you draw between begin()/end() shows up from the view of the camera
	if (usecamera) {
		camera.begin();
	}

	ofSetColor(255);
	//do the same thing from the first example...
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	for (unsigned int i = 1; i < points.size(); i++) {

		//find this point and the next point
		ofVec3f thisPoint = points[i - 1];
		ofVec3f nextPoint = points[i];

		//get the direction from one to the next.
		//the ribbon should fan out from this direction
		ofVec3f direction = (nextPoint - thisPoint);

		//get the distance from one point to the next
		float distance = direction.length();

		//get the normalized direction. normalized vectors always have a length of one
		//and are really useful for representing directions as opposed to something with length
		ofVec3f unitDirection = direction.getNormalized();

		//find both directions to the left and to the right
		ofVec3f toTheLeft = unitDirection.getRotated(-90, ofVec3f(0, 0, 1));
		ofVec3f toTheRight = unitDirection.getRotated(90, ofVec3f(0, 0, 1));

		//use the map function to determine the distance.
		//the longer the distance, the narrower the line.
		//this makes it look a bit like brush strokes
		float thickness = ofMap(distance, 0, 100, 60, 2, true);
		//ofMap(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp)

		//calculate the points to the left and to the right
		//by extending the current point in the direction of left/right by the length
		ofVec3f leftPoint = thisPoint + toTheLeft*thickness;
		ofVec3f rightPoint = thisPoint + toTheRight*thickness;

		//add these points to the triangle strip
		mesh.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
		mesh.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));
	}

	//end the shape
	mesh.draw();


	//if we're using the camera, take it away
	if (usecamera) {
		camera.end();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	//hitting any key swaps the camera view
	switch (key) {
	case 'f':
		ofToggleFullscreen();
		break;
	case OF_KEY_ALT:
		isMouseDraw = true;
		break;
	case ' ':
		isMouseDraw = false;
		usecamera = !usecamera;
	default:
		break;
	}
}

void ofApp::keyReleased(int key) {
	switch (key) {
	case OF_KEY_ALT:
		isMouseDraw = false;
		break;
	default:
		break;
	}
}

void ofApp::collectPoints(int x, int y, int z) {
	//if we are using the camera, the mouse moving should rotate it around the whole sculpture
	if (usecamera) {
		float rotateAmount = ofMap(ofGetElapsedTimef(), 0, ofGetWidth(), 0, 360);
		ofVec3f furthestPoint;
		if (points.size() > 0) {
			furthestPoint = points[0];
		}
		else
		{
			furthestPoint = ofVec3f(x, y, z);
		}

		ofVec3f directionToFurthestPoint = (furthestPoint - center);
		ofVec3f directionToFurthestPointRotated = directionToFurthestPoint.getRotated(rotateAmount, ofVec3f(0, 1, 0));
		camera.setPosition(center + directionToFurthestPointRotated);
		camera.lookAt(center);
	}
	//otherwise add points like before
	else {
		ofVec3f mousePoint(x, y, 0);
		points.push_back(mousePoint);
	}
}

//--------------------------------------------------------------


//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	if (!isMouseDraw) return;
	cout << "Mouse " << x << " " << y << endl;
	collectPoints(x, y, 0);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
