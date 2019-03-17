//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Mars HiRise Project - startup scene
//
//  This is an openFrameworks 3D scene that includes an EasyCam
//  and example 3D geometry which I have reconstructed from Mars
//  HiRis photographs taken the Mars Reconnaisance Orbiter
//
//  You will use this source file (and include file) as a starting point
//  to implement assignment 5  (Parts I and II)
//
//  Please do not modify any of the keymappings.  I would like
//  the input interface to be the same for each student's
//  work.  Please also add your name/date below.

//  Please document/comment all of your work !
//  Have Fun !!
//
//  Student Name:   Harita Shroff.
//  Date: 05/10/2018


#include "ofApp.h"
#include "Util.h"


//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){
    
    bWireframe = false;
    bDisplayPoints = false;
    bAltKeyDown = false;
    bCtrlKeyDown = false;
    bPointSelected = false; // Harita
    bRoverLoaded = false;
    bRoverSelected = false;
    bTerrainSelected = true;
    bDrawOfPath = false;
    bPointDrag = false;
    defaultCamSelected = true;
    trackingCamSelected = false;
    followUpCamSelected = false;
    driverCamSelected = false;
    
    ofSetWindowShape(1024, 768); // Harita

    cam.setDistance(10);
    trackCam.setDistance(10);
    followCam.setDistance(10);
    driverCam.setDistance(10);
    backupCam.setDistance(10);
    
    cam.setNearClip(.1);
    trackCam.setNearClip(.1);
    followCam.setNearClip(.1);
    driverCam.setNearClip(.1);
    backupCam.setNearClip(.1);
    
    cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
    trackCam.setFov(65.5);
    followCam.setFov(65.5);
    driverCam.setFov(65.5);
    backupCam.setFov(65.5);
    
    ofSetVerticalSync(true);
    
    cam.disableMouseInput();
    
    
    ofEnableSmoothing();
    ofEnableDepthTest();
    bNoAxis = true;
    // setup rudimentary lighting
    //
    initLightingAndMaterials();
    
    mars.loadModel("geo/mars-low-v2.obj");
    mars.setScaleNormalization(false);
    
    boundingBox = meshBounds(mars.getMesh(0));
    
    vector<ofVec3f> point_indices;
    point_indices = mars.getMesh(0).getVertices();
    
    rootNode = Octree(boundingBox);
    rootNode.list_of_indices = point_indices;
    
    createOcTree(boundingBox, rootNode, 1);
    
    p1 = ofPolyline();
    // Text file
    myTextFile = ofFile(filePath);
    if (myTextFile.exists()) {
        loadEditPointsFromFile();
    }
    
    gui.setup();
    gui.add(speed.setup("Speed", 5, 1, 10));
    
}

