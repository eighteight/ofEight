//
//  VectorAnimator.h
//  animatorVector
//
//  Created by Ben McChesney on 11/11/12.
//
//

#ifndef animatorVector_VectorAnimator_h
#define animatorVector_VectorAnimator_h

#include "ofMain.h"
#include "ofxXmlSettings.h"

#include "VectorFrame.h"

class VectorAnimator
{
    public :
        VectorAnimator( ) { }
    
        void setup ( int _maxFrames , string xmlPath  = "" ) ;
        void update ( ) ;
        void draw ( ) ;
        

        void addPoint ( ofPoint p ) ;
        void clear( ) ;
        void debugDraw( ) ;
    
        vector<VectorFrame> frames ;
        int currentFrame ;
        int maxFrames ;
    
        //Draw into FBO to still get nice trails
        ofFbo fbo ;
    
        //Xml Stuff
        void saveToXml( string xmlPath ) ;
        bool loadFromXml( string xmlPath ) ;
        ofxXmlSettings xml ;
    
        //Drag parameters
        bool bDragging ;    //If the mouse is dragging show the frame before
        void startDrag() ;
        void endDrag() ;
    
};

#endif
