#pragma once

//#include "ofMain.h"
#include <ofBaseApp.h>
#include <ofColor.h>
#include <ofVideoGrabber.h>
#include <ofTrueTypeFont.h>
#include <ofFbo.h>
#include <ofxDatGui.h>
#include <ofGstUtils.h>
#include <functional>

#include "ofxV4l2loopback.h"


#include <map>
using namespace std;


class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
//    void keyReleased(int key);
//    void mouseMoved(int x, int y );
//    void mouseDragged(int x, int y, int button);
//    void mousePressed(int x, int y, int button);
//    void mouseReleased(int x, int y, int button);
//    void mouseEntered(int x, int y);
//    void mouseExited(int x, int y);
    void windowResized(int w, int h);
//    void dragEvent(ofDragInfo dragInfo);
//    void gotMessage(ofMessage msg);

private:
    void validateGui(ofxDatGuiButtonEvent e);
    int setupAsciiCaracter(ofRectangle letterRect);

    void updateCam();

    ofVideoGrabber grabber;
    ofTrueTypeFont font;

    //setup gui
    ofxDatGui *initScreen;


    ofxDatGuiTextInput *text;
    ofxDatGuiToggle *createCam;

    ofxDatGui *globalScreen;
    ofxDatGuiSlider *seuilSlider;
    ofxDatGuiSlider *printablesSize;
    float seuil;

//    ofxGuiGroup gui;
//    ofxLabel label,camLabel;
//    ofxTextField text;
//    ofxToggle createCam;
//    ofxButton valider;

//    ofxGuiGroup sliderGroup;
//    ofxSlider<int> seuil;

    //ascii art variable and functions
    string caracterList;
    map <float,string> sortCaracter;
    ofRectangle letterRect;
    ofImage img;

    void sliderEvent(ofxDatGuiSliderEvent e);
    int fontSize;

    //virtual cam
    ofxV4l2loopback virtualcam;

    //renderType
    map<string,function<void(ofPixels,int,int)>> renders;
    function<void(ofPixels,int,int)> renderFunction;
    void toggleEvent(ofxDatGuiToggleEvent e);

    void ascii(ofPixels pixs, int left, int top);

    void border(ofPixels pixs, int left, int top);
    bool compareColor(ofColor color1, ofColor color2, float seuil);

    void raw(ofPixels pixs, int left, int top);

    //other
    bool test;
    int panel;
    ofFbo render;
    bool resize;
};
