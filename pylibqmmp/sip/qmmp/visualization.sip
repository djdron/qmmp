
class Visualization
{
%TypeHeaderCode
#include <visualization.h>
%End
public:
    Visualization();

    virtual ~Visualization();

    //virtual void add(Buffer *, unsigned long, int, int) = 0;
    //virtual void prepare() = 0;

    //Decoder *decoder() const;
    //void setDecoder(Decoder *decoder);

    //Output *output() const;
    //void setOutput(Output *output);

    QMutex *mutex();
};
