#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    kinect.init(false, false, true);
    flowSolver.setup(kinect.getWidth()/2, kinect.getHeight()/2, 0.5, 3, 10, 1, 7, 1.5, false, false);
    ofBackground(0,0,0);
    
    colorImg.allocate(640,480);
    grayImage.allocate(640,480);
    grayBg.allocate(640,480);
    grayDiff.allocate(640,480);

}

//--------------------------------------------------------------
void testApp::update(){
    kinect.setDepthClipping(0,2550);
    kinect.update();
    if ( kinect.isFrameNew() ){
        flowSolver.update(kinect.getDepthPixels(), 640, 480, OF_IMAGE_GRAYSCALE);
//        grayDiff.absDiff(grayBg, grayImage);
//        grayDiff.threshold(30);
        grayDiff.setFromPixels(kinect.getDepthPixelsRef());
        grayDiff.threshold(30);
        contourFinder.findContours(grayDiff, 5, (640*480)/4, 2, false, true);
        updateContours();
    }
}

void testApp::updateContours(){
    contourPoly.clear();
    for (int i = 0; i <contourFinder.nBlobs; i++){
        
        // does blob have any points?
        if(contourFinder.blobs[i].pts.size()>5){
            
            ofPolyline tempPoly;
            
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

    kinect.drawDepth(0,0,ofGetWindowWidth(),ofGetWindowHeight());
    
    flowSolver.draw(ofGetWindowWidth(),ofGetWindowHeight());
    contourFinder.draw(ofGetWindowWidth(), 0, ofGetWindowWidth(), ofGetWindowHeight());
    
    ofColor c(255, 255, 255);
    
    ofScale(2.0, 2.0);
    for (int i = 0; i < contourPoly.size(); i++){
        c.setHsb(i * 64, 255, 255);
        ofSetColor(c);
        contourPoly[i].draw();
        
        ofBeginShape();
        for( int j = 0; j < contourPoly[i].getVertices().size(); j++) {
            ofVertex(contourPoly[i].getVertices().at(j).x, contourPoly[i].getVertices().at(j).y);
        }
        ofEndShape();
    }
    

    
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
    
    ofDrawBitmapString(m.str(), 20, 20);
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