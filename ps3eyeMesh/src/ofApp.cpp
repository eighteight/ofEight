#include "ofApp.h"
#include <limits>
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
    float maxZ = .0;
	for(int y = 0; y < img.getHeight(); y += skip) {
		for(int x = 0; x < img.getWidth(); x += skip) {
			ofColor cur = img.getColor(x, y);
			if(cur.a > 0) {
				// the alpha value encodes depth, let's remap it to a good depth range
				float z = ofMap(cur.a, 0, 255, -300, 300);
                if (abs(z)>maxZ) maxZ = abs(z);
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
	if (!ps3eye.initGrabber(img.getWidth(),img.getHeight(), false)){
        cout<<"CAM not initialized"<<endl;
    };
    camServer.setName("CAM");
    faceServer.setName("FACE");
    isCameraGrabbing = false;
    
}

//--------------------------------------------------------------
void ofApp::update() {
    ps3eye.update();
	if(ps3eye.isFrameNew()) {
        isCameraGrabbing = true;
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

    if (isCameraGrabbing){
        camServer.publishTexture(&ps3eye.getTextureReference());
    }
    //ofBackgroundGradient(ofColor::black, ofColor::black, OF_GRADIENT_CIRCULAR);
    ofBackgroundGradient(ofColor::gray, ofColor::black, OF_GRADIENT_CIRCULAR);
	
    
    cam.begin();
    //sofa.draw();
	ofScale(2, -2, 2); // flip the y axis and zoom in a bit
	//ofRotateY(90);
	ofTranslate(-img.getWidth() *.5, -img.getHeight() * 0.5);
	mesh.draw();
	cam.end();
    
    
    
    cam.begin();
    //sofa.draw();
	ofScale(2, -2, 2); // flip the y axis and zoom in a bit
	//ofRotateY(90);
    ofTranslate(-img.getWidth() *.5, -img.getHeight() * 0.5, -img.getHeight() * 2.5);
	mesh.draw();
	cam.end();
    
    faceServer.publishScreen();
}

//--------------------------------------------------------------
void ofApp::drawCam(float factor){
    cam.begin();
    //sofa.draw();
	ofScale(2, -2, 2); // flip the y axis and zoom in a bit
	//ofRotateY(90);
	ofTranslate(-img.getWidth() *.5, -img.getHeight() * 0.5, -img.getHeight() * factor*10);
	mesh.draw();
	cam.end();
}

ofVec3f ofApp::cubicInterpolate(ofVec3f &p0, ofVec3f &p1, ofVec3f &p2, ofVec3f &p3, float t){
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key){
        case OF_KEY_UP:
            //areaLight.move(0,10,0);
            break;
        case OF_KEY_DOWN:
            //areaLight.move(0,-10,0);
            break;
        case OF_KEY_LEFT:
            //areaLight.rotate(1,0,0,1);
            break;
        case OF_KEY_RIGHT:
            //areaLight.rotate(-1,0,0,1);
            break;
	}

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