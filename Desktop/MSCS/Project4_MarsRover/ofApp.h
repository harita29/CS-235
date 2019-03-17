#pragma once

#include "ofMain.h"
#include  "ofxAssimpModelLoader.h"
#include "box.h"
#include "ray.h"
#include "ofxGui.h"


class Octree {
public:
    vector<Octree> children;
    vector<ofVec3f> list_of_indices;
    Box currentbox;
    Octree() {};
    Octree(Box b) {
        this->currentbox = b;
    }
};

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void drawAxis(ofVec3f);
    void initLightingAndMaterials();
    void savePicture();
    void toggleMotionPathTool();
    void toggleWireframeMode();
    void togglePointsDisplay();
    void toggleSelectTerrain();
    void ofTogglePointSelection();
    void ofToggleDrawSlider();
    void setCameraTarget();
    void teleportCamera();
    void moveRover(int speed);
    void loadEditPointsFromFile();
    void loadEditPointsToFile();
    bool  doPointSelection();
    void drawBox(const Box &box);
    Box meshBounds(const ofMesh &);
    Box getBoxAtPoint(ofVec3f point);
    void subDivideBox8(const Box &b, vector<Box> & boxList);
    void createOcTree(Box &b, Octree &root, int depth);
    bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
    void findThePoint(Octree &root, Ray &ray);
    bool pointInRoverBox(ofVec3f point);
    ofEasyCam cam, trackCam, followCam, driverCam, backupCam;
    ofxAssimpModelLoader mars, rover;
    ofLight light;
    Box boundingBox;
    Box roverBox;
    vector<Box> level1, level2, level3, level4, level5;
    vector<ofVec3f> editPoints;
    Octree rootNode;
    ofVec3f teleportStartPoint;
    ofVec3f roverStartPoint;
    bool bAltKeyDown;
    bool bCtrlKeyDown;
    bool bWireframe;
    bool bDisplayPoints;
    bool bPointSelected;
    bool bAxisOnTheRover;
    bool bRoverLoaded;
    bool bRoverSelected;
    bool bTerrainSelected;
    bool bNoAxis;
    bool bRoverMove;
    bool bTeleport;
    bool bTeleportRover;
    bool bMotionPathTool;
    bool bDrawOfPath;
    bool bDrawSlider;
    bool bPointSelection;
    bool bPointDrag;
    bool defaultCamSelected;
    bool trackingCamSelected;
    bool followUpCamSelected;
    bool driverCamSelected;
    bool backupCamSelected;
    int bDraggedPoint;
    float startTime;
    float endTime;
    ofPath p;
    ofPolyline p1;
    ofVec3f selectedPoint;
    ofVec3f intersectPoint;
    ofFile myTextFile;
    ofxIntSlider speed;
    ofxPanel gui;
    string filePath = "points.txt";
    const float selectionRange = 4.0;
};
