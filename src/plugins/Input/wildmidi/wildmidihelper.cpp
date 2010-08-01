#include <QApplication>
extern "C"{
#include <wildmidi_lib.h>
}
#include "wildmidihelper.h"

WildMidiHelper *WildMidiHelper::m_instance = 0;

WildMidiHelper::WildMidiHelper(QObject *parent) :
    QObject(parent)
{
    m_inited = false;
}

WildMidiHelper::~WildMidiHelper()
{
    if(m_inited)
        WildMidi_Shutdown();
    m_instance = 0;
}

bool WildMidiHelper::initialize()
{
    if(m_inited)
        return true;
    if (WildMidi_Init ("/etc/timidity/timidity.cfg", 48000, 0) < 0)
    {
        qWarning("WildMidiHelper: unable to initialize WildMidi library");
        return false;
    }
    m_inited = true;
    return true;
}


WildMidiHelper *WildMidiHelper::instance()
{
    if(!m_instance)
        m_instance = new WildMidiHelper(qApp);
    return m_instance;
}
