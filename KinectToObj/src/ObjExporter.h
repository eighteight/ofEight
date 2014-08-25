//
//  ObjExporter.h
//  openniMesh
//
//  Created by Gusev, Vladimir on 9/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef openniMesh_ObjExporter_h
#define openniMesh_ObjExporter_h

#include "boost/unordered/unordered_map.hpp"
#include "boost/lexical_cast.hpp"
#include <fstream>

class Vertex{
public:
	Vertex(float x, float y, float z, float xT, float yT):x(x), y(y), z(z), xT(xT), yT(yT), i(0){};
	~Vertex(){};
	bool operator < (const Vertex &other) const {
		if (x != other.x) return x < other.x;
		if (y != other.y) return y < other.y;
		else return z < other.z;
	}
	bool operator == (const Vertex &other) const { return x == other.x && y == other.y && z == other.z; }
    float x, y, z, xT, yT;
    size_t i;
};

class VertexComp {
public:
	bool operator()(Vertex s1, Vertex s2) {
		return (! (s1.x == s2.x && s1.y==s2.y && s1.z == s2.z));
	}
};

class Face{
public:
	Face(Vertex a, Vertex b, Vertex c): a(a), b(b), c(c){};
	~Face(){};
	Vertex a,b,c;
};

struct VertexHash: std::unary_function<Vertex, std::size_t>
{
    std::size_t operator()(Vertex const& v) const
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, v.x);
        boost::hash_combine(seed, v.y);
        boost::hash_combine(seed, v.z);
        return seed;
    }
};

struct ObjData{
    ObjData(){};
    ObjData(ofPoint* d, ofImage t):d(d), t(t){};
    ofPoint* d;
    ofImage t;
};

class ObjExporter{
public:
    ObjExporter(std::string filePrefix, size_t sizeX, size_t sizeY):filePrefix(filePrefix), fileFrameCounter(0),sizeX(sizeX), sizeY(sizeY), size(sizeX*sizeY){
        averagedDepth = new ofPoint[size];
    };

    ~ObjExporter(){
    	if (averagedDepth) delete [] averagedDepth;

    }

private:
    size_t sizeX, sizeY, size;
    std::string filePrefix;
    ofPoint*              averagedDepth;
    size_t fileFrameCounter;
    std::vector<Face> faces;
    std::vector<ofPoint*> depthQueue;
    std::vector<Vertex> indV;
    
