#-------------------------------------------------
#
# Project created by QtCreator 2016-06-22T04:41:52
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VideoTranscoder
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    task.cpp \
    profile.cpp \
    json.cpp \
    setting.cpp \
    taskitemview.cpp \
    reportdialog.cpp \
    CMoviePlayer.cpp \
    CVideoDecoder.cpp \
    utils.cpp

HEADERS  += mainwindow.h \
    task.h \
    profile.h \
    json.h \
    setting.h \
    taskitemview.h \
    reportdialog.h \
    CMoviePlayer.h \
    CVideoDecoder.h \
    utils.h

FORMS    += mainwindow.ui \
    taskitemview.ui \
    reportdialog.ui

RESOURCES += \
    videotranscoder.qrc

RC_FILE = videotranscoder.rc

DEFINES += "_NO_OLDNAMES"

unix|win32: LIBS += -L$$PWD/thirdparty/libffmpeg/lib/ -lavcodec-54

INCLUDEPATH += $$PWD/thirdparty/libffmpeg/include
DEPENDPATH += $$PWD/thirdparty/libffmpeg/include

unix|win32: LIBS += -L$$PWD/thirdparty/libffmpeg/lib/ -lavdevice-54

INCLUDEPATH += $$PWD/thirdparty/libffmpeg/include
DEPENDPATH += $$PWD/thirdparty/libffmpeg/include

unix|win32: LIBS += -L$$PWD/thirdparty/libffmpeg/lib/ -lavfilter-3

INCLUDEPATH += $$PWD/thirdparty/libffmpeg/include
DEPENDPATH += $$PWD/thirdparty/libffmpeg/include

unix|win32: LIBS += -L$$PWD/thirdparty/libffmpeg/lib/ -lavformat-54

INCLUDEPATH += $$PWD/thirdparty/libffmpeg/include
DEPENDPATH += $$PWD/thirdparty/libffmpeg/include

unix|win32: LIBS += -L$$PWD/thirdparty/libffmpeg/lib/ -lavutil-52

INCLUDEPATH += $$PWD/thirdparty/libffmpeg/include
DEPENDPATH += $$PWD/thirdparty/libffmpeg/include
unix|win32: LIBS += -L$$PWD/thirdparty/libffmpeg/lib/ -lpostproc-52

INCLUDEPATH += $$PWD/thirdparty/libffmpeg/include
DEPENDPATH += $$PWD/thirdparty/libffmpeg/include

unix|win32: LIBS += -L$$PWD/thirdparty/libffmpeg/lib/ -lswresample-0

INCLUDEPATH += $$PWD/thirdparty/libffmpeg/include
DEPENDPATH += $$PWD/thirdparty/libffmpeg/include

unix|win32: LIBS += -L$$PWD/thirdparty/libffmpeg/lib/ -lswscale-2

INCLUDEPATH += $$PWD/thirdparty/libffmpeg/include
DEPENDPATH += $$PWD/thirdparty/libffmpeg/include

unix|win32: LIBS += -L$$PWD/thirdparty/libcbs/lib/ -llibcbs

INCLUDEPATH += $$PWD/thirdparty/libcbs/include
DEPENDPATH += $$PWD/thirdparty/libcbs/include
    wM��m h�ʷ�`h�
����/,uD�s��+��{�G��q�zl˛���F�U��f�M}�C�G�4�ѭ��yA}��3� ��-�����UV�4�ӧ�'H�'ѶI�=ó�+AZ�\Cf��S��^����=&�����O�9	M�G��F��wJ����1����Y��ɜޫ��Ws��2��݋G�&x�	9