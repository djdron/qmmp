#ifndef AUDIOPARAMETERS_H
#define AUDIOPARAMETERS_H

#include <QtGlobal>

class AudioParameters
{
public:
    AudioParameters();
    AudioParameters(quint32 srate, int chan, int bps);
    AudioParameters(const AudioParameters &other);

    void operator=(const AudioParameters &p);
    bool operator==(const AudioParameters &p) const;
    bool operator!=(const AudioParameters &p) const;

    quint32 sampleRate() const;
    int channels() const;
    int bits() const;

private:
    quint32 m_srate;
    int m_chan;
    int m_bps;
};

#endif // AUDIOPARAMETERS_H
