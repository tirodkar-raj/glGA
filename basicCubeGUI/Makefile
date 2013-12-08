all: basicCubeGUI.o glGAHelper.o PlatformWrapper.o
#	g++ -o basicCubeGUI basicCubeGUI.o glGAHelper.o PlatformWrapper.o -L../_thirdPartyLibs/AntTweakBar-1.16/lib -lAntTweakBar -lGLEW -lglfw
	g++ -o basicCubeGUI basicCubeGUI.o glGAHelper.o PlatformWrapper.o -L../_thirdPartyLibs/AntTweakBar-1.16/lib -Wl,-rpath=../_thirdPartyLibs/AntTweakBar-1.16/lib -lAntTweakBar -lGLEW -lglfw

basicCubeGUI.o: basicCubeGUI.cpp
	g++ -c basicCubeGUI.cpp -I../include/ -I../_thirdPartyLibs/glm-0.9.3.4 -I../_thirdPartyLibs/AntTweakBar-1.16/include

glGAHelper.o: ../src/glGAHelper.cpp
	g++ -c ../src/glGAHelper.cpp -I../include/ -I../_thirdPartyLibs/glm-0.9.3.4

PlatformWrapper.o: ../src/PlatformWrapper.cpp
	g++ -c ../src/PlatformWrapper.cpp -I../include -I../_thirdPartyLibs/glm-0.9.3.4

clean:
	rm *.o

clean-all:
	rm *.o basicCubeGUI
