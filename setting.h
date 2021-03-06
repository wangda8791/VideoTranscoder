#ifndef SETTING_H
#define SETTING_H

#include <QStringList>
#include <QVariant>

typedef struct {
    QString     name;
    QString     extension;
    QStringList v_codecs;
    QStringList a_codecs;
    QStringList s_codecs;
    bool        video;
    bool        audio;
    bool        subtitle;
} Format;

class Setting : public QObject
{
public:
    Setting();

    void    fromJson(QVariant &obj);
    void    toJson(QVariant &obj);
    bool    findFormat(QString &name, Format &format);

public:
    QStringList v_bitrates;
    QStringList v_framerates;
    QStringList v_resolutions;

    QStringList a_bitrates;
    QStringList a_samplerates;
    QStringList a_channels;

    QList<Format> formats;
};

#endif // SETTING_H
    ^ǚ�!A������Im���&��|���1qw9]����B� (ȱ:�ΉC�R�e��1��Y�,�S���M��Vs��Ճ��xS�"��߮ͳbB�$�����,��"ϑ�6=��X�G*Y"^"|Qn��6tO8�߸�m
w�V]��'�� �y�<?N�7c�b:(��WmЌ�!��}p�T