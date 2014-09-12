//
//  VectorFrame.h
//  animatorVector
//
//  Created by Ben McChesney on 11/11/12.
//
//

#ifndef animatorVector_VectorFrame_h
#define animatorVector_VectorFrame_h

#include "ofMain.h"

class VectorFrame
{
    public :
        VectorFrame( ) {}
        
        void setup ( ) ;
        void update ( ) ;
        void draw ( ) ;


        void addPoint( ofPoint p ) ;
        vector<ofPoint> points ;
        void clear( ) ;
        void debugDraw( ) ; 
    /*
        int createNewPath ( ) ;
        void closePath ( ) ; 
    */
        
};
#endif
