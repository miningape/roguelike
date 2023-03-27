import sys
from os import getcwd
from PySide6 import QtWidgets, QtCore, QtGui
from typing import List, Optional

from src.toast import Toast
from src.line import QHLine, QVLine
from src.list import ListWidget
from src.file_selection import InputFilesSelector, OutputFileLocation
from src.options import CliArgInput, NamedDropDown


class CompileButton(QtWidgets.QWidget):
    button: QtWidgets.QPushButton
    progressBar: QtWidgets.QProgressBar

    def __init__(self):
        super(CompileButton, self).__init__()

        self.button = QtWidgets.QPushButton("Compile Video")
        self.progressBar = QtWidgets.QProgressBar()

        policy = QtWidgets.QSizePolicy()
        policy.setHorizontalStretch(QtWidgets.QSizePolicy.PolicyFlag.ExpandFlag)
        policy.setRetainSizeWhenHidden(True)
        self.progressBar.setSizePolicy(policy)
        self.progressBar.hide()

        layout = QtWidgets.QVBoxLayout(self)
        layout.addWidget(self.button, alignment=QtCore.Qt.AlignmentFlag.AlignBottom)
        layout.addWidget(self.progressBar, alignment=QtCore.Qt.AlignmentFlag.AlignTop | QtCore.Qt.AlignmentFlag.AlignCenter)


class Window (QtWidgets.QMainWindow):
    process: Optional[QtCore.QProcess]

    inputFiles: InputFilesSelector
    outputFile: OutputFileLocation
    compileButton: CompileButton
    frameRate: NamedDropDown
    format: NamedDropDown
    encoding: NamedDropDown
    pixelFormat: NamedDropDown
    cliArgs: CliArgInput

    lastStderr: str

    def __init__(self):
        super(Window, self).__init__()
        self.process = None
        self.createWidgets()
        
    def createWidgets(self):
        title = QtWidgets.QLabel("FFMPEG Image Sequence Compiler")
        title.setFont(QtGui.QFont("Arial", 25))
        title.setMaximumHeight(50)

        self.frameRate = NamedDropDown("Framerate", ['24', '30', '60', '128'])
        self.format = NamedDropDown("Output format", ['mp4', 'mov'])
        self.encoding = NamedDropDown("Encoding Preset", [ 'medium', 'ultrafast', 'superfast', 'veryfast', 'faster', 'fast', 'slow', 'veryslow' ])
        self.pixelFormat = NamedDropDown("Pixel Format", ['yuv420p'])
        self.cliArgs = CliArgInput()
        
        optionList = ListWidget()
        optionList.addWidget(self.frameRate)
        optionList.addWidget(self.format)
        optionList.addWidget(self.encoding)
        optionList.addWidget(self.pixelFormat)
        optionList.addWidget(self.cliArgs)

        cwd = getcwd()
        self.inputFiles = InputFilesSelector(cwd)
        self.outputFile = OutputFileLocation(cwd)
        self.compileButton = CompileButton()
        self.compileButton.button.clicked.connect(self.compileVideo)

        grid = QtWidgets.QGridLayout()
        grid.addWidget(title, 0, 0, 1, 5, QtCore.Qt.AlignmentFlag.AlignHCenter)
        grid.addWidget(QHLine(), 1, 0, 1, 5)
        grid.addWidget(self.inputFiles, 2, 0, 2, 1, QtCore.Qt.AlignmentFlag.AlignCenter)
        grid.addWidget(QVLine(), 2, 1, 2, 1)
        grid.addWidget(optionList, 2, 2, 2, 3)
        grid.addWidget(QHLine(), 4, 0, 1, 5)
        grid.addWidget(self.outputFile, 5, 0, 2, 3, QtCore.Qt.AlignmentFlag.AlignHCenter)
        grid.addWidget(self.compileButton, 5, 3, 2, 2, QtCore.Qt.AlignmentFlag.AlignHCenter)
        grid.setSpacing(10)

        centralWidget = QtWidgets.QWidget()
        centralWidget.setLayout(grid)
        self.setCentralWidget(centralWidget)

    def getParams(self) -> List[str]:
        # Get + format the values from all of the widgets on screen
        framerate = self.frameRate.dropdown.currentText()
        inputFiles = '|'.join(self.inputFiles.getFiles())
        outputFile = self.outputFile.selectedFile
        pixelFormat = self.pixelFormat.dropdown.currentText()
        outputFormat = self.format.dropdown.currentText()
        encodingPreset = self.encoding.dropdown.currentText()
        cliArgs = self.cliArgs.getArgs()

        return [
            '-framerate', framerate, 
            '-i', f"concat:{inputFiles}", 
            '-pix_fmt', pixelFormat, 
            '-f', outputFormat, 
            '-preset', encodingPreset
        ] + cliArgs + [
            outputFile
        ]

    def compileVideo(self):
        if self.process is not None:
            return

        if len(self.inputFiles.getFiles()) == 0:
            return Toast().error("Before compiling a video you must include at least 1 file.\n\n- Press the \'Add a file to be compiled\' button")

        if len(self.outputFile.selectedFile) == 0:
            return Toast().error("Before compiling a video you must choose where to put the output.\n\n- Press the \'Choose a location to save the video\' button")

        self.compileButton.progressBar.show()
        self.process = QtCore.QProcess()
        self.process.readyReadStandardOutput.connect(self.handleStdout)
        self.process.readyReadStandardError.connect(self.handleStderr)
        self.process.finished.connect(self.compileVideoFinished)
        self.process.start("ffmpeg", self.getParams())

    def handleStdout(self):
        if self.process is None:
            raise Exception("ffmpeg sent data via stdout after finishing")

    def handleStderr(self):
        if self.process is None:
            raise Exception("ffmpeg sent data via stderr after finishing")

        data = self.process.readAllStandardError()
        stderr = bytes(data).decode() # type: ignore
        self.lastStderr = stderr
        
        if not stderr.startswith('frame='):
            return

        start = len('frame=')
        end = stderr.find("fps=")

        if end == -1:
            return
        
        currentFrame = int(stderr[start:end].strip())
        totalNumFrames = len(self.inputFiles.getFiles())
        self.compileButton.progressBar.setValue(int((currentFrame / totalNumFrames) * 100))

    def compileVideoFinished(self):
        self.compileButton.progressBar.setValue(100)

        if self.process is None:
            raise Exception("ffmpeg signalled it finished twice")

        if self.process.exitCode() != 0:
            Toast().error("There was an error while compiling your video.\n\nHere's what we got from ffmpeg: " + self.lastStderr)

        self.compileButton.progressBar.hide()
        self.compileButton.progressBar.setValue(0)
        self.process = None

if __name__ == '__main__':
    app = QtWidgets.QApplication([])

    window = Window()
    window.resize(840, 620)
    window.show()

    sys.exit(app.exec())
