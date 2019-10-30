#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QTimer>
#include <QStandardPaths>
#include "taskitemview.h"
#include "json.h"
#include "utils.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

#define DBG_FUNC (qDebug()<<__FILE__<<__LINE__<<__PRETTY_FUNCTION__)
#define FILE_PROFILES   "profiles.vti"
#define FILE_SETTINGS   "settings.vti"

/////////////////////////////////////////////////////////////
QString readFile(const QString &filename) {
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        return QString();
    } else {
        QTextStream in(&f);
        in.setCodec("UTF-8");
        return in.readAll();
    }
}

void jsonFromFile(QString &json, QString &filename) {
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        json = "";
    } else {
        QTextStream in(&f);
        in.setCodec("UTF-8");
        json = in.readAll();
    }
}

void jsonToFile(QString &json, QString &filename) {
    QFile f(filename);
    if (!f.open(QFile::WriteOnly | QFile::Text)) {
        return ;
    } else {
        QTextStream out(&f);
        out.setCodec("UTF-8");
        out.setGenerateByteOrderMark(false);
        out << json;
    }
}

void printJson(const JsonObject &result) {
    qDebug() << QtJson::serialize(result);
}

void streamsFromView(QList<Task_Stream> &streams, QListWidget *view)
{
    for(int row = 0; row < view->count(); row++){
        QListWidgetItem *item = view->item(row);

        streams[row].valid = item->checkState() == Qt::Checked;
    }
}

void streamsToView(QList<Task_Stream> &streams, QListWidget *view)
{
    view->clear();
    foreach (Task_Stream s, streams) {
        QListWidgetItem *item = new QListWidgetItem(s.name);
        item->setCheckState(s.valid ? Qt::Checked : Qt::Unchecked);
        view->addItem(item);
    }
}


////////////////////////////////////////////////////////////
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settingFromFile(FILE_SETTINGS);
    profilesFromFile(FILE_PROFILES);

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exitApp()));
    connect(ui->actionAddFolder, SIGNAL(triggered()), this, SLOT(addFolder()));
    connect(ui->actionAddFile, SIGNAL(triggered()), this, SLOT(addFiles()));
    connect(ui->actionStart, SIGNAL(triggered()), this, SLOT(startTranscode()));
    connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(stopTranscode()));
    connect(ui->actionImport, SIGNAL(triggered()), this, SLOT(importTasks()));
    connect(ui->actionExport, SIGNAL(triggered()), this, SLOT(exportTasks()));
    connect(ui->actionApplyProfile, SIGNAL(triggered()), this, SLOT(applyProfile()));
    connect(ui->actionRemoveProfile, SIGNAL(triggered()), this, SLOT(removeProfile()));
    connect(ui->actionSaveProfile, SIGNAL(triggered()), this, SLOT(saveProfile()));

    connect(ui->cbProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(changedProfile()));
    connect(ui->cbFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(changedFormat()));

    connect(ui->pbRemove, SIGNAL(clicked()), this, SLOT(removeProfile()));
    connect(ui->pbSave, SIGNAL(clicked()), this, SLOT(saveProfile()));
    connect(ui->pbNew, SIGNAL(clicked()), this, SLOT(newProfile()));

    connect(ui->pbChoose, SIGNAL(clicked()), this, SLOT(chooseDest()));
    connect(ui->taskView, SIGNAL(currentRowChanged(int)), this, SLOT(changedTask()));

    curTaskRow = -1;
    isTranscoding = false;
    curTask = NULL;
    reportDlg = NULL;
    ui->edDestination->setText(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(transcodeTasks()));

    av_register_all();
    avcodec_register_all();
    avformat_network_init();
    //av_log_set_callback( LibavutilCallback );
    av_log_set_level(AV_LOG_DEBUG);

    initUI();
    updateButtonsUI();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(isTranscoding) {
        QMessageBox::warning(this, "", "Transcoding is now working...\nPlease stop it!");
        event->ignore();
        return;
    }
    event->accept();
    qDebug() << "close...";
}

