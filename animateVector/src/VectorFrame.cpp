//
//  VectorFrame.cpp
//  animatorVector
//
//  Created by Ben McChesney on 11/11/12.
//
//

#include "VectorFrame.h"

void VectorFrame::setup ( )
{

}
void VectorFrame::update ( )
{
    
}

void VectorFrame::draw ( )
{
    for ( int p = 0 ; p < points.size() ; p++ )
    {
        ofSetColor( 255 , 0 , 0 ) ;
        ofCircle( points[p] , 25 ) ;
    }
}


void VectorFrame::addPoint( ofPoint p )
{
    points.push_back( p ) ; 
}

void VectorFrame::clear( )
{
    points.clear( ) ;
}

void VectorFrame::debugDraw( )
{
    for ( int i = 0 ; i < points.size() ; i++ )
    {
        if ( i < (points.size() - 1) )
        {
            ofLine( points[i].x , points[i].y , points[i+1].x , points[i+1].y ) ;
        }
    }
}

/*
int VectorFrame::createNewPath ( )
{
    
}
void VectorFrame::closePath ( )
{
    
}
*/
