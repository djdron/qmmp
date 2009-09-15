#include "audioparameters.h"

AudioParameters::AudioParameters()
{
    m_srate = 0;
    m_chan = 0;
    m_bps = 0;
}

AudioParameters::AudioParameters(const AudioParameters &other)
{
    m_srate = other.sampleRate();
    m_chan = other.channels();
    m_bps = other.bits();
}

AudioParameters::AudioParameters(quint32 srate, int chan, int bps)
{
    m_srate = srate;
    m_chan = chan;
    m_bps = bps;
}

void AudioParameters::operator=(const AudioParameters &p)
{
    m_srate = p.sampleRate();
    m_chan = p.channels();
    m_bps = p.bits();
}

bool AudioParameters::operator==(const AudioParameters &p) const
{
    return m_srate == p.sampleRate() && m_chan == p.channels() && m_bps == p.bits();
}

bool AudioParameters::operator!=(const AudioParameters &p) const
{
    return !operator==(p);
}

quint32 AudioParameters::sampleRate() const
{
    return m_srate;
}

int AudioParameters::channels() const
{
    return m_chan;
}

int AudioParameters::bits() const
{
    return m_bps;
}
