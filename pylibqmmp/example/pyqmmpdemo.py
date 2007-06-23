#! /usr/bin/python

import sys , os
from PyQt4 import QtCore, QtGui
from libqmmp import SoundCore, OutputState



class Gui(QtGui.QDialog):
    def __init__(self,parent) :
        QtGui.QDialog.__init__(self,parent)

        self.setWindowTitle("PyQMMP demo")
        self.core = SoundCore(self)

        vl = QtGui.QVBoxLayout()
        hl = QtGui.QHBoxLayout()
        playB = QtGui.QPushButton("Play")
        pauseB = QtGui.QPushButton("Pause")
        stopB = QtGui.QPushButton("Stop")

        # making the connections
        self.connect(playB, QtCore.SIGNAL("clicked()"), self.play)
        self.connect(pauseB, QtCore.SIGNAL("clicked()"), self.pause)
        self.connect(stopB, QtCore.SIGNAL("clicked()"), self.stop)

        hl.addWidget(playB)
        hl.addWidget(pauseB)
        hl.addWidget(stopB)
        vl.addLayout(hl)

        posLabel = QtGui.QLabel("Pos: ")
        self.posSlider = QtGui.QSlider(QtCore.Qt.Horizontal,self)
        self.connect(self.posSlider, QtCore.SIGNAL("sliderReleased()"), self.setPosition)
        volLabel = QtGui.QLabel("Volume: ")
        self.volumeSlider = QtGui.QSlider(QtCore.Qt.Horizontal,self)
        self.connect(self.volumeSlider, QtCore.SIGNAL("sliderMoved(int)"), self.setVolume)

        self.connect(self.core,QtCore.SIGNAL("outputStateChanged(const OutputState&)"),self.handleOutputStateChanged)

        hl = QtGui.QHBoxLayout()
        hl.addWidget(posLabel)
        hl.addWidget(self.posSlider)
        hl.addWidget(volLabel)
        hl.addWidget(self.volumeSlider)
        vl.addLayout(hl)

        hl = QtGui.QHBoxLayout()
        self.fileEdit = QtGui.QLineEdit(self)
        fileButton = QtGui.QToolButton(self)
        fileButton.setText("...")
        self.connect(fileButton,QtCore.SIGNAL("clicked()"),self.openFile)
        hl.addWidget(self.fileEdit)
        hl.addWidget(fileButton)

        vl.addLayout(hl)
        self.setLayout(vl)


    def play(self):
        self.core.play(self.fileEdit.text())
        self.posSlider.setRange(0,self.core.length())

    def pause(self):
        self.core.pause()

    def stop(self):
        self.core.stop()

    def setPosition(self):
        pos = self.posSlider.value()
        self.core.seek(pos)

    def setVolume(self,pos):
        self.core.setVolume(self.volumeSlider.value(),self.volumeSlider.value())

    def handleOutputStateChanged(self,st):
        if st.type() == OutputState.Info:
            self.posSlider.setValue(st.elapsedSeconds());

        if st.type() == OutputState.Volume:
            self.volumeSlider.setValue(st.leftVolume())    

    def openFile(self):
        str = QtGui.QFileDialog.getOpenFileName(self,"Open File");
        self.fileEdit.setText(str)

    def reject(self):
        self.core.stop()



if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)

    gui = Gui(None)
    gui.show()

    sys.exit(app.exec_())



