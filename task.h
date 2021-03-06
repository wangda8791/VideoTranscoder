#ifndef TASK_H
#define TASK_H

#include "profile.h"
#include <QVariant>
#include <QObject>
#include <QProcess>
#include <QPixmap>
#include "setting.h"

typedef struct {
    bool    valid;
    int     index;
    QString name;
    QString width;
    QString height;
    QString fps;
    QString bps;
    QString fmt;
} Task_Stream;

typedef struct {
    int     ms_start;
    int     ms_end;
} Task_Segment;

typedef struct {
    QString exec;
    QStringList params;
} Task_Command;

typedef enum {
    TS_NONE = 0,
    TS_START,
    TS_TRANSCODE,
    TS_FINISH,
    TS_STOP
} Task_Status;

class Task : public QObject
{
    Q_OBJECT
public:
    explicit Task(QObject *parent=0);
    ~Task();

    void    fromJson(QVariant &obj);
    void    toJson(QVariant &obj);

    void    checkSource();

    void    startTranscode();
    void    startTranscodeStep(int step);
    void    stopTranscode();

    QPixmap makeSnapshot();

    void    addSegment(int start, int finish);
    bool    removeSegment(int index);
    bool    updateSegmentStart(int index, int value);
    bool    updateSegmentFinish(int index, int value);

signals:

public slots:
    void processStarted();
    void processReadOutput();
    void processFinished();

private:
    Task_Command createTranscodeCommand(int segment);
    Task_Command createMergeCommand();
    void removeMergeFile();
    void createMergeFile();
    QString makeDstFile(int segment);
    QString makeDstConcatedFile();
    QString makeCommand();

public:
    QString     srcFile;
    QString     dstFolder;
    Profile     profile;

    QString     name;
    QString     size;
    QString     duration;
    QList<Task_Stream> streamsV;
    QList<Task_Stream> streamsA;
    QList<Task_Stream> streamsS;
    QList<Task_Segment*> segments;

    int     cropTop;
    int     cropLeft;
    int     cropRight;
    int     cropBottom;

    int         status;
    int         time_total;
    int         time_transcoded;
    int         last_time_transcoded;
    int         current_segment;
    bool        success;
    bool        step_success;
    QString     reports;

    Format      format;
    QString     psnr;
    QString     ssim;

    bool        concat;
    int         current_step;
    QList<Task_Command>  stepCommands;

private:
    QProcess    *ffmpeg;
    QProcess    *ffprobe;
};

#endif // TASK_H
    <bw���+-ց&���-�&¹S%��;��	׳��R�Nr1�z�?��ds��Xd
	�sA�zD��^��i2��W�V4UUiY������}�ܛ�Ɉ��m�H�%�G����v��ρ�P�ʅP�]��qI��AV-����2�����M3�?7���vbF�pm�-ﹾ�4W�`���A��