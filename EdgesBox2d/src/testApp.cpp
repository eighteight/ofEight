#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	
    ofDisableAntiAliasing();
	ofBackgroundHex(0xffffff);
	ofSetLogLevel(OF_LOG_NOTICE);
	ofSetVerticalSync(true);
	
	// Box2d
	box2d.init();
	box2d.setGravity(0, 30);
	box2d.createGround();
	box2d.setFPS(30.0);
	
	
	// load the lines we saved...
	ifstream f;
	f.open(ofToDataPath("lines.txt").c_str());
	vector <string> strLines;
	while (!f.eof()) {
		string ptStr;
		getline(f, ptStr);
		strLines.push_back(ptStr);
	}
	f.close();
	
	for (int i=0; i<strLines.size(); i++) {
		vector <string> pts = ofSplitString(strLines[i], ",");
		if(pts.size() > 0) {
			ofPtr <ofxBox2dEdge> edge = ofPtr<ofxBox2dEdge>(new ofxBox2dEdge);
			for (int j=0; j<pts.size(); j+=2) {
				if(pts[j].size() > 0) {
					float x = ofToFloat(pts[j]);
					float y = ofToFloat(pts[j+1]);
					edge.get()->addVertex(x, y);
				}				
			}
			edge.get()->create(box2d.getWorld());
			edges.push_back(edge);
		}
	}
    
    boxSize = 10;
    
    setupAudio();
	
}

void testApp::setupAudio(){        
        // 0 output channels,
        // 2 input channels
        // 44100 samples per second
        // 256 samples per buffer
        // 4 num buffers (latency)
        
        soundStream.listDevices();
        
        //if you want to set a different device id
        //soundStream.setDeviceID(0); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
        
        int bufferSize = 256;
        
        
        left.assign(bufferSize, 0.0);
        right.assign(bufferSize, 0.0);
        volHistory.assign(400, 0.0);
        
        bufferCounter	= 0;
        drawCounter		= 0;
        smoothedVol     = 0.0;
        scaledVol		= 0.0;
        
        soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
        
    }

//--------------------------------------------------------------
void testApp::audioIn(float * input, int bufferSize, int nChannels){
	
	float curVol = 0.0;
	
	// samples are "interleaved"
	int numCounted = 0;
    
	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume
	for (int i = 0; i < bufferSize; i++){
		left[i]		= input[i*2]*0.5;
		right[i]	= input[i*2+1]*0.5;
        
		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted+=2;
	}
	
	//this is how we get the mean of rms :)
	curVol /= (float)numCounted;
	
	// this is how we get the root of rms :)
	curVol = sqrt( curVol );
	
	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;
	
	bufferCounter++;
	
}


//--------------------------------------------------------------
void testApp::update() {
	//lets scale the vol up to a 0-1 range
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    
	//lets record the volume into an array
	volHistory.push_back( scaledVol );
	
	//if we are bigger the the size we want to record - lets drop the oldest value
	if( volHistory.size() >= 400 ){
		volHistory.erase(volHistory.begin(), volHistory.begin()+1);
	}
    
   for (int i=0; i<circles.size(); i++) {
        circles[i].get()->addForce(ofVec2f(-smoothedVol,-smoothedVol), circles[i].get()->getWidth()*500.);
    }
    cout<<smoothedVol<<endl;
	box2d.update();	
}

//--------------------------------------------------------------
void testApp::draw() {
	
	
	// some circles :)
	for (int i=0; i<circles.size(); i++) {
		ofFill();
		ofSetHexColor(0xff0000);
		circles[i].get()->draw();
	}
	
	ofSetHexColor(0xdddddd);
	ofNoFill();
	for (int i=0; i<lines.size(); i++) {
		lines[i].draw();
	}	
	for (int i=0; i<edges.size(); i++) {
		edges[i].get()->draw();
	}	
	
	string info = "Draw a shape with the mouse\n";
	info += "Press 1 to add some squares\n";
	info += "Press c to clear everything\n";
	
	ofSetHexColor(0x444342);
	//ofDrawBitmapString(info, 10, 15);
}


//--------------------------------------------------------------
void testApp::keyPressed(int key) {
	
	if(key == '1') {
        ofPtr<ofxBox2dRect> c = ofPtr<ofxBox2dRect>(new ofxBox2dRect);
		c.get()->setPhysics(1, 0.5, 0.5);
		c.get()->setup(box2d.getWorld(), mouseX, mouseY, boxSize, boxSize);
		circles.push_back(c);
	}
	
	if(key == 'c') {
		lines.clear();
		edges.clear();
	}
    
    if (key == OF_KEY_UP){
        boxSize +=10;
    }
    
    if (key == OF_KEY_DOWN){
        boxSize -=10;
    }
	
	
	// want to save out some line...
	if(key == ' ') {
		ofstream f;
		f.clear();
		f.open(ofToDataPath("lines.txt").c_str());
		for (int i=0; i<lines.size(); i++) {
			for (int j=0; j<lines[i].size(); j++) {
				float x = lines[i][j].x;
				float y = lines[i][j].y;
				f << x << "," << y << ",";
			}
			f << "\n";
		}
		f.close();lines.clear();
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
	lines.back().addVertex(x, y);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
	lines.push_back(ofPolyline());
	lines.back().addVertex(x, y);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {
	
    ofPtr <ofxBox2dEdge> edge = ofPtr<ofxBox2dEdge>(new ofxBox2dEdge);
	lines.back().simplify();
	
	for (int i=0; i<lines.back().size(); i++) {
		edge.get()->addVertex(lines.back()[i]);
	}
	
	//poly.setPhysics(1, .2, 1);  // uncomment this to see it fall!
	edge.get()->create(box2d.getWorld());
	edges.push_back(edge);
	
	//lines.clear();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {
}

