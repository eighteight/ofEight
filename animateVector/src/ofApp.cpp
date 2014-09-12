#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    //Setup our animator with it's xml path
    vectorAnimator.setup( 60 , ofToDataPath( "animatorData.xml") ) ;
    ofBackground(0, 0, 0) ;
    ofSetFrameRate( 60 ) ;
}

//--------------------------------------------------------------
void ofApp::update()
{
    vectorAnimator.update( ) ;
    
    //Add a point when we drag
    if ( vectorAnimator.bDragging )
        vectorAnimator.addPoint( ofPoint ( mouseX , mouseY ) ) ;
}

//--------------------------------------------------------------
void ofApp::draw()
{
    vectorAnimator.draw ( ) ;
    
    string instructions = "c - clear , s - save settings " ;
    ofDrawBitmapStringHighlight( instructions , ofPoint ( 15 , 15 ) ) ;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch ( key )
    {
        case 'c':
        case 'C':
            vectorAnimator.clear( ) ;
            break ;
            
        case 's':
        case 'S':
            vectorAnimator.saveToXml( ofToDataPath( "animationData.xml" ) ) ;
            break ;
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
    vectorAnimator.startDrag() ;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    vectorAnimator.endDrag() ;
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

ofApp::~ofApp()
{
    //on exit, save XML data
    vectorAnimator.saveToXml( ofToDataPath( "animatorData.xml" ) ) ;
}


