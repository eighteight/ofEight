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
    
    isSaving = false;
    
     pointSize = 3;
    
}

//--------------------------------------------------------------
void ofApp::update(){
    kinect.update();
    if( kinect.isFrameNew() ){
        texDepth.loadData( kinect.getDepthPixels() );
        texRGB.loadData( kinect.getRgbPixels() );
        //ofMesh mesh;
        vbomesh.clear();
        vbomesh.setMode(OF_PRIMITIVE_POINTS);


		unsigned char* pix = new unsigned char[DEPTH_WIDTH*DEPTH_HEIGHT];
		int skpDepth = 4;
		for(int x=0; x < DEPTH_WIDTH; x+=skpDepth) {
			for(int y=0; y < DEPTH_HEIGHT; y+=skpDepth) {
				float distance = kinect.getDepthPixels()[y * DEPTH_WIDTH + x]; //   getDistanceAt(x, y);
                
				if(distance > 100 && distance < 1100) {
					vbomesh.addVertex(ofVec3f(x, y, distance));
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
//    texDepth.draw(10, 100);
//    texRGB.draw(10, 110 + texDepth.getHeight(), 1920/4, 1080/4);
    
    if (texRGB.isAllocated()) {
        syphonServerRGB.publishTexture(&texRGB);
    }
    
    if (texDepth.isAllocated()){
        syphonServerDepth.publishTexture(&texDepth);
    }
    
    
    ofSetColor(255, 255, 255);
    
    easyCam.begin();
    drawPointCloud();
    easyCam.end();
    
	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;
	reportStream
    << "fps: " << ofGetFrameRate() << endl
	<< "step = " << step << endl
    << "pointSize = " << pointSize << endl;
	ofDrawBitmapString(reportStream.str(),20,20);
}

void ofApp::drawPointCloud() {
    if (!texDepth.isAllocated()) return;
    
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 4;
	for(int y = 0; y < DEPTH_HEIGHT; y += step) {
		for(int x = 0; x < DEPTH_WIDTH; x += step) {
            float distance = kinect.getDepthPixels()[y * DEPTH_WIDTH + x];
			if(distance > 0 && distance < 10000) {
				//mesh.addColor(kinect.getColorAt(x,y));
				mesh.addVertex(ofVec3f(x,y,distance));
			}
		}
	}
	glPointSize(2);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards'
	ofScale(1, -1, -1);
	//ofTranslate(0, 0, -1000); // center the points a bit
	ofEnableDepthTest();
	mesh.drawVertices();
	ofDisableDepthTest();
	ofPopMatrix();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == 's'){
        isSaving = !isSaving;
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
