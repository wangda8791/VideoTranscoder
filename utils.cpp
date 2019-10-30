#include "utils.h"

Utils::Utils()
{
}

QString Utils::GetTimeString(int ts)
{
    int ms = ts % 1000;
    int sec = ts / 1000;
    int hour = sec / 3600;
    sec = sec % 3600;
    int min = sec / 60;
    sec = sec % 60;

    QString shour = ((hour<10)?"0":"") + QString().setNum(hour);
    QString smin = ((min<10)?"0":"") + QString().setNum(min);
    QString ssec = ((sec<10)?"0":"") + QString().setNum(sec);
    QString sms = ms<10?"00" + QString().setNum(ms):ms<100?"0" + QString().setNum(ms):QString().setNum(ms);

    return shour + ":" + smin + ":" + ssec + "." + sms;
}
    �Id�+��k4��>O�/$��"7ö��B썂Vz�.T�����~A�3�o��}�_ԁ���d)�9S�/�b��G_�i��ٙa��;����%
l)���M�� s�#�T���%��$ P�JZ	TCP"6n��0����un��(%i�q�K�bG��R�#��U�l��x�&XJ�\�5!x%���,K