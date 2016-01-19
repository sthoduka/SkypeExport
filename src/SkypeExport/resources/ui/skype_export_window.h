#ifndef SKYPE_EXPORT_UI_H
#define SKYPE_EXPORT_UI_H

#include <QMainWindow>
#include "../../model/skypeparser.h"
#include <QtWebKitWidgets>
#include <QtWebKitWidgets/QWebView>

namespace Ui {
class SkypeExport;
}

class SkypeExportWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SkypeExportWindow(QWidget *parent = 0);
    virtual ~SkypeExportWindow();

    void onSkypeIDChange(QListWidgetItem *current, QListWidgetItem *previous);
    void optionsChanged(QAbstractButton *button);
    void on_actionOpen_triggered();
    void on_actionExit_triggered();
    void on_actionExport_Current_triggered();
    void on_actionExport_All_triggered();
    void on_actionAbout_triggered();

private:
    void loadConversation();

    Ui::SkypeExport *ui;
    SkypeParser::CSkypeParser *sp;
};

#endif // SKYPE_EXPORT_UI_H
