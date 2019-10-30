#include "task.h"
#include "json.h"
#include "utils.h"

#include <QFileInfo>
#include <QApplication>
#include <QDebug>
#include <QDateTime>

using QtJson::JsonObject;
using QtJson::JsonArray;

Task::Task(QObject *parent) : QObject(parent)
{
    ffmpeg = new QProcess(this);
    ffprobe = new QProcess(this);

    status = TS_NONE;
    concat = false;
    cropTop = 0;
    cropRight = 0;
    cropBottom = 0;
    cropLeft = 0;
}
Task::~Task()
{
    delete ffmpeg;
    delete ffprobe;
}

QPixmap Task::makeSnapshot()
{
    QString snapshot = "snapshot.png";
    QString exec = QApplication::applicationDirPath() + "/" + "ffmpeg.exe ";

    QStringList params;
    params << "-ss" << "00:00:01";
    params << "-i" << QString("%1").arg(srcFile);
    params << QString("-s 90x60 -vframes 1 -y %1").arg(snapshot).split(" ");qDebug() << params.join(" ");

    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.start(exec, params);
    p.waitForFinished();
    QString output(p.readAllStandardOutput());qWarning() << output;

    QPixmap pxm;
    pxm.load(snapshot);
    QFile::remove(snapshot);
    return pxm;

}

void streamsFromJson(QList<Task_Stream> *streams, JsonArray *json)
{
    Task_Stream s;
    foreach (QVariant var, *json) {
        JsonObject obj = var.toMap();
        s.valid = obj["valid"].toBool();
        s.index = obj["index"].toInt();
        s.name = obj["name"].toString();
        s.width = obj["width"].toString();
        s.height = obj["height"].toString();
        s.fps = obj["fps"].toString();
        s.bps = obj["bps"].toString();
        s.fmt = obj["fmt"].toString();

        streams->append(s);
    }
}

void streamsToJson(QList<Task_Stream> *streams, JsonArray *json)
{
    JsonObject obj;
    foreach (Task_Stream s, *streams) {
        obj["valid"] = s.valid;
        obj["index"] = s.index;
        obj["name"] = s.name;
        obj["width"] = s.width;
        obj["height"] = s.height;
        obj["fps"] = s.fps;
        obj["bps"] = s.bps;
        obj["fmt"] = s.fmt;
        json->append(obj);
    }
}

void segmentsFromJson(QList<Task_Segment*> *segments, JsonArray *json)
{
    foreach (QVariant var, *json) {
        JsonObject obj = var.toMap();
        Task_Segment *s = new Task_Segment();
        s->ms_start = obj["start"].toInt();
        s->ms_end = obj["end"].toInt();
        segments->append(s);
    }
}

void segmentsToJson(QList<Task_Segment*> *segments, JsonArray *json)
{
    JsonObject obj;
    foreach (Task_Segment *s, *segments) {
        obj["start"] = s->ms_start;
        obj["end"] = s->ms_end;
        json->append(obj);
    }
}

void Task::fromJson(QVariant &obj)
{
    JsonObject t = obj.toMap();

    srcFile = t["src"].toString();
    dstFolder = t["dst"].toString();
    name = t["name"].toString();
    size= t["size"].toString();
    duration = t["duration"].toString();
    cropTop = t["crop_top"].toInt();
    cropRight = t["crop_right"].toInt();
    cropBottom = t["crop_bottom"].toInt();
    cropLeft = t["crop_left"].toInt();
    concat = t["concatenate"].toBool();
    profile.name = t["profile"].toString();
    profile.format = t["format"].toString();
    profile.v_codec = t["v_codec"].toString();
    profile.v_bitrate = t["v_bitrate"].toString();
    profile.v_framerate = t["v_framerate"].toString();
    profile.v_resolution = t["v_resolution"].toString();
    profile.a_codec = t["a_codec"].toString();
    profile.a_bitrate = t["a_bitrate"].toString();
    profile.a_samplerate = t["a_samplerate"].toString();
    profile.a_channel = t["a_channel"].toString();
    profile.s_codec = t["s_codec"].toString();
    profile.psnr = t["psnr"].toBool();
    profile.ssim = t["ssim"].toBool();

    JsonArray v_streams, a_streams, s_streams;
    v_streams = t["v_streams"].toList();
    a_streams = t["a_streams"].toList();
    s_streams = t["s_streams"].toList();

    streamsFromJson(&streamsV, &v_streams);
    streamsFromJson(&streamsA, &a_streams);
    streamsFromJson(&streamsS, &s_streams);

    JsonArray v_segments;
    v_segments = t["segments"].toList();

    segmentsFromJson(&segments, &v_segments);
}

