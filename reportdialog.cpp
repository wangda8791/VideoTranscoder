#include "reportdialog.h"
#include "ui_reportdialog.h"

ReportDialog::ReportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReportDialog)
{
    ui->setupUi(this);
}

ReportDialog::~ReportDialog()
{
    delete ui;
}

void ReportDialog::setReports(QString &reports)
{
    ui->plainTextEdit->setPlainText(reports);

}
    ��7���M9+`˳�����RmIKK�����~��4Q��p�+Z�bO�0�T[��y��=�9���{�_fǛ=�p#��,�p����L�`r�åO�z�l�s�H6��SS�K�-gA[� '��|:�,�V���u^��L:PfWc.C������-���ܓ(bI������`Q4҂�D-