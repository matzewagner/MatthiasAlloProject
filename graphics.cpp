#include "Cuttlebone/Cuttlebone.hpp"
#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
#include <vector>    // vector
#include <string>    // memcpy

using namespace al;
using namespace std;

#include "state.hpp"

struct Gra : OmniStereoGraphicsRenderer {
  cuttlebone::Taker<State, 1400> taker;
  State* state;

  Light light;
  Material material;
  unsigned short g_ModelIndex;
  double timeFlag = 0;

  Mesh freqEnv[N_TRACKS];
  Mesh playHead;
  Mesh box[N_TRACKS];
  Mesh ball[N_TRACKS];
  vector<float> out[N_TRACKS];

  float offColor[N_TRACKS], colorScaler[N_TRACKS], sample[N_TRACKS];


  Vec3f playHeadPosition;
  float boxWidth[NUM_MODELS][N_TRACKS];
  
  Color agentColor[N_TRACKS];
  Color playHeadColor;
  Color selectedColor;

  Gra() {
    state = new State;
    memset(state, 0, sizeof(State)); // set everything to 0
    for (int i=0; i<N_TRACKS; ++i) {
    	freqEnv[i].primitive(Graphics::LINE_STRIP);
      box[i].primitive(Graphics::LINES);
      ball[i].primitive(Graphics::TRIANGLES);
      addSphere(ball[i], 1, 64, 64);
      ball[i].generateNormals();
    }
    
    playHead.primitive(Graphics::LINE_STRIP);
    playHead.vertex(0, 0.05, 0);
    playHead.vertex(0, -0.05, 0);
    playHeadPosition = Vec3f(0, 0, 0);
    playHeadColor = RGB(0, 0.75, 1.0);

    selectedColor = RGB(0.5, 0, 0.5);

    // for testing, set each lighting mode to a different RGB max
    // and look what effect each one has
    light.ambient(Color(0.25, 0.25, 0.25, 0.0));
    light.diffuse(Color(0.25, 0.25, 0.25, 1.0));
    light.specular(Color(0.25, 0.25, 0.25, 1.0));
  }

  virtual ~Gra() {}

  virtual void onAnimate(double dt) {
    static float count = 0;
    count += 1;
        	
          if (timeFlag < 0.5) {

      			for (int i=0; i < NUM_MODELS; ++i) {
          		for (int j=0; j < N_TRACKS; ++j) {
          			for (int k=0; k < state->g_Models[i].g_Tracks[j].nSamples/441.0; ++k) {
          				float xVert = k*0.01;
          				freqEnv[j].vertex(xVert, 0, 0);
          			}
                    boxWidth[i][j] = state->g_Models[i].g_Tracks[j].nSamples/44100.0;
                    float boxHeight = 0.1;
                    box[j].vertex(0, boxHeight, 0);
                    box[j].vertex(boxWidth[i][j], boxHeight, 0);
                    box[j].vertex(boxWidth[i][j], boxHeight, 0);
                    box[j].vertex(boxWidth[i][j], -boxHeight, 0);
                    box[j].vertex(boxWidth[i][j], -boxHeight, 0);
                    box[j].vertex(0, -boxHeight, 0);
                    box[j].vertex(0, -boxHeight, 0);
                    box[j].vertex(0, boxHeight, 0);
          		}
          	}
        		cout << "Built graphics agents" << endl;
        	}

            static cuttlebone::Stats fps("onAnimate()");
            fps(dt);
            while (taker.get(*state));
            // int popCount = taker.get(*state);
            pose = state->pose;
            // light.pos(state->lightPos + Vec3f(sinf(count * 0.01), 0.0, 0.0));
            g_ModelIndex = state->modelIndex;

            // cout << "Index: " << g_ModelIndex << endl;
            
            for(int i=0; i<state->g_Models[g_ModelIndex].numTracks; ++i) {
                  offColor[i] = state->g_Models[g_ModelIndex].g_Tracks[i].offColor;

                  if (state->g_Models[g_ModelIndex].g_Tracks[i].play) 
                  {
                      sample[i] = state->g_Models[g_ModelIndex].g_Tracks[i].sample;
                  }
                  else
                  {
                       sample[i] = 0;
                  }
                  colorScaler[i] = state->g_Models[g_ModelIndex].g_Tracks[i].colorScaler;
              
                  agentColor[i] = Color(
                                      offColor[i] + sample[i]*colorScaler[i],
                                      offColor[i] + (sample[i]*colorScaler[i]*0.5), 
                                      offColor[i] + (sample[i]*colorScaler[i]*0.5),
                                      1.0
                                      );
              // agentColor[i] = RGB(1, 1, 1);
            }

          state->print();
          timeFlag += dt;
  }

  virtual void onDraw(Graphics& g) {
          g.clear(Graphics::COLOR_BUFFER_BIT | Graphics::DEPTH_BUFFER_BIT);
          g.clearColor(0, 0, 0, 1.0);
          g.depthTesting(true);
          // cout << "check" << endl;

          shader().uniform("lighting", 1.0);
          material.useColorMaterial(true);
          material();

          for (int i=0; i<state->g_Models[g_ModelIndex].numTracks; ++i) {
            light.dir(state->lightPos - state->g_Models[g_ModelIndex].g_Tracks[i].position);
            light();
            
            //draw agents
            g.pushMatrix();
            g.translate(state->g_Models[g_ModelIndex].g_Tracks[i].position);

            if (state->g_Models[g_ModelIndex].g_Tracks[i].drawMode%2 == 0)
            {
                    g.pushMatrix();
                    g.color(agentColor[i]);
                    // g.color(Color(0.5, 0.5, 0.5, 1.0));
                    g.scale(0.1);
                    g.draw(ball[i]);
                    g.popMatrix();
            }
            else
            {
                    g.pushMatrix();
                    g.color(agentColor[i]);
                    g.scale(1.0);
                    g.draw(freqEnv[i]);
                   //  if (state->g_Models[g_ModelIndex].g_Tracks[i].play) {
                    //   g.pushMatrix();
                    //   g.translate(state->g_Models[g_ModelIndex].g_Tracks[i].playHeadPosition[0], 0);
                    //   g.color(playHeadColor);
                    //   g.draw(playHead);
                    //   g.popMatrix();
                    // }
                    if (state->g_Models[g_ModelIndex].g_Tracks[i].selected) {
                      g.color(selectedColor);
                      g.draw(box[i]);
                      g.popMatrix();
                    }
            }


            g.popMatrix(); 

          }

          // shader().uniform("lighting", 0.0);
          omni().clearColor() = state->bgColor;

          lens().near(state->nearClip);
          lens().far(state->farClip);
          lens().eyeSep(state->eyeSeparation);
  }

  virtual void start() {
    taker.start();                        // non-blocking
    OmniStereoGraphicsRenderer::start();  // blocks
  }
};

int main() { Gra().start(); }