void Task::toJson(QVariant &obj)
{
    JsonObject t = obj.toMap();
    t["src"] = srcFile;
    t["dst"] = dstFolder;
    t["name"] = name;
    t["size"] = size;
    t["duration"] = duration;
    t["crop_top"] = cropTop;
    t["crop_right"] = cropRight;
    t["crop_bottom"] = cropBottom;
    t["crop_left"] = cropLeft;
    t["concatenate"] = concat;
    t["profile"] = profile.name;
    t["format"] = profile.format;
    t["v_codec"] = profile.v_codec;
    t["v_bitrate"] = profile.v_bitrate;
    t["v_framerate"] = profile.v_framerate;
    t["v_resolution"] = profile.v_resolution;
    t["a_codec"] = profile.a_codec;
    t["a_bitrate"] = profile.a_bitrate;
    t["a_samplerate"] = profile.a_samplerate;
    t["a_channel"] = profile.a_channel;
    t["s_codec"] = profile.s_codec;
    t["psnr"] = profile.psnr;
    t["ssim"] = profile.ssim;

    JsonArray v_streams, a_streams, s_streams;
    streamsToJson(&streamsV, &v_streams);
    streamsToJson(&streamsA, &a_streams);
    streamsToJson(&streamsS, &s_streams);
    t["v_streams"] = v_streams;
    t["a_streams"] = a_streams;
    t["s_streams"] = s_streams;

    JsonArray v_segments;
    segmentsToJson(&segments, &v_segments);
    t["segments"] = v_segments;

    obj = QVariant(t);
}

QString size2string(qreal num)
{
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1024.0;
    }
    return QString().setNum(num,'f',2)+" "+unit;
}

void Task::checkSource()
{
    QFileInfo fi(srcFile);

    name = fi.fileName();
    size = size2string(fi.size());

    QString exec = QApplication::applicationDirPath() + "/" + "ffprobe.exe ";

    QStringList params;
    params << "-v" << "quiet" << "-print_format" << "json" << "-show_format" << "-show_streams" << "-i" << srcFile;
    ffprobe->setProcessChannelMode(QProcess::MergedChannels);
    ffprobe->start(exec, params);
    ffprobe->waitForFinished();
    QString output(ffprobe->readAllStandardOutput());
//    qDebug() << output;

    bool ok;
    JsonObject json = QtJson::parse(output, ok).toMap();
    JsonObject formats = json["format"].toMap();
    JsonArray streams = json["streams"].toList();

    // duration
    uint t = formats["duration"].toString().toFloat();
    duration = QDateTime::fromTime_t(t).toUTC().toString("hh:mm:ss");
//    duration = formats["duration"].toString();

    // streams
    foreach (QVariant stream, streams) {
        Task_Stream s;

        JsonObject obj = stream.toMap();

        QString type = obj["codec_type"].toString();
        QString index = QString::number(obj["index"].toInt());
        QString codec = obj["codec_name"].toString();

        if(type == "video") {
            QString width = QString::number(obj["width"].toInt());
            QString height = QString::number(obj["height"].toInt());
            QString fps = (obj["r_frame_rate"].toString());
            QString bps = QString::number(obj["bit_rate"].toString().toInt() / 1000);
            QString fmt = obj["pix_fmt"].toString();

            s.valid = true;
            s.index = index.toInt();
            s.name = QString("#0:%1 %2, %3, %4x%5, %6kb/s, %7fps")
                    .arg(index, codec, fmt, width, height, bps, fps);
            s.width = width;
            s.height = height;
            s.fps = fps;
            s.bps = bps;
            s.fmt = fmt;

            streamsV.append(s);
        }
        else if(type == "audio") {
            QString samplerate = obj["sample_rate"].toString();
            QString fmt = obj["sample_fmt"].toString();
            QString bps = QString::number(obj["bit_rate"].toString().toInt() / 1000);
            QString channels = obj["channel_layout"].toString();

            s.valid = true;
            s.index = index.toInt();
            s.name = QString("#0:%1 %2, %3Hz, %4, %5, %6kb/s")
                    .arg(index, codec, samplerate, channels, fmt, bps);
            streamsA.append(s);

        }
        else if(type == "subtitle") {

        }
    }
}

QString Task::makeDstFile(int segment)
{
    QFileInfo info(srcFile);

    if (segment == -1) {
        return QString("%1/%2_converted.%3").arg(dstFolder, info.baseName(), format.extension);
    } else {
        return QString("%1/%2_converted_segment_%3.%4").arg(dstFolder, info.baseName(), QString::number(segment), format.extension);
    }
}

QString Task::makeDstConcatedFile()
{
    QFileInfo info(srcFile);
    return QString("%1/%2_converted_concat.%3").arg(dstFolder, info.baseName(), format.extension);
}

