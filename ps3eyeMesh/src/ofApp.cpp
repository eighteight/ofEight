#include "ofApp.h"
#include <limits>
//--------------------------------------------------------------
void ofApp::setup() {
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
    return;
    
    ///area light end

	
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
	ps3eye.initGrabber(img.getWidth(),img.getHeight(), false);
    return;
	ofEnableDepthTest();
	glEnable(GL_POINT_SMOOTH); // use circular points instead of square points
	glPointSize(3); // make the points bigger
    
    ofxObjLoader::load("sofa.obj", sofa, true);
    sofa.enableColors();
    sofa.enableTextures();
    
    
    gui.setup("fog control"); // most of the time you don't need a name
    
	gui.add(density.setup( "density", 0.0035f, 0.0f , 0.5f ));
	gui.add(fog_start.setup( "fog start", 10.0f, 0.0f, 255.0f ));
	gui.add(fog_end.setup( "fog end", 50.0f, 0.0f, 255.0f ));
	gui.add(r.setup( "redColor", 0.1f, 0, 1 ));
	gui.add(g.setup( "greenColor", 0.2f, 0, 1 ));
	gui.add(b.setup( "blueColor", 0.5f, 0, 1 ));
	gui.add(fogFilter.setup( "switch fog",1, 0,2));
    
    
    ofDisableArbTex();
    
    ofEnableLighting();
    
    fogShader.load("shaders/FogShaderTex");
    
    color.loadImage("textures/pond1.jpg");
    
    fogShader.begin();
    
    
    fogShader.setUniformTexture( "baseMap", color, color.getTextureReference().getTextureData().textureID );
    
    fogShader.end();
    
    
    bHide = true;
    fog.setup();
    
    ofSetWindowShape(camWidth,camHeight);
    
    
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
    
    return;
    
    
    
    fogColor.set(r,g,b) ;//= {r,g,b,1.0f};   // Fog Color
    
    
    fog.setfogColor(fogColor);
    fog.setFogMode(fogFilter);
    fog.setDensity(density);
    fog.setFogStartEnd(fog_start,fog_end);
    fog.enableCoordinate(true);
    fog.enable();

}

//--------------------------------------------------------------
void ofApp::draw() {
    
    camera.begin();
	materialPlane.begin();
	plane.draw();
	ofDrawSphere(0,-300,0,10000);
	materialPlane.end();
	areaLight.draw();
	camera.end();
    
    return;
    
    
    
    
    
    
    
    
    ofEnableLighting();
    
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);                       // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);                        // Type of Depth Test to perform
    
    
    
    glShadeModel(GL_SMOOTH);
    
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    

    
    int factor = 2;
	drawCam(factor);
    //drawCam(.2);
    drawCam(0.02);
//
//    faceServer.publishScreen();
    ofBox(100);
    

    ofDisableLighting();
    glDisable(GL_DEPTH_TEST);
    
    
    
    if( bHide ){
        
        fog.disable();
		gui.draw();
        
    }
    ofSetColor(220,10,0);
    ofDrawBitmapString("I'm a string not coloured by the fog!", 10,720);

    ofDrawBitmapString("I'm a string not coloured by the fog!", 10,720);
}

//--------------------------------------------------------------
void ofApp::drawCam(float factor){
        fogShader.begin();
    cam.begin();
    //ofEnableBlendMode(OF_BLENDMODE_SCREEN);

    sofa.draw();
	ofScale(factor, -factor, factor); // flip the y axis and zoom in a bit
	//ofRotateY(90);
	ofTranslate(-img.getWidth() *.5, -img.getHeight() * 0.5);
	mesh.draw();
    //    ofDisableBlendMode();
        fogShader.end();
	cam.end();
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