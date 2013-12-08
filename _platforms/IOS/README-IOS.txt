To enable the build of glGA examples for IOS:

In PlatformWrapper.h
- Comment the line: #define __WINDOWS__MAC__LINUX__
- Uncomment the line: #define __IOS__

In glGAHelper.h
- Comment the line #define USE_MAGICK