#ifndef REPORTDIALOG_H
#define REPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ReportDialog;
}

class ReportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReportDialog(QWidget *parent = 0);
    ~ReportDialog();

public:
    void setReports(QString &reports);

private:
    Ui::ReportDialog *ui;
};

#endif // REPORTDIALOG_H
    ���^}n�rRо4�%ֆ�rq6�Q�P��Y���]K��=Lh�f�_��O4�俰) �d*^���xE�;���2��XN{<<������W6��N�����]���}�!NO����%���J�l���G�eGb	��G c�i)��o�d��׳�k@����5^�(T�D[C�pZ��	z}V�)@