//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {
    if (bRoverLoaded) {
        Vector3 center = roverBox.center();
        trackCam.setTarget(ofVec3f(ofVec3f(center.x(), center.y(), center.z())));
        driverCam.setPosition(ofVec3f(ofVec3f(center.x(), center.y(), center.z()))+ofVec3f(0,1,0));
        backupCam.setPosition(ofVec3f(ofVec3f(center.x(), center.y(), center.z()))+ofVec3f(0,1,0));
        followCam.lookAt(ofVec3f(0,1,0));
        followCam.setTarget(ofVec3f(ofVec3f(center.x(), center.y(), center.z())));
    }
    if (bTeleport)
        teleportCamera();
}
//--------------------------------------------------------------
void ofApp::draw(){
    ofEasyCam *currentCam;
    if (defaultCamSelected) {
        currentCam = &cam;
    } else if (trackingCamSelected) {
        currentCam = &trackCam;
    } else if (followUpCamSelected) {
        currentCam = &followCam;
    } else if (driverCamSelected) {
        currentCam = &driverCam;
    } else if (backupCamSelected) {
        currentCam = &backupCam;
    }
    
    if (bPointDrag) {
        ofVec3f pointInfo = selectedPoint;
        ofSetColor(ofColor::white);
        ofDrawBitmapString(" x: " + ofToString(pointInfo.x), 10, 10);
        ofDrawBitmapString(" y: " + ofToString(pointInfo.y), 10, 25);
        ofDrawBitmapString(" z: " + ofToString(pointInfo.z), 10, 40);
    }
    
    ofBackground(ofColor::black);
    
    currentCam->begin();
    ofPushMatrix();
    ofEnableDepthTest();
    if (bWireframe) {                    // wireframe mode  (include axis)
        ofDisableLighting();
        ofSetColor(ofColor::slateGray);
        mars.drawWireframe();
        if (bRoverLoaded) {
            rover.drawWireframe();
            if (!bTerrainSelected)
                drawAxis(rover.getPosition());
        }
        if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
    }
    else {
        ofEnableLighting();              // shaded mode
        mars.drawFaces();
        // Change to Rover selected instead of loaded.
        if (bRoverLoaded) {
            rover.drawFaces();
            if (!bTerrainSelected)
                drawAxis(rover.getPosition());
        }
        if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
    }
    
    
    if (bDisplayPoints) {                // display points as an option
        glPointSize(3);
        ofSetColor(ofColor::green);
        mars.drawVertices();
    }
    
    if (bPointSelected) {
        ofSetColor(ofColor::blue);
        ofDrawSphere(selectedPoint, 0.1);
    }
    if (bRoverSelected) {
        ofNoFill();
        ofSetColor(ofColor::white);
        drawBox(roverBox);
    }
    
    if (bMotionPathTool) {
        ofSetColor(ofColor::green);
        if (editPoints.size() > 0) {
            for (int i=0; i<editPoints.size(); i++) {
                ofDrawSphere(editPoints[i], 0.1);
            }
        }
    }
    
    if (bTeleportRover && bRoverLoaded) {
        moveRover(speed);
    }
    
    
    if (bDrawOfPath) {
        if (editPoints.size() < 1)
            return;
        p1.curveTo(editPoints[0]);
        for(int i=1;i<editPoints.size()-1; i++){
            p1.curveTo(editPoints[i].x, editPoints[i].y, editPoints[i].z);
        }
        p1.curveTo(editPoints.back());
        p1.setClosed(false);
        p1.draw();
    }
    
    ofPopMatrix();
    currentCam->end();
        if (bDrawSlider) {
            ofNoFill();
            ofDisableDepthTest();
            //cout << speed << endl;
            gui.draw();
        }

}

//

// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {
    
    ofPushMatrix();
    ofTranslate(location);
    
    ofSetLineWidth(2.0);
    
    // X Axis
    ofSetColor(ofColor(255, 0, 0));
    if (bAxisOnTheRover) {
        ofPoint roverPosition = rover.getPosition();
        ofDrawLine(roverPosition, ofPoint(roverPosition.x+1, roverPosition.y, roverPosition.z));
    }
    else {
        ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
    }
    
    
    // Y Axis
    ofSetColor(ofColor(0, 255, 0));
    if (bAxisOnTheRover) {
        ofPoint roverPosition = rover.getPosition();
        ofDrawLine(roverPosition, ofPoint(roverPosition.x, roverPosition.y + 1, roverPosition.z));
    }
    else {
        ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));
    }
    
    // Z Axis
    ofSetColor(ofColor(0, 0, 255));
    if (bAxisOnTheRover) {
        ofPoint roverPosition = rover.getPosition();
        ofDrawLine(roverPosition, ofPoint(roverPosition.x, roverPosition.y, roverPosition.z + 1));
    }
    else {
        ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));
    }
    
    ofPopMatrix();
}


