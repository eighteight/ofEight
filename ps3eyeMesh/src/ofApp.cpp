#include "ofApp.h"
#define WIDTH 640
#define HEIGHT 480
//--------------------------------------------------------------
void ofApp::setup() {
    ofSetVerticalSync(true);
    //ofSetFrameRate(30.0);
    setCamLocations();
    isEasyCamMoving = false;
	// load an image from disk
	img.loadImage("linzer-640x480-cleaned.png");
	
	// we're going to load a ton of points into an ofMesh
	mesh.setMode(OF_PRIMITIVE_POINTS);
	
	// loop through the image in the x and y axes
	int skip = 4; // load a subset of the points
    int width = img.getHeight();
    int height = img.getHeight();
    float maxZ = .0;
	for(int y = 0; y <HEIGHT; y += skip) {
		for(int x = 0; x < WIDTH; x += skip) {
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

	ps3eye.listDevices();
	if (!ps3eye.initGrabber(WIDTH,HEIGHT, false)){
        cout<<"CAM not initialized"<<endl;
    }


    camServer.setName("CAM");
    faceServer.setName("FACE");
    isCameraGrabbing = false;

	glEnable(GL_POINT_SMOOTH); // use circular points instead of square points
	glPointSize(pointSize); // make the points bigger
    
    ofSetWindowShape(WIDTH,HEIGHT);
    currentRotation = 0.;
    
    
    setPS3EyeGUI();

}

void ofApp::setPS3EyeGUI(){
    // Gui
    gui.setup("PS3Eye", "ps3eye.xml");
	gui.setPosition(ofPoint(10,10));
	gui.add(pointSize.setup("Point Size", 3, 1, 10));
    gui.add(easyCamFarClip.setup("Far Clip", 1900, 200, 10000));
	gui.add(isEasyCamMoving.setup("Animate Cam", false));
    gui.add(animationSpeed.setup("Animate Speed", 0.1, 0.01, 0.5));

    ofxToggle * autoGainAndShutter = new ofxToggle();
    autoGainAndShutter->setup("Auto Gain and Shutter", false);
    autoGainAndShutter->addListener(this, &ofApp::onAutoGainAndShutterChange);
    gui.add(autoGainAndShutter);
    
    ofxFloatSlider * gain = new ofxFloatSlider();
    gain->setup("Gain", 0.5, 0.0, 1.0);
    gain->addListener(this, &ofApp::onGainChange);
    gui.add(gain);
    
    ofxFloatSlider * shutter = new ofxFloatSlider();
    shutter->setup("Shutter", 0.5, 0.0, 1.0);
    shutter->addListener(this, &ofApp::onShutterChange);
    gui.add(shutter);
    
    ofxFloatSlider * gamma = new ofxFloatSlider();
    gamma->setup("Gamma", 0.5, 0.0, 1.0);
    gamma->addListener(this, &ofApp::onGammaChange);
    gui.add(gamma);
    
    ofxFloatSlider * brightness = new ofxFloatSlider();
    brightness->setup("Brightness", 0.5, 0.0, 1.0);
    brightness->addListener(this, &ofApp::onBrightnessChange);
    gui.add(brightness);
    
    ofxFloatSlider * contrast = new ofxFloatSlider();
    contrast->setup("Contrast", 0.5, 0.0, 1.0);
    contrast->addListener(this, &ofApp::onContrastChange);
    gui.add(contrast);
    
    ofxFloatSlider * hue = new ofxFloatSlider();
    hue->setup("Hue", 0.5, 0.0, 1.0);
    hue->addListener(this, &ofApp::onHueChange);
    gui.add(hue);
    
    ofxIntSlider * flicker = new ofxIntSlider();
    flicker->setup("Flicker Type", 0, 0, 2);
    flicker->addListener(this, &ofApp::onFlickerChange);
    gui.add(flicker);
    
    ofxIntSlider * wb = new ofxIntSlider();
    wb->setup("White Balance Mode", 4, 1, 4);
    wb->addListener(this, &ofApp::onFlickerChange);
    gui.add(wb);
	
	ofxToggle * led = new ofxToggle();
    led->setup("LED", true);
	led->addListener(this, &ofApp::onLedChange);
	gui.add(led);
    
    ofxToggle * resetCam = new ofxToggle();
    resetCam->setup("Reset PS3EYE", true);
    resetCam->addListener(this, &ofApp::onResetCamera);
    //gui.add(resetCam);
	
	// Load initial values
    
    gui.loadFromFile("ps3eye.xml");
    bool b;
    float f;
    int i;
    b = gui.getToggle("Auto Gain and Shutter");
    onAutoGainAndShutterChange(b);
    f = gui.getFloatSlider("Gain");
    onGainChange(f);
    f = gui.getFloatSlider("Shutter");
    onShutterChange(f);
    f = gui.getFloatSlider("Gamma");
    onGammaChange(f);
    f = gui.getFloatSlider("Brightness");
    onBrightnessChange(f);
    f = gui.getFloatSlider("Contrast");
    onContrastChange(f);
    f = gui.getFloatSlider("Hue");
    onHueChange(f);
    b = gui.getToggle("LED");
    onLedChange(b);
    i = gui.getIntSlider("Flicker Type");
    onFlickerChange(i);
    i = gui.getIntSlider("White Balance Mode");
    onWhiteBalanceChange(i);

}

void ofApp::setCamLocations(){
    startTime = ofGetElapsedTimef();
    myLocations.clear();
    myLocations.push_back(ofVec3f(0., 0., 665.));
    myLocations.push_back(ofVec3f(0., 0., 0.));
    myLocations.push_back(ofVec3f(0., 0., -1000.));
    myLocations.push_back(ofVec3f(0, 0, -2000.));
    myLocations.push_back(ofVec3f(0, 0, -3000.));
    myLocations.push_back(ofVec3f(0, 0., -4000.));
    myLocations.push_back(ofVec3f(0, 0., -5000.));
    myLocations.push_back(ofVec3f(0, 0., -6000.));
    myLocations.push_back(ofVec3f(-WIDTH, 0., -6000.));
    myLocations.push_back(ofVec3f(0, 0., -4000.));
//    myLocations.push_back(ofVec3f(0., 0., 0));
//    myLocations.push_back(ofVec3f(WIDTH, 0, -HEIGHT));
//    myLocations.push_back(ofVec3f(WIDTH, HEIGHT, 0.));
//    myLocations.push_back(ofVec3f(0, 0., 0));
//    myLocations.push_back(ofVec3f(-WIDTH, 0., HEIGHT));

    cam.setGlobalPosition(myLocations[0]);
    cam.setFarClip(easyCamFarClip);
    cam.disableMouseInput();
    ////anim
    currentAnim = 0;
    animatable.setPosition(myLocations[0]);
    animatable.animateTo(myLocations[1]);
    animatable.setCurve(EASE_IN_EASE_OUT);
    
    currentRotation = 0.;
}

void ofApp::updateCamPosition(){
    if (!isEasyCamMoving) return;
    
    if (animatable.hasFinishedAnimating()){
        ofVec3f start = animatable.getTargetPosition();
        ofVec3f end;
        if (animatable.getTargetPosition() ==  myLocations[myLocations.size() - 1]){
            end = myLocations[0];
            currentAnim = -1;
        } else {
            currentAnim++;
            end = myLocations[currentAnim+1];
        }
        animatable = ofxAnimatableOfPoint();
        animatable.setPosition(start);
        animatable.animateTo(end);
        cout << currentAnim<<" GOAL: "<< end <<endl;
        animatable.setCurve(EASE_IN_EASE_OUT);
    }
    animatable.update( animationSpeed/60.0f );

    cam.setGlobalPosition(animatable.getCurrentPosition());
    //cam.lookAt(animatable.getTargetPosition());
    //cam.setTarget(animatable.getCurrentPosition());

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
        
//        float connectionDistance = 30;
//        int numVerts = mesh.getNumVertices();
//        for (int a=0; a<numVerts; ++a) {
//            ofVec3f verta = mesh.getVertex(a);
//            for (int b=a+1; b<numVerts; ++b) {
//                ofVec3f vertb = mesh.getVertex(b);
//                float distance = verta.distance(vertb);
//                if (distance > connectionDistance) {
//                    mesh.addIndex(a);
//                    mesh.addIndex(b);
//                }
//            }
//        }
	}
    
    updateCamPosition();
    
    if (isCameraGrabbing)
        ofSetWindowTitle("OK Camera");
    else
        ofSetWindowTitle("NO Camera");
    
    glPointSize(pointSize);
    cam.setFarClip(easyCamFarClip);
    
}


//--------------------------------------------------------------
void ofApp::draw() {
    
    if (isCameraGrabbing){
        camServer.publishTexture(&ps3eye.getTextureReference());
    }
    ofEnableDepthTest();

    //ofBackgroundGradient(ofColor::black, ofColor::black, OF_GRADIENT_CIRCULAR);
    ofBackgroundGradient(ofColor::black, ofColor::black, OF_GRADIENT_CIRCULAR);
	
    cam.begin();
        for (int i = 1; i < myLocations.size(); i++){
            ofPushMatrix();
            //ofSphere(animatable.getCurrentPosition(),100);
//            sofa.draw();
                ofScale(2, -2, 2); // flip the y axis and zoom in a bit
            if (isEasyCamMoving){
                currentRotation += .002;
            }
            ofRotate(currentRotation, 0, 0, 1);
            ofTranslate(myLocations[i].x-img.getWidth() *.5, myLocations[i].y-img.getHeight() * 0.5, myLocations[i].z );
            mesh.draw();
            ofPopMatrix();
        }
	cam.end();
    
    ofDisableDepthTest();
    faceServer.publishScreen();
    

    ofPushStyle();
        ofSetColor(255,255,255,255);
        gui.draw();
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    switch(key){
        case OF_KEY_UP:

            break;
        case OF_KEY_DOWN:

            break;
        case OF_KEY_LEFT:

            break;
        case OF_KEY_RIGHT:

            break;
        case 's':
            isEasyCamMoving = !isEasyCamMoving;
            break;
        case 'r':
            setCamLocations();
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

//--------------------------------------------------------------
void ofApp::onAutoGainAndShutterChange(bool & value){
	ps3eye.setAutoGainAndShutter(value);
}

//--------------------------------------------------------------
void ofApp::onGainChange(float & value){
	// Only set if auto gain & shutter is off
	if(!(bool&)gui.getToggle("Auto Gain and Shutter")){
        ps3eye.setGain(value);
	}
}

//--------------------------------------------------------------
void ofApp::onShutterChange(float & value){
	// Only set if auto gain & shutter is off
	if(!(bool&)gui.getToggle("Auto Gain and Shutter")){
        ps3eye.setShutter(value);
	}
}

//--------------------------------------------------------------
void ofApp::onGammaChange(float & value){
	ps3eye.setGamma(value);
}

//--------------------------------------------------------------
void ofApp::onBrightnessChange(float & value){
	ps3eye.setBrightness(value);
}

//--------------------------------------------------------------
void ofApp::onContrastChange(float & value){
	ps3eye.setContrast(value);
}

//--------------------------------------------------------------
void ofApp::onHueChange(float & value){
	ps3eye.setHue(value);
}

//--------------------------------------------------------------
void ofApp::onLedChange(bool & value){
	ps3eye.setLed(value);
}

//--------------------------------------------------------------
void ofApp::onFlickerChange(int & value){
	ps3eye.setFlicker(value);
}

//--------------------------------------------------------------
void ofApp::onWhiteBalanceChange(int & value){
	ps3eye.setWhiteBalance(value);
}

void ofApp::onResetCamera(bool &value){
    ps3eye.initGrabber(WIDTH, HEIGHT, false);
}