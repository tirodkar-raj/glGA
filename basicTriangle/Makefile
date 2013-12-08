all: basicTriangle.o glGAHelper.o PlatformWrapper.o
	g++ -o basicTriangle basicTriangle.o glGAHelper.o PlatformWrapper.o -lGLEW -lglfw

basicTriangle.o: basicTriangle.cpp
	g++ -c basicTriangle.cpp -I../include/ -I../_thirdPartyLibs/glm-0.9.3.4

glGAHelper.o: ../src/glGAHelper.cpp
	g++ -c ../src/glGAHelper.cpp -I../include/ -I../_thirdPartyLibs/glm-0.9.3.4

PlatformWrapper.o: ../src/PlatformWrapper.cpp
	g++ -c ../src/PlatformWrapper.cpp -I../include -I../_thirdPartyLibs/glm-0.9.3.4

clean:
	rm *.o

clean-all:
	rm *.o basicTriangle
