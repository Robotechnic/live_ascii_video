#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofLog()<<"SETUP CAMERA";
    //setup camera
    grabber.setDeviceID(0);
    grabber.setDesiredFrameRate(60);
    grabber.setPixelFormat(ofPixelFormat::OF_PIXELS_RGB);
    grabber.initGrabber(640,360);


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
    this->printablesSize = this->globalScreen->addSlider("Size",2,30);

    this->printablesSize->bind(fontSize);
    printablesSize->onSliderEvent(this,&ofApp::sliderEvent);

    this->printablesSize->setValue(10);
    this->font.load("CONSOLA.ttf",10);

    seuilSlider->bind(seuil);

    //add renders
    renders["Ascii art"] = bind(&ofApp::ascii,this,placeholders::_1,placeholders::_2,placeholders::_3);
    renders["Border"] = bind(&ofApp::border,this,placeholders::_1,placeholders::_2,placeholders::_3);
    renders["Raw"] = bind(&ofApp::raw,this,placeholders::_1,placeholders::_2,placeholders::_3);

    //then create gui
    globalScreen->onToggleEvent(this,&ofApp::toggleEvent);
    ofxDatGuiToggle *t;
    for (map<string,function<void(ofPixels,int,int)>>::iterator it = renders.begin(); it !=renders.end(); it++){
        t = globalScreen->addToggle(it->first,it == renders.begin() ? true : false);
    }
    this->renderFunction = renders["Raw"];

}

//--------------------------------------------------------------
void ofApp::update(){
    if (createCam->getChecked() && panel == 1){
        ofPixels p;

        render.readToPixels(p);

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
        ofClear(0);
        ofSetColor(255);



        render.draw(0,0);

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

    render.allocate(ofGetWidth(),ofGetHeight());
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

    if (this->renderFunction != NULL)
        this->renderFunction(pixs,top,left);
}

void ofApp::sliderEvent(ofxDatGuiSliderEvent e){
    this->font.load("CONSOLA.ttf",fontSize);
}


void ofApp::ascii(ofPixels pixs, int left, int top){
    pixs.setImageType(OF_IMAGE_GRAYSCALE);
    int height = font.stringHeight("p");
    int width = font.stringWidth("p");

    float gray = 0;

    float caracter;
    //for each "cell"
    render.begin();
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
    render.end();
}

bool ofApp::compareColor(ofColor color1, ofColor color2, float seuil){
    float r1 = color1.r, r2 = color2.r;
    float g1 = color1.g, g2 = color2.g;
    float b1 = color1.b, b2 = color2.b;

    float rMax = r1 > r2 ? r1 : r2;
    float rMin = r1 > r2 ? r2 : r1;

    float gMax = g1 > g2 ? g1 : g2;
    float gMin = g1 > g2 ? g2 : g1;

    float bMax = b1 > b2 ? b1 : b2;
    float bMin = b1 > b2 ? b2 : b1;

    float rDiff = (rMax-rMin)/rMax*100;
    float gDiff = (gMax-gMin)/gMax*100;
    float bDiff = (bMax-bMin)/bMax*100;

//    ofLog()<<rDiff<<" "<<gDiff<<" "<<bDiff;

    if ((rDiff+gDiff+bDiff)/3>seuil){
        return true;
    }

    return false;
}

void ofApp::border(ofPixels pixs, int left, int top){
    ofColor last = ofColor::white;

    render.begin();
    ofClear(0);

    for (int x = 0; x < grabber.getWidth(); ++x) {
        for (int y = 0; y < grabber.getHeight(); ++y){
            if (compareColor(last,pixs.getColor(x,y),seuil)){
                ofColor(0);
                ofDrawRectangle(x,y,1,1);
                last = pixs.getColor(x,y);
            }
        }
    }
    render.end();
}


void ofApp::raw(ofPixels pixs, int left, int top){
    render.begin();
    img.setFromPixels(pixs);
    img.draw(left,top);
    render.end();
}

void ofApp::toggleEvent(ofxDatGuiToggleEvent e){
    if (this->renders.at(e.target->getLabel()) && e.target->getChecked()){ //check if this is a valid toggle
        renderFunction = renders[e.target->getLabel()]; //change render function

        //uncheck all others buttons
        for (map<string,function<void(ofPixels,int,int)>>::iterator it = renders.begin(); it !=renders.end(); it++){
            if (e.target->getLabel() != it->first)
                this->globalScreen->getToggle(it->first)->setChecked(false);
        }
    }
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
            ofSetWindowShape(render.getWidth(),render.getHeight());
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
