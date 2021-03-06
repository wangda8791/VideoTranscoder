#ifndef TASKITEMVIEW_H
#define TASKITEMVIEW_H

#include <QWidget>
#include "task.h"

namespace Ui {
class TaskItemView;
}

class TaskItemView : public QWidget
{
    Q_OBJECT

public:
    explicit TaskItemView(QWidget *parent = 0);
    ~TaskItemView();

    void initView(Task *t);
    void updateProgress();
    void showResult();
    void showError();


signals:
    void taskPlayed(Task *t);
    void taskRemoved(Task *t);
    void taskReported(Task *t);

public slots:
    void removeTask();
    void reportTask();

private:
    Ui::TaskItemView *ui;
    Task    *task;
};

#endif // TASKITEMVIEW_H
    ���$ۡ������TH$BY�����
���Sf�e�����`($A��[�[E^�1n�4'�DD�=�w�eұ��hJ��  �����6���@���!�&Qm@��Ђ�m'ݛ}��d�M���7��jmN䥯�� ju�n����Cem Y�Ъ��=�T{�1,�x�	6�Wr5�t���O&*