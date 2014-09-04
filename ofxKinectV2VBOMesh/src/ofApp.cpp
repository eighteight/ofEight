#include "ofApp.h"
#define DEPTH_WIDTH 512
#define DEPTH_HEIGHT 424
//--------------------------------------------------------------
void ofApp::setup(){
    if( !ofFile::doesFileExist("11to16.bin") ){
        ofSystemAlertDialog("Make sure you have 11to16.bin, xTable.bin and zTable.bin in your data folder!");
        ofExit(); 
    }

    ofBackground(30, 30, 30);

    panel.setup("distance in mm", "settings.xml", 540, 100);
    panel.add(kinect.minDistance);
    panel.add(kinect.maxDistance);

	panel.setPosition(ofPoint(10,10));
	panel.add(noiseAmount.setup("Noise Amount", 0.0, 0.0,20.0));
	panel.add(pointSkip.setup("Point Skip", 1, 1,20));
	panel.add(useRealColors.setup("Real Colors", false));
	panel.add(colorAlpha.setup("Color Alpha", 255,0,255));
	showGui = true;
    panel.loadFromFile("settings.xml");

    kinect.open();
    ////////
    // fx
	postFx.init(ofGetWidth(), ofGetHeight());
	
	postFx.createPass<BloomPass>();
	postFx.createPass<FxaaPass>();
    
    pointSkip = 40;
    
    syphonServerRGB.setName("kinectRGB");
    syphonServerDepth.setName("kinectDepth");
    syphonServerScreen.setName("screen");
    
    isSaving = false;
    
     pointSize = 3;
    
    
    ///
    
    // load an image from disk
	pointImg.loadImage("red-chair.png");
	
	// we're going to load a ton of points into an ofMesh
	vbomesh.setMode(OF_PRIMITIVE_POINTS);
	
	// loop through the image in the x and y axes
	int skip = 4; // load a subset of the points
	for(int y = 0; y < pointImg.getHeight(); y += skip) {
		for(int x = 0; x < pointImg.getWidth(); x += skip) {
			ofColor cur = pointImg.getColor(x, y);
			if(cur.a > 0) {
				// the alpha value encodes depth, let's remap it to a good depth range
				float z = ofMap(cur.a, 0, 255, -300, 300);
				cur.a = 255;
				vbomesh.addColor(cur);
				ofVec3f pos(x, y, z);
				vbomesh.addVertex(pos);
			}
		}
	}
    
	ofEnableDepthTest();
	glEnable(GL_POINT_SMOOTH); // use circular points instead of square points
	glPointSize(3); // make the points bigger
}

//--------------------------------------------------------------
void ofApp::update(){
    kinect.update();
    if( kinect.isFrameNew() ){
        ofPixels depths = kinect.getDepthPixels();
        ofPixels colors = kinect.getRgbPixels();
        texDepth.loadData(depths);
        texRGB.loadData(colors);

        vbomesh.clear();
        vbomesh.setMode(OF_PRIMITIVE_POINTS);

		int skpDepth = 4;
		for(int x=0; x < DEPTH_WIDTH; x+=skpDepth) {
			for(int y=0; y < DEPTH_HEIGHT; y+=skpDepth) {
				float distance = depths[y * DEPTH_WIDTH + x]; //   getDistanceAt(x, y);
                
				if(distance > 100. && distance < 1100) {
                    ofColor c = colors[y * DEPTH_WIDTH + x];
					vbomesh.addVertex(ofVec3f(x, y, distance));
                    vbomesh.addColor(c);
				}
			}
		}
    }
    
    if (isSaving) {
        ofxObjLoader::save("delaunay" + ofToString(ofGetFrameNum()), vbomesh);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    texDepth.draw(10, 100);
    texRGB.draw(10, 110 + texDepth.getHeight(), 1920/4, 1080/4);
    
    ofSetColor(255, 255, 255);
    
    easyCam.begin();
    drawPointCloud();
    easyCam.end();
    
    
    if (texRGB.isAllocated()) {
        syphonServerRGB.publishTexture(&texRGB);
    }
    
    if (texDepth.isAllocated()){
        syphonServerDepth.publishTexture(&texDepth);
    }
    
    syphonServerScreen.publishScreen();

    if (showGui){
        panel.draw();
    }
	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;
	reportStream
    << "fps: " << ofGetFrameRate() << endl;
	ofDrawBitmapString(reportStream.str(),10,300);
}

void ofApp::exit(){
}
void ofApp::drawPointCloud() {
    if (vbomesh.getNumVertices() == 0) return;

    glPointSize(1);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards'
	ofScale(1, -1, -1);
	//ofTranslate(0, 0, -1000); // center the points a bit
	ofEnableDepthTest();
    //pointImg.bind();

    vbomesh.draw();
	ofDisableDepthTest();
	ofPopMatrix();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == 'g'){
        showGui = !showGui;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
