#pragma once

#include "ofMain.h"

class Image {
    
public:
    ofImage image;
    ofVec2f trans, scale;
    float rot;
    bool bSelected;
    void draw(bool isSelectMode = false, int index = 0);
    void drawSelectionGrid();
    
    Image();
    
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
    
    void exit(); // Used to exit the application, deletes all images in the vector
    void doMouseTransform(int x, int y);
    void doMouseScale(int x, int y);
    void deleteImage();
    void pushImageUp();
    void pushImageDown();
    void processSelection(int x, int y);
    void renderSelection();
    
private:
    Image *selectedImage;
    vector<Image*> images;
    ofVec2f        lastScaleMousePosition;
    ofVec2f        lastMousePosition;
    bool           scaleMode=false;
    ofImage        picture;
    int            selectedPoint;
};
