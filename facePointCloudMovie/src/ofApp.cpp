#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(true);

	// load an image from disk
	img.loadImage("linzercopy.png");
	
	// we're going to load a ton of points into an ofMesh
	mesh.setMode(OF_PRIMITIVE_POINTS);
	
	// loop through the image in the x and y axes
	int skip = 4; // load a subset of the points
	for(int y = 0; y < img.getHeight(); y += skip) {
		for(int x = 0; x < img.getWidth(); x += skip) {
			ofColor refColor = img.getColor(x, y);
			if(refColor.a > 0) {
				// the alpha value encodes depth, let's remap it to a good depth range
				float z = ofMap(refColor.a, 0, 255, -300, 300);
				refColor.a = 1;
				mesh.addColor(refColor);
				ofVec3f pos(x, y, z);
                int indx = y*camera.width+x;
				mesh.addVertex(pos);
			}
		}
	}

    vector<ofVideoDevice> allCameras = camera.listDevices();
    
    cout << endl << "Listing camera devices:" << endl;
    for(int i = 0; i < allCameras.size(); i++){
        cout << allCameras[i].id << ": " << allCameras[i].deviceName;
        if( allCameras[i].bAvailable ){
            cout << endl;
        }else{
            cout << " - unavailable " << endl;
        }
    }
    
    camera.setDeviceID(0);
    
    
    camera.initGrabber(img.getWidth(), img.getHeight());


	ofEnableDepthTest();
	glEnable(GL_POINT_SMOOTH); // use circular points instead of square points
	glPointSize(3); // make the points bigger
}

//--------------------------------------------------------------
void ofApp::update() {
    camera.update();
	if(camera.isFrameNew()) {
        mesh.clear();
        ofPixelsRef pix = camera.getPixelsRef();
        int skip = 4; // load a subset of the points
        for(int y = 0; y < camera.getHeight(); y += skip) {
            for(int x = 0; x < camera.getWidth(); x += skip) {
                ofColor cur = pix.getColor(x, y);
                ofColor refColor = img.getColor(x, y);
                if(refColor.a > 0) {
                    // the alpha value encodes depth, let's remap it to a good depth range
                    float z = ofMap(refColor.a, 0, 255, -300, 300);
                    cur.a = 255;
                    int indx = y*camera.width+x;
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

	ofBackgroundGradient(ofColor::black, ofColor::black, OF_GRADIENT_CIRCULAR);
	
	// even points can overlap with each other, let's avoid that
	cam.begin();
	ofScale(2, -2, 2); // flip the y axis and zoom in a bit
	//ofRotateY(90);
	ofTranslate(-img.getWidth() / 2, -img.getHeight() / 2);
	mesh.draw();
	cam.end();
    
    cam.begin();
	ofScale(.2, -.2, .2); // flip the y axis and zoom in a bit
	//ofRotateY(90);
	ofTranslate(-img.getWidth() / 2, -img.getHeight() / 2);
	mesh.draw();
	cam.end();
    
    cam.begin();
	ofScale(.02, -.02, .02); // flip the y axis and zoom in a bit
	//ofRotateY(90);
	ofTranslate(-img.getWidth() / 2, -img.getHeight() / 2);
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