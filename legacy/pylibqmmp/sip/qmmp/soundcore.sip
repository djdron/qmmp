

class SoundCore : QObject
{
%TypeHeaderCode
#include <soundcore.h>
%End

public:
    SoundCore(QObject * /TransferThis/);
    bool play(const QString &source);
    void stop();
    void pause();
    void seek(int pos);
    int length();
    bool isInitialized();
    bool isPaused();
    void setEQ(int* bands, const int &preamp);
    void setEQEnabled(bool on);
    void setVolume(int L, int R);
    void updateConfig();
    //void addVisualization(Visualization *visual);
    enum ErrorType { NoError = 0, DecoderError, OutputError };
    uint error();
signals:
    void decoderStateChanged(const DecoderState& state);
    void outputStateChanged(const OutputState& state);
};
