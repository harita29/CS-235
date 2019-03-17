#include "ofApp.h"

Image::Image()
{
    trans.x = 0;
    trans.y = 0;
    scale.x = 1.0;
    scale.y = 1.0;
    rot = 0;
    bSelected = false;
}

void Image::draw(bool isSelectMode, int index)
{
    ofPushMatrix();
    ofTranslate(trans);
    ofScale(scale);
    ofRotate(rot);
    if (isSelectMode) {
        ofFill();
        ofSetColor(index + 1, 0, 0);
        ofDrawRectangle(-image.getWidth() / 2.0, -image.getHeight() / 2.0,
                        image.getWidth(), image.getHeight());
    }
    else {
        if (bSelected) {
            ofNoFill();
            ofSetColor(255, 0, 0);
            ofSetLineWidth(3);
            drawSelectionGrid();
        }
        ofSetColor(255, 255, 255, 255);
        image.draw(-image.getWidth() / 2.0, -image.getHeight() / 2.0);
    }
    ofPopMatrix();
}

void Image::drawSelectionGrid() {
    ofDrawRectangle(-image.getWidth() / 2.0, -image.getHeight() / 2.0,
                    image.getWidth(), image.getHeight());
    ofFill();
    //x scaling
    ofSetColor(254, 0, 0);
    ofDrawCircle(-image.getWidth() / 2.0, 0, 20);
    ofSetColor(254, 1, 0);
    ofDrawCircle(image.getWidth() / 2.0, 0, 20);
    //y scalling
    ofSetColor(253, 0, 0);
    ofDrawCircle(0, -image.getHeight() / 2.0, 20);
    ofSetColor(253, 1, 0);
    ofDrawCircle(0, image.getHeight() / 2.0, 20);
    //xy scalling
    ofSetColor(252, 0, 0);
    ofDrawCircle(-image.getWidth() / 2.0, -image.getHeight() / 2.0, 20);
    ofSetColor(252, 1, 0);
    ofDrawCircle(image.getWidth() / 2.0, -image.getHeight() / 2.0, 20);
    ofSetColor(252, 2, 0);
    ofDrawCircle(-image.getWidth() / 2.0, image.getHeight() / 2.0, 20);
    ofSetColor(252, 3, 0);
    ofDrawCircle(image.getWidth() / 2.0, image.getHeight() / 2.0, 20);
}


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    selectedImage = NULL;
}

