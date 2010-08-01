#ifndef WILDMIDIHELPER_H
#define WILDMIDIHELPER_H

#include <QObject>

class WildMidiHelper : public QObject
{
Q_OBJECT
public:
    explicit WildMidiHelper(QObject *parent = 0);
    ~WildMidiHelper();
    bool initialize();
    static WildMidiHelper *instance();

private:
    static WildMidiHelper *m_instance;
    bool m_inited;

};

#endif // WILDMIDIHELPER_H