void MainWindow::settingFromFile(QString fn)
{
    bool ok;
    fn = QApplication::applicationDirPath() + "/" + fn;
    QString json;
    jsonFromFile(json, fn);
    if (json.isEmpty())
        QMessageBox::critical(this,"", "Could not read settings.vti file!");
    QVariant obj = QtJson::parse(json, ok);
    mSetting.fromJson(obj);

}

void MainWindow::profilesFromFile(QString fn)
{
    bool ok;
    fn = QApplication::applicationDirPath() + "/" + fn;
    QString json;
    jsonFromFile(json, fn);
    if (json.isEmpty())
        QMessageBox::critical(this,"", "Could not read profiles.vti file!");

    // Profiles
    JsonObject obj = QtJson::parse(json, ok).toMap();
    foreach(QVariant var, obj["profile"].toList())
    {
        Profile p;
        p.fromJson(var);
        mProfiles.append(p);
    }

}

void MainWindow::profilesToFile(QString fn)
{
    fn = QApplication::applicationDirPath() + "/" + fn;
    if(QFile::exists(fn))
        QFile::remove(fn);

    JsonArray profiles;
    foreach (Profile f, mProfiles) {
        QVariant var;
        f.toJson(var);
        JsonObject obj = var.toMap();
        profiles.append(obj);
    }

    JsonObject obj;
    obj["profile"] = profiles;

    QString json = QtJson::serialize(obj);
    jsonToFile(json, fn);
}

void MainWindow::initUI()
{
    ui->cbBitrateV->addItems(mSetting.v_bitrates);
    ui->cbFramerateV->addItems(mSetting.v_framerates);
    ui->cbResolutionV->addItems(mSetting.v_resolutions);
    ui->cbBitrateA->addItems(mSetting.a_bitrates);
    ui->cbSamplerateA->addItems(mSetting.a_samplerates);
    ui->cbChannelA->addItems(mSetting.a_channels);

    QStringList pp, ff;
    foreach(Format var, mSetting.formats)
        ff << var.name;
    foreach(Profile var, mProfiles)
        pp << var.name;

    ui->cbFormat->addItems(ff);
    ui->cbProfile->addItems(pp);

    ui->previewVideo->setBackgroundRole(QPalette::Dark);
    ui->previewVideo->setAlignment(Qt::AlignCenter);
    ui->previewVideo->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->previewVideo->setAutoFillBackground(true);

    ui->tblSegments->setHorizontalHeaderItem(0, new QTableWidgetItem("From"));
    ui->tblSegments->setHorizontalHeaderItem(1, new QTableWidgetItem("To"));
    ui->tblSegments->setSelectionBehavior(QAbstractItemView::SelectRows);

    previewPlayer = new CMoviePlayer();
    ui->previewLayout->addWidget(previewPlayer);
}

void MainWindow::updateButtonsUI()
{
    bool isEnabled = isTranscoding == false;

    ui->centralWidget->setEnabled(isEnabled);
    ui->actionAddFolder->setEnabled(isEnabled);
    ui->actionAddFile->setEnabled(isEnabled);
    ui->actionImport->setEnabled(isEnabled);
    ui->actionStart->setEnabled(isEnabled);
    ui->actionStop->setEnabled(!isEnabled);

    isEnabled = isEnabled && mTasks.count() > 0;
    ui->actionStart->setEnabled(isEnabled);
    ui->actionExport->setEnabled(isEnabled);
    ui->actionApplyProfile->setEnabled(isEnabled);

}

