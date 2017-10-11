import os
import sys
from typing import List, Dict, Set

from PyQt5.QtCore import QObject, QUrl, QRect, pyqtSignal, pyqtSlot, pyqtProperty, QPoint
from PyQt5.QtGui import QGuiApplication
from PyQt5.QtQml import qmlRegisterType, QQmlApplicationEngine, QQmlComponent, QQmlContext, QQmlEngine
from PyQt5.QtQuick import QQuickItem

import main


class TableViewPrivateElement:
    def __init__(self, cell: main.Cell):
        self.cell = cell
        self.item = None

    def __eq__(self, other: 'TableViewPrivateElement') -> bool:
        return other and self.cell == other.cell

    def __hash__(self):
        return hash(self.cell)


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
        self.elements: List[TableViewPrivateElement] = []
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
        for element in self.elements:
            element.item.deleteLater()
            element.item = None

        # Do nothing if the delegate is null
        if self.cell_delegate.isNull():
            return

        # Recreate the items for each cell
        for element in self.elements:
            element.item = self.create_cell_item(element.cell)

    cell_delegate_changed = pyqtSignal()
    cellDelegate = pyqtProperty(type=QQmlComponent, fget=cell_delegate, fset=set_cell_delegate,
                                notify=cell_delegate_changed)

    def on_visible_area_changed(self):
        """
        Called when the visible area changes
        """
        # Determine the visible cells
        rect = main.Rect(main.Point(self.visible_area.left(), self.visible_area.top()),
                         main.Point(self.visible_area.right(), self.visible_area.bottom()))
        visible_cells = set(self.table.cells_in_visual_rect(rect))
        current_cells = set([x.cell for x in self.elements])

        # Remove elements that are not visible anymore
        for element in [e for e in self.elements if e.cell in current_cells - visible_cells]:
            element.item.deleteLater()
            element.item = None
            self.elements.remove(element)

        # Add elements for the new visible cells
        for cell in visible_cells - current_cells:
            element = TableViewPrivateElement(cell)
            element.item = self.create_cell_item(cell)
            self.elements.append(element)

    def create_cell_item(self, cell: main.Cell) -> QQuickItem:
        """
        Create a cell delegate with the given parameters
        """
        context = QQmlContext(QQmlEngine.contextForObject(self), None)
        context.setContextProperty("row", cell.row)
        context.setContextProperty("column", cell.column)
        result: QQuickItem = self.cell_delegate.beginCreate(context)
        QQmlEngine.setObjectOwnership(result, QQmlEngine.CppOwnership)
        result.setParentItem(self)
        result.setParent(self)
        result.setX(cell.x)
        result.setY(cell.y)
        result.setWidth(cell.width)
        result.setHeight(cell.height)
        self.cell_delegate.completeCreate()
        context.setParent(result)
        return result


if __name__ == '__main__':
    qmlRegisterType(TableViewPrivate, 'AdvancedViews', 1, 0, 'TableViewPrivate')
    app = QGuiApplication(sys.argv)
    script_path = os.path.dirname(os.path.abspath(__file__))
    engine = QQmlApplicationEngine()
    engine.load(QUrl.fromLocalFile(os.path.join(script_path, 'ui.qml')))
    sys.exit(app.exec())
