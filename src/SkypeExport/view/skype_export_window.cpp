#include "skype_export_window.h"
#include "ui_skype_export.h"
#include <iostream>


SkypeExportWindow::SkypeExportWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SkypeExport),
    sp(NULL)
{
    ui->setupUi(this);
    connect(ui->contactListWidget, &QListWidget::currentItemChanged, this, &SkypeExportWindow::onSkypeIDChange);
    connect(ui->buttonGroupTimeFormat, static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), this, &SkypeExportWindow::optionsChanged);
    connect(ui->buttonGroupTimeReference, static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), this, &SkypeExportWindow::optionsChanged);

    connect(ui->actionOpen, &QAction::triggered, this, &SkypeExportWindow::on_actionOpen_triggered);
    connect(ui->actionExit, &QAction::triggered, this, &SkypeExportWindow::on_actionExit_triggered);
    connect(ui->actionExport_Current, &QAction::triggered, this, &SkypeExportWindow::on_actionExport_Current_triggered);
    connect(ui->actionExport_All, &QAction::triggered, this, &SkypeExportWindow::on_actionExport_All_triggered);
    connect(ui->actionAbout, &QAction::triggered, this, &SkypeExportWindow::on_actionAbout_triggered);
}

SkypeExportWindow::~SkypeExportWindow()
{
    delete ui;
}

void SkypeExportWindow::loadConversation()
{
    int timeFormat = (ui->radioButton12h->isChecked() ? 1 : 2);
    int timeReference = (ui->radioButtonLocal->isChecked() ? 1 : 0);
    if (sp != NULL)
    {
        std::string xhtmlOutput = sp->getFullHistoryAsXHTML(ui->contactListWidget->currentItem()->text().toStdString(), timeFormat, timeReference);
        ui->htmlViewer->setHtml(QString::fromStdString(xhtmlOutput));
        ui->htmlViewer->adjustSize();
    }
}

void SkypeExportWindow::onSkypeIDChange(QListWidgetItem *current, QListWidgetItem *previous)
{
    loadConversation();
}

void SkypeExportWindow::optionsChanged(QAbstractButton *button)
{
    loadConversation();
}

void SkypeExportWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Select Database File"), ".", tr("DB files (*.db)"));
    std::string status_msg;
    if (filename.isEmpty())
    {
        status_msg = "Database file not selected";
        ui->statusBar->showMessage(QString::fromStdString(status_msg), 10000);
        return;
    }
    if (fs::file_size(filename.toStdString()) == 0)
    {
        status_msg = "Database is empty!";
        ui->statusBar->showMessage(QString::fromStdString(status_msg), 10000);
        QMessageBox::critical(this, "Error", "Database is empty!");
        return;
    }
    if (sp != NULL)
    {
        delete sp;
        sp = NULL;
        ui->contactListWidget->clear();
    }
    sp = new SkypeParser::CSkypeParser(filename.toStdString());
    const SkypeParser::skypeIDs_t &users = sp->getSkypeUsers();
    for( SkypeParser::skypeIDs_t::const_iterator it( users.begin() ); it != users.end(); ++it ){
        const std::string &skypeID = (*it);
        ui->contactListWidget->addItem(QString::fromStdString(skypeID));
    }
    if (ui->contactListWidget->count() > 0)
    {
        ui->contactListWidget->item(0)->setSelected(true);
        ui->actionExport_Current->setEnabled(true);
        ui->actionExport_All->setEnabled(true);

        status_msg = "Opened file " + filename.toStdString();
        ui->statusBar->showMessage(QString::fromStdString(status_msg), 10000);
    }
    else
    {
        status_msg = "Database is empty";
        ui->statusBar->showMessage(QString::fromStdString(status_msg), 10000);
    }
}

void SkypeExportWindow::on_actionExit_triggered()
{
    if (sp != NULL)
    {
        delete sp;
    }
    close();
}

void SkypeExportWindow::exportUsers(const std::vector<std::string> &skypeIDs)
{
    int timeFormat = (ui->radioButton12h->isChecked() ? 1 : 2);
    int timeReference = (ui->radioButtonLocal->isChecked() ? 1 : 0);

    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Export Directory"), ".",
                                                    QFileDialog::ShowDirsOnly |
                                                    QFileDialog::DontResolveSymlinks);
    std::string status_msg;
    if (dir.isEmpty())
    {
        status_msg = "No export directory selected";
        ui->statusBar->showMessage(QString::fromStdString(status_msg), 10000);
        return;
    }

    for (int i = 0; i < skypeIDs.size(); i++)
    {
        fs::path logPath( dir.toStdString() );
        std::string safeFilename = SkypeParser::makeSafeFilename( skypeIDs[i], '$' ); // replace illegal characters with $ instead; some skype IDs are "live:username", and will become "live$username"
        logPath /= ( safeFilename + ".skypelog.htm" ); // appends the log filename and chooses the appropriate path separator
        sp->exportUserHistory(skypeIDs[i], logPath.string(), timeFormat, timeReference);
    }
}

void SkypeExportWindow::on_actionExport_Current_triggered()
{
    std::string skypeID = ui->contactListWidget->currentItem()->text().toStdString();
    std::vector<std::string> users_to_export;
    users_to_export.push_back(skypeID);

    exportUsers(users_to_export);

    std::string status_msg = "Exported contact " + skypeID;
    ui->statusBar->showMessage(QString::fromStdString(status_msg), 10000);

}

void SkypeExportWindow::on_actionExport_All_triggered()
{
    std::vector<std::string> users_to_export;
    const SkypeParser::skypeIDs_t &users = sp->getSkypeUsers();
    for( SkypeParser::skypeIDs_t::const_iterator it( users.begin() ); it != users.end(); ++it ){
        users_to_export.push_back(*it);
    }

    exportUsers(users_to_export);

    std::string status_msg;
    if (users.size() > 0)
    {
        std::ostringstream s;
        s << "Successfully exported " << users.size() << " contacts";
        status_msg = s.str();
    }
    else
    {
        status_msg = "No contacts in database";
    }
    ui->statusBar->showMessage(QString::fromStdString(status_msg), 10000);
}

void SkypeExportWindow::on_actionAbout_triggered()
{
    QString about_txt("<font size = 4>Skype History Exporter v1.4.0 Stable<br>Website: <a href=\"https://github.com/Temptin/SkypeExport\">https://github.com/Temptin/SkypeExport</a></font>");
    QMessageBox::about(this, QString("About"), about_txt);
}
