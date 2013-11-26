#include "testApp.h"
#define WIDTH 640
#define HEIGHT 480
//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(30);
    kinect.init(false, false, true);
    flowSolver.setup(kinect.getWidth()/2, kinect.getHeight()/2, 0.5, 3, 10, 1, 7, 1.5, false, false);
    ofBackground(0,0,0);

    grayDiff.allocate(WIDTH,HEIGHT);
    far = 2550.0;
    near = 1.;
    showShape = showFlow = showRect = showDepth = true;
    setUI();
    syphonServer.setName("kinectFlow");
}

void testApp::exit(){
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
    kinect.close();
}

//--------------------------------------------------------------
void testApp::update(){
    kinect.setDepthClipping(near,far);
    // kinect.setDepthClipping(0.0,2550.0);
    kinect.update();
    if ( kinect.isFrameNew() ){
        flowSolver.update(kinect.getDepthPixels(), WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
        grayDiff.setFromPixels(kinect.getDepthPixelsRef());
        grayDiff.threshold(30);
        contourFinder.findContours(grayDiff, 5, (WIDTH*HEIGHT)/4, 2, false, true);
        
        cv::Mat img;
        //cv::cvtColor(...); //not sure where the variables in your example come from
        std::string store_path("..."); //put your output path here
        
        bool write_success = cv::imwrite(store_path, img);
        
        updateContours();
    }
}

void testApp::updateContours(){
    contourPoly.clear();
    rectangles.clear();
    for (int i = 0; i <contourFinder.nBlobs; i++){

        if(contourFinder.blobs[i].pts.size()>5){
            
            ofPolyline tempPoly;
            ofRectangle rect = contourFinder.blobs[i].boundingRect;
            rectangles.push_back(rect);
            tempPoly.addVertices(contourFinder.blobs[i].pts);
            tempPoly.setClosed(true);
            
            // smoothing is set to 200
            ofPolyline smoothTempPoly = tempPoly.getSmoothed(20, 0.5);
            
            if(!smoothTempPoly.isClosed()){
                smoothTempPoly.close();
            }

            contourPoly.push_back(smoothTempPoly);
        }
        
    }// smooth blobs
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(255, 255, 255);

    if (showDepth) kinect.drawDepth(0,0,ofGetWindowWidth(),ofGetWindowHeight());
    
    if (showFlow) flowSolver.draw(ofGetWindowWidth(),ofGetWindowHeight(), 10, 10);
    
    ofColor c(255, 255, 255);
    
    drawBoundingRects();
    
    syphonServer.publishScreen();
    
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
    
    gui->addWidgetDown(new ofxUINumberDialer(1., 5000., &near, 1,  "NEAR", OFX_UI_FONT_MEDIUM));
    gui->addWidgetRight(new ofxUINumberDialer(1., 5000., &far, 1,  "FAR", OFX_UI_FONT_MEDIUM));

    gui->setDrawBack(false);

    gui->addWidgetDown(new ofxUILabelToggle(dim, dim, &showFlow, "FLOW", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUILabelToggle(dim, dim, &showShape, "SHAPE", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUILabelToggle(dim, dim, &showRect, "RECT", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUILabelToggle(dim, dim, &showDepth, "DEPTH", OFX_UI_FONT_MEDIUM));
    
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->loadSettings("GUI/guiSettings.xml");
}

void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
}

void testApp::drawBoundingRects() {
    float scalex = (float)ofGetWindowWidth()/WIDTH;
    float scaley = (float)ofGetWindowHeight()/HEIGHT;

    ofPushStyle();
    glPushMatrix();
    glTranslatef( 0, 0, 0.0 );
    glScalef( scalex, scaley, 0.0 );
    ofColor c(255, 255, 255, 14);
    if (showRect){
        for( int i=0; i<rectangles.size(); i++ ) {
            c.setHsb(i * 64, 255, 255);
            ofSetColor(c);
            ofRect( rectangles[i].x, rectangles[i].y, rectangles[i].width, rectangles[i].height );
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