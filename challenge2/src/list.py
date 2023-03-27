from PySide6 import QtWidgets, QtGui, QtCore

class ListWidget(QtWidgets.QListWidget):
    def __init__(self) -> None:
        super(ListWidget, self).__init__()

        self.viewport().setBackgroundRole(QtGui.QPalette.ColorRole.Window)
        self.setFlow(self.Flow.LeftToRight)
        self.setWrapping(True)
        self.setMovement(self.Movement.Static)

        self.setResizeMode(self.ResizeMode.Adjust)

        self.setHorizontalScrollMode(self.ScrollMode.ScrollPerPixel)
        self.setVerticalScrollMode(self.ScrollMode.ScrollPerPixel)
        self.setSpacing(2)

        self.setFrameStyle(QtWidgets.QFrame.Shape.NoFrame)

    def addWidget(self, widget: QtWidgets.QWidget):
        item = QtWidgets.QListWidgetItem()
        item.setFlags(item.flags() & ~(QtCore.Qt.ItemFlag.ItemIsSelectable | QtCore.Qt.ItemFlag.ItemIsEnabled))
        self.addItem(item)
        item.setSizeHint(widget.sizeHint())
        self.setItemWidget(item, widget)