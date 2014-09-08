#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(true);

	// load an image from disk
	img.loadImage("linzer-640x480.png");
	
	// we're going to load a ton of points into an ofMesh
	mesh.setMode(OF_PRIMITIVE_POINTS);
	
	// loop through the image in the x and y axes
	int skip = 4; // load a subset of the points
    int width = img.getHeight();
    int height = img.getHeight();
	for(int y = 0; y < img.getHeight(); y += skip) {
		for(int x = 0; x < img.getWidth(); x += skip) {
			ofColor cur = img.getColor(x, y);
			if(cur.a > 0) {
				// the alpha value encodes depth, let's remap it to a good depth range
				float z = ofMap(cur.a, 0, 255, -300, 300);
				cur.a = 255;
				mesh.addColor(cur);
				ofVec3f pos(x, y, z);
				mesh.addVertex(pos);
			}
		}
	}
    int camWidth = 640;
	int camHeight = 480;
	
	ps3eye.listDevices();
	
	//ps3eye.setDesiredFrameRate(60);
	ps3eye.initGrabber(img.getWidth(),img.getHeight(), false);
	
//	ps3eye.setAutoGainAndShutter(false); // otherwise we can't set gain or shutter
//	ps3eye.setGain(1.0);
//	ps3eye.setShutter(1.0);
//	ps3eye.setGamma(0.4);
//	ps3eye.setBrightness(0.6);
//	ps3eye.setContrast(1.0);
//	ps3eye.setHue(0.5);
//	
//	ps3eye.setFlicker(1);
    
	ofEnableDepthTest();
	glEnable(GL_POINT_SMOOTH); // use circular points instead of square points
	glPointSize(3); // make the points bigger
    
    faceServer.setName("face");
}

//--------------------------------------------------------------
void ofApp::update() {
    ps3eye.update();
	if(ps3eye.isFrameNew()) {
        mesh.clear();
        ofPixelsRef pix = ps3eye.getPixelsRef();
        int width = ps3eye.getWidth();
        int height = ps3eye.getHeight();
        int skip = 4; // load a subset of the points
        for(int y = 0; y < ps3eye.getHeight(); y += skip) {
            for(int x = 0; x < ps3eye.getWidth(); x += skip) {
                ofColor cur = pix.getColor(x, y);
                ofColor refColor = img.getColor(x, y);
                if(refColor.a > 0) {
                    // the alpha value encodes depth, let's remap it to a good depth range
                    float z = ofMap(refColor.a, 0, 255, -300, 300);
                    cur.a = 255;
                    mesh.addColor(cur);
                    ofVec3f pos(x, y, z);
                    mesh.addVertex(pos);
                }
            }
        }
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
    
	ofBackgroundGradient(ofColor::gray, ofColor::black, OF_GRADIENT_CIRCULAR);
    int factor = 2;
	drawCam(factor);
    drawCam(.2);
    drawCam(0.02);
    
    faceServer.publishScreen();
}

//--------------------------------------------------------------
void ofApp::drawCam(float factor){
    cam.begin();
	ofScale(factor, -factor, factor); // flip the y axis and zoom in a bit
	//ofRotateY(90);
	ofTranslate(-img.getWidth() *.5, -img.getHeight() * 0.5);
	mesh.draw();
	cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
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
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}