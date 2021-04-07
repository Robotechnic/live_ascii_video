#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofLog()<<"SETUP CAMERA";
    //setup camera
    grabber.setDeviceID(0);
    grabber.setDesiredFrameRate(60);
    grabber.setPixelFormat(ofPixelFormat::OF_PIXELS_GRAY);
    grabber.initGrabber(640,360);

    //setup font
    font.load("CONSOLA.ttf",10);

    ofLog()<<"SETUP GUI";
    //setup gui
    this->initScreen = new ofxDatGui(0,0);
    this->initScreen->setAutoDraw(false);
    this->initScreen->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    this->initScreen->addHeader("Live ascii video",false);

    this->initScreen->update();
    this->text = this->initScreen->addTextInput("Printables:");
    this->text->setTextUpperCase(false);
    this->text->setLabelAlignment(ofxDatGuiAlignment::LEFT);
    createCam = this->initScreen->addToggle("Create virtual camera",true);
    this->initScreen->update();
    this->initScreen->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    this->initScreen->addButton("OK");
    this->initScreen->update();
    this->initScreen->onButtonEvent(this, &ofApp::validateGui);
    this->initScreen->setPosition(ofGetWidth()/2-this->initScreen->getWidth()/2,ofGetHeight()/2-this->initScreen->getHeight()/2);


    this->globalScreen = new ofxDatGui(0,0);
    this->globalScreen->setAutoDraw(false);
    this->globalScreen->addHeader("Setings");
    this->globalScreen->addFRM();
    this->seuilSlider = this->globalScreen->addSlider("Seuil",0,255);



    seuilSlider->bind(seuil);
}

//--------------------------------------------------------------
void ofApp::update(){
    if (createCam->getChecked() && panel == 1){
        ofPixels p;

        ascii.readToPixels(p);

        virtualcam.update(p);
    }
    if (panel == 0){
        this->initScreen->update();
    } else if (panel == 1){
        this->globalScreen->update();
    }

    updateCam();
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (panel == 0){
        this->initScreen->draw();
    }
    else if (panel == 1){
        ofBackground(0);
        ofSetColor(255);



        ascii.draw(0,0);

        this->globalScreen->draw();
    }


}

void ofApp::validateGui(ofxDatGuiButtonEvent e){
    panel = 1;
    string caracter = text->getText();
    for (char c: caracter){
        ofFill();
        ofSetColor(255);
        letterRect = font.getGlyphBBox();
        letterRect.setX(ofGetWidth()/2);
        letterRect.setY(ofGetHeight()/2-letterRect.height/2);
        ofDrawRectangle(letterRect);
        ofSetColor(0);
        font.drawString(string(1,c),ofGetWidth()/2,ofGetHeight()/2);
        ofLog()<<"Letter "<<string(1,c)<<" :";
        sortCaracter[setupAsciiCaracter(letterRect)] = string(1,c);
    }

    for (map<float,string>::iterator i = sortCaracter.begin(); i != sortCaracter.end(); i++){
         caracterList += i->second;
    }
    ofLog()<<caracterList;

    if (createCam->getChecked()){
        virtualcam.setup(ofGetWidth(),ofGetHeight(),2);
    }

    ascii.allocate(ofGetWidth(),ofGetHeight());
}

int ofApp::setupAsciiCaracter(ofRectangle letterRect){
    //ofLog()<<letterRect;
    img.grabScreen(letterRect.x,letterRect.y,letterRect.width,letterRect.height);
    //img.save("image.png");


    float totalPix = 0,letterPix = 0;

    for (int x = 0; x<letterRect.width;x++){
        for (int y = 0; y<letterRect.height;y++){
            ofColor c = img.getColor(x,y);
            if (c != ofColor::white){
                letterPix += 1;
            }
            totalPix += 1;
        }
    }
    ofLog()<<letterPix;
    return letterPix;
}

void ofApp::updateCam(){
    grabber.update();

    float ratiowh = grabber.getWidth()/grabber.getHeight();
    float ratiohw = grabber.getHeight()/grabber.getWidth();
    float left = 0;
    float top = 0;

    img.setFromPixels(grabber.getPixels());

    if (ofGetHeight()*ratiowh < ofGetWidth()){
        img.resize(ofGetHeight()*ratiowh,ofGetHeight());
        left = (ofGetWidth()-ofGetHeight()*ratiowh)/2;
    } else {
        img.resize(ofGetWidth(),ofGetWidth()*ratiohw);
        top = (ofGetHeight()-ofGetWidth()*ratiohw)/2;
    }

    ofPixels pixs = img.getPixels();

    int height = font.stringHeight("p");
    int width = font.stringWidth("p");

    float gray = 0;

    float caracter;
    //for each "cell"
    ascii.begin();
    ofClear(0);

    for (int x = 0; x < (int)img.getWidth(); x += width)
        for (int y = 0; y <(int)img.getHeight(); y += height){
            //get each pixel of cell and calculate the average
            gray = pixs.getColor(x,y).r;
            for (int x1 = x; x1<x+width; x1++)
                for (int y1 = y; y1<y+height; y1++){
                    gray = (gray+pixs.getColor(x1,y1).r)/2;
                }

            if (gray < seuil){
                caracter = 0;
            } else {
               caracter = gray/(255-seuil)*float(caracterList.length());
            }

            //ofLog()<<caracter;
            font.drawString(string(1,caracterList[caracter]),x+left,y+top);
        }
    ascii.end();
}

////--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == OF_KEY_ESC){
        panel = 0;
    }
}

////--------------------------------------------------------------
//void ofApp::keyReleased(int key){

//}

////--------------------------------------------------------------
//void ofApp::mouseMoved(int x, int y ){

//}

////--------------------------------------------------------------
//void ofApp::mouseDragged(int x, int y, int button){

//}

////--------------------------------------------------------------
//void ofApp::mousePressed(int x, int y, int button){

//}

////--------------------------------------------------------------
//void ofApp::mouseReleased(int x, int y, int button){

//}

////--------------------------------------------------------------
//void ofApp::mouseEntered(int x, int y){

//}

////--------------------------------------------------------------
//void ofApp::mouseExited(int x, int y){

//}

////--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    if (panel == 1){
        if (!resize){
            ofSetWindowShape(ascii.getWidth(),ascii.getHeight());
            resize = true;
        } else {
            resize = false;
        }
    }
}

////--------------------------------------------------------------
//void ofApp::gotMessage(ofMessage msg){

//}

////--------------------------------------------------------------
//void ofApp::dragEvent(ofDragInfo dragInfo){

//}