void MainWindow::profileFromUI(Profile &p)
{
    DBG_FUNC;
    p.name = ui->cbProfile->currentText();
    p.format = ui->cbFormat->currentText();
    p.psnr = ui->ckPSNR->checkState();
    p.ssim = ui->ckSSIM->checkState();
    p.v_codec = ui->cbCodecV->currentText();
    p.v_bitrate = ui->cbBitrateV->currentText();
    p.v_framerate = ui->cbFramerateV->currentText();
    p.v_resolution = ui->cbResolutionV->currentText();
    p.a_codec = ui->cbCodecA->currentText();
    p.a_bitrate = ui->cbBitrateA->currentText();
    p.a_samplerate = ui->cbSamplerateA->currentText();
    p.a_channel = ui->cbChannelA->currentText();
    p.s_codec = ui->cbCodecS->currentText();
}

void MainWindow::profileToUI(Profile &p)
{
    DBG_FUNC;
    ui->cbProfile->setCurrentText(p.name);
    ui->cbFormat->setCurrentText(p.format);
    ui->ckPSNR->setChecked(p.psnr);
    ui->ckSSIM->setChecked(p.ssim);
    ui->cbCodecV->setCurrentText(p.v_codec);
    ui->cbBitrateV->setCurrentText(p.v_bitrate);
    ui->cbFramerateV->setCurrentText(p.v_framerate);
    ui->cbResolutionV->setCurrentText(p.v_resolution);
    ui->cbCodecA->setCurrentText(p.a_codec);
    ui->cbBitrateA->setCurrentText(p.a_bitrate);
    ui->cbSamplerateA->setCurrentText(p.a_samplerate);
    ui->cbChannelA->setCurrentText(p.a_channel);
    ui->cbCodecS->setCurrentText(p.s_codec);ui->cbBitrateV->update();
}

void MainWindow::taskFromUI(Task *t)
{
    DBG_FUNC;
    profileFromUI(t->profile);
    t->dstFolder = ui->edDestination->text();
    streamsFromView(t->streamsV,ui->videoView);
    streamsFromView(t->streamsA,ui->audioView);
    streamsFromView(t->streamsS,ui->subtitleView);
}

void MainWindow::taskToUI(Task *t)
{
    DBG_FUNC;
    previewPlayer->Stop();
    previewPlayer->Open(t->srcFile);
    previewPlayer->OnSeek(0);

    profileToUI(t->profile);
    ui->edDestination->setText(t->dstFolder);
    streamsToView(t->streamsV, ui->videoView);
    streamsToView(t->streamsA, ui->audioView);
    streamsToView(t->streamsS, ui->subtitleView);

    ui->previewFrame->setEnabled(true);
    ui->trimCropFrame->setEnabled(true);
    ui->chkConcat->setChecked(t->concat);
    this->applyCropSize(t);
    this->applySegments(t);
}

void MainWindow::appendTask(Task *t)
{
//    statusBar()->showMessage(t->name, 1000);
    mTasks.append(t);

    // add to task view
    TaskItemView *item_view = new TaskItemView(this);

    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(item_view->minimumSizeHint());
    item_view->initView(t);
    ui->taskView->addItem(item);
    ui->taskView->setItemWidget(item, item_view);
    ui->taskView->scrollToItem(item);
    ui->taskView->setCurrentItem(item);
    ui->taskView->repaint();

    connect(item_view, SIGNAL(taskRemoved(Task*)), this, SLOT(removeTask(Task*)));
    connect(item_view, SIGNAL(taskReported(Task*)), this, SLOT(reportTask(Task*)));

    updateButtonsUI();
}

void MainWindow::appendFiles(QStringList &files)
{
    if(files.length() == 0)
        return;

    foreach (QString fn, files) {

        Task *t = new Task(this);
        t->srcFile = fn;
        t->checkSource();

        taskFromUI(t);        

        appendTask(t);
    }
}

////////////////////////////// EVENT ///////////////////////////////
void MainWindow::exitApp()
{
    close();
}


