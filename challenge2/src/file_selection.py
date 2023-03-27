from PySide6 import QtWidgets, QtCore

class OutputFileLocation(QtWidgets.QWidget):
    cwd: str
    outputLabel: QtWidgets.QLabel
    selectedFile: str = ""
    
    def __init__(self, cwd: str):
        super(OutputFileLocation, self).__init__()
        self.cwd = cwd

        fileSelectorButton = QtWidgets.QPushButton('Choose a location to save the video')
        fileSelectorButton.clicked.connect(self.openFileSelector)

        self.outputLabel = QtWidgets.QLabel("No file selected.")
        
        layout = QtWidgets.QVBoxLayout(self)
        layout.addWidget(fileSelectorButton, alignment=QtCore.Qt.AlignmentFlag.AlignBottom | QtCore.Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(self.outputLabel, alignment=QtCore.Qt.AlignmentFlag.AlignTop | QtCore.Qt.AlignmentFlag.AlignCenter)

    def openFileSelector(self):
        dir = QtWidgets.QFileDialog.getSaveFileName(self, "Output File Selector", self.cwd) # type: ignore
        self.selectedFile = dir[0] # type: ignore
        self.outputLabel.setText(
            "Output location: " + self.selectedFile # type: ignore
        )

class InputFilesSelector(QtWidgets.QWidget):
    chooseFileTextArea: QtWidgets.QTextEdit
    cwd: str

    def __init__(self, cwd: str):
        super(InputFilesSelector, self).__init__()
        self.cwd = cwd

        label = QtWidgets.QLabel("Images to be compiled")

        fileButton = QtWidgets.QPushButton("Add a file to be compiled")
        fileButton.clicked.connect(self.openFileSelector)

        self.chooseFileTextArea = QtWidgets.QTextEdit()
        self.chooseFileTextArea.setLineWrapMode(QtWidgets.QTextEdit.LineWrapMode.NoWrap)

        layout = QtWidgets.QGridLayout(self)
        layout.addWidget(label, 0, 0, QtCore.Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(self.chooseFileTextArea, 1, 0, 2, 1)
        layout.addWidget(fileButton, 3, 0, QtCore.Qt.AlignmentFlag.AlignCenter)

    def openFileSelector(self):
        files = QtWidgets.QFileDialog.getOpenFileNames(self, "File Selector", self.cwd) # type: ignore
        if len(files[0]) > 0: # type: ignore
            for file in files[0]: # type: ignore
                self.chooseFileTextArea.append(file) # type: ignore

    def getFiles(self):
        return list(
            filter(
                lambda s: len(s.strip()) > 0, 
                self.chooseFileTextArea.toPlainText().split('\n')))