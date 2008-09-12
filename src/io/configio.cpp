
#include "configio.h"


#include <DEdit/dedit_widget.h>
#include <DEdit/dedit_appearance.h>
#include <DEdit/dedit_graphicalstate.h>
#include <DEdit/dedit_graphicaltransition.h>
#include <DEdit/dedit_mainwindow.h>
#include <QSettings>
#include <QtDebug>
#include <QDir>

#include <QMenuBar>
#include <QStatusBar>

ConfigIO::ConfigIO()
{
    m_pEditorWidget = NULL;
    m_pMainWindow = NULL;
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "$HOME/.dea_editor");
    QSettings::setDefaultFormat(QSettings::IniFormat);
}

ConfigIO::~ConfigIO()
{
    
}




void ConfigIO::setEditorWidget(DEdit_Widget* pEditorWidget)
{
    m_pEditorWidget = pEditorWidget;
}

DEdit_Widget* ConfigIO::editorWidget()
{
    return m_pEditorWidget;
}


void ConfigIO::setMainWindow(DEdit_MainWindow* pMainWindow)
{
    m_pMainWindow = pMainWindow;
}

DEdit_MainWindow* ConfigIO::mainWindow()
{
    return m_pMainWindow;
}

bool ConfigIO::saveConfig()
{
    if(!createConfigDir())
    {
        return FALSE;
    }
    QString fileName = getConfigDirPath() + "dea_editorrc";
    QSettings settings(fileName, QSettings::IniFormat);
    
    //qDebug("saving settings to:");
    //qDebug() << settings.fileName();
    
    if(m_pMainWindow)
    {
        settings.beginGroup("MainWindow");
        settings.setValue("size", m_pMainWindow->size());
        settings.setValue("maximized", m_pMainWindow->isMaximized());
        settings.setValue("state", m_pMainWindow->saveState());
        settings.setValue("menuBarVisible", m_pMainWindow->menuBar()->isVisible());
        settings.setValue("statusBarVisible", m_pMainWindow->statusBar()->isVisible());
        settings.endGroup();
    }
    if(m_pEditorWidget)
    {
        
        settings.beginGroup("EditorWidget_Behavior");
        settings.setValue("autoEditNewStates", m_pEditorWidget->autoEditNewStates());
        settings.setValue("gridResolution", m_pEditorWidget->gridResolution());
        settings.endGroup();
        
        
        DEdit_Appearance* app = m_pEditorWidget->appearance();
        if(app)
        {
            settings.beginGroup("EditorWidget_Colors");
            settings.setValue("stateNormalFill",        app->m_cStateNormal.m_cColor[0].name());
            settings.setValue("stateNormalInnerBorder", app->m_cStateNormal.m_cColor[1].name());
            settings.setValue("stateNormalOuterBorder", app->m_cStateNormal.m_cColor[2].name());
            settings.setValue("stateSelectedFill",        app->m_cStateSelected.m_cColor[0].name());
            settings.setValue("stateSelectedInnerBorder", app->m_cStateSelected.m_cColor[1].name());
            settings.setValue("stateSelectedOuterBorder", app->m_cStateSelected.m_cColor[2].name());
            settings.setValue("stateExecutedFill",        app->m_cStateExecuted.m_cColor[0].name());
            settings.setValue("stateExecutedInnerBorder", app->m_cStateExecuted.m_cColor[1].name());
            settings.setValue("stateExecutedOuterBorder", app->m_cStateExecuted.m_cColor[2].name());
            settings.setValue("stateResultDeniedFill",        app->m_cStateResultDenied.m_cColor[0].name());
            settings.setValue("stateResultDeniedInnerBorder", app->m_cStateResultDenied.m_cColor[1].name());
            settings.setValue("stateResultDeniedOuterBorder", app->m_cStateResultDenied.m_cColor[2].name());
            settings.setValue("stateResultAcceptedFill",        app->m_cStateResultAccepted.m_cColor[0].name());
            settings.setValue("stateResultAcceptedInnerBorder", app->m_cStateResultAccepted.m_cColor[1].name());
            settings.setValue("stateResultAcceptedOuterBorder", app->m_cStateResultAccepted.m_cColor[2].name());
            settings.setValue("stateLabel",        app->m_cStateLabelColor.name());
            
            
            settings.setValue("transitionNormal", app->m_cTransitionNormal.name());
            settings.setValue("transitionHovered", app->m_cTransitionHovered.name());
            settings.setValue("transitionSelected", app->m_cTransitionSelected.name());
            settings.setValue("transitionExecuted", app->m_cTransitionExecuted.name());
            settings.setValue("transitionLabel", app->m_cTransitionLabelColor.name());
            settings.endGroup();
        }
        
        settings.beginGroup("EditorWidget_Proportions");
        settings.setValue("stateDiameter", DEdit_GraphicalState::m_nDiameter);
        settings.setValue("stateTransitionLineWidth", DEdit_GraphicalTransition::m_nLineWidth);
        settings.endGroup();
        
        settings.beginGroup("EditorWidget_History");
        settings.setValue("historySize", m_pEditorWidget->maxHistorySize());
        settings.endGroup();
    }
    
    return TRUE;
}


#define LOAD_DEDIT_WIDGET_VALUE(szName, fGetter, fSetter, fConversion) \
 m_pEditorWidget->fSetter(settings.value(szName, m_pEditorWidget->fGetter()).fConversion())

#define LOAD_DEDIT_COLOR(pAppearance, szName, color_member) \
 pAppearance->color_member.setNamedColor( \
  settings.value(szName, pAppearance->color_member.name()).toString())

