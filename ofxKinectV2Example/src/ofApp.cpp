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
    
    blob.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_GRAYSCALE);
    // fx
	postFx.init(ofGetWidth(), ofGetHeight());
	
	postFx.createPass<BloomPass>();
	postFx.createPass<FxaaPass>();
    
    pointSkip = 40;
    
    syphonServerRGB.setName("kinectRGB");
    syphonServerDepth.setName("kinectDepth");
    
    isSaving = false;
    
}

//--------------------------------------------------------------
void ofApp::update(){
    kinect.update();
    if( kinect.isFrameNew() ){
        texDepth.loadData( kinect.getDepthPixels() );
        texRGB.loadData( kinect.getRgbPixels() );
        ////effects
        del.reset();

		unsigned char* pix = new unsigned char[DEPTH_WIDTH*DEPTH_HEIGHT];
		
		for(int x=0; x < DEPTH_WIDTH; x+=1) {
			for(int y=0; y < DEPTH_HEIGHT; y+=1) {
				float distance = kinect.getDepthPixels()[y * DEPTH_WIDTH + x]; //   getDistanceAt(x, y);
				
				int pIndex = x + y * DEPTH_WIDTH;
				pix[pIndex] = 0;
                
				if(distance > 100 && distance < 1100) {
					pix[pIndex] = 255;
				}
                
			}
		}
		
		blob.setFromPixels(pix, DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_GRAYSCALE);
		
		int numPoints = 0;
		
		for(int x=0; x < DEPTH_WIDTH; x+=pointSkip*2) {
			for(int y=0; y < DEPTH_HEIGHT; y+=pointSkip*2) {
				int pIndex = x + DEPTH_WIDTH * y;
                
				if(blob.getPixels()[pIndex]> 0) {
					ofVec3f wc(x,y,kinect.getDepthPixels()[y * DEPTH_WIDTH + x]);//kinect.getWorldCoordinateAt(x, y);
					
					wc.x = x - DEPTH_WIDTH * 0.5;
					wc.y = y - DEPTH_HEIGHT * 0.5;
					
					if(abs(wc.z) > 100 && abs(wc.z ) < 2000) {
						
						wc.z = -wc.z;
						
						wc.x += ofSignedNoise(wc.x,wc.z)*noiseAmount;
						wc.y += ofSignedNoise(wc.y,wc.z)*noiseAmount;
						
						wc.x = ofClamp(wc.x, - DEPTH_WIDTH * 0.5, DEPTH_WIDTH * 0.5);
						wc.y = ofClamp(wc.y, - DEPTH_HEIGHT * 0.5, DEPTH_HEIGHT * 0.5);
						
						del.addPoint(wc);
					}
					numPoints++;
				}
                
			}
		}
        
        cout << "Delaunay points "<<numPoints<<endl;
        if (true){
		
		if(numPoints >0)
            del.triangulate();
		
		for(int i=0;i<del.triangleMesh.getNumVertices();i++) {
			del.triangleMesh.addColor(ofColor(0,0,0));
		}
		
		for(int i=0;i<del.triangleMesh.getNumIndices()/3;i+=1) {
			ofVec3f v = del.triangleMesh.getVertex(del.triangleMesh.getIndex(i*3));
			
			v.x = ofClamp(v.x, -319,319);
			v.y = ofClamp(v.y, -239, 239);
            
			ofColor c = useRealColors ? kinect.getRgbPixels()[v.x+DEPTH_WIDTH * .5, v.y+DEPTH_HEIGHT*.5] : ofColor(255,0,0);
            
			c.a = colorAlpha;
			
			del.triangleMesh.setColor(del.triangleMesh.getIndex(i*3),c);
			del.triangleMesh.setColor(del.triangleMesh.getIndex(i*3+1),c);
			del.triangleMesh.setColor(del.triangleMesh.getIndex(i*3+2),c);
		}

        convertedMesh.clear();
        wireframeMesh.clear();
        wireframeMesh.setMode(OF_PRIMITIVE_TRIANGLES);
            cout<<"mesh points "<< del.triangleMesh.getNumVertices()<< endl;
        for(int i=0;i<del.triangleMesh.getNumIndices()/3;i+=1) {
            
            int indx1 = del.triangleMesh.getIndex(i*3);
            ofVec3f p1 = del.triangleMesh.getVertex(indx1);
            int indx2 = del.triangleMesh.getIndex(i*3+1);
            ofVec3f p2 = del.triangleMesh.getVertex(indx2);
            int indx3 = del.triangleMesh.getIndex(i*3+2);
            ofVec3f p3 = del.triangleMesh.getVertex(indx3);
            
            ofVec3f triangleCenter = (p1+p2+p3)/3.0;
            triangleCenter.x += DEPTH_WIDTH * .5;
            triangleCenter.y += DEPTH_HEIGHT* .5;
            
            triangleCenter.x = floor(ofClamp(triangleCenter.x, 0, DEPTH_WIDTH));
            triangleCenter.y = floor(ofClamp(triangleCenter.y, 0, DEPTH_HEIGHT));
            
            int pixIndex = triangleCenter.x + triangleCenter.y * DEPTH_WIDTH;
            if(pix[pixIndex] > 0) {
                
                convertedMesh.addVertex(p1);
                convertedMesh.addColor(del.triangleMesh.getColor(indx1));
                
                convertedMesh.addVertex(p2);
                convertedMesh.addColor(del.triangleMesh.getColor(indx2));
                
                convertedMesh.addVertex(p3);
                convertedMesh.addColor(del.triangleMesh.getColor(indx3));
                
                wireframeMesh.addVertex(p1);
                wireframeMesh.addVertex(p2);
                wireframeMesh.addVertex(p3);
            }
        }
        }
        
        delete pix;
    }
    
    if (isSaving) {
        ofxObjLoader::save("delaunay" + ofToString(ofGetFrameNum()), convertedMesh);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
//    texDepth.draw(10, 100);
//    texRGB.draw(10, 110 + texDepth.getHeight(), 1920/4, 1080/4);
    
    if (texRGB.isAllocated()) {
        syphonServerRGB.publishTexture(&texRGB);
    }
    
    if (texDepth.isAllocated()){
        syphonServerDepth.publishTexture(&texDepth);
    }

    //panel.draw();
    
    ofBackground(219, 214, 217);

    
    ofPushMatrix();
    
	cam.begin();
	cam.setScale(1,-1,1);
    
	ofSetColor(255,255,255);
	ofTranslate(0, -80,1100);
	ofFill();
	
	postFx.begin();
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glShadeModel(GL_FLAT);
	glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
	convertedMesh.drawFaces();
	glShadeModel(GL_SMOOTH);
	glPopAttrib();
	
	if(useRealColors) {
		ofSetColor(30,30,30, 255);
	} else
        ofSetColor(124,136,128,255);
	
	ofPushMatrix();
	ofTranslate(0, 0,0.5);
	wireframeMesh.drawWireframe();
	ofPopMatrix();
	cam.end();
	ofPopMatrix();
    
	postFx.end();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == 's'){
        isSaving = !isSaving;
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