//--------------------------------------------------------------
void ofApp::update() {
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(240, 240, 215, 100);
    ofFill();
    for (int i = 0; i < images.size(); i++) {
        images[i]->draw();
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    cout << "mouse( " << x << "," << y << ")" << endl;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    if (scaleMode) { doMouseScale(x, y); }
    else doMouseTransform(x, y);
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    unsigned char res[4];
    GLint viewport[4];
    
    glGetIntegerv(GL_VIEWPORT, viewport);
    glReadPixels(x, viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);
    
    switch (res[0]) {
        case 252:
            //xy scalling
            scaleMode = true;
            lastScaleMousePosition = ofVec2f(x, y);
            selectedPoint = res[1];
            return;
        case 253:
            //y scalling
            scaleMode = true;
            lastScaleMousePosition = ofVec2f(0, y);
            selectedPoint = res[1];
            return;
        case 254:
            //x scalling
            lastScaleMousePosition = ofVec2f(x, 0);
            scaleMode = true;
            selectedPoint = res[1];
            return;
    }
    
    renderSelection();
    processSelection(x, y);
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if (scaleMode) scaleMode = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::exit() {
    for (int i = 0; i < images.size(); i++) {
        delete images[i];
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case OF_KEY_BACKSPACE:
        case OF_KEY_DEL:
            deleteImage();
            break;
        case OF_KEY_UP:
            pushImageUp();
            break;
        case OF_KEY_DOWN:
            pushImageDown();
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch (key) {
        // User can save final image by pressing 's' or 'S'
        case 's':
        case 'S':
            picture.grabScreen(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
            picture.save("final.jpg");
            break;
    }
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    Image *imageObj = new Image();
    imageObj->trans.x = dragInfo.position.x;
    imageObj->trans.y = dragInfo.position.y;
    if (imageObj->image.load(dragInfo.files[0]) == true)
        images.push_back(imageObj);
    else {
        cout << "Can't load image: " << dragInfo.files[0] << endl;
        delete imageObj;
    }
}

void ofApp::processSelection(int x, int y) {
    unsigned char pixels[4];
    GLint viewport[4];
    
    glGetIntegerv(GL_VIEWPORT, viewport);
    glReadPixels(x, viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixels);
    
    if (selectedImage) {
        selectedImage->bSelected = false;
        selectedImage = NULL;
    }
    
    if (pixels[0] > 0 && pixels[0] <= images.size()) {
        Image *image = images[pixels[0] - 1];
        image->bSelected = true;
        selectedImage = image;
        
        images.erase(images.begin() + (pixels[0] - 1));
        images.push_back(image);
    }
    
}

//--------------------------------------------------------------
void ofApp::renderSelection() {
    ofBackground(0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (int i = 0; i < images.size(); i++) {
        images[i]->draw(true, i);
    }
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

//--------------------------------------------------------------
void ofApp::doMouseScale(int x, int y) {
    
    //if there are no images stored in the list, just return
    if (!selectedImage) return;
    
    //scalling along y axis
    if (lastScaleMousePosition.x == 0) {
        if (selectedPoint == 1) {//if cursor is moving away from image
            selectedImage->scale.y = 1 + (y - lastScaleMousePosition.y)/100;
        }
        else {//if cursor is moving towards the image
            selectedImage->scale.y = 1 + (lastScaleMousePosition.y - y) / 100;
        }
    } //scalling along x axis
    else if (lastScaleMousePosition.y == 0) {
        if (selectedPoint == 0) {
            selectedImage->scale.x = 1 + (lastScaleMousePosition.x - x) / 100;
        }
        else {
            selectedImage->scale.x = 1 + (x - lastScaleMousePosition.x) / 100;
        }
    } //scalling along both x,y axis
    else {
        if (selectedPoint == 0) {
            selectedImage->scale.y = 1 + (-y + lastScaleMousePosition.y) / 100;
            selectedImage->scale.x = 1 + (-x + lastScaleMousePosition.x) / 100;
        }
        else if (selectedPoint == 1) {
            selectedImage->scale.y = 1 + (lastScaleMousePosition.y - y) / 100;
            selectedImage->scale.x = 1 + (-lastScaleMousePosition.x + x) / 100;
        }
        else if (selectedPoint == 2) {
            selectedImage->scale.y = 1 + (y - lastScaleMousePosition.y) / 100;
            selectedImage->scale.x = 1 + (-x + lastScaleMousePosition.x) / 100;
        }
        else {
            selectedImage->scale.y = 1 + (y - lastScaleMousePosition.y) / 100;
            selectedImage->scale.x = 1 + (x - lastScaleMousePosition.x) / 100;
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::doMouseTransform(int x, int y) {
    
    //if there are no images stored in the list, just return
    if (!selectedImage) return;
    
    selectedImage->trans = ofVec2f(x, y);
}

//--------------------------------------------------------------
void ofApp::deleteImage()
{
    if (!selectedImage) {
        return;
    }
    int delIndex = 0;
    
    for (int i = 0; i < images.size(); i++) {
        if (images[i] == selectedImage) {
            delIndex = i;
        }
    }
    images.erase(images.begin() + delIndex);
    
    selectedImage->bSelected = false;
    selectedImage = NULL;
}

//--------------------------------------------------------------
void ofApp::pushImageUp()
{
    if (!selectedImage)
        return;
    
    Image* i = images[0];
    images.erase(images.begin());
    images.push_back(i);
}

//--------------------------------------------------------------
void ofApp::pushImageDown()
{
    if (!selectedImage)
        return;
    
    Image* i = images[images.size()-1];
    images.pop_back();
    images.insert(images.begin(), i);
}