void ofApp::keyPressed(int key) {
    
    switch (key) {
        case '0':
            defaultCamSelected = true;
            trackingCamSelected = false;
            followUpCamSelected = false;
            driverCamSelected = false;
            cout << "Default Cam Selected" << endl;
            break;
        case '1':
            defaultCamSelected = false;
            trackingCamSelected = true;
            followUpCamSelected = false;
            driverCamSelected = false;
            backupCamSelected = false;
            cout << "cam 1 selected" << endl;
            break;
        case '2':
            defaultCamSelected = false;
            trackingCamSelected = false;
            followUpCamSelected = true;
            driverCamSelected = false;
            backupCamSelected = false;
            cout << "cam 2 selected" << endl;
            break;
        case '3':
            defaultCamSelected = false;
            trackingCamSelected = false;
            followUpCamSelected = false;
            driverCamSelected = true;
            backupCamSelected = false;
            cout << "cam 3 selected" << endl;
            break;
        case '4':
            defaultCamSelected = false;
            trackingCamSelected = false;
            followUpCamSelected = false;
            driverCamSelected = false;
            backupCamSelected = true;
            cout << "cam 4 selected" << endl;
            break;
        case 'a':
            toggleSelectTerrain();
            break;
        case 'b':
            if (bRoverLoaded) {
                    bAxisOnTheRover = !bAxisOnTheRover;
            }
            break;
        case 'C':
        case 'c':
            if (cam.getMouseInputEnabled()) cam.disableMouseInput();
            else cam.enableMouseInput();
            break;
        case 'd':
            if (bMotionPathTool) {
                bDrawOfPath = true;
            }
            
            break;
        case 'F':
        case 'f':
            ofToggleFullscreen();
            break;
        case 'g':
            cout << cam.getPosition() << endl;
            break;
        case 'H':
        case 'h':
            ofToggleDrawSlider();
            break;
        /*
        case 'l':
            ofTogglePointSelection();
            break;
         */
        case 'p':
            if (bTerrainSelected)
                    toggleMotionPathTool();
            break;
        case 'r':
            cam.reset();
            defaultCamSelected = true;
            trackingCamSelected = false;
            followUpCamSelected = false;
            driverCamSelected = false;
            backupCamSelected = false;
            break;
        case 's':
            if (bMotionPathTool) {
                if (!editPoints.empty()) {
                    loadEditPointsToFile();
                }
            } else {
                savePicture();
            }
            break;
        case 't':
            setCameraTarget();
            break;
        case 'u':
            break;
        case 'v':
            togglePointsDisplay();
            break;
        case 'V':
            break;
        case 'w':
            toggleWireframeMode();
            break;
        case OF_KEY_ALT:
            cam.enableMouseInput();
            bAltKeyDown = true;
            break;
        case OF_KEY_CONTROL:
            bCtrlKeyDown = true;
            break;
        case OF_KEY_SHIFT:
            break;
        case OF_KEY_DEL:
        case OF_KEY_BACKSPACE:
            if (bMotionPathTool) {
                if (std::find(editPoints.begin(), editPoints.end(), selectedPoint) != editPoints.end()) {
                    editPoints.erase(std::remove(editPoints.begin(), editPoints.end(), selectedPoint), editPoints.end());
                }
            }
            break;
        case OF_KEY_F1:
            cout << "F1 key pressed" << endl;
            teleportStartPoint = cam.getPosition();
            bTeleport = true;
            break;
        case OF_KEY_F2:
            cout << "F2 key pressed" << endl;
            roverStartPoint = rover.getPosition();
            bTeleportRover = true;
            break;
        default:
            break;
    }
}

void ofApp::toggleWireframeMode() {
    bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
    bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
    bDisplayPoints = !bDisplayPoints;
}

void ofApp::toggleMotionPathTool() {
    cout << bMotionPathTool << endl;
    bMotionPathTool = !bMotionPathTool;
}

void ofApp::ofToggleDrawSlider() {
    bDrawSlider = !bDrawSlider;
}

void ofApp::ofTogglePointSelection() {
    bPointSelection = !bPointSelection;
}

void ofApp::keyReleased(int key) {
    
    switch (key) {
            
        case OF_KEY_ALT:
            cam.disableMouseInput();
            bAltKeyDown = false;
            break;
        case OF_KEY_CONTROL:
            bCtrlKeyDown = false;
            break;
        case OF_KEY_SHIFT:
            break;
        default:
            break;
            
    }
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    //cout << "Mouse Position X:" << x << endl;
    //cout << "Mouse Position Y:" << y << endl;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    if (!bAxisOnTheRover) {
        ofVec3f mouse(mouseX, mouseY);
        ofVec3f rayPoint = cam.screenToWorld(mouse);
        ofVec3f rayDir = rayPoint - cam.getPosition();
        rayDir.normalize();
        Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
                      Vector3(rayDir.x, rayDir.y, rayDir.z));
        bPointSelected = false;
        findThePoint(rootNode, ray);
        
        if (pointInRoverBox(mouse)) {
            cout << "Rover Selected" << endl;
            bRoverSelected = true;
            bTerrainSelected = false;
            Vector3 v = roverBox.center();
            ofVec3f center = ofVec3f(v.x(), v.y(), v.z());
            cam.setTarget(center);
        } else {
            cout << "Rover Not Selected" << endl;
            if (bMotionPathTool) {
                if (std::find(editPoints.begin(), editPoints.end(), selectedPoint) != editPoints.end()) {
                    cout << "Point Exists in the Edit point Vectors" << endl;
                    bDraggedPoint = std::find(editPoints.begin(), editPoints.end(), selectedPoint) - editPoints.begin();
                    bPointDrag = true;
                } else {
                    editPoints.push_back(selectedPoint);
                }
            }
            bTerrainSelected = true;
            bRoverSelected = false;
        }
    } else {
        if (bAxisOnTheRover) {
            bRoverMove = true;
        }
    }
}

