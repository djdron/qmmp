
class DecoderState
{
%TypeHeaderCode
#include <decoder.h>
%End
public:
    enum Type
    {
         Decoding,
         Stopped,
         Finished,
         Error
     };

    DecoderState(Type t);
    DecoderState(const QString &e);
    DecoderState();
    ~DecoderState();
    const QString *errorMessage() const;
    Type type() const / PyName=type_ /;
};



