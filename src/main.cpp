#include "ofAppGLFWWindow.h"
#include "ofMain.h"
#include "ofGLProgrammableRenderer.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGui.h"

class xApp : public ofBaseApp{

	public:
	    ofxPanel gui;
	    ofParameter<ofVec3f>  xPos,offset;
        ofxIntSlider contatore;
        ofxVec3Slider dist;
        ofxToggle drawBox;
        ofxButton clearfbo;

        ofxLabel label;
        ofShader shader;
        ofEasyCam camera;
        ofImage img;
        ofFbo fbo;
        bool bAnimate;
        bool bAnimateMouse;

        float animationPosition;

        int w,h,counter;
        int NUM_MESH  = 0;

        vector<ofVboMesh> mesh;
        ofxAssimpModelLoader model;

		void setup() {
            ofSetFrameRate(60);
		    ofSetVerticalSync(true);
		    gui.setup("","",10,255);
            gui.add(xPos.set("grid",ofVec3f(10.,0.,10.),ofVec3f(-55.,-55.,-55.),ofVec3f(55.,55.,55.)));
            gui.add(offset.set("offset",ofVec3f(0.,0.,0.),ofVec3f(-2.,-2.,-2.),ofVec3f(2.,2.,2.)));
            gui.add(contatore.setup("counter",10,0,100));
            gui.add(label.setup("num model",""));
            gui.add(drawBox.setup("draw box",false));
            gui.add(clearfbo.setup("clear fbo",false));
            gui.add(dist.setup("dist",ofVec3f(0,0,0),ofVec3f(-0.999,-0.999,-0.999),ofVec3f(0.999,0.999,0.999)));

			bAnimate = false;
			bAnimateMouse = false;
			animationPosition = 0;
			model.loadModel("model/avatar.dae");
			model.setPosition( ofGetWidth() * 0.5, (float)ofGetHeight() * 0.75 , 0 );
			model.setLoopStateForAllAnimations(OF_LOOP_NORMAL);

			model.playAllAnimations();

			if(!bAnimate) {
			        model.setPausedForAllAnimations(true);
			}

			w=ofGetScreenWidth();
			h=ofGetScreenHeight();
		    shader.load( "shaders/instanced.vert", "shaders/instanced.frag" );
			camera.setFarClip(100000);
			camera.setNearClip(1.);


			for( int i = 0; i < model.getNumMeshes(); i++ ) {
				ofVboMesh p = model.getMesh(i);
				mesh.push_back(p);
			}

			ofDisableArbTex();
			img.loadImage("texture.jpg");
			fbo.allocate(250,250);
			fbo.begin();
			ofClear(0,0,0,255);
			fbo.end();

			xPos.addListener(this, &xApp::position);
		}

        void position(ofVec3f &val){
            //fbo.allocate(val.x,val.y);
        }

		void update() {
			ofSetWindowTitle(ofToString(ofGetFrameRate()));
			model.update();
		   	animationPosition = ofMap(counter,0,55,0.0,1.0);
			counter++;
			if(counter>95)
				counter=0;
			model.setPositionForAllAnimations(animationPosition);
			for( int i = 0; i < mesh.size(); i++ )
                    mesh[i] = model.getCurrentAnimatedMesh(NUM_MESH);
            if(clearfbo){
                fbo.begin();
                ofClear(0,0,0,255);
                fbo.end();
            }
		}

		void draw() {
			ofBackgroundGradient( ofColor(210), ofColor(10), OF_GRADIENT_BAR);

            fbo.draw(0,0);

			label = ofToString(contatore*contatore);
			ofEnableDepthTest();
			camera.begin();
			shader.begin();
			shader.setUniformTexture("tex0",    fbo.getTextureReference(),0);
			shader.setUniform4f("globalColor",  0.6,0.5,8.3,1.0);
			shader.setUniform2f("offset",       offset->x,offset->y);
            shader.setUniform3f("xPos",         xPos->x,xPos->y,xPos->z);
            shader.setUniform1f("counter",      contatore);
            shader.setUniform3f("dist",         dist->x,dist->y,dist->z);

            if(drawBox){
                ofBoxPrimitive box;
                box.set(0.8);
                mesh[0]=box.getMesh();
                for( int i = 0; i < mesh.size(); i++ ) {
                    mesh[i].drawInstanced(OF_MESH_FILL, contatore*contatore);
                }
            }else{
                for( int i = 0; i < mesh.size(); i++ ) {
                    mesh[i].drawInstanced(OF_MESH_FILL, contatore*contatore);
                }
            }

			shader.end();
			ofDisableDepthTest();
			camera.end();
			gui.draw();
		}

		void keyPressed(int key) {
			if(key == 'f')
				ofToggleFullscreen();
		}

        void mouseMoved(int x, int y){
            fbo.begin();
                ofSetColor(0,255,0);
                ofRect(x,y,1,1);
            fbo.end();
        }
};

int main(){
       ofAppGLFWWindow window;
       //ofSetupOpenGL(&window, 1024,768, OF_WINDOW);
       ofPtr<ofBaseRenderer> renderer(new ofGLProgrammableRenderer(false));
       ofSetCurrentRenderer(renderer, false);
       ofSetupOpenGL(&window,1024,768,OF_WINDOW);
       ofRunApp( new xApp());
}