//draw a box from a "Box" class
//
void ofApp::drawBox(const Box &box) {
    Vector3 min = box.parameters[0];
    Vector3 max = box.parameters[1];
    Vector3 size = max - min;
    Vector3 center = size / 2 + min;
    ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
    //ofVec3f p = rover.getPosition();
    float w = size.x();
    float h = size.y();
    float d = size.z();
    //cout << "P:" << p << endl;
    ofDrawBox(p, w, h, d);
}

// return a Mesh Bounding Box for the entire Mesh
//
Box ofApp::meshBounds(const ofMesh & mesh) {
    int n = mesh.getNumVertices();
    ofVec3f v = mesh.getVertex(0);
    ofVec3f max = v;
    ofVec3f min = v;
    for (int i = 1; i < n; i++) {
        ofVec3f v = mesh.getVertex(i);
        
        if (v.x > max.x) max.x = v.x;
        else if (v.x < min.x) min.x = v.x;
        
        if (v.y > max.y) max.y = v.y;
        else if (v.y < min.y) min.y = v.y;
        
        if (v.z > max.z) max.z = v.z;
        else if (v.z < min.z) min.z = v.z;
    }
    return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

//  Subdivide a Box into eight(8) equal size boxes, return them in boxList;
//
void ofApp::subDivideBox8(const Box &box, vector<Box> & boxList) {
    Vector3 min = box.parameters[0];
    Vector3 max = box.parameters[1];
    Vector3 size = max - min;
    Vector3 center = size / 2 + min;
    float xdist = (max.x() - min.x()) / 2;
    float ydist = (max.y() - min.y()) / 2;
    float zdist = (max.z() - min.z()) / 2;
    Vector3 h = Vector3(0, ydist, 0);
    
    //  generate ground floor
    //
    Box b[8];
    b[0] = Box(min, center);
    b[1] = Box(b[0].min() + Vector3(xdist, 0, 0), b[0].max() + Vector3(xdist, 0, 0));
    b[2] = Box(b[1].min() + Vector3(0, 0, zdist), b[1].max() + Vector3(0, 0, zdist));
    b[3] = Box(b[2].min() + Vector3(-xdist, 0, 0), b[2].max() + Vector3(-xdist, 0, 0));
    
    boxList.clear();
    for (int i = 0; i < 4; i++)
        boxList.push_back(b[i]);
    
    // generate second story
    //
    for (int i = 4; i < 8; i++) {
        b[i] = Box(b[i - 4].min() + h, b[i - 4].max() + h);
        boxList.push_back(b[i]);
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    if (bRoverMove) {
        ofVec3f intersection = rover.getPosition();
        //Get the plane normal
        ofVec3f planeNormal = cam.getZAxis().normalize();
        //Check if the intersection occurs
        bool doesIntersect = mouseIntersectPlane(intersection, planeNormal, intersection);
        //Set the rover position according to the intersections
        if (doesIntersect) {
            if (bNoAxis) {
                rover.setPosition(intersection.x, intersection.y, intersection.z);
            }
        }
    }
    
    if (bPointDrag) {
        editPoints[bDraggedPoint] = selectedPoint;
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    bRoverMove = false;
    bPointDrag = false;
}


//
//  Select Target Point on Terrain by comparing distance of mouse to
//  vertice points projected onto screenspace.
//  if a point is selected, return true, else return false;
//
bool ofApp::doPointSelection() {
    
    ofMesh mesh = mars.getMesh(0);
    int n = mesh.getNumVertices();
    float nearestDistance = 0;
    int nearestIndex = 0;
    
    bPointSelected = false;
    
    ofVec2f mouse(mouseX, mouseY);
    vector<ofVec3f> selection;
    
    // We check through the mesh vertices to see which ones
    // are "close" to the mouse point in screen space.  If we find
    // points that are close, we store them in a vector (dynamic array)
    //
    for (int i = 0; i < n; i++) {
        ofVec3f vert = mesh.getVertex(i);
        ofVec3f posScreen = cam.worldToScreen(vert);
        float distance = posScreen.distance(mouse);
        if (distance < selectionRange) {
            selection.push_back(vert);
            bPointSelected = true;
        }
    }
    
    //  if we found selected points, we need to determine which
    //  one is closest to the eye (camera). That one is our selected target.
    //
    if (bPointSelected) {
        float distance = 0;
        for (int i = 0; i < selection.size(); i++) {
            ofVec3f point =  cam.worldToCamera(selection[i]);
            
            // In camera space, the camera is at (0,0,0), so distance from
            // the camera is simply the length of the point vector
            //
            float curDist = point.length();
            
            if (i == 0 || curDist < distance) {
                distance = curDist;
                selectedPoint = selection[i];
            }
        }
    }
    return bPointSelected;
}

// Set the camera to use the selected point as it's new target
//
void ofApp::setCameraTarget() {
    if (bAxisOnTheRover) {
        cam.setTarget(rover.getPosition());
    }
    else {
        cam.setTarget(selectedPoint);
    }
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



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {
    
    static float ambient[] =
    { .5f, .5f, .5, 1.0f };
    static float diffuse[] =
    { 1.0f, 1.0f, 1.0f, 1.0f };
    
    static float position[] =
    {5.0, 5.0, 5.0, 0.0 };
    
    static float lmodel_ambient[] =
    { 1.0f, 1.0f, 1.0f, 1.0f };
    
    static float lmodel_twoside[] =
    { GL_TRUE };
    
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, position);
    
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //    glEnable(GL_LIGHT1);
    glShadeModel(GL_SMOOTH);
}

void ofApp::savePicture() {
    ofImage picture;
    picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
    picture.save("screenshot.png");
    cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {
    
    ofVec3f point;
    mouseIntersectPlane(ofVec3f(0, 0, 0), cam.getZAxis(), point);
    
    if (rover.loadModel(dragInfo.files[0])) {
        rover.setScaleNormalization(false);
        rover.setScale(.005, .005, .005);
        rover.setPosition(point.x, point.y, point.z);
        cout << "Point" << point << endl;
        
        ofVec3f min = rover.getSceneMin();
        ofVec3f max = rover.getSceneMax();
        roverBox = Box(Vector3(min.x*0.005+point.x, min.y*0.005+point.y, min.z*0.005+point.z),
                       Vector3(max.x*0.005+point.x, max.y*0.005+point.y, max.z*0.005+point.z));
        
        bRoverLoaded = true;
        bRoverSelected = true;
        bTerrainSelected = false;
        
        
        trackCam.setPosition(10, 3, 10);
        trackCam.lookAt(ofVec3f(0,1,0));
        
        Vector3 center = roverBox.center();
        
        trackCam.setTarget(ofVec3f(center.x(), center.y(), center.z()));
        driverCam.setPosition(ofVec3f(center.x(), center.y(), center.z())+ofVec3f(0,1,0));
        backupCam.setPosition(ofVec3f(center.x(), center.y(), center.z())+ofVec3f(0,1,0));
        
        followCam.setPosition(ofVec3f(center.x(), center.y(), center.z())-ofVec3f(0,-3,5));
        followCam.lookAt(ofVec3f(0,1,0));
        followCam.setTarget(ofVec3f(center.x(), center.y(), center.z()));
        
        
        
    }
    else cout << "Error: Can't load model" << dragInfo.files[0] << endl;
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
    ofVec2f mouse(mouseX, mouseY);
    ofVec3f rayPoint = cam.screenToWorld(mouse);
    ofVec3f rayDir = rayPoint - cam.getPosition();
    rayDir.normalize();
    return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}


void ofApp::createOcTree(Box &b, Octree &root, int depth)
{
    if (root.list_of_indices.size() == 1) return;
    vector<Box> boxList;
    subDivideBox8(b, boxList);
    
    for (int i = 0; i < boxList.size(); i++) {
        vector<ofVec3f> list;
        for (int j=0; j< root.list_of_indices.size(); j++) {
            Vector3 temp(root.list_of_indices[j].x, root.list_of_indices[j].y, root.list_of_indices[j].z);
            if (temp <= boxList[i].max() && boxList[i].min() < temp) {
                list.push_back(root.list_of_indices[j]);
            }
        }
        
        if (!list.empty()) {
            Octree child = Octree(boxList[i]);
            child.list_of_indices = list;
            switch (depth) {
                case 1:
                    level1.push_back(boxList[i]);
                    break;
                case 2:
                    level2.push_back(boxList[i]);
                    break;
                case 3:
                    level3.push_back(boxList[i]);
                    break;
                case 4:
                    level4.push_back(boxList[i]);
                    break;
                case 5:
                    level5.push_back(boxList[i]);
                    break;
                default:
                    break;
            }
            createOcTree(boxList[i], child, depth+1);
            root.children.push_back(child);
        }
    }
}

void ofApp::findThePoint(Octree &rootNode, Ray &ray)
{
    if (rootNode.currentbox.intersect(ray, -100, 100) && rootNode.list_of_indices.size() == 1) {
        bPointSelected = true;
        selectedPoint = rootNode.list_of_indices[0];
        return;
    }
    
    for (int i=0; i<rootNode.children.size(); i++) {
        if (rootNode.children[i].currentbox.intersect(ray, -100, 100)) {
            findThePoint(rootNode.children[i], ray);
        }
     }
}

bool ofApp::pointInRoverBox (ofVec3f point) {
    if (bRoverLoaded) {
        ofVec3f mouse(mouseX, mouseY);
        ofVec3f rayPoint = cam.screenToWorld(mouse);
        ofVec3f rayDir = rayPoint - cam.getPosition();
        rayDir.normalize();
        Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
                      Vector3(rayDir.x, rayDir.y, rayDir.z));
        if (roverBox.intersect(ray, -100, 100)) {
            cout << "Intersects" << endl;
            return true;
        } else {
            cout << "Does not Intersect" << endl;
            return false;
        }
    }
}

// Teleport camera to selected point.
void ofApp::teleportCamera() {
    ofVec3f currentCamPosition = cam.getPosition();
    
    //distance covered till now
    double percentCompelete = currentCamPosition.distance(teleportStartPoint) / teleportStartPoint.distance(selectedPoint);
    
    //calculating step using sin
    double step = sin(percentCompelete*PI);
    if (step == 0) {
        step = 0.001;
    }
    
    ofVec3f dir = (selectedPoint - currentCamPosition).normalize() * step;
    
    cam.setPosition(currentCamPosition + dir);
    
    if (percentCompelete > 0.95) {
        bTeleport = false;
    }
    
}

// Teleport rover
void ofApp::moveRover(int speed) {
    
    ofVec3f direction;
    ofVec3f roverPosition = rover.getPosition();
    ofVec3f nextPoint = p1.getPointAtPercent(0.001*speed);
    
    roverBox = getBoxAtPoint(nextPoint);
    Vector3 center = roverBox.center();
    ofVec3f updatedCenter = ofVec3f(center.x(), center.y(), center.z());
    rover.setPosition(nextPoint.x, nextPoint.y, nextPoint.z);
    direction = roverPosition-nextPoint;
    float m = ofVec3f(0,0,-1).normalize().dot(direction.normalize());
    float angle =  acos (m) * 180.0 / PI;
    rover.setRotation(0, angle, 0,1, 0);
    followCam.setPosition(updatedCenter+direction.normalize()*5+ofVec3f(0,1,0));
    driverCam.lookAt(updatedCenter-direction.normalize()*5);
    backupCam.lookAt(updatedCenter+direction.normalize()*5);
}


void ofApp::loadEditPointsToFile() {
    myTextFile.open(filePath, ofFile::Append);
    for (int i=0; i<editPoints.size(); i++) {
        string editp = ofToString(editPoints[i]);
        myTextFile << editp << endl;
    }
}

void ofApp::loadEditPointsFromFile() {
    ofBuffer buffer(myTextFile);
    while (!buffer.isLastLine()) {
        string line = buffer.getNextLine();
        ofVec3f editp = ofFromString<ofVec3f>(line);
        editPoints.push_back(editp);
    }
}

Box ofApp::getBoxAtPoint(ofVec3f point) {
    ofVec3f min = rover.getSceneMin();
    ofVec3f max = rover.getSceneMax();
    
    Vector3 Min = Vector3(min.x*0.005+point.x, min.y*0.005+point.y, min.z*0.005+point.z);
    Vector3 Max = Vector3(max.x*0.005+point.x, max.y*0.005+point.y, max.z*0.005+point.z);
    return Box(Min, Max);
}


