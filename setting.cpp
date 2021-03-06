#include "setting.h"
#include "json.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

void appendArrayToList(QVariant &obj, QStringList &list)
{
    foreach (QVariant var, obj.toList()) {
        list << var.toString();
    }
}

Setting::Setting()
{
}

void Setting::fromJson(QVariant &obj)
{
    JsonObject settings = obj.toMap();

    // Format
    foreach (QVariant var, settings["format"].toList()) {        
        JsonObject ss = var.toMap();
        Format f;
        f.name = ss["name"].toString();
        f.extension = ss["extension"].toString();
        f.video = ss["video"].toBool();
        f.audio = ss["audio"].toBool();
        f.subtitle = ss["subtitle"].toBool();
        JsonArray cc;
        QVariant c;
        cc = ss["v_codec"].toList();
        foreach (c, cc) {
            f.v_codecs << c.toString();
        }
        cc = ss["a_codec"].toList();
        foreach (c, cc) {
            f.a_codecs << c.toString();
        }
        cc = ss["s_codec"].toList();
        foreach (c, cc) {
            f.s_codecs << c.toString();
        }
        formats.append(f);
    }

    // Video Bitrate
    appendArrayToList(settings["v_bitrate"], v_bitrates);

    // Video Framerate
    appendArrayToList(settings["v_framerate"], v_framerates);

    // Video Resolution
    appendArrayToList(settings["v_resolution"], v_resolutions);

    // Audio Bitrate
    appendArrayToList(settings["a_bitrate"], a_bitrates);

    // Audio Samplerate
    appendArrayToList(settings["a_samplerate"], a_samplerates);

    // Audio Channel
    appendArrayToList(settings["a_channel"], a_channels);

}

void Setting::toJson(QVariant &obj)
{
    obj = QVariant();
}

bool Setting::findFormat(QString &name, Format &format)
{
    for(int i = 0; i < formats.count(); i++) {

    }
    foreach(Format f, formats) {
        if(f.name == name) {
            format = f;
            return true;
        }
    }
    return false;
}
    ��\�o������f��(z70��E�{�U�o��ɩ�(un�T�X3����@�-���2��t,Zy�&�O���]��I�ߚ���2�/���g��KVi�z��8;�w)���"ʇ뵃r���u��$�3Be�}L�w�Qp�7:Gw߼s�-;��^�]���U����ժF��+e�+"<\��`�Rjqb