//--------------------------------------------------------------
//
// CS235 - Rotate Pivot Manipulator Functions
//
//  This is implemented as a set of methods in the app class
//  (and not generalied as a seperate Manipulator class) 
//  as it is intended to be given as a test problem.
//
// Kevin M. Smith - SJSU - 3/17/2018
//
//
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(ofColor::black);
	rotator.setColor(ofColor::blueViolet);
	pivot.setColor(ofColor::lightGreen);
}

//  Set Pivot and Rotator Handle locations to center of image
//  and outside top edge.
//
void ofApp::setupHandles() {

	float w = picture.image.getWidth();
	float h = picture.image.getHeight();
	picture.pivot = ofVec3f(w / 2, h / 2);
	pivot.setPosition(picture.trans.x, picture.trans.y);
	rotator.setPosition(picture.trans.x, picture.trans.y - h / 2 - 20);
}


//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
// Draw picture and manipulator handles.
//
void ofApp::draw(){
	if (imageLoaded) {
		picture.draw();
		drawHandles();
	}
}

//-------------------------------------------------------------
// Draw Handles with a line connecting them
//
void ofApp::drawHandles() {
	ofDrawLine(pivot.trans, rotator.trans);
	pivot.draw();
	rotator.draw();
}

//--------------------------------------------------------------
// Delete image data 
//
void ofApp::doDelete() {
	if (imageLoaded) {
		picture.image.clear();
		imageLoaded = false;
	}
}


//--------------------------------------------------------------
//
void ofApp::keyPressed(int key){
	switch (key) {
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'd':
		doDelete();
		break;
	case 'c':
	{
		// set image in a neutral postion, reset handles
		// 
		picture.rot = 0;
		picture.trans = dropPoint;
		setupHandles();
		break;
	}
	
	case OF_KEY_DEL:
		doDelete();
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
// mouseDragged - Do dragging of rotator and pivot handles.  
//
// if rotator dragged, we need to rotate image 
// if pivot dragged, update handle location but don't modifty 
// actual image pivot until mouse released.
//
void ofApp::mouseDragged(int x, int y, int button){
    ofVec3f mouse_cur = ofVec3f(x,y);
    ofVec3f delta = mouse_cur - mouse_last;
    if (pivot_selected) {
        pivot.trans += delta;
    } else if (rotator_selected) {
        rotator.trans += delta;
        float rotation;
        
        ofVec3f pivot_point = ofVec3f(pivot.trans.x, pivot.trans.y);
        rotation = mouse_cur.angle(pivot_point);
        
        if (mouse_cur.getCrossed(pivot_point).length() > mouse_last.getCrossed(pivot_point).length())
            picture.rot += rotation;
        else
            picture.rot -= rotation;
    }
    mouse_last = mouse_cur;
}

//--------------------------------------------------------------
//  mousePressed - check to see which handle we have pressed down
//  on. Save any state needed to do drag.
//
void ofApp::mousePressed(int x, int y, int button){
    mouse_last = ofPoint(x,y);
    if (pivot.inside(x, y)) {
        pivot_selected = true;
    } else if (rotator.inside(x, y)) {
        rotator_selected = true;
    }
    
}

//--------------------------------------------------------------
// if pivot dragged, update pivot usign Image::setPivot(). 
// you need to convert from screen space to image space since pivot is in image space.
// You need to tanslate image aftewards to the screen space location of the pivot
// 
void ofApp::mouseReleased(int x, int y, int button){
    
    if (pivot_selected) {
        int pivot_x = x - (picture.trans.x - (picture.image.getWidth()/2));
        int pivot_y = y - (picture.trans.y - (picture.image.getHeight()/2));
        ofVec3f pivot_point = ofVec3f(pivot_x,pivot_y);
        picture.setPivot(pivot_point);
        picture.screenToImageSpace(picture.trans.x - (picture.image.getWidth()/2), picture.trans.y - (picture.image.getHeight()/2));
        picture.trans = ofVec3f(x,y);
        pivot_selected = false;
    }
    
    if (rotator_selected) rotator_selected = false;

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//  Drop a single image into picture.  Only keeps one image at a time.
//  After images dropped. setup handles.
//
void ofApp::dragEvent(ofDragInfo dragInfo) {

	if (picture.image.load(dragInfo.files[0]) == true)
	{
		// translate image to drop location
		//
		picture.trans = ofVec3f(dragInfo.position.x, dragInfo.position.y);
		dropPoint = picture.trans;
		imageLoaded = true;
		setupHandles();

	}
	else {
		cout << "Can't load image: " << dragInfo.files[0] << endl;
	}
}
