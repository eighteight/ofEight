#include "ofApp.h"

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
    
    blob.allocate(640,480,OF_IMAGE_GRAYSCALE);
    // fx
	postFx.init(ofGetWidth(), ofGetHeight());
	
	postFx.createPass<BloomPass>();
	postFx.createPass<FxaaPass>();
    
    pointSkip = 200;
}

//--------------------------------------------------------------
void ofApp::update(){
    kinect.update();
    if( kinect.isFrameNew() ){
        texDepth.loadData( kinect.getDepthPixels() );
        texIr.loadData( kinect.getRawIrPixels() );
        texRGB.loadData( kinect.getRgbPixels() );
        return;
        ////effects
        del.reset();
        
		
		unsigned char* pix = new unsigned char[640*480];
		
		unsigned char* gpix = new unsigned char[640*480];
		
		for(int x=0;x<640;x+=1) {
			for(int y=0;y<480;y+=1) {
				float distance = kinect.getDepthPixels()[y * 640 + x]; //   getDistanceAt(x, y);
				
				int pIndex = x + y * 640;
				pix[pIndex] = 0;
                
				if(distance > 100 && distance < 1100) {
					pix[pIndex] = 255;
				}
                
			}
		}
		
		blob.setFromPixels(pix, 640, 480, OF_IMAGE_GRAYSCALE);
		
		int numPoints = 0;
		
		for(int x=0;x<640;x+=pointSkip*2) {
			for(int y=0;y<480;y+=pointSkip*2) {
				int pIndex = x + 640 * y;
                
				if(blob.getPixels()[pIndex]> 0) {
					ofVec3f wc(x,y,kinect.getDepthPixels()[y * 640 + x]);//kinect.getWorldCoordinateAt(x, y);
					
					wc.x = x - 320.0;
					wc.y = y - 240.0;
					
					if(abs(wc.z) > 100 && abs(wc.z ) < 2000) {
						
						wc.z = -wc.z;
						
						wc.x += ofSignedNoise(wc.x,wc.z)*noiseAmount;
						wc.y += ofSignedNoise(wc.y,wc.z)*noiseAmount;
						
						wc.x = ofClamp(wc.x, -320,320);
						wc.y = ofClamp(wc.y, -240,240);
						
						del.addPoint(wc);
					}
					numPoints++;
				}
                
			}
		}
        if (false){
		
		if(numPoints >0)
            del.triangulate();
		
		for(int i=0;i<del.triangleMesh.getNumVertices();i++) {
			del.triangleMesh.addColor(ofColor(0,0,0));
		}
		
		for(int i=0;i<del.triangleMesh.getNumIndices()/3;i+=1) {
			ofVec3f v = del.triangleMesh.getVertex(del.triangleMesh.getIndex(i*3));
			
			v.x = ofClamp(v.x, -319,319);
			v.y = ofClamp(v.y, -239, 239);
            
			ofColor c = kinect.getRgbPixels()[v.x+320.0, v.y+240.0];
            
			if(!useRealColors)
                c = ofColor(255,0,0);
            
			c.a = colorAlpha;
			
			del.triangleMesh.setColor(del.triangleMesh.getIndex(i*3),c);
			del.triangleMesh.setColor(del.triangleMesh.getIndex(i*3+1),c);
			del.triangleMesh.setColor(del.triangleMesh.getIndex(i*3+2),c);
		}
        

        
        
        convertedMesh.clear();
        wireframeMesh.clear();
        wireframeMesh.setMode(OF_PRIMITIVE_TRIANGLES);
        for(int i=0;i<del.triangleMesh.getNumIndices()/3;i+=1) {
            
            int indx1 = del.triangleMesh.getIndex(i*3);
            ofVec3f p1 = del.triangleMesh.getVertex(indx1);
            int indx2 = del.triangleMesh.getIndex(i*3+1);
            ofVec3f p2 = del.triangleMesh.getVertex(indx2);
            int indx3 = del.triangleMesh.getIndex(i*3+2);
            ofVec3f p3 = del.triangleMesh.getVertex(indx3);
            
            ofVec3f triangleCenter = (p1+p2+p3)/3.0;
            triangleCenter.x += 320;
            triangleCenter.y += 240;
            
            triangleCenter.x = floor(ofClamp(triangleCenter.x, 0,640));
            triangleCenter.y = floor(ofClamp(triangleCenter.y, 0, 480));
            
            int pixIndex = triangleCenter.x + triangleCenter.y * 640;
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
        delete gpix;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    texDepth.draw(10, 100);
    texIr.draw(600, 100);
    texRGB.draw(10, 110 + texDepth.getHeight(), 1920/4, 1080/4);
    
    panel.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
