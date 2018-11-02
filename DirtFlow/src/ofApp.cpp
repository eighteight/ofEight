#include "ofApp.h"
#include "ofxInkSim.h"
#include "ofxInkSim.h"

void ofApp::setup()
{
    dirtSim.setup();
    gui.setup(dirtSim.getUniformInfo());
    
    fbo.allocate(ofGetWidth(), ofGetHeight());
    cout << ofGetWidth() << " " << ofGetHeight() << endl;
    brushDir.open("brushes");
    brushDir.listDir();
    //brushes.resize(brushDir.size());
    int j = 0;
    for (int i = 0; i < brushDir.size(); i++)
    {
        ofFile f = brushDir.getFile(i);
        if (!f.isDirectory()) {
            cout<< f.getFileName()<<endl;
            ofTexture txt;
            brushes.push_back(txt);
            ofLoadImage(brushes.at(j), f.getAbsolutePath());
            brushList.push_back(f);
            j++;
        }
    }
    brushDir.close();
    
    base.load("imgs/base.jpeg");
    psBlend.setup(ofGetWidth(), ofGetHeight());
    blendMode = 0;
    currDrawMode = "INKFIX";
    ofSetWindowTitle(currDrawMode);
    syphonServer.setName("DirtFlow");
    currBrush = 0;
    gui.toggleVisible();
    
    ofStyle st;
    st.bgColor = ofColor(0);
}

void ofApp::update()
{
    dirtSim.update();

    psBlend.begin();
    base.draw(0,0);
    psBlend.end();
}

void ofApp::draw()
{
    dirtSim.draw();

  //  psBlend.draw(dirtSim.screen.getTexture(), blendMode);
    syphonServer.publishTexture(&dirtSim.screen.getTexture());
    gui.draw();
    
    ofDrawBitmapStringHighlight("press 'f' or 't' to fill buffer", 10, ofGetHeight() - 60);
    ofDrawBitmapStringHighlight("press 'c' to clear buffer", 10, ofGetHeight() - 40);
    ofDrawBitmapStringHighlight("press ' ' to hide gui", 10, ofGetHeight() - 20);
}

void ofApp::mouseDragged(int x, int y, int button)
{
    ofColor c = ofColor(ofRandom(240,255), ofRandom(140,180), ofRandom(30,90));
    dirtSim.stroke(&brush, x, y, getInkColor(c.getHueAngle(), 255, 10));
}

void ofApp::mouseReleased(int x, int y, int button)
{
    dirtSim.stopStroke(&brush);
}

void ofApp::dragEvent(ofDragInfo dragInfo){
    for (auto &&path:dragInfo.files) {
        ofFile file(path);
        int i = 0;
        for (auto exFile: brushList){
            if (exFile.getFileName() == file.getFileName()){
                currBrush = i;
                setBrush(currBrush);
                return;
            }
            i++;
        }
        file.copyTo("brushes", true, true);
        brushDir.open("brushes");
        
        brushList.push_back(file);
        
        brushes.resize(brushes.size()+1);
        ofLoadImage(brushes.at(brushes.size()-1),path);
        currBrush = brushes.size()-1;
        setBrush(currBrush);
    }
}

