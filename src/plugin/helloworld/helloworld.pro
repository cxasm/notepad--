TEMPLATE	= lib
LANGUAGE	= C++

CONFIG	+= qt warn_on
QT += core gui widgets

HEADERS	+= *.h
SOURCES	+= *.cpp
FORMS	+= *.ui

INCLUDEPATH	+= ../../include
INCLUDEPATH	+= ../../qscint/src
INCLUDEPATH	+= ../../qscint/src/Qsci


win32 {
   if(contains(QMAKE_HOST.arch, x86_64)){
    CONFIG(Debug, Debug|Release){
        DESTDIR = ../../x64/Debug/plugin
		LIBS += -L../../x64/Debug
		LIBS += -lqmyedit_qt5d
    }else{
        DESTDIR = ../../x64/Release/plugin
		LIBS += -L../../x64/Release
		LIBS += -lqmyedit_qt5
    }
   }
}

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
