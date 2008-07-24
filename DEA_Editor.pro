######################################################################
# Automatically generated by qmake (2.01a) So Jul 13 11:35:48 2008
######################################################################
# And edited by twi on So Jul 13 11:48:00 2008
######################################################################

TEMPLATE = app
DESTDIR = bin/
TARGET = 
DEPENDPATH += . src src/DEdit
CONFIG += qt  #warn_on #Debugging-Informationen und Warnungen werden ausgegeben
INCLUDEPATH += . src

TRANSLATIONS = translations/dea_editor_english.ts \
               translations/dea_editor_german.ts

# Input
HEADERS += \ # DEA
           src/DEA/dea.h \
           src/DEA/dea_state.h \
           src/DEA/dea_transition.h \
           \ # file - io
           src/io/xmlparser.h \
           src/io/xmlencoder.h \
           src/io/xmlloader.h \
           src/io/iconcatcher.h \
           \ # widgets
           src/widgets/commandbuttondnd.h \ 
           src/widgets/colorbutton.h \ 
           \ # general dialogs
           src/dialogs/dia_deasourceviewer.h \
           src/dialogs/dia_configurededitwidget.h \
           src/dialogs/dia_about.h \
           \ # dialogs for editor
           src/DEdit/dedit_editstatedia.h \
           src/DEdit/dedit_edittransitiondia.h \
           \ # Main Editor
           src/DEdit/dedit_mainwindow.h \
           src/DEdit/dedit_widget.h \
           src/DEdit/dedit_widgetpainter.h \
           src/DEdit/dedit_graphicalstate.h \
           src/DEdit/dedit_graphicaltransition.h \
           src/DEdit/dedit_execdeawidget.h \
           src/DEdit/dedit_propertieswidget.h
SOURCES += src/main.cpp \
           \ # DEA
           src/DEA/dea.cpp \
           src/DEA/dea_state.cpp \
           src/DEA/dea_transition.cpp \
           \ # file - io
           src/io/xmlparser.cpp \
           src/io/xmlencoder.cpp \
           src/io/xmlloader.cpp \
           src/io/iconcatcher.cpp \
           \ # widgets
           src/widgets/commandbuttondnd.cpp \ 
           src/widgets/colorbutton.cpp \ 
           \ # general dialogs
           src/dialogs/dia_deasourceviewer.cpp \
           src/dialogs/dia_configurededitwidget.cpp \
           src/dialogs/dia_about.cpp \
           \ # dialogs for editor
           src/DEdit/dedit_editstatedia.cpp \
           src/DEdit/dedit_edittransitiondia.cpp \
           \ # Main Editor
           src/DEdit/dedit_mainwindow.cpp \
           src/DEdit/dedit_widget.cpp \
           src/DEdit/dedit_widgetpainter.cpp \
           src/DEdit/dedit_graphicalstate.cpp \
           src/DEdit/dedit_graphicaltransition.cpp \
           src/DEdit/dedit_execdeawidget.cpp \
           src/DEdit/dedit_propertieswidget.cpp
