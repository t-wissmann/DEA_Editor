

#ifndef __CONFIG_IO_H_
#define __CONFIG_IO_H_

#include <QString>

class DEdit_Widget;
class DEdit_MainWindow;

class ConfigIO
{
public:
    ConfigIO();
    ~ConfigIO();
    
    void setEditorWidget(DEdit_Widget* pEditorWidget);
    DEdit_Widget* editorWidget();
    
    void setMainWindow(DEdit_MainWindow* pMainWindow);
    DEdit_MainWindow* mainWindow();
    
    bool saveConfig();
    bool loadConfig();
    bool createConfigDir();
    static QString getConfigDirPath();
    static bool isConfigDirExisting();
    
    static int rangeIntValue(int value, int min, int max);
    
private:
    DEdit_Widget*       m_pEditorWidget;
    DEdit_MainWindow*   m_pMainWindow;
    
    
};


#endif

 
