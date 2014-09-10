#include "ofApp.h"
#include "ofxPS3EyeGrabber.h"
#include "ofxObjLoader.h"
//--------------------------------------------------------------
void ofApp::setup(){
	ofEnableLighting();
	ofEnableDepthTest();
	areaLight.setup();
	//areaLight.enable();
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
	plane.move(ofVec3f(0,-600,0));
	materialPlane.setAmbientColor(ofFloatColor(0.1,0.1,0.1,1.0));
	materialPlane.setDiffuseColor(ofFloatColor(0.8,0.8,0.8,1.0));
	materialPlane.setSpecularColor(ofFloatColor(0.8,0.8,0.8,1.0));
	//materialPlane.setShininess(10);
    
    vidGrabber.setGrabber(ofPtr<ofxPS3EyeGrabber>(new ofxPS3EyeGrabber()));
    if (!vidGrabber.initGrabber(640, 480)){
        cout<<"CAM NOT INITIALIZED"<<endl;
    };
    
    
    ofxObjLoader::load("Neutral.obj", mesh, true);
    
    sofa.enableColors();
    sofa.enableTextures();
    return;
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
    
    setNormals(mesh);

}

void ofApp::setNormals( ofMesh &mesh ){
    //The number of the vertices
    int nV = mesh.getNumVertices();
    //The number of the triangles
    int nT = mesh.getNumIndices() / 3;
    vector<ofPoint> norm( nV ); //Array for the normals
    //Scan all the triangles. For each triangle add its
    //normal to norm's vectors of triangle's vertices
    for (int t=0; t<nT; t++) {
        //Get indices of the triangle t
        int i1 = mesh.getIndex( 3 * t );
        int i2 = mesh.getIndex( 3 * t + 1 );
        int i3 = mesh.getIndex( 3 * t + 2 );
        //Get vertices of the triangle
        const ofPoint &v1 = mesh.getVertex( i1 );
        const ofPoint &v2 = mesh.getVertex( i2 );
        const ofPoint &v3 = mesh.getVertex( i3 );
        //Compute the triangle's normal
        ofPoint dir = ( (v2 - v1).crossed( v3 - v1 ) ).normalized();
        //Accumulate it to norm array for i1, i2, i3
        norm[ i1 ] += dir;
        norm[ i2 ] += dir;
        norm[ i3 ] += dir;
    }
    //Normalize the normal's length
    for (int i=0; i<nV; i++) {
        norm[i].normalize();
    }
    //Set the normals to mesh
    mesh.clearNormals();
    mesh.addNormals( norm );
}

//--------------------------------------------------------------
void ofApp::update(){
	//areaLight.setPosition(0,-200,0);
    vidGrabber.update();
    if(vidGrabber.isFrameNew()) {
        mesh.clear();
        ofPixelsRef pix = vidGrabber.getPixelsRef();
        int width = vidGrabber.getWidth();
        int height = vidGrabber.getHeight();
        int skip = 4; // load a subset of the points
        for(int y = 0; y < vidGrabber.getHeight(); y += skip) {
            for(int x = 0; x < vidGrabber.getWidth(); x += skip) {
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
            setNormals(mesh);
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
    vidGrabber.draw(0, 0, 100, 100);

	cam.begin();
	materialPlane.begin();
	plane.draw();

	//ofDrawSphere(0,-300,0,10000);
	materialPlane.end();

    //ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    
//    sofa.draw();
    float factor = 2.0;
	ofScale(factor, -factor, factor); // flip the y axis and zoom in a bit
	//ofRotateY(90);
	ofTranslate(-img.getWidth() *.5, -img.getHeight() * 0.5);
	mesh.draw();
    //    ofDisableBlendMode();
    //fogShader.end();


    
    
    areaLight.draw();
    
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