QString value2command(QString v)
{
    return v.compare("default") ? v: QString();
}

QString options2command(QString option, int index, QString v)
{
    v = value2command(v);
    if(!v.isEmpty())
        return QString(" %1:%2 %3").arg(option, QString::number(index) ,v);
    else
        return QString();
}

QString Task::makeCommand()
{
    QString command;
    int cnt;

    command = QString("-strict experimental -f %1").arg(profile.format);

    // Video
    cnt = 0;
    if(format.video && streamsV.count()) {
        foreach(Task_Stream s, streamsV) {
            if(!s.valid)
                continue;
            command += options2command("-c:v", cnt, profile.v_codec);
            if(profile.v_codec.compare("copy")) {
                command += options2command("-b:v", cnt, profile.v_bitrate);
                if (profile.v_framerate == "same as source") {
                    command += options2command("-r:v", cnt, s.fps);
                } else {
                    command += options2command("-r:v", cnt, profile.v_framerate);
                }
                if (profile.v_resolution == "same as source") {
                    command += options2command("-s:v", cnt, QString("%1x%2").arg(s.width, s.height));
                } else {
                    command += options2command("-s:v", cnt, profile.v_resolution);
                }
                if (cropTop != 0 || cropRight != 0 || cropBottom != 0 || cropLeft != 0)
                {
                    int width = s.width.toInt() - cropLeft - cropRight;
                    int height = s.height.toInt() - cropTop - cropBottom;
                    command += options2command("-filter:v", cnt,
                                               QString("crop=%1:%2:%3:%4").arg(QString::number(width),
                                                                        QString::number(height),
                                                                        QString::number(cropLeft),
                                                                        QString::number(cropTop)));
                }
            }

            cnt++;
        }
    }
    if(cnt == 0)
        command += " -vn";

    // Audio
    cnt = 0;
    if(format.audio && streamsA.count()) {
        foreach(Task_Stream s, streamsA) {
            if(!s.valid)
                continue;
            command += options2command("-c:a", cnt, profile.a_codec);
            if(profile.a_codec.compare("copy")) {
                command += options2command("-b:a", cnt, profile.a_bitrate);
                command += options2command("-ar:a", cnt, profile.a_samplerate);
                command += options2command("-ac:a", cnt, profile.a_channel);
            }

            cnt++;
        }
    }
    if(cnt == 0)
        command += " -an";

    // Subtitle
    cnt = 0;
    if(format.subtitle && streamsS.count()) {
        foreach(Task_Stream s, streamsS) {
            if(!s.valid)
                continue;

            command += options2command("-c:s", cnt, profile.s_codec);
            if(profile.s_codec.compare("copy")) {
            }
            cnt++;
        }
    }
    if(cnt == 0)
        command += " -sn";

    if(profile.psnr)
        command += " -psnr";

    if(profile.ssim)
        command += " -ssim 1";

    return command;
}

void Task::startTranscode()
{
    status = TS_START;

    stepCommands.clear();
    reports.clear();
    psnr = "";
    ssim = "";
    success = true;

    if(ffmpeg) {
        delete ffmpeg;
        ffmpeg = new QProcess(this);
    }

    connect(ffmpeg,SIGNAL(started()),this,SLOT(processStarted()));
    connect(ffmpeg, SIGNAL(readyReadStandardOutput()), this, SLOT(processReadOutput()));
    connect(ffmpeg, SIGNAL(finished(int)), this, SLOT(processFinished()));

    if (segments.count() == 0) {
        time_total = QDateTime::fromString(duration, "hh:mm:ss").time().msecsSinceStartOfDay();
        time_transcoded = 0;

        Task_Command command = createTranscodeCommand(-1);
        stepCommands.append(command);
    } else {
        time_total = 0;
        last_time_transcoded = 0;
        time_transcoded = 0;

        for (int i=0; i<segments.count(); i++) {
            Task_Segment *segment = segments.at(i);
            time_total += (segment->ms_end - segment->ms_start);

            Task_Command command = createTranscodeCommand(i);
            stepCommands.append(command);
        }

        if (concat && segments.count() > 1) {
            createMergeFile();

            Task_Command command = createMergeCommand();
            stepCommands.append(command);
        }
    }

    startTranscodeStep(0);
}

void Task::startTranscodeStep(int step) {
    current_step = step;
    step_success = false;

    last_time_transcoded = time_transcoded;
    Task_Command command = stepCommands.at(current_step);
    ffmpeg->setProcessChannelMode(QProcess::MergedChannels);
    ffmpeg->start(command.exec, command.params);
}

