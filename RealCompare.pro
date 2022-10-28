TEMPLATE = app
LANGUAGE = C++

TARGET = Notepad--

CONFIG	+= qt warn_on release

QT += core gui widgets concurrent network


HEADERS	+= *.h \
        cceditor/ccnotepad.h \
        cceditor/filemanager.h

		
SOURCES	+= *.cpp \
                cceditor/ccnotepad.cpp \
                cceditor/filemanager.cpp

		
FORMS += *.ui \
                cceditor/ccnotepad.ui


RESOURCES += RealCompare.qrc

INCLUDEPATH	+= qscint/src
INCLUDEPATH	+= qscint/src/Qsci
INCLUDEPATH	+= qscint/scintilla/include
INCLUDEPATH += cceditor

DEFINES +=  QSCINTILLA_DLL

TRANSLATIONS += realcompare_zh.ts

win32 {
    CONFIG(debug, debug|release) {
        DESTDIR = x64/Debug
                LIBS	+= -L$$PWDx64/Debug
		LIBS += -lqmyedit_qt5d
    }else{
        DESTDIR = x64/Release
                LIBS += -L$$PWD/x64/Release
		LIBS += -lqmyedit_qt5
                QMAKE_CXXFLAGS += /openmp
    }

   }
unix{
if(CONFIG(debug, debug|release)){
          LIBS += -L/home/yzw/build/CCNotePad/lib -lprotobuf
          LIBS += -L/home/yzw/build/CCNotePad/x64/Debug -lqmyedit_qt5

QMAKE_CXXFLAGS += -fopenmp
LIBS += -lgomp -lpthread
    }else{
          LIBS += -L/home/yzw/build/CCNotePad/lib -lprotobuf
          LIBS += -L/home/yzw/build/CCNotePad/x64/Release -lqmyedit_qt5
        DESTDIR = x64/Release

        QMAKE_CXXFLAGS += -fopenmp -O2
        LIBS += -lgomp -lpthread
    }
   }


RC_FILE += RealCompare.rc
unix
{
unix:!macx: LIBS += -L$$PWD/lib/ -lprotobuf

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

unix:!macx: PRE_TARGETDEPS += $$PWD/lib/libprotobuf.a

unix:!macx: LIBS += -L$$PWD/x64/Release/ -lqmyedit_qt5

INCLUDEPATH += $$PWD/x64/Release
DEPENDPATH += $$PWD/x64/Release

unix:!macx: PRE_TARGETDEPS += $$PWD/x64/Release/libqmyedit_qt5.a
}