void MainWindow::addFolder()
{
    QString path = QFileDialog::getExistingDirectory();
    if(path.isEmpty())
        return;

    QDir dir(path);

    QStringList filters;
    filters << "*.avi" << "*.mkv" << "*.mp4" << "*.mpg" << "*.mpeg" << "*.vob" << "*.m2ts" << "*.ogm" << "*.flv" << "*.mov";
    QStringList fileNames = dir.entryList(filters);
    QStringList files;
    foreach (QString fn, fileNames) {
        files << path + "/" + fn;
    }
    appendFiles(files);
}

void MainWindow::addFiles()
{
    QString videoFilter = "Videos(*.avi *.mkv *.mp4 *.mpg *.mpeg *.vob *.m2ts *.ogm *.flv *.mov)";
    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            "Select one or more files to add",
                            "",
                            videoFilter + ";;All Files (*.*)");

    appendFiles(files);
}

void MainWindow::removeTask(Task *t)
{
    int row = mTasks.indexOf(t);
    if(row != -1) {
        ui->taskView->takeItem(row);
        mTasks.removeAt(row);
        delete t;
    }

    updateButtonsUI();
}

void MainWindow::reportTask(Task *t)
{
    if(reportDlg)
        delete reportDlg;
    reportDlg = new ReportDialog(this);
    reportDlg->setWindowTitle(t->name);
    reportDlg->setReports(t->reports);
    reportDlg->show();

}

void MainWindow::importTasks()
{
    QString fn = QFileDialog::getOpenFileName(
                            this,
                            "Select file to import",
                            "",
                            "(*.vtp)");
    if(fn.isEmpty())
        return;

    QString json;
    jsonFromFile(json, fn);
    JsonObject obj = QtJson::parse(json).toMap();
    JsonArray tasks = obj["tasks"].toList();

    foreach (QVariant json, tasks) {
        Task *t = new Task;
        t->fromJson(json);
        appendTask(t);
    }
}

void MainWindow::exportTasks()
{    
    updateCurrentTask();
    QString fn = QFileDialog::getSaveFileName(this,
                                              "Select file to export",
                                              "",
                                              "*.vtp");

    if(fn.isEmpty())
        return;

    if(!fn.contains(".vtp"))
        fn += ".vtp";

    JsonArray tasks;
    foreach (Task *t, mTasks) {
        QVariant var;
        t->toJson(var);
        JsonObject obj = var.toMap();
        tasks.append(obj);
    }

    JsonObject obj;
    obj["tasks"] = tasks;

    QString json = QtJson::serialize(obj);
    jsonToFile(json, fn);

}

void MainWindow::changedProfile()
{
    DBG_FUNC;
    QString newProfile =  ui->cbProfile->currentText();
    foreach (Profile p, mProfiles) {
        if (p.name == newProfile) {
            profileToUI(p);
            return;
        }
    }
}

void MainWindow::changedFormat()
{
    DBG_FUNC;
    Format f = mSetting.formats[ui->cbFormat->currentIndex()];

    ui->cbCodecV->clear();
    ui->cbCodecA->clear();
    ui->cbCodecS->clear();

    ui->cbCodecV->addItems(f.v_codecs);
    ui->cbCodecA->addItems(f.a_codecs);
    ui->cbCodecS->addItems(f.s_codecs);

    ui->gbVideo->setEnabled(f.video);
    ui->gbAudio->setEnabled(f.audio);
    ui->gbSubtitle->setEnabled(f.subtitle);
}

void MainWindow::changedTask()
{
    DBG_FUNC;
    int old_row = curTaskRow;
    curTaskRow = ui->taskView->currentRow();

    qDebug() << old_row << "-" << curTaskRow;

    if(old_row >= 0 && old_row < mTasks.count())
        taskFromUI(mTasks[old_row]);

    if(curTaskRow >= 0 && curTaskRow < mTasks.count())
        taskToUI(mTasks[curTaskRow]);
}