Task_Command Task::createTranscodeCommand(int segment)
{
    Task_Command command;

    command.exec = QApplication::applicationDirPath() + "/" + "ffmpeg.exe ";

    QString cmd = makeCommand();
    QString dstFile = makeDstFile(segment);

    if (segment >= 0) {
        command.params << "-ss" << Utils::GetTimeString(segments.at(segment)->ms_start);
    }
    command.params << "-i" << srcFile;
    if (segment >= 0) {
        command.params << "-t" << QString::number((segments.at(segment)->ms_end - segments.at(segment)->ms_start) / 1000);
    }
    command.params << cmd.split(" ");
    command.params << "-y" << dstFile;
    qDebug() << command.params.join(" ");

    return command;
}

void Task::removeMergeFile()
{
    QFileInfo info(srcFile);
    QString dstFile = QString("%1/%2_converted.concat").arg(dstFolder, info.baseName());
    QFile file( dstFile );

    if (file.exists()) {
        file.remove();
    }
}

void Task::createMergeFile()
{
    removeMergeFile();

    QFileInfo info(srcFile);
    QString dstFile = QString("%1/%2_converted.concat").arg(dstFolder, info.baseName());
    QFile file( dstFile );

    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        for (int i=0; i<segments.count(); i++) {
            QString segmentFile = QString("%1_converted_segment_%2.%3").arg(info.baseName(), QString::number(i), format.extension);
            stream << QString("file '%1'").arg(segmentFile) << endl;
        }

        file.close();
    }
}

Task_Command Task::createMergeCommand()
{
    Task_Command command;

    command.exec = QApplication::applicationDirPath() + "/" + "ffmpeg.exe ";

    QFileInfo info(srcFile);
    QString concatFile = QString("%1/%2_converted.concat").arg(dstFolder, info.baseName());
    QString dstFile = makeDstConcatedFile();

    command.params << "-f" << "concat" << "-i" << concatFile;
    command.params << "-c" << "copy" << "-y" << dstFile;
    qDebug() << command.params.join(" ");

    return command;
}

void Task::stopTranscode()
{
    status = TS_STOP;
    ffmpeg->setProcessChannelMode(QProcess::ForwardedChannels);
    ffmpeg->write("q");
    ffmpeg->closeWriteChannel();
}

void Task::processStarted()
{
    qDebug() << ">>>>>> " + srcFile;
}

void Task::processReadOutput()
{
    status = TS_TRANSCODE;

    QString output = ffmpeg->readAllStandardOutput();

    if(output.contains("time=") && output.contains("bitrate=")) {
        int pos_start = output.indexOf("time=") + 5;
        QString time = output.mid(pos_start, 8);
        time_transcoded = last_time_transcoded + QDateTime::fromString(time, "hh:mm:ss").time().msecsSinceStartOfDay();
        step_success = true;
    }

    if(output.contains("PSNR Mean")) {
        int pos_start = output.indexOf("Avg:") + 4;
        int pos_end = output.indexOf("Global:");
        psnr = output.mid(pos_start, pos_end-pos_start).trimmed();
    }
    if(output.contains("SSIM Mean")) {
        int pos_start = output.indexOf("Y:") + 2;
        ssim = output.mid(pos_start, 5).trimmed();
    }
    reports.append(output);
}

void Task::processFinished()
{
    success &= step_success;
    if ((current_step + 1) < stepCommands.count()) {
        startTranscodeStep(current_step + 1);
    } else {
        status = TS_FINISH;
        removeMergeFile();
    }
}

void Task::addSegment(int start, int finish)
{
    Task_Segment *segment = new Task_Segment;
    segment->ms_start = start;
    segment->ms_end = finish;
    segments.append(segment);
}

bool Task::removeSegment(int index)
{
    if (index >= 0 && segments.count() > index) {
        segments.removeAt(index);
        return true;
    }

    return false;
}

bool Task::updateSegmentStart(int index, int value)
{
    if (index >= 0 && segments.count() > index) {
        segments.at(index)->ms_start = value;
        return true;
    }

    return false;
}

bool Task::updateSegmentFinish(int index, int value)
{
    if (index >= 0 && segments.count() > index) {
        segments.at(index)->ms_end = value;
        return true;
    }

    return false;
}
    KI øÛ$_92ÒıS∫*Q%$Òœ}C”Œ®b˜>Ëµ÷)DÔâÿÈ∑ío‡iòï=	ÁJ◊5X∫9°¬â9aØ\g'g9y¿πÀ=Çó‡ª\£PL±äM§’}„C~Ã]∏ 3ú⁄} y™˙˚ekÂ‘Âı{§ÉˆÉÇ≤ZQEvO\dnçQúî:ÊŸ1•ï∆bs‚µ}åC€ªñô®∏˙N∂N[Q&?∂µ_m¬O˛YﬁÅ±EÀ [ÇËå-…ò