#include "testApp.h"
#define WIDTH 640
#define HEIGHT 480
#define OSC_ADDRESS "/isadora/1"
//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(30);
    kinect.init(false, true, true);

    ofBackground(0,0,0);

    grayDiff.allocate(WIDTH,HEIGHT);
    far = 2550.0;
    near = 500.0;
    showRect = showDepth = true;
    smoothSize = 20.0;
    smoothShape = 0.5;
    shapeScale = 0.5;
    
    depthThreshold = 30.;
    
    setUI();

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
        grayDiff.setFromPixels(kinect.getDepthPixelsRef());
        grayDiff.threshold((int)depthThreshold);
        contourFinder.findContours(grayDiff, 5, (WIDTH*HEIGHT)/4, 2, false, true);
        
        updateContours();
    }
}

void testApp::updateContours(){
    contourPoly.clear();
    rectangles.clear();
    for (int i = 0; i <contourFinder.nBlobs; i++){

        if(contourFinder.blobs[i].pts.size()>0){
            
            ofPolyline tempPoly;
            ofRectangle rect = contourFinder.blobs[i].boundingRect;
            rectangles.push_back(rect);
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
        
    }// smooth blobs
}

void testApp::sendMessage(ofRectangle rect){
    sendMessage("/isadora/1", rect.x);
    sendMessage("/isadora/2", rect.y);
    sendMessage("/isadora/3", rect.width);
    sendMessage("/isadora/4", rect.height);
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
    

    ofColor c(255, 255, 255);
    
    drawShapes();
    
    drawPointCloud();
    
    stringstream m;
    m << "fps " << ofGetFrameRate() << endl;
    
    //ofDrawBitmapString(m.str(), 20, 20);
}

void testApp::setUI(){
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 255-xInit;
    float dim = 16; 
    gui = new ofxUICanvas(0, 0, length+xInit, ofGetHeight());
    
    gui->addWidgetDown(new ofxUILabel("NEAR     FAR", OFX_UI_FONT_MEDIUM));
    
    gui->addWidgetDown(new ofxUINumberDialer(500., 4000., &near, 1,  "NEAR", OFX_UI_FONT_MEDIUM));
    gui->addWidgetRight(new ofxUINumberDialer(500., 8000., &far, 1,  "FAR", OFX_UI_FONT_MEDIUM));

    gui->addSpacer(length, 2);
    gui->addWidgetDown(new ofxUIMinimalSlider(length-xInit, dim, 1.0, 200.0, &smoothSize, "SMOOTHSIZE",OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUIMinimalSlider(length-xInit, dim, 0.0, 1.0, &smoothShape, "SMOOTHSHAPE",OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUIMinimalSlider(length-xInit, dim, 0.0, 1.0, &shapeScale, "SHAPE SCALE",OFX_UI_FONT_MEDIUM));
    
    gui->addSpacer(length, 2);
    gui->addWidgetDown(new ofxUILabelToggle(dim, dim, &showRect, "RECT", OFX_UI_FONT_MEDIUM));
    gui->addSpacer(length, 2);
    gui->addWidgetDown(new ofxUILabelToggle(dim, dim, &showDepth, "DEPTH", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUIMinimalSlider(length-xInit, dim, 1.0, 255.0, &depthThreshold, "D THRESHOLD",OFX_UI_FONT_MEDIUM));
    gui->addSpacer(length, 2);    
    gui->addWidgetDown(new ofxUILabelToggle(dim, dim, &showVideo, "VIDEO", OFX_UI_FONT_MEDIUM));
    
    gui->addSpacer(length, 2);

    gui->addLabelButton("SNAP BACK", false, length-xInit);
    gui->setDrawBack(false);
    
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
    ofColor c(255, 255, 255, 14);
    if (showRect){
        for( int i=0; i<rectangles.size(); i++ ) {
            c.setHsb(i * 64, 255, 255);
            ofSetColor(c);
            ofRect( rectangles[i].x, rectangles[i].y, rectangles[i].width, rectangles[i].height );
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