void MainWindow::applyProfile()
{
    QString msg = "Are you sure to apply configuration to all video files?";
    int ret = QMessageBox::question(this, "Apply profile to all tasks", msg);

    if(ret == QMessageBox::No)
        return;

    DBG_FUNC;
    foreach (Task *t, mTasks) {
        profileFromUI(t->profile);
        t->dstFolder = ui->edDestination->text();
    }
}


void MainWindow::newProfile()
{
    bool ok;
    QString text = QInputDialog::getText(this,
                                         tr("New Profile"),
                                         tr("Profile Name:"), QLineEdit::Normal,
                                         "New Profile", &ok);
    if (ok && !text.isEmpty()) {
        Profile p;
        profileFromUI(p);
        p.name = text;

        mProfiles.append(p);
        ui->cbProfile->addItem(text);

        int idx = mProfiles.count() - 1;
        ui->cbProfile->setCurrentIndex(idx);

        profilesToFile(FILE_PROFILES);
    }

}

void MainWindow::removeProfile()
{
    QString name = ui->cbProfile->currentText();
    int ret = QMessageBox::question(this, tr("Remove Profile"),
                                   tr("Do you want to remove this profile?"),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);
    if(ret == QMessageBox::Yes) {
        int idx = ui->cbProfile->currentIndex();
        ui->cbProfile->removeItem(idx);

        mProfiles.removeAt(idx);
        if(idx >= mProfiles.count())
            idx--;
        ui->cbProfile->setCurrentIndex(idx);

        profilesToFile(FILE_PROFILES);
    }
}

void MainWindow::saveProfile()
{
    QString msg = "Do you want to save this profile?";
    int ret = QMessageBox::question(this, "Save Profile",
                                   msg,
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);
    if(ret == QMessageBox::No)
        return;

    int idx = ui->cbProfile->currentIndex();
    profileFromUI(mProfiles[idx]);

    profilesToFile(FILE_PROFILES);

}

void MainWindow::chooseDest()
{
    QString dir = QFileDialog::getExistingDirectory(this, QString(), ui->edDestination->text());
    if(dir.isEmpty())
        return;

    ui->edDestination->setText(dir);
}

void MainWindow::updateCurrentTask()
{
    Task *t = this->getCurrentTask();
    if (t) taskFromUI(t);
}

void MainWindow::startNewTask(int index)
{
    if(index < 0 || index >= mTasks.count()) {
        stopTranscode();
        return;
    }

    QListWidgetItem *item = ui->taskView->item(index);
    ui->taskView->scrollToItem(item);
    ui->taskView->setCurrentItem(item);

    curTask = mTasks[index];
    mSetting.findFormat(curTask->profile.format, curTask->format);
    curTask->startTranscode();
}


void MainWindow::startTranscode()
{
    isTranscoding = true;
    updateCurrentTask();
    updateButtonsUI();

    timer->start(300);
    startNewTask(0);
}

void MainWindow::stopTranscode()
{
    isTranscoding = false;
    updateButtonsUI();

    timer->stop();

    if(curTask)
        curTask->stopTranscode();

    curTask = NULL;
}

void MainWindow::transcodeTasks()
{
    if(!curTask)
        return;

    int row = mTasks.indexOf(curTask);

    QListWidgetItem *item = ui->taskView->item(row);
    TaskItemView *item_view = (TaskItemView*)ui->taskView->itemWidget(item);
    item_view->updateProgress();

    if(curTask->status == TS_FINISH) {
        item_view->showResult();
        startNewTask(++row);
    }
}

/////////////////////////////////////////////////////////////

void MainWindow::on_cbBitrateV_editTextChanged(const QString &arg1)
{
    QRegExp rx("^[0-9]*[k]?$");
    if (arg1 != "default" && rx.exactMatch(arg1) == false) {
        QMessageBox::critical(this,"", "Wrong format for video bitrate.");
        ui->cbBitrateV->clearEditText();
    }
}

