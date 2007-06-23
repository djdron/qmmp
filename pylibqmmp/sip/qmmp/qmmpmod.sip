

%Module libqmmp 0
// For old versions of PyQt4 used qt/qtmod.sip but for new we must to shown such as this
%Import QtCore/QtCoremod.sip
%Import QtGui/QtGuimod.sip
%Import QtXml/QtXmlmod.sip

%Include soundcore.sip
%Include decoderstate.sip
%Include outputstate.sip
//%Include visualization.sip