//
//  VectorAnimator.cpp
//  animatorVector
//
//  Created by Ben McChesney on 11/11/12.
//
//

#include "VectorAnimator.h"


    
void VectorAnimator::setup ( int _maxFrames , string xmlPath )
{
    //try and load the XML config
    if ( loadFromXml(xmlPath ) != true )
    {
        maxFrames = _maxFrames ;
        currentFrame = 0 ;
        
        for ( int i = 0 ; i < maxFrames ; i++ )
        {
            frames.push_back( VectorFrame() ) ;
            frames[ i ].draw( ) ;
        }
    }
    
    //fbo for trails
    fbo.allocate( ofGetWidth() , ofGetHeight() , GL_RGBA32F_ARB ) ;
    
    fbo.begin();
	ofClear(255,255,255, 0);
    fbo.end();
    
    bDragging = false ;    
}

void VectorAnimator::update ( )
{
    //Mod the frame buy our max frames for easy looping.
    currentFrame = ( ofGetFrameNum() % maxFrames ) ; 
}

void VectorAnimator::draw ( )
{
    ofEnableAlphaBlending() ;
    fbo.begin() ;
    
    ofSetColor( 0 , 0 , 0 , 16 ) ;
    ofRect( 0 , 0 , ofGetWidth() , ofGetHeight() ) ;
    if ( bDragging == true )
    {
        int prevFrame = currentFrame - 1 ;
        if ( prevFrame < 0 )
            prevFrame = maxFrames - 1 ;
        frames[ prevFrame ].draw( ) ; 
    }
    frames [ currentFrame ].draw( ) ;
    
    fbo.end() ;
    
    ofSetColor( 255 , 255 , 255 ) ;
    fbo.draw( 0 , 0 ) ;
}

void VectorAnimator::startDrag()
{
    bDragging = true ;
    //TO DO find a way to save colors ? other styles ?
    //dragColor = ofColor::fromHsb( ofRandom( 255 ) , ofRandom( 215 , 255 ) , ofRandom( 225 , 255 ) ) ;
    
}

void VectorAnimator::endDrag()
{
    bDragging = false ; 
}
ofColor dragColor;

void VectorAnimator::addPoint ( ofPoint p )
{
    frames[ currentFrame ].addPoint( p ) ;
}

void VectorAnimator::saveToXml( string xmlPath )
{
    xml.clear( ) ;
    
    
    //Save out each frame
    for ( int i = 0 ; i < frames.size() ; i++ )
    {
        xml.addTag("frame" ) ;
        xml.pushTag("frame" , i ) ;
        //Per frame save all the points
        for ( int p = 0 ; p < frames[i].points.size() ; p++ )
        {
            xml.setValue("x", frames[i].points[p].x / ofGetWidth() , p ) ;
            xml.setValue("y", frames[i].points[p].y / ofGetHeight() , p ) ;
        }
        xml.popTag( ) ;
        
    }
    
    xml.saveFile(xmlPath) ; 
}

bool VectorAnimator::loadFromXml( string xmlPath )
{
    xml.clear( ) ;
    
    bool bResult = xml.loadFile(xmlPath ) ;
    maxFrames = xml.getNumTags("frame" ) ;
    //load each frame
    for ( int i = 0 ; i < maxFrames ; i++ )
    {
        xml.pushTag("frame" , i ) ;
            frames.push_back( VectorFrame() ) ;
            //Load each point within each frame
            int numPoints = xml.getNumTags( "x" ) ;
            for ( int p = 0 ; p < numPoints ; p++ )
            {
                frames[i].addPoint( ofPoint ( xml.getValue("x", 0.0f , p ) * ofGetWidth()  , xml.getValue("y", 0.0f , p ) * ofGetHeight() ) );
            }
            
        xml.popTag( ) ;
        
    }
    
    return bResult ;
}

void VectorAnimator::clear( )
{
    for ( int i = 0 ; i < frames.size() ; i++ )
    {
        frames[i].clear( ) ;
    }
    
    fbo.begin() ;
        ofClear(255.0f , 255.0f , 255.0f  ) ;
    fbo.end( );
}