void MainWindow::on_cbBitrateA_editTextChanged(const QString &arg1)
{
    QRegExp rx("^[0-9]*[k]?$");
    if (arg1 != "default" && rx.exactMatch(arg1) == false) {
        QMessageBox::critical(this,"", "Wrong format for audio bitrate.");
        ui->cbBitrateA->clearEditText();
    }
}

void MainWindow::on_btnSegFrom_clicked()
{
    Task *task = this->getCurrentTask();
    int segmentIndex = ui->tblSegments->currentRow();
    int start = previewPlayer->CurrentTime();

    if (task && segmentIndex >= 0) {
        task->updateSegmentStart(segmentIndex, previewPlayer->CurrentTime());
        ui->tblSegments->item(segmentIndex, 0)->setText(Utils::GetTimeString(start));

        setPreviewSegment();
    }
}

void MainWindow::on_btnSegTo_clicked()
{
    Task *task = this->getCurrentTask();
    int segmentIndex = ui->tblSegments->currentRow();
    int finish = previewPlayer->CurrentTime();

    if (task && segmentIndex >= 0) {
        task->updateSegmentFinish(segmentIndex, previewPlayer->CurrentTime());
        ui->tblSegments->item(segmentIndex, 1)->setText(Utils::GetTimeString(finish));

        setPreviewSegment();
    }
}

void MainWindow::on_btnSegDel_clicked()
{
    Task *task = this->getCurrentTask();
    int segmentIndex = ui->tblSegments->currentRow();
    if (segmentIndex < 0) return;

    task->removeSegment(segmentIndex);
    ui->tblSegments->removeRow(segmentIndex);
    if (ui->tblSegments->rowCount() == 0) {
        previewPlayer->SetSegment(0, previewPlayer->GetDuration());
        return;
    }

    if (segmentIndex == 0) {
        ui->tblSegments->selectRow(0);
    } else {
        ui->tblSegments->selectRow(segmentIndex - 1);
    }

    setPreviewSegment();
}

void MainWindow::on_btnSegAdd_clicked()
{
    Task *task = this->getCurrentTask();
    if(task) {
        int start = previewPlayer->CurrentTime();
        int finish = previewPlayer->GetDuration();
        task->addSegment(start, finish);
        int rowIndex = this->addSegment(start, finish);
        ui->tblSegments->selectRow(rowIndex);
        previewPlayer->SetSegment(start, finish);
    }
}

void MainWindow::on_cropTop_editingFinished()
{
    Task *task = this->getCurrentTask();
    if(task)
    {
        if (ui->cropTop->value() + ui->cropBottom->value() > task->streamsV[0].height.toInt()) {
            ui->cropTop->setValue(task->cropTop);
        } else {
            task->cropTop = ui->cropTop->value();
            this->applyCropSize(task);
        }
    }
}

void MainWindow::on_cropLeft_editingFinished()
{
    Task *task = this->getCurrentTask();
    if(task)
    {
        if (ui->cropLeft->value() + ui->cropRight->value() > task->streamsV[0].width.toInt()) {
            ui->cropLeft->setValue(task->cropLeft);
        } else {
            task->cropLeft = ui->cropLeft->value();
            this->applyCropSize(task);
        }
    }
}

void MainWindow::on_cropRight_editingFinished()
{
    Task *task = this->getCurrentTask();
    if(task)
    {
        if (ui->cropLeft->value() + ui->cropRight->value() > task->streamsV[0].width.toInt()) {
            ui->cropRight->setValue(task->cropRight);
        } else {
            task->cropRight = ui->cropRight->value();
            this->applyCropSize(task);
        }
    }
}

void MainWindow::on_cropBottom_editingFinished()
{
    Task *task = this->getCurrentTask();
    if(task)
    {
        if (ui->cropTop->value() + ui->cropBottom->value() > task->streamsV[0].height.toInt()) {
            ui->cropBottom->setValue(task->cropBottom);
        } else {
            task->cropBottom = ui->cropBottom->value();
            this->applyCropSize(task);
        }
    }
}