void ofApp::keyPressed(int key)
{
    if (key == OF_KEY_LEFT) {
        currBrush ++;
        currBrush = currBrush >= brushes.size() ? 0 : currBrush;
        setBrush(currBrush);
        ofSetWindowTitle(currDrawMode + ": " + brushList.at(currBrush).getFileName());
    } else if (key == OF_KEY_RIGHT) {
        currBrush --;
        currBrush = currBrush < 0 ? brushes.size() -1 : currBrush;
        setBrush(currBrush);
        ofSetWindowTitle(currDrawMode + ": " + brushList.at(currBrush).getFileName());
    }
    
    if(key & OF_KEY_MODIFIER){
        isControlKey = (key == OF_KEY_LEFT_CONTROL || key == OF_KEY_RIGHT_CONTROL);
    } else if (isControlKey) {}
    
    if (key == '1')
    {
        dirtSim.setDrawMode(ofxInkSim::INKFIX);
        currDrawMode = "INKFIX";
    }
    else if (key == '2')
    {
        dirtSim.setDrawMode(ofxInkSim::INKSURF);
        currDrawMode = "INKSURF";
    }
    else if (key == '3')
    {
        dirtSim.setDrawMode(ofxInkSim::INKFLOW);
        currDrawMode = "INKFLOW";
    }
    else if (key == '4')
    {
        dirtSim.setDrawMode(ofxInkSim::WATERFLOW);
        currDrawMode = "WATERFLOW";
    }
    else if (key == 'd')
    {
        dirtSim.toggleDebug();
    }
    else if (key == ' ')
    {
        gui.toggleVisible();
    }
    else if (key == 'r')
    {
        gui.resetToDefault();
    }
    else if (key == 'c')
    {
        dirtSim.clear();
    }
    
    if (key == 'f')
    {
        int angRange = ofRandom(100, 200);
        int angMin = ofRandom(360);
        
        fbo.begin();
        ofClear(0);
        for (int i = 0; i < 500; i++)
        {
            int ang = ofRandom(angMin, angRange);
            if (ang > 360)
                ang -= 360;
            
            float rad = ofRandom(2, 10);
            float depth = 255;//ofMap(rad, 2, 10, 255, 200, true);
            
            ofColor cTemp = ofColor(ofRandom(240,255), ofRandom(140,180), ofRandom(30,90));
            ofColor c = getInkColor(cTemp.getHueAngle(), 255, 255);
            //ofColor c = getInkColor(ang, 255, depth);
            ofPushStyle();
            ofSetColor(c);
            ofEllipse(ofRandomWidth(), ofRandomHeight(), rad, rad * 2.0);
            ofPopStyle();
        }
        fbo.end();
        
        //        fbo.begin();
        //        ofClear(0);
        //        for (int i = 0; i < 100; i++)
        //        {
        //            int ang = ofRandom(angMin, angRange);
        //            if (ang > 360)
        //                ang -= 360;
        //
        //            float rad = ofRandom(10, 20);
        //
        //            for (int j = 0; j < 10; j++)
        //            {
        //                float iteRad = ofMap(j, 0, 10, rad/10, rad, true);
        //                float iteAng = ang;// + ofRandom(-10, 10);
        //
        //                if (iteAng < 0)
        //                    iteAng += 360;
        //                else if (iteAng > 360)
        //                    iteAng -= 360;
        //
        //                float depth = 255;
        //                ofColor c = getInkColor(iteAng, 255, depth);
        //                ofPushStyle();
        //                ofSetColor(c);
        //                ofCircle(ofRandomWidth(), ofRandomHeight(), iteRad);
        //                ofPopStyle();
        //            }
        //        }
        //        fbo.end();
        
        dirtSim.begin();
        fbo.draw(0, 0);
        dirtSim.end();
    }
    
    if (key == 't')
    {
        setBrush(ofRandom(brushes.size()));
    }
    if (key == OF_KEY_UP)
    {
        if (blendMode >= 24)
        {
            blendMode = 0;
        }
        else {
            blendMode++;
        }
    }
    if (key == OF_KEY_DOWN)
    {
        if (blendMode <= 0)
        {
            blendMode = 24;
        }
        else
        {
            blendMode--;
        }
    }
    if (key == 's')
    {
        ofSaveFrame();
    }
    
    //ofSetWindowTitle(currDrawMode + ": " + brushDir.getName(currBrush));
}

void ofApp::setBrush(int brsh) {
    if (brsh >= brushes.size()) return;
    //ofColor c = ofColor(ofRandom(255), ofRandom(255), ofRandom(255));
    ofColor c = ofColor(ofRandom(110,120), ofRandom(76,86), ofRandom(32,37));
    float depth = ofRandom(10, 100);
    
    ofSetRectMode(OF_RECTMODE_CENTER);
    dirtSim.begin();
    ofPushMatrix();
    ofTranslate(ofRandomWidth(), ofRandomHeight(), ofRandom(-100, -1500));
    ofPushStyle();
    ofSetColor(getInkColor(c.getHueAngle(), 255, 255));
    brushes.at(brsh).draw(0, 0);
    //brush = brushes
    ofPopStyle();
    ofPopMatrix();
    dirtSim.end();
    ofSetRectMode(OF_RECTMODE_CORNER);
    
    currBrush = brsh;
}

void ofApp::keyReleased(int key){
    if (key & OF_KEY_MODIFIER){
        isControlKey = !(key == OF_KEY_LEFT_CONTROL || key == OF_KEY_RIGHT_CONTROL);
    }
}
void ofApp::mouseMoved(int x, int y){}
void ofApp::mousePressed(int x, int y, int button){}
void ofApp::windowResized(int w, int h){}
void ofApp::gotMessage(ofMessage msg){}