bool ConfigIO::loadConfig()
{
    if(!isConfigDirExisting())
    {
        return TRUE;
    }
    QString fileName = getConfigDirPath() + "dea_editorrc";
    QSettings settings(fileName, QSettings::IniFormat);
    
    
    if(m_pMainWindow)
    {
        settings.beginGroup("MainWindow");
        m_pMainWindow->resize(settings.value("size", m_pMainWindow->size()).toSize());
        // maximized will be ignored
        m_pMainWindow->restoreState(settings.value("state").toByteArray());
        m_pMainWindow->mnaShowMenuBar->setChecked(settings.value("menuBarVisible", TRUE).toBool());
        m_pMainWindow->mnaShowStatusBar->setChecked(settings.value("statusBarVisible", TRUE).toBool());
        settings.endGroup();
    }
    
    if(m_pEditorWidget)
    {
        
        settings.beginGroup("EditorWidget_Behavior");
        LOAD_DEDIT_WIDGET_VALUE("autoEditNewStates", autoEditNewStates, setAutoEditNewStates, toBool);
        LOAD_DEDIT_WIDGET_VALUE("gridResolution", gridResolution, setGridResolution, toInt);
        settings.endGroup();
        
        
        DEdit_Appearance* app = m_pEditorWidget->appearance();
        if(app)
        {
            settings.beginGroup("EditorWidget_Colors");
            LOAD_DEDIT_COLOR(app, "stateNormalFill",        m_cStateNormal.m_cColor[0]);
            LOAD_DEDIT_COLOR(app, "stateNormalInnerBorder", m_cStateNormal.m_cColor[1]);
            LOAD_DEDIT_COLOR(app, "stateNormalOuterBorder", m_cStateNormal.m_cColor[2]);
            
            LOAD_DEDIT_COLOR(app, "stateSelectedFill",          m_cStateSelected.m_cColor[0]);
            LOAD_DEDIT_COLOR(app, "stateSelectedInnerBorder",   m_cStateSelected.m_cColor[1]);
            LOAD_DEDIT_COLOR(app, "stateSelectedOuterBorder",   m_cStateSelected.m_cColor[2]);
            
            LOAD_DEDIT_COLOR(app, "stateExecutedFill",          m_cStateExecuted.m_cColor[0]);
            LOAD_DEDIT_COLOR(app, "stateExecutedInnerBorder",   m_cStateExecuted.m_cColor[1]);
            LOAD_DEDIT_COLOR(app, "stateExecutedOuterBorder",   m_cStateExecuted.m_cColor[2]);
            
            LOAD_DEDIT_COLOR(app, "stateResultDeniedFill",          m_cStateResultDenied.m_cColor[0]);
            LOAD_DEDIT_COLOR(app, "stateResultDeniedInnerBorder",   m_cStateResultDenied.m_cColor[1]);
            LOAD_DEDIT_COLOR(app, "stateResultDeniedOuterBorder",   m_cStateResultDenied.m_cColor[2]);
            
            LOAD_DEDIT_COLOR(app, "stateResultAcceptedFill",        m_cStateResultAccepted.m_cColor[0]);
            LOAD_DEDIT_COLOR(app, "stateResultAcceptedInnerBorder", m_cStateResultAccepted.m_cColor[1]);
            LOAD_DEDIT_COLOR(app, "stateResultAcceptedOuterBorder", m_cStateResultAccepted.m_cColor[2]);
            
            
            LOAD_DEDIT_COLOR(app, "stateLabel", m_cStateLabelColor);
            
            LOAD_DEDIT_COLOR(app, "transitionNormal", m_cTransitionNormal);
            LOAD_DEDIT_COLOR(app, "transitionHovered", m_cTransitionHovered);
            LOAD_DEDIT_COLOR(app, "transitionSelected", m_cTransitionSelected);
            LOAD_DEDIT_COLOR(app, "transitionExecuted", m_cTransitionExecuted);
            LOAD_DEDIT_COLOR(app, "transitionLabel", m_cTransitionLabelColor);
            
            settings.endGroup();
        }
        
        
        settings.beginGroup("EditorWidget_Proportions");
        int newValue = settings.value("stateDiameter", DEdit_GraphicalState::m_nDiameter).toInt();
        DEdit_GraphicalState::m_nDiameter
                = rangeIntValue(newValue, 26, 504);
        newValue = settings.value("stateTransitionLineWidth", DEdit_GraphicalTransition::m_nLineWidth).toInt();
        DEdit_GraphicalTransition::m_nLineWidth = rangeIntValue(newValue, 1, 40);
        settings.endGroup();
        
        // proportions and colors has been changed
        m_pEditorWidget->recreateAllGuiTemplates();
        
        
        settings.beginGroup("EditorWidget_History");
        m_pEditorWidget->setMaxHistorySize(settings.value("historySize", m_pEditorWidget->maxHistorySize()).toInt());
        settings.endGroup();
    }
    
    return TRUE;
}


bool ConfigIO::createConfigDir()
{
    if(isConfigDirExisting())
    {
        return TRUE;
    }
    QDir dir (QDir::home());
#ifdef Q_WS_WIN
    return dir.mkdir("dea_editor");
#else
    return dir.mkdir(".dea_editor");
#endif
}


bool ConfigIO::isConfigDirExisting()
{
#ifdef Q_WS_WIN
    return QDir::home().exists("dea_editor");
#else
    return QDir::home().exists(".dea_editor");
#endif
}

QString ConfigIO::getConfigDirPath()
{
#ifdef Q_WS_WIN
    return QDir::homePath() + QDir::separator() + "dea_editor" + QDir::separator();
#else
    return QDir::homePath() + QDir::separator() + ".dea_editor" + QDir::separator();
#endif
}


int ConfigIO::rangeIntValue(int value, int min, int max)
{
    if(value < min)
    {
        return min;
    }
    if(value > max)
    {
        return max;
    }
    
    return value;
}