void MainWindow::on_cropTop_valueChanged(int arg1)
{
    this->on_cropTop_editingFinished();
}

void MainWindow::on_cropLeft_valueChanged(int arg1)
{
    this->on_cropLeft_editingFinished();
}

void MainWindow::on_cropBottom_valueChanged(int arg1)
{
    this->on_cropBottom_editingFinished();
}

void MainWindow::on_cropRight_valueChanged(int arg1)
{
    this->on_cropRight_editingFinished();
}

void MainWindow::on_tblSegments_itemSelectionChanged()
{
    setPreviewSegment();
}

void MainWindow::on_chkConcat_toggled(bool checked)
{
    Task *task = this->getCurrentTask();

    if (task) {
        task->concat = checked;
    }
}

void MainWindow::applyCropSize(Task *t)
{
    Task_Stream stream = t->streamsV.at(0);

    ui->cropTop->setValue(t->cropTop);
    ui->cropRight->setValue(t->cropRight);
    ui->cropBottom->setValue(t->cropBottom);
    ui->cropLeft->setValue(t->cropLeft);
    int croppedWidth = stream.width.toInt() - t->cropLeft - t->cropRight;
    int croppedHeight = stream.height.toInt() - t->cropTop - t->cropBottom;
    ui->txtCroppedSize->setText(QString("%1 x %2").arg(QString::number(croppedWidth), QString::number(croppedHeight)));

    previewPlayer->Crop(ui->cropTop->value(),
                        ui->cropRight->value(),
                        ui->cropBottom->value(),
                        ui->cropLeft->value());
}

void MainWindow::applySegments(Task *t)
{
    ui->tblSegments->setRowCount(0);
    for (int i=0; i<t->segments.count(); i++) {
        Task_Segment *segment = t->segments.at(i);
        this->addSegment(segment->ms_start, segment->ms_end);
    }
}

Task* MainWindow::getCurrentTask()
{
    int taskIndex = ui->taskView->currentRow();
    Task *task = NULL;

    if(taskIndex >= 0 && taskIndex < mTasks.count())
    {
        task = mTasks.at(taskIndex);
    }

    return task;
}

void MainWindow::setPreviewSegment()
{
    Task *task = this->getCurrentTask();
    int segmentIndex = ui->tblSegments->currentRow();

    if (task && segmentIndex >= 0 and task->segments.count() > segmentIndex) {
        int start = task->segments.at(segmentIndex)->ms_start;
        int finish = task->segments.at(segmentIndex)->ms_end;
        previewPlayer->SetSegment(start, finish);
    }
}

int MainWindow::addSegment(int start, int end)
{
    int rowIndex = ui->tblSegments->rowCount();
    ui->tblSegments->insertRow(rowIndex);
    QTableWidgetItem *itemStart = new QTableWidgetItem(Utils::GetTimeString(start));
    QTableWidgetItem *itemEnd = new QTableWidgetItem(Utils::GetTimeString(end));
    itemStart->setFlags(itemStart->flags() ^ Qt::ItemIsEditable);
    itemEnd->setFlags(itemEnd->flags() ^ Qt::ItemIsEditable);
    ui->tblSegments->setItem(rowIndex, 0, itemStart);
    ui->tblSegments->setItem(rowIndex, 1, itemEnd);

    return rowIndex;
}
    i,cÒ+•!^’ÑŒ‚¿öŒ«eE‚%Å¯úÿ£ílş=ñ³§ÁrMÕÙV¤ëîŞ4¥jPÿ6²rO‚êÑ?räŒnÃö´Õ¾ãM³ÔHìÊ‘ŒØš´ªé<®0lÉYz«+:Q¦¬c™ú©MVcøú°w‹såA\VŠıµq=o·FÈÇ©€è¯Z¤âÁSÈşF ‹”¬¤ÀÙÁ« ‹µi
<,X6ï|©î’eY¤Sø¨ÕQ_