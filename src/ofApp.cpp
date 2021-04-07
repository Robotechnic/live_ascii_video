#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //setup camera
    grabber.setDeviceID(0);
    grabber.setDesiredFrameRate(60);
    grabber.setPixelFormat(ofPixelFormat::OF_PIXELS_GRAY);
    grabber.initGrabber(640,360);

    //setup font
    font.load("CONSOLA.ttf",10);

    //setup gui
    gui.setup("Mise en place");
    gui.setPosition(ofGetWidth()/4,ofGetHeight()/4);
    gui.setWidthElements(ofGetWidth()/2);

    text.setSize(ofGetWidth()/2,ofGetHeight()/2);


    gui.add(label.setup("carateresLabel","Caractères a utiliser :"));
    gui.add(text.setup("caracteres",""));

    gui.add(camLabel.setup("camLabel","Créer une caméra virtuelle :"));
    gui.add(createCam.setup("Créer une caméra virtuelle"));

    valider.addListener(this,&ofApp::validateGui);
    gui.add(valider.setup("Valider"));
    panel = 0;

    sliderGroup.setup("Seuil de tolérance");
    sliderGroup.setPosition(0,0);
    sliderGroup.add(seuil.setup("Tolérance",0,0,255));

}

//--------------------------------------------------------------
void ofApp::update(){
    if (createCam && panel == 1){
        ofPixels p;

        ascii.readToPixels(p);

        virtualcam.update(p);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (panel == 0)
        gui.draw();
    else if (panel == 1){
        ofBackground(0);
        drawCam();


        ascii.draw(0,0);
        sliderGroup.draw();
    }


}


void ofApp::drawCam(){
    grabber.update();

    float ratiowh = grabber.getWidth()/grabber.getHeight();
    float ratiohw = grabber.getHeight()/grabber.getWidth();
    float left = 0;
    float top = 0;
    ofPixels pixs =  grabber.getPixels();


    if (ofGetHeight()*ratiowh < ofGetWidth()){
        pixs.resize(ofGetHeight()*ratiowh,ofGetHeight());
        left = (ofGetWidth()-ofGetHeight()*ratiowh)/2;
    } else {
        pixs.resize(ofGetWidth(),ofGetWidth()*ratiohw);
        top = (ofGetHeight()-ofGetWidth()*ratiohw)/2;
    }

    int height = font.stringHeight("p");
    int width = font.stringWidth("p");

    float gray = 0;

    float caracter;
    //for each "cell"
    ascii.begin();
    ofClear(0);
    ofNoFill();
    ofSetColor(255);
    for (int x = 0; x < img.getWidth(); x += width)
        for (int y = 0; y < img.getHeight(); y += height){
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

void ofApp::validateGui(){
    text.saveToFile("caracter.xml");
    panel = 1;
    string caracter = text.getParameter().toString();
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

    if (createCam){
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
