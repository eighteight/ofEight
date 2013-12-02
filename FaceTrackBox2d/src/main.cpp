#include "FaceTrackBox2dApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 640, 480, OF_WINDOW);
	ofRunApp(new FaceTrackBox2dApp());
}
