import os
import sys
from typing import List, Dict

from PyQt5.QtCore import QObject, QUrl, QRect, pyqtSignal, pyqtSlot, pyqtProperty, QPoint
from PyQt5.QtGui import QGuiApplication
from PyQt5.QtQml import qmlRegisterType, QQmlApplicationEngine, QQmlComponent, QQmlContext, QQmlEngine
from PyQt5.QtQuick import QQuickItem

import main


class TableViewPrivate(QQuickItem):
    """
    This class manages the visible cells in the TableView
    """

    def __init__(self, parent=None):
        """
        Initialization function
        """
        super(TableViewPrivate, self).__init__(parent)
        self.table = main.Table()
        for i in range(0, 1000):
            self.table.xAxis.append(100)
        for i in range(0, 1000):
            self.table.yAxis.append(100)
        self.visible_area = QRect(0, 0, 0, 0)
        self.cells = []
        self.cell_items = {}
        self.cell_delegate = QQmlComponent()

    def bounding_rect(self) -> QRect:
        """
        Return the TableView bounding rect
        """
        result = self.table.bounding_rect
        return QRect(QPoint(result.left, result.top),
                     QPoint(result.right, result.bottom))

    bounding_rect_changed = pyqtSignal()
    boundingRect = pyqtProperty(type=QRect, fget=bounding_rect, notify=bounding_rect_changed)

    def visible_area(self) -> QRect:
        """
        Return the Flickable visible area
        """
        return self.visible_area

    def set_visible_area(self, area: QRect):
        """
        Sets the Flickable visible area
        """
        if self.visible_area == area:
            return
        self.visible_area = area
        self.visible_area_changed.emit()
        self.on_visible_area_changed()

    visible_area_changed = pyqtSignal()
    visibleArea = pyqtProperty(type=QRect, fget=visible_area, fset=set_visible_area, notify=visible_area_changed)

    def cell_delegate(self) -> QQmlComponent:
        """
        Return the cell delegate
        """
        return self.cell_delegate

    def set_cell_delegate(self, cell_delegate: QQmlComponent) -> None:
        """
        Sets the cell delegate to be instantiated for each cell
        """
        if self.cell_delegate == cell_delegate \
                or self.cell_delegate.url() == cell_delegate.url():
            return
        self.cell_delegate = cell_delegate
        self.cell_delegate_changed.emit()

        # Destroy the previous cell items
        self.cell_items.clear()

        # Do nothing if the delegate is null
        if self.cell_delegate.isNull():
            return

        # Recreate the items for each cell
        for cell in self.cells:
            item = self.create_cell_item(cell.x, cell.y, cell.width, cell.height)
            self.cell_items[(cell.row, cell.column)] = item

    cell_delegate_changed = pyqtSignal()
    cellDelegate = pyqtProperty(type=QQmlComponent, fget=cell_delegate, fset=set_cell_delegate, notify=cell_delegate_changed)

    def on_visible_area_changed(self):
        """
        Called when the visible area changes
        """
        # Determine the visible cells
        rect = main.Rect(main.Point(self.visible_area.left(), self.visible_area.top()),
                         main.Point(self.visible_area.right(), self.visible_area.bottom()))
        visible_cells = self.table.cells_in_visual_rect(rect)

        for cell in set(self.cells) - set(visible_cells):
            item: QQuickItem = self.cell_items[(cell.row, cell.column)]
            item.deleteLater()
            del self.cell_items[(cell.row, cell.column)]
            self.cells.remove(cell)

        for cell in set(visible_cells) - set(self.cells):
            self.cells.append(cell)
            self.cell_items[(cell.row, cell.column)] = self.create_cell_item(cell.x, cell.y, cell.width, cell.height)

    def create_cell_item(self, x: int, y: int, width: int, height: int) -> QQuickItem:
        """
        Create a cell delegate with the given parameters
        """
        context: QQmlContext = QQmlEngine.contextForObject(self)
        result: QQuickItem = self.cell_delegate.beginCreate(context)
        QQmlEngine.setObjectOwnership(result, QQmlEngine.CppOwnership)
        result.setParentItem(self)
        result.setParent(self)
        result.setX(x)
        result.setY(y)
        result.setWidth(width)
        result.setHeight(height)
        self.cell_delegate.completeCreate()
        return result


if __name__ == '__main__':
    qmlRegisterType(TableViewPrivate, 'AdvancedViews', 1, 0, 'TableViewPrivate')
    app = QGuiApplication(sys.argv)
    script_path = os.path.dirname(os.path.abspath(__file__))
    engine = QQmlApplicationEngine()
    engine.load(QUrl.fromLocalFile(os.path.join(script_path, 'ui.qml')))
    sys.exit(app.exec())