    boost::unordered_map<Vertex, size_t, VertexHash> vrtxMap;

public:
    void exportDepthToObj(ofPoint* depth, ofImage colorSurface){
        double start;//
        
        computeFaces(faces, depth, averagedDepth, depthQueue);

        std::string myPath = /*ofGetHomgetHomeDirectory().string()+*/"eliot2/"+filePrefix+"."+getFileNameSuffix();
        std::cout << "Exporting OBJ: " << myPath << " "<<std::endl;
        std::ofstream oStream( (myPath+".obj").c_str() );	
        oStream << "# -----------------" << std::endl;
        oStream << "# Start of obj file" << std::endl;
        oStream << "g depth" << std::endl;
        
        oStream << "mtllib " + myPath + ".mat" << std::endl;
        
        computeAndWriteVertices(faces, vrtxMap, indV, oStream);
        
        //	for (boost::unordered_map <Vertex, size_t, VertexComp>::iterator vrt = vrtxMap.begin(); vrt != vrtxMap.end(); vrt++) {
        //		oStream << "vt "<<vrt->first.xT << " " << vrt->first.yT << endl;
        //	}
        //	for (boost::unordered_map <Vertex, size_t, VertexComp>::iterator vrt = vertexMap2.begin(); vrt != vertexMap2.end(); vrt++) {
        //		oStream << "vt "<<vrt->first.xT << " " << vrt->first.yT << endl;
        //	}
        for (std::vector <Vertex>::iterator ind = indV.begin(); ind != indV.end(); ind++) {
            oStream << "vt "<<ind->xT << " " <<ind->yT << std::endl;
        }
        
        for (std::vector <Vertex>::iterator ind = indV.begin(); ind != indV.end(); ind++) {
            oStream << "vt "<<ind->xT << " " <<ind->yT << std::endl;
        }
        
        oStream << "usemtl rgb" << std::endl;
        for (std::vector <Face>::iterator f = faces.begin(); f != faces.end(); f++) {
            oStream << "f " << toObjFaceIndex(vrtxMap[f->a])  << " " << toObjFaceIndex(vrtxMap[f->b])<< " " << toObjFaceIndex(vrtxMap[f->c])<< std::endl;
        }

        oStream << std::endl << std::endl;
        oStream.close();
        ///MTL FILE WRITE//////////////////////////////////////////////
        std::ofstream mtl((myPath+".mat").c_str() );
        mtl << "newmtl rgb" << std::endl;
        mtl << "Ka 1.000000 1.000000 1.000000" << std::endl;
        mtl << "Kd 1.000000 1.000000 1.000000" << std::endl;
        mtl << "Ks 0.000000 0.000000 0.000000" << std::endl;
        
        mtl << "illum 0" << std::endl;
        
        mtl << "map_Kd " + myPath + ".jpg" << std::endl;
        
        //        ///SECOND POINT CLOUD
        mtl << "newmtl rgb2" << std::endl;
        mtl << "Ka 1.000000 1.000000 1.000000" << std::endl;
        mtl << "Kd 1.000000 1.000000 1.000000" << std::endl;
        mtl << "Ks 0.000000 0.000000 0.000000" << std::endl;
        
        mtl << "illum 0" << std::endl;
        mtl << "map_Kd " + myPath + "_.jpg" << std::endl;
        mtl.close();
        
        
        ///SAVE RGB TEXTURE//////////////////////////////////////////////

//        writeImage( fs::path(myPath+".jpg"), colorSurface, ImageTarget::Options().colorModel( ImageIo::CM_RGB ).quality( 1.0f ) );

        //ip::flipVertical(&colorSurface2);
//        writeImage( fs::path(myPath+"_.jpg"), colorSurface2, ImageTarget::Options().colorModel( ImageIo::CM_RGB ).quality( 1.0f ) );
//        std::cout << "Export "<<fileFrameCounter<< " finished in " << (cinder::app::getElapsedSeconds() - start)<< " s. Accum: "<<depthQueue.size()<<std::endl;

        fileFrameCounter++;

    }

    void computeFaces(std::vector<Face> &faces, ofPoint* depthNew, ofPoint* avD, std::vector<ofPoint*>& depthQueue){
        computeDepthAverage(depthNew, avD, depthQueue);
        float maxDist = 2500.0;
        faces.clear();
        uint step = 2;
        for (size_t y = 0; y < sizeY - 1; y+=step) { // WRITE VERTICES which have faces
            for (size_t x = 0; x < sizeX - 1; x+=step) {
                size_t b = y * sizeX + x ;
                if (ofPoint(avD[b].x, avD[b].y, avD[b].z).lengthSquared() > 0.0) {
                    int a = b + step;
                    int c = (y + step) * sizeX + x;
                    int d = c + step;
                    ofPoint av(avD[a].x, avD[a].y, avD[a].z);
                    ofPoint bv(avD[b].x, avD[b].y, avD[b].z);
                    ofPoint cv(avD[c].x, avD[c].y, avD[c].z);
                    ofPoint dv(avD[d].x, avD[d].y, avD[d].z);
                    
                    float ab = av.distanceSquared(bv);
                    float bc = bv.distanceSquared(cv);
                    float ad = av.distanceSquared(dv);
                    float cd = cv.distanceSquared(dv);
                    
                    if (ab<maxDist && bc<maxDist && cd<maxDist&& ad<maxDist){
                        if (avD[a].z > 0 && avD[b].z > 0 && avD[c].z > 0) {
                            Vertex v1(avD[a].x, avD[a].y, avD[a].z, float(x+step)/(float)sizeX, float(y)/(float)sizeY);
                            Vertex v2(avD[b].x, avD[b].y, avD[b].z, float(x)/(float)sizeX, float(y)/(float)sizeY);
                            Vertex v3(avD[c].x, avD[c].y, avD[c].z, float(x)/(float)sizeX, float(y+step)/(float)sizeY);
                            faces.push_back(Face(v1, v2, v3));
                        }
                        if (avD[a].z > 0 && avD[d].z > 0 && avD[c].z > 0){
                            Vertex v1(avD[a].x, avD[a].y, avD[a].z, float(x+step)/(float)sizeX, float(y)/(float)sizeY);
                            Vertex v2(avD[c].x, avD[c].y, avD[c].z, float(x)/(float)sizeX, float(y+step)/(float)sizeY);
                            Vertex v3(avD[d].x, avD[d].y, avD[d].z, float(x+step)/(float)sizeX, float(y+step)/(float)sizeY);
                            faces.push_back(Face(v1, v2, v3));
                        }
                    }
                }
            }
        }
    }

