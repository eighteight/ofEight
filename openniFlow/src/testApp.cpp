#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
    
    ofSetLogLevel(OF_LOG_NOTICE);
    

    //openNIDevices[deviceID].setLogLevel(OF_LOG_VERBOSE); // ofxOpenNI defaults to ofLogLevel, but you can force to any level
    //openNIDevice.setup();
    openNIDevice.setupFromONI("/Users/vgusev/repos/onis/20120811-141213-calib-dev0.oni");
    openNIDevice.addDepthGenerator();
    openNIDevice.addImageGenerator();
    openNIDevice.addUserGenerator();
    openNIDevice.setRegister(true);
    openNIDevice.setMirror(true);
    openNIDevice.start();
    
    // NB: Only one device can have a user generator at a time - this is a known bug in NITE due to a singleton issue
    // so it's safe to assume that the fist device to ask (ie., deviceID == 0) will have the user generator...
    
    openNIDevice.setMaxNumUsers(1); // defualt is 4
    ofAddListener(openNIDevice.userEvent, this, &testApp::userEvent);
    
    ofxOpenNIUser user;
    user.setUseMaskTexture(true);
    user.setUsePointCloud(true);
    user.setPointCloudDrawSize(2); // this is the size of the glPoint that will be drawn for the point cloud
    user.setPointCloudResolution(2); // this is the step size between points for the cloud -> eg., this sets it to every second point
    openNIDevice.setBaseUserClass(user); // this becomes the base class on which tracked users are created
                                             // allows you to set all tracked user properties to the same type easily
                                             // and allows you to create your own user class that inherits from ofxOpenNIUser
    
    // if you want to get fine grain control over each possible tracked user for some reason you can iterate
    // through users like I'm doing below. Please not the use of nID = 1 AND nID <= openNIDevices[0].getMaxNumUsers()
    // as what you're doing here is retrieving a user that is being stored in a std::map using it's XnUserID as the key
    // that means it's not a 0 based vector, but instead starts at 1 and goes upto, and includes maxNumUsers...
//    for (XnUserID nID = 1; nID <= openNIDevices[0].getMaxNumUsers(); nID++){
//        ofxOpenNIUser & user = openNIDevices[0].getUser(nID);
//        user.setUseMaskTexture(true);
//        user.setUsePointCloud(true);
//        //user.setUseAutoCalibration(false); // defualts to true; set to false to force pose detection
//        //user.setLimbDetectionConfidence(0.9f); // defaults 0.3f
//        user.setPointCloudDrawSize(2);
//        user.setPointCloudResolution(1);
//    }
    verdana.loadFont(ofToDataPath("verdana.ttf"), 24);
    
    
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 255-xInit;
    gui = new ofxUICanvas(length+xInit+2, 0, length+xInit, ofGetHeight());  
    gui->addWidgetDown(new ofxUILabel("NEAR", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUINumberDialer(0, 10000, 5000, 1, "NEAR", OFX_UI_FONT_MEDIUM));
    
    gui->addSpacer(length-xInit, 2);
    gui->addWidgetDown(new ofxUILabel("FAR", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUINumberDialer(0, 10000, 5000, 1, "FAR", OFX_UI_FONT_MEDIUM));
    gui->setDrawBack(false);
    
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->loadSettings("GUI/guiSettings.xml"); 

}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();
	cout << "got event from: " << name << endl;
    
    if(name == "NEAR")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		cout << "NEAR " << slider->getScaledValue() << endl;
		near = slider->getScaledValue();

	}
    
    if(name == "FAR")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		cout << "FAR " << slider->getScaledValue() << endl;
		far = slider->getScaledValue();
	}
	
}

//--------------------------------------------------------------
void testApp::update(){
    ofBackground(0, 0, 0);
    openNIDevice.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(255, 255, 255);
    
    ofPushMatrix();

//        openNIDevice.drawDebug(); // debug draw does the equicalent of the commented methods below
          openNIDevice.drawDepth(0, 0, 640, 480);
//        openNIDevices[deviceID].drawImage(640, 0, 640, 480);
//        openNIDevices[deviceID].drawSkeletons(640, 0, 640, 480);
        

    // do some drawing of user clouds and masks
    ofPushMatrix();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    int numUsers = openNIDevice.getNumTrackedUsers();
    for (int nID = 0; nID < numUsers; nID++){
        ofxOpenNIUser & user = openNIDevice.getTrackedUser(nID);
        user.drawMask();
        ofPushMatrix();
        ofTranslate(320, 240, -1000);
        user.drawPointCloud();
        ofPopMatrix();
    }
    ofDisableBlendMode();
    ofPopMatrix();

}

//--------------------------------------------------------------
void testApp::userEvent(ofxOpenNIUserEvent & event){
    ofLogNotice() << getUserStatusAsString(event.userStatus) << "for user" << event.id << "from device" << event.deviceID;
}

//--------------------------------------------------------------
void testApp::exit(){
    
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
    
    // this often does not work -> it's a known bug -> but calling it on a key press or anywhere that isnt std::aexit() works
    // press 'x' to shutdown cleanly...
        openNIDevice.stop();


}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    int cloudRes = -1;
    switch (key) {
        case '1':
            cloudRes = 1;
            break;
        case '2':
            cloudRes = 2;
            break;
        case '3':
            cloudRes = 3;
            break;
        case '4':
            cloudRes = 4;
            break;
        case '5':
            cloudRes = 5;
            break;
        case 'x':
            openNIDevice.stop();
            break;
        case 'i':
            if (openNIDevice.isImageOn()){
                openNIDevice.removeImageGenerator();
                openNIDevice.addInfraGenerator();
            }
            if (openNIDevice.isInfraOn()){
                openNIDevice.removeInfraGenerator();
                openNIDevice.addImageGenerator();
            }
            break;
        case 'b':
            openNIDevice.setUseBackBuffer(!openNIDevice.getUseBackBuffer());
            break;
        case 'h':
            gui->toggleVisible();
        default:
            break;
    }
    for (int deviceID = 0; deviceID < numDevices; deviceID++){
		openNIDevice.setPointCloudResolutionAllUsers(cloudRes);
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

