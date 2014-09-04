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
	//pointImg.loadImage("red-chair.png");
    ofDisableArbTex();
    ofLoadImage(pointImg, "red-chair.png");
//	ofEnableDepthTest();
//	glEnable(GL_POINT_SMOOTH); // use circular points instead of square points
//	glPointSize(3); // make the points bigger
    
    
    // randomly add a point on a sphere
	int   num = 500;
	float radius = 1000;
	for(int i = 0; i<num; i++ ) {
		
		float theta1 = ofRandom(0, TWO_PI);
		float theta2 = ofRandom(0, TWO_PI);
		
		ofVec3f p;
		p.x = cos( theta1 ) * cos( theta2 );
		p.y = sin( theta1 );
		p.z = cos( theta1 ) * sin( theta2 );
		p *= radius;
		
		addPoint(p.x, p.y, p.z);
        
	}
	
	// upload the data to the vbo
	int total = (int)points.size();
	vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);

    
    
    // load the shader
#ifdef TARGET_OPENGLES
    shader.load("shaders_gles/shader");
#else
    shader.load("shaders/shader");
#endif
    
    
    ///bullet
    
    world.setup();
	world.enableGrabbing();
	world.enableDebugDraw();
	world.setCamera(&easyCam);
	
	sphere = new ofxBulletSphere();
	sphere->create(world.world, ofVec3f(0, 0, 0), 0.1, .25);
	sphere->add();
	
	box = new ofxBulletBox();
	box->create(world.world, ofVec3f(7, 0, 0), .05, .5, .5, .5);
	box->add();
	
	cone = new ofxBulletCone();
	cone->create(world.world, ofVec3f(-1, -1, .2), .2, .4, 1.);
	cone->add();
	
	capsule = new ofxBulletCapsule();
	capsule->create(world.world, ofVec3f(1, -2, -.2), .4, .8, 1.2);
	capsule->add();
	
	cylinder = new ofxBulletCylinder();
	cylinder->create(world.world, ofVec3f(0, -2.4, 0), .8, .9, 1.8);
	cylinder->add();
	
	ground.create( world.world, ofVec3f(0., 5.5, 0.), 0., 100.f, 1.f, 100.f );
	ground.setProperties(.25, .95);
	ground.add();

}

//--------------------------------------------------------------
void ofApp::update(){
    kinect.update();
    if( kinect.isFrameNew() ){
        ofPixels depths = kinect.getDepthPixels();
        ofPixels colors = kinect.getRgbPixels();
        texDepth.loadData(depths);
        texRGB.loadData(colors);
        int   num = 500;
        float radius = 1000;
        for(int i = 0; i<num; i++ ) {
            
            float theta1 = ofRandom(0, TWO_PI);
            float theta2 = ofRandom(0, TWO_PI);
            
            ofVec3f p;
            p.x = cos( theta1 ) * cos( theta2 );
            p.y = sin( theta1 );
            p.z = cos( theta1 ) * sin( theta2 );
            p *= radius;
            
            addPoint(p.x, p.y, p.z);
            
        }
        
        // upload the data to the vbo
        int total = (int)depths.size();
        vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
        vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);

        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_POINTS);

		int skpDepth = 4;
		for(int x=0; x < DEPTH_WIDTH; x+=skpDepth) {
			for(int y=0; y < DEPTH_HEIGHT; y+=skpDepth) {
				float distance = depths[y * DEPTH_WIDTH + x]; //   getDistanceAt(x, y);
                
				if(distance > 100. && distance < 1100) {
                    ofColor c = colors[y * DEPTH_WIDTH + x];
					mesh.addVertex(ofVec3f(x, y, distance));
                    mesh.addColor(c);
				}
			}
		}
        
        vbo.setMesh(mesh, GL_STATIC_DRAW);
        if (isSaving) {
            ofxObjLoader::save("delaunay" + ofToString(ofGetFrameNum()), mesh);
        }
    }
    
    
    //bullet
	world.update();
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 0));

}

//--------------------------------------------------------------
void ofApp::addPoint(float x, float y, float z) {
	ofVec3f p(x, y, z);
	points.push_back(p);
	
	// we are passing the size in as a normal x position
	float size = ofRandom(5, 50);
	sizes.push_back(ofVec3f(size));
}

//--------------------------------------------------------------
void ofApp::draw(){
//    texDepth.draw(10, 100);
//    texRGB.draw(10, 110 + texDepth.getHeight(), 1920/4, 1080/4);
    glDepthMask(GL_FALSE);

    ofSetColor(255, 255, 255);
    // this makes everything look glowy :)
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnablePointSprites();
    // bind the shader and camera
	// everything inside this function
	// will be effected by the shader/camera
	shader.begin();
	easyCam.begin();
	
	// bind the texture so that when all the points
	// are drawn they are replace with our dot image
	pointImg.bind();
	vbo.draw(GL_POINTS, 0, (int)points.size());
	pointImg.unbind();
	
	easyCam.end();
	shader.end();
	
	ofDisablePointSprites();
	ofDisableBlendMode();
	
	// check to see if the points are
	// sizing to the right size
	ofEnableAlphaBlending();
	easyCam.begin();
	for (unsigned int i=0; i<points.size(); i++) {
		ofSetColor(255, 80);
		ofVec3f mid = points[i];
		mid.normalize();
		mid *= 300;
		ofLine(points[i], mid);
	}
	easyCam.end();
	
	glDepthMask(GL_TRUE);
	


    
    
    
    
    
    
    ////
    easyCam.begin();
    drawPointCloud();
    easyCam.end();
    
    
    
    glEnable( GL_DEPTH_TEST );
	easyCam.begin();
	
	ofSetLineWidth(1.f);
	ofSetColor(255, 0, 200);
	world.drawDebug();
	
	ofSetColor(100, 100, 100);
	ground.draw();
	
	ofSetColor(225, 225, 225);
	sphere->draw();
	
	ofSetColor(225, 225, 225);
	box->draw();
	
	ofSetColor(225, 225, 225);
	cylinder->draw();
	
	ofSetColor(225, 225, 225);
	capsule->draw();
	
	ofSetColor(225, 225, 225);
	cone->draw();
	
	easyCam.end();

    
    
    
    return;    
    
    
    
    
    
    
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

}

void ofApp::exit(){
}
void ofApp::drawPointCloud() {
    if (vbo.getNumVertices() == 0) return;

    glPointSize(1);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards'
	ofScale(1, -1, -1);
	//ofTranslate(0, 0, -1000); // center the points a bit
	ofEnableDepthTest();
    //pointImg.bind();

    //vbo.draw();
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
