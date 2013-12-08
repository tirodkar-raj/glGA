all: basicCube.o glGAHelper.o PlatformWrapper.o
	g++ -o basicCube basicCube.o glGAHelper.o PlatformWrapper.o -lGLEW -lglfw

basicCube.o: basicCube.cpp
	g++ -c basicCube.cpp -I../include/ -I../_thirdPartyLibs/glm-0.9.3.4

glGAHelper.o: ../src/glGAHelper.cpp
	g++ -c ../src/glGAHelper.cpp -I../include/ -I../_thirdPartyLibs/glm-0.9.3.4

PlatformWrapper.o: ../src/PlatformWrapper.cpp
	g++ -c ../src/PlatformWrapper.cpp -I../include -I../_thirdPartyLibs/glm-0.9.3.4

clean:
	rm *.o

clean-all:
	rm *.o basicCube