    void computeAndWriteVertices(std::vector<Face> &faces, boost::unordered_map<Vertex, size_t, VertexHash>& vertexMap, std::vector<Vertex>&indV, std::ofstream& oStream){
        vertexMap.clear();
        indV.clear();
        size_t cn = 0;
        for (std::vector <Face>::iterator fc = faces.begin(); fc != faces.end(); fc++) {
            if (vertexMap.insert(std::make_pair(fc->a,0)).second){
                indV.push_back(fc->a);
                vertexMap[fc->a] = ++cn;
                oStream << "v "<<fc->a.x << " " << fc->a.y << " " << fc->a.z << std::endl;
            }
            
            if (vertexMap.insert(std::make_pair(fc->b,0)).second){
                indV.push_back(fc->b);
                vertexMap[fc->b] = ++cn;
                oStream << "v "<<fc->b.x << " " << fc->b.y << " " << fc->b.z << std::endl;
            }
            
            if (vertexMap.insert(std::make_pair(fc->c,0)).second){
                indV.push_back(fc->c);
                vertexMap[fc->c] = ++cn;
                oStream << "v "<<fc->c.x << " " << fc->c.y << " " << fc->c.z << std::endl;
            }
        }
    }
    void computeDepthAverage(ofPoint* depthNew, ofPoint* avD, std::vector<ofPoint*>& depthQueue ){
        depthQueue.push_back(depthNew);
        uint limit = 10;
        if (depthQueue.size()>limit){
           depthQueue.erase (depthQueue.begin()+0 );
        }
        ofPoint zr;
        zr.x = zr.y = zr.z = 0.0f;

        std::fill(avD, avD+size, zr);

        for (std::vector<ofPoint*>::iterator dpth = depthQueue.begin(); dpth != depthQueue.end(); dpth ++){
            for (size_t i = 0; i < size; i++){
                avD[i].x +=(*dpth)[i].x;
                avD[i].y +=(*dpth)[i].y;
                avD[i].z +=(*dpth)[i].z;
            }
        }
        float oneOverSize = 1.0/(float)depthQueue.size();
        for (size_t i = 0; i < size; i++){
            avD[i].x *= oneOverSize;
            avD[i].y *= oneOverSize;
            avD[i].z *= oneOverSize;
        }
    }
    std::string toObjFaceIndex(size_t ind){
        std::string temp = boost::lexical_cast<std::string>(ind);
        return temp + "/" +temp;// + "/"+temp;
    }
    std::string getFileNameSuffix(){
        if (fileFrameCounter<10)
            return "000000"+boost::lexical_cast<std::string>(fileFrameCounter);
        if (fileFrameCounter<100)
            return "00000"+boost::lexical_cast<std::string>(fileFrameCounter);
        if (fileFrameCounter<1000)
            return "0000"+boost::lexical_cast<std::string>(fileFrameCounter);
        if (fileFrameCounter<10000)
            return "000"+boost::lexical_cast<std::string>(fileFrameCounter);
        if (fileFrameCounter<100000)
            return "00"+boost::lexical_cast<std::string>(fileFrameCounter);
        if (fileFrameCounter<1000000)
            return "0"+boost::lexical_cast<std::string>(fileFrameCounter);
        return boost::lexical_cast<std::string>(fileFrameCounter);
    }
};

#endif
