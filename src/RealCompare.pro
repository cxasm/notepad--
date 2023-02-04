TEMPLATE = app
LANGUAGE = C++

TARGET = Notepad--

CONFIG	+= qt warn_on

QT += core gui widgets concurrent network xmlpatterns


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

#DEFINES +=  QSCINTILLA_DLL

TRANSLATIONS += realcompare_zh.ts

 if(contains(QMAKE_HOST.arch, x86_64|loongarch64)){
    CONFIG(Debug, Debug|Release){
        DESTDIR = x64/Debug
		LIBS	+= -Lx64/Debug
		LIBS += -lqmyedit_qt5d
    }else{
        DESTDIR = x64/Release
		LIBS	+= -Lx64/Release
		LIBS += -lqmyedit_qt5
        #QMAKE_CXXFLAGS += /openmp
    }
   }
unix{
if(CONFIG(debug, Debug|Release)){
          LIBS += -L/home/yzw/build/CCNotePad/x64/Debug -lqmyedit_qt5

QMAKE_CXXFLAGS += -fopenmp
LIBS += -lgomp -lpthread
    }else{
          LIBS += -L/home/yzw/build/CCNotePad/x64/Release -lqmyedit_qt5
        DESTDIR = x64/Release

        QMAKE_CXXFLAGS += -fopenmp -O2
        LIBS += -lgomp -lpthread
    }
   }


RC_FILE += RealCompare.rc
unix
{

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.


unix:!macx: LIBS += -L$$PWD/x64/Release/ -lqmyedit_qt5

INCLUDEPATH += $$PWD/x64/Release
DEPENDPATH += $$PWD/x64/Release

unix:!macx: PRE_TARGETDEPS += $$PWD/x64/Release/libqmyedit_qt5.a
}

