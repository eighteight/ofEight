#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofEnableLighting();
	ofEnableDepthTest();
	areaLight.setup();
	areaLight.enable();
	areaLight.setAreaLight(120,400);
	//areaLight.setSpotlight(80,3);
	areaLight.setAmbientColor(ofFloatColor(0.1,0.1,0.1));
	areaLight.setAttenuation(1.0,0.0001,0.0001);
	areaLight.setDiffuseColor(ofFloatColor(1,1,1));
	areaLight.setSpecularColor(ofFloatColor(1,1,1));
	areaLight.rotate(-90,ofVec3f(1,0,0));
	areaLight.rotate(30,ofVec3f(0,0,1));
	areaLight.setPosition(0,-200,0);

	ofBackground(0);
	plane.set(20000,20000,2,2);
	plane.rotate(-90,ofVec3f(1,0,0));
	plane.move(ofVec3f(0,-300,0));
	materialPlane.setAmbientColor(ofFloatColor(0.1,0.1,0.1,1.0));
	materialPlane.setDiffuseColor(ofFloatColor(0.8,0.8,0.8,1.0));
	materialPlane.setSpecularColor(ofFloatColor(0.8,0.8,0.8,1.0));
	materialPlane.setShininess(10);


	camera.setFarClip(20000);
	camera.move(0,0,3000);
    
    
    
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
    
    
    
    
    
    
    //syphon
    syphonClient.setup();
    
    //using Syphon app Simple Server, found at http://syphon.v002.info/
    syphonClient.set("PS3 EYE","ps3eyeToSyphon");

}

//--------------------------------------------------------------
void ofApp::update(){
	//areaLight.setPosition(0,-200,0);
    
    syphonClient.bind();
    
    ofTexture tex = syphonClient.getTexture();
    
    ofImage syphonImg;
    tex.readToPixels(img);
    syphonClient.unbind();
    
	if(tex.isAllocated()) {
        mesh.clear();
        int width = tex.getWidth();
        int height = tex.getHeight();
        int skip = 4; // load a subset of the points
        for(int y = 0; y < height; y += skip) {
            for(int x = 0; x < width; x += skip) {
                ofColor cur = syphonImg.getColor(x, y);
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
void ofApp::draw(){
    
    syphonClient.draw(100,100);
    return;
	camera.begin();
	materialPlane.begin();
	plane.draw();
	ofDrawSphere(0,-300,0,10000);
	materialPlane.end();
    
    float factor = 2.0;
	ofScale(factor, -factor, factor); // flip the y axis and zoom in a bit
	//ofRotateY(90);
	ofTranslate(-img.getWidth() *.5, -img.getHeight() * 0.5);
	mesh.draw();
    //    ofDisableBlendMode();
    //fogShader.end();
    
    
    
    
    areaLight.draw();

    
    
	areaLight.draw();
	camera.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
	case OF_KEY_UP:
		areaLight.move(0,10,0);
		break;
	case OF_KEY_DOWN:
		areaLight.move(0,-10,0);
		break;
	case OF_KEY_LEFT:
		areaLight.rotate(1,0,0,1);
		break;
	case OF_KEY_RIGHT:
		areaLight.rotate(-1,0,0,1);
		break;
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
