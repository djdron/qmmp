
class OutputState
{
%TypeHeaderCode
#include <output.h>
%End
public:

    enum Type { Playing, Buffering, Info, Paused, Stopped, Volume, Error };

    OutputState();

    OutputState(Type t);
    OutputState(long s, unsigned long w, int b, int f, int p, int c);
    OutputState(int L, int R);
    OutputState(const QString &e);
    ~OutputState();

    const QString *errorMessage() const;

    long elapsedSeconds() const;
    unsigned long writtenBytes() const;
    int bitrate() const;
    int frequency() const;
    int precision() const;
    int channels() const;
    Type type() const;
    int leftVolume();
    int rightVolume();
};
