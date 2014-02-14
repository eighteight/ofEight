#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetLogLevel(ofxOpenNI::LOG_NAME,OF_LOG_VERBOSE);
	ofSetLogLevel(ofxOpenNICapture::LOG_NAME,OF_LOG_VERBOSE);
	
	bool live = true;
	
	if(live){
		openNI.setupFromXML("openni/config/ofxopenni_config.xml",false);
	}else{
		openNI.setupFromRecording("recording.oni");
	}
	
	bRecording = false;
    
    ofSetFrameRate(30);
	
}

//--------------------------------------------------------------
void testApp::update(){
	openNI.update();
	if(bRecording){
		recorder.update();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	openNI.draw(0,0);
	openNI.drawRGB(640,0);
	ofDrawBitmapString(ofToString(ofGetFrameRate()),20,20);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	if(key=='r'){
		bRecording = !bRecording;
		if(bRecording){
            
            time_t rawtime;
            struct tm *timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            XnChar strFileName[XN_FILE_MAX_PATH];
            sprintf(strFileName, "%s/%04d%02d%02d-%02d%02d%02d-dev%d.oni", ".",
                    timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, 0);

            
			recorder.setup(openNI,strFileName);
			recorder.startCapture();
		}else{
			recorder.stopCapture();
		}
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

