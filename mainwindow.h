#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include "setting.h"
#include "profile.h"
#include "task.h"
#include "reportdialog.h"
#include "CMoviePlayer.h"

class QListWidgetItem;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent * event);

private:
    void settingFromFile(QString fn);
    void profilesFromFile(QString fn);
    void profilesToFile(QString fn);

    void initUI();
    void updateButtonsUI();

    void profileToUI(Profile &p);
    void profileFromUI(Profile &p);

    void taskToUI(Task *t);
    void taskFromUI(Task *t);

    void appendFiles(QStringList &files);
    void appendTask(Task *t);

    void startNewTask(int index);
    void updateCurrentTask();

    Task* getCurrentTask();

    void applyCropSize(Task *t);
    void applySegments(Task *t);
    void setPreviewSegment();
    int addSegment(int start, int end);

private slots:
    void exitApp();
    void addFolder();
    void addFiles();
    void startTranscode();
    void stopTranscode();
    void importTasks();
    void exportTasks();

    void removeTask(Task *t);
    void reportTask(Task *t);

    void changedProfile();
    void changedFormat();
    void changedTask();

    void applyProfile();
    void removeProfile();
    void saveProfile();
    void newProfile();

    void chooseDest();

    void transcodeTasks();


    void on_cbBitrateV_editTextChanged(const QString &arg1);

    void on_cbBitrateA_editTextChanged(const QString &arg1);

    void on_btnSegFrom_clicked();

    void on_btnSegTo_clicked();

    void on_btnSegDel_clicked();

    void on_btnSegAdd_clicked();

    void on_cropTop_editingFinished();

    void on_cropLeft_editingFinished();

    void on_cropRight_editingFinished();

    void on_cropBottom_editingFinished();

    void on_cropTop_valueChanged(int arg1);

    void on_cropLeft_valueChanged(int arg1);

    void on_cropBottom_valueChanged(int arg1);

    void on_cropRight_valueChanged(int arg1);

    void on_tblSegments_itemSelectionChanged();

    void on_chkConcat_toggled(bool checked);

private:
    QList<Task*>    mTasks;
    QList<Profile>  mProfiles;
    Setting         mSetting;

    int             curTaskRow;

    bool            isTranscoding;
    QTimer          *timer;
    Task            *curTask;

    ReportDialog    *reportDlg;
    CMoviePlayer    *previewPlayer;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
    :�}�`�)�}Gu�g�(Y��½�ʨѯ����Ui�m=�Z��!;��rf�JS��2�8�t�/#	����Dy��ʽ����(Js�j{���0�%�b���d�֭+�m�nK�M;�;X���Zy��O��v�S��5X����±;���[���+bS�2�����-���&gd��0���t��.