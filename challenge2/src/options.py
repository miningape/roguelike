from typing import List
from PySide6 import QtWidgets

class NamedDropDown(QtWidgets.QFrame):
    dropdown: QtWidgets.QComboBox

    def __init__(self, label: str, options: List[str]):
        super(NamedDropDown, self).__init__()
        
        self.dropdown = QtWidgets.QComboBox()
        self.dropdown.addItems(options)

        labelWidget = QtWidgets.QLabel(label)

        layout = QtWidgets.QHBoxLayout(self)
        layout.addWidget(labelWidget)
        layout.addWidget(self.dropdown)

        self.setFrameStyle(QtWidgets.QFrame.Shape.Box)


class CliArgInput(QtWidgets.QFrame):
    line: QtWidgets.QLineEdit

    def __init__(self):
        super(CliArgInput, self).__init__()
        
        self.line = QtWidgets.QLineEdit()

        labelWidget = QtWidgets.QLabel("FFMPEG Args")

        layout = QtWidgets.QVBoxLayout(self)
        layout.addWidget(labelWidget)
        layout.addWidget(self.line)

        self.setFrameStyle(QtWidgets.QFrame.Shape.Box)

    def getArgs(self):
        return list(
            filter(
                lambda s: len(s.strip()) > 0, 
                self.line.text().split(' ')))