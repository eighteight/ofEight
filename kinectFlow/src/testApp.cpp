#include "testApp.h"
#define WIDTH 640
#define HEIGHT 480
#define OSC_ADDRESS "/isadora/1"
//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(30);
    kinect.init(false, true, true);
    flowSolver.setup(kinect.getWidth()/2, kinect.getHeight()/2, 0.5, 3, 10, 1, 7, 1.5, false, false);
    ofBackground(0,0,0);

    grayDiff.allocate(WIDTH,HEIGHT);
    far = 2550.0;
    near = 500.0;
    showShape = showFlow = showRect = showDepth = true;
    smoothSize = 20.0;
    smoothShape = 0.5;
    shapeScale = 0.5;
    
    depthThreshold = 30.;
    trackedCirclesMax = 4;
    trackedCircles = 1;
    trackedCirclesInt = (int) trackedCircles;
    circleHistory.reserve((int)trackedCirclesMax);
    circleAverage.reserve((int)trackedCirclesMax);
    
    setUI();
    syphonServer.setName("kinectFlow");
    
   	oscSender.setup("localhost", 3333);
}

void testApp::exit(){
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
    kinect.close();
}

//--------------------------------------------------------------
void testApp::update(){
    // kinect.setDepthClipping(0.0,2550.0);
    kinect.update();
    if ( kinect.isFrameNew() ){
        flowSolver.update(kinect.getDepthPixels(), WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
        grayDiff.setFromPixels(kinect.getDepthPixelsRef());
        grayDiff.threshold((int)depthThreshold);
        contourFinder.findContours(grayDiff, 5, (WIDTH*HEIGHT)/4, 2, false, true);
        
        updateContours();
    }

    //kinect.setDepthClipping(near, near + (far-near)*scaledVol*10);
}

void testApp::updateContours(){
    
    contourPoly.clear();
    rectangles.clear();
    trackedCirclesInt = (int)trackedCircles;
    if (trackedCirclesInt>contourFinder.nBlobs) trackedCirclesInt = contourFinder.nBlobs;
    for (int i = 0; i <contourFinder.nBlobs && i < trackedCirclesInt; i++){

        if(contourFinder.blobs[i].pts.size()>0){
            
            ofPolyline tempPoly;
            ofRectangle rect = contourFinder.blobs[i].boundingRect;
            rectangles.push_back(rect);
            
            if (circleHistory.size() <= i){
                circleHistory.push_back(vector<ofPoint>());
            }
            circleHistory[i].push_back(rect.getCenter());
            //if we are bigger the the size we want to record - lets drop the oldest value
            if( circleHistory[i].size() >= historySize ){
                circleHistory[i].erase(circleHistory[i].begin(), circleHistory[i].begin()+1);
            }

            sendMessage(rect);

            vector <ofPoint> pts = contourFinder.blobs[i].pts;
            for (int j = 0; j < pts.size(); j++){
                pts[j].x += (contourFinder.blobs[i].centroid.x - pts[j].x) * (1.0 - shapeScale);
            }
            
            tempPoly.addVertices(pts);
            tempPoly.setClosed(true);
            
            // smoothing is set to 200 (20)
            ofPolyline smoothTempPoly = tempPoly.getSmoothed((int)smoothSize, smoothShape);
            
            if(!smoothTempPoly.isClosed()){
                smoothTempPoly.close();
            }
            contourPoly.push_back(smoothTempPoly);
        }
    }
    
    averageCircles();
}

void testApp::averageCircles(){
    vector<ofPoint> tmp;
    tmp.reserve(trackedCirclesInt);
    for (int i = 0; i < trackedCirclesInt; i++){
        tmp[i] = ofPoint(0,0,0);
    }
    for (int i = 0; i < trackedCirclesInt; i++){
        for (int j = 0; j < circleHistory[i].size(); j++){
            tmp[i] += circleHistory[i][j];
        }
        circleAverage[i] = tmp[i]/circleHistory[i].size();
    }
}

void testApp::sendMessage(ofRectangle rect){
    for (int i = 0; i < trackedCirclesInt; i++){
        sendMessage("/isadora/"+ofToString(i), circleAverage[i].x);
        sendMessage("/isadora/"+ofToString(i+1), circleAverage[i].y);
    }
}

void testApp::sendMessage(string address, float val){
    ofxOscMessage msg;
	msg.setAddress(address);
	msg.addFloatArg(val);
    oscSender.sendMessage(msg);
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(255, 255, 255);
    if (showVideo) {
        kinect.draw(0,0,ofGetWindowWidth(),ofGetWindowHeight());
    }
    if (showDepth) grayDiff.draw(0,0,ofGetWindowWidth(),ofGetWindowHeight());//kinect.drawDepth(0,0,ofGetWindowWidth(),ofGetWindowHeight());
    
    if (showFlow) flowSolver.draw(ofGetWindowWidth(),ofGetWindowHeight(), 10, 10);

    ofColor c(255, 255, 255);
    
    drawShapes();
    
    //drawPointCloud();
    
    syphonServer.publishScreen();//Texture(&kinect.getDepthTextureReference());
    
    stringstream m;
    m << "fps " << ofGetFrameRate() << endl;
//    << "pyramid scale: " << flowSolver.getPyramidScale() << " p/P" << endl
//    << "pyramid levels: " << flowSolver.getPyramidLevels() << " l/L" << endl
//    << "averaging window size: " << flowSolver.getWindowSize() << " w/W" << endl
//    << "iterations per level: " << flowSolver.getIterationsPerLevel() << " i/I" << endl
//    << "expansion area: " << flowSolver.getExpansionArea() << " a/A" << endl
//    << "expansion sigma: " << flowSolver.getExpansionSigma() << " s/S" << endl
//    << "flow feedback: " << flowSolver.getFlowFeedback() << " f/F" << endl
//    << "gaussian filtering: " << flowSolver.getGaussianFiltering() << " g/G";
    
    //ofDrawBitmapString(m.str(), 20, 20);
}

void testApp::setUI(){
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 255-xInit;
    float dim = 16; 
    gui = new ofxUICanvas(0, 0, length+xInit, ofGetHeight());
    
    gui->addWidgetDown(new ofxUILabel("NEAR     FAR", OFX_UI_FONT_MEDIUM));
    
    gui->addWidgetDown(new ofxUINumberDialer(500., 8000., &near, 1,  "NEAR", OFX_UI_FONT_MEDIUM));
    gui->addWidgetRight(new ofxUINumberDialer(500., 10000., &far, 1,  "FAR", OFX_UI_FONT_MEDIUM));

//    gui->addSpacer(length, 2);
//    gui->addWidgetDown(new ofxUILabelToggle(dim, dim, &showFlow, "FLOW", OFX_UI_FONT_MEDIUM));
    showFlow = false;
//    gui->addSpacer(length, 2);
//    gui->addWidgetDown(new ofxUILabelToggle(dim, dim, &showShape, "SHAPE", OFX_UI_FONT_MEDIUM));
    showShape = false;
//    gui->addWidgetDown(new ofxUIMinimalSlider(length-xInit, dim, 1.0, 200.0, &smoothSize, "SMOOTHSIZE",OFX_UI_FONT_MEDIUM));
//    gui->addWidgetDown(new ofxUIMinimalSlider(length-xInit, dim, 0.0, 1.0, &smoothShape, "SMOOTHSHAPE",OFX_UI_FONT_MEDIUM));
//    gui->addWidgetDown(new ofxUIMinimalSlider(length-xInit, dim, 0.0, 1.0, &shapeScale, "SHAPE SCALE",OFX_UI_FONT_MEDIUM));
    
    gui->addSpacer(length, 2);
    gui->addWidgetDown(new ofxUILabelToggle(dim, dim, &showRect, "CIRCLE", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUIMinimalSlider(length-xInit, dim, 0.0, 1.0, &circleScale, "CIRCLE SCALE",OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUIMinimalSlider(length-xInit, dim, 0.0, 400.0, &historySize, "HISTORY SIZE",OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUIMinimalSlider(length-xInit, dim, 0.0, 4.0, &trackedCircles, "CIRCLE NUM",OFX_UI_FONT_MEDIUM));
    
    gui->addSpacer(length, 2);
    gui->addWidgetDown(new ofxUILabelToggle(dim, dim, &showDepth, "DEPTH", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUIMinimalSlider(length-xInit, dim, 1.0, 255.0, &depthThreshold, "D THRESHOLD",OFX_UI_FONT_MEDIUM));
    gui->addSpacer(length, 2);    
    gui->addWidgetDown(new ofxUILabelToggle(dim, dim, &showVideo, "VIDEO", OFX_UI_FONT_MEDIUM));
    
    gui->addSpacer(length, 2);

//    gui->addLabelButton("SNAP BACK", false, length-xInit);
//    gui->setDrawBack(false);
    
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->loadSettings("GUI/guiSettings.xml");
    gui->setDrawWidgetPaddingOutline(true);
}

void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
    
    if ("SNAP BACK" == name){
        videoTexture = kinect.getTextureReference();
    }
    if ("FAR" == name || "NEAR" == name) {
        kinect.setDepthClipping(near,far);
    }

}

void testApp::drawShapes() {
    float scalex = (float)ofGetWindowWidth()/WIDTH;
    float scaley = (float)ofGetWindowHeight()/HEIGHT;

    ofPushStyle();
    glPushMatrix();
    glTranslatef( 0, 0, 0.0 );
    glScalef( scalex, scaley, 0.0 );
    ofColor c(255, 255, 255, 255);
    if (showRect){
        for( int i=0; i<trackedCirclesInt; i++ ) {
            //c.setHsb(255, 255, 255);
            ofSetColor(c);
            ofCircle(circleAverage[i].x, circleAverage[i].y, circleScale*WIDTH);
        }
    }
    
    if (showShape){
        for (int i = 0; i < contourPoly.size(); i++){
            c.setHsb(255, i * 64, 255);
            ofSetColor(c);
            contourPoly[i].draw();

            ofBeginShape();
            for( int j = 0; j < contourPoly[i].getVertices().size(); j++) {
                ofVertex(contourPoly[i].getVertices().at(j).x, contourPoly[i].getVertices().at(j).y);
            }
            ofEndShape();
        }
    }
	glPopMatrix();
	ofPopStyle();
}

void testApp::drawPointCloud(){
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_POINTS);
        int step = 2;
        ofColor c(255, 255, 255, 14);
        for (int i = 0; i < contourPoly.size(); i++){
            c.setHsb(255, 255, i * 64);

            for(int y = 0; y < HEIGHT; y += step) {
                for(int x = 0; x < WIDTH; x += step) {
                    ofPoint point (x,y);
                    if(kinect.getDistanceAt(x, y) > 0 && contourPoly[i].inside(point)) {
                        mesh.addColor(kinect.getColorAt(x,y));
                        mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
                    }
                }
            }
        }
    
        glPointSize(1);
        ofPushMatrix();
        // the projected points are 'upside down' and 'backwards'
        ofScale(1, 1, -1);
        ofTranslate(0, 0, -1000); // center the points a bit
        glEnable(GL_DEPTH_TEST);
        mesh.drawVertices();
        glDisable(GL_DEPTH_TEST);
        ofPopMatrix();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == 'p') { flowSolver.setPyramidScale(ofClamp(flowSolver.getPyramidScale() - 0.01,0.0,1.0)); }
    else if(key == 'P') { flowSolver.setPyramidScale(ofClamp(flowSolver.getPyramidScale() + 0.01,0.0,1.0)); }
    else if(key == 'l') { flowSolver.setPyramidLevels(MAX(flowSolver.getPyramidLevels() - 1,1)); }
    else if(key == 'L') { flowSolver.setPyramidLevels(flowSolver.getPyramidLevels() + 1); }
    else if(key == 'w') { flowSolver.setWindowSize(MAX(flowSolver.getWindowSize() - 1,1)); }
    else if(key == 'W') { flowSolver.setWindowSize(flowSolver.getWindowSize() + 1); }
    else if(key == 'i') { flowSolver.setIterationsPerLevel(MAX(flowSolver.getIterationsPerLevel() - 1,1)); }
    else if(key == 'I') { flowSolver.setIterationsPerLevel(flowSolver.getIterationsPerLevel() + 1); }
    else if(key == 'a') { flowSolver.setExpansionArea(MAX(flowSolver.getExpansionArea() - 2,1)); }
    else if(key == 'A') { flowSolver.setExpansionArea(flowSolver.getExpansionArea() + 2); }
    else if(key == 's') { flowSolver.setExpansionSigma(ofClamp(flowSolver.getExpansionSigma() - 0.01,0.0,10.0)); }
    else if(key == 'S') { flowSolver.setExpansionSigma(ofClamp(flowSolver.getExpansionSigma() + 0.01,0.0,10.0)); }
    else if(key == 'f') { flowSolver.setFlowFeedback(false); }
    else if(key == 'F') { flowSolver.setFlowFeedback(true); }
    else if(key == 'g') { flowSolver.setGaussianFiltering(false); }
    else if(key == 'G') { flowSolver.setGaussianFiltering(true); }
    
    switch (key)
	{
        case 'h':
            gui->toggleVisible();
            break;
    }
}


//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}