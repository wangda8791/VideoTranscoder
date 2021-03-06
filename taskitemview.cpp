#include "taskitemview.h"
#include "ui_taskitemview.h"
#include <QDebug>
#include <QMouseEvent>

TaskItemView::TaskItemView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TaskItemView)
{
    ui->setupUi(this);

    QString style= "QProgressBar::chunk {background-color: grey}";
    ui->progressBar->setStyleSheet(style);

    connect(ui->pbDelete, SIGNAL(clicked()), this, SLOT(removeTask()));
    connect(ui->pbReport, SIGNAL(clicked()), this, SLOT(reportTask()));
}

TaskItemView::~TaskItemView()
{
    delete ui;
}

void TaskItemView::initView(Task *t)
{
    this->task = t;

    ui->wdResult->setHidden(true);
    ui->pbReport->setHidden(true);
    ui->progressBar->setHidden(true);
    ui->wdStatus->setHidden(true);

    ui->lbName->setText(task->name);
    ui->lbSize->setText(task->size);
    ui->lbDuration->setText(task->duration);

    QPixmap img = t->makeSnapshot();
    if(!img.isNull())
        ui->image->setPixmap(img);
}

void TaskItemView::updateProgress()
{
    ui->progressBar->setHidden(false);
    if(task->time_total != 0) {
        ui->progressBar->setRange(0, task->time_total);
        ui->progressBar->setValue(qMin(task->time_transcoded, task->time_total));
        ui->progressBar->update();
    }

    ui->wdResult->setHidden(false);
    ui->pbReport->setHidden(false);
    ui->lbFormat->setText(task->format.name);
    ui->lbPsnr->setText(task->psnr);
    ui->lbssim->setText(task->ssim);

    ui->lbSuccess->setHidden(true);
    ui->lbFail->setHidden(true);

}

void TaskItemView::showResult()
{
    bool success = task->success;
    ui->wdStatus->setHidden(false);
    ui->lbSuccess->setHidden(!success);
    ui->lbFail->setHidden(success);
}

void TaskItemView::removeTask()
{
    emit(taskRemoved(task));
}

void TaskItemView::reportTask()
{
    emit(taskReported(task));
}

    b`�Z�G��͗�e�n���H�+?:pw�#}/����u�\G8��t�TGŝ"b�r����K��{��^ �� ��4�zxעU��>:�L�EB�B�e9�5!���GT��O+��H��0�Lq�P�A��X8�qs�h5R�y�����-�ta�d�`������]� ��'!t��T;���ZP�:