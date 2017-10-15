import os
import sys
from typing import List, Dict, Set, Callable

from PyQt5.QtCore import QObject, QUrl, QRect, pyqtSignal, pyqtSlot, pyqtProperty, QPoint
from PyQt5.QtGui import QGuiApplication
from PyQt5.QtQml import qmlRegisterType, QQmlApplicationEngine, QQmlComponent, QQmlContext, QQmlEngine, QQmlIncubator
from PyQt5.QtQuick import QQuickItem

import main


class TableViewIncubator(QQmlIncubator):
    def __init__(self,
                 status_changed_callback: Callable[[QQmlIncubator.Status], None],
                 set_initial_state_callback: Callable[[QObject], None],
                 mode: QQmlIncubator.IncubationMode=QQmlIncubator.Asynchronous):
        super(TableViewIncubator, self).__init__(mode)
        self.status_changed_callback = status_changed_callback
        self.set_initial_state_callback = set_initial_state_callback

    def statusChanged(self, status: 'QQmlIncubator.Status'):
        super(TableViewIncubator, self).statusChanged(status)
        self.status_changed_callback(status)

    def setInitialState(self, a0: QObject):
        super(TableViewIncubator, self).setInitialState(a0)
        self.set_initial_state_callback(a0)

class TableViewPrivateElement:
    def __init__(self, table: 'TableViewPrivate', cell: main.Cell):
        self.table = table
        self.cell = cell
        self.context: QQmlContext = None
        self.incubator: TableViewIncubator = None
        self.item: QQuickItem = None

    def clear_item(self):
        if self.item:
            self.item.deleteLater()
            self.item = None
        if self.context:
            self.context.deleteLater()
            self.context = None
        if self.incubator:
            self.incubator.clear()
            self.incubator = None

    def create_item(self):
        assert self.incubator is None
        assert self.item is None
        assert self.context is None
        assert self.cell
        table_context = QQmlEngine.contextForObject(self.table)
        # Initialize the context
        self.context = QQmlContext(table_context, None)
        self.context.setContextProperty("row", self.cell.row)
        self.context.setContextProperty("column", self.cell.column)
        # Initialize the incubator
        self.incubator = TableViewIncubator(status_changed_callback=self.on_incubator_status_changed,
                                            set_initial_state_callback=self.on_incubator_set_initial_state)
        # Begin creation
        self.table.cell_delegate.create(self.incubator, self.context, table_context)

    def on_incubator_set_initial_state(self, item: QObject):
        item.setParentItem(self.table)
        item.setParent(self.table)
        item.setX(self.cell.x)
        item.setY(self.cell.y)
        item.setWidth(self.cell.width)
        item.setHeight(self.cell.height)

    def on_incubator_status_changed(self, status: QQmlIncubator.Status):
        if status == QQmlIncubator.Ready:
            self.item = self.incubator.object()
            self.context.setParent(self.item)

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
            element.clear_item()

        # Do nothing if the delegate is null
        if self.cell_delegate.isNull():
            return

        # Recreate the items for each cell
        for element in self.elements:
            element.create_item()

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
            element.clear_item()
            self.elements.remove(element)

        # Add elements for the new visible cells
        for cell in visible_cells - current_cells:
            element = TableViewPrivateElement(table=self, cell=cell)
            element.create_item()
            self.elements.append(element)


if __name__ == '__main__':
    qmlRegisterType(TableViewPrivate, 'AdvancedViews', 1, 0, 'TableViewPrivate')
    app = QGuiApplication(sys.argv)
    script_path = os.path.dirname(os.path.abspath(__file__))
    engine = QQmlApplicationEngine()
    engine.load(QUrl.fromLocalFile(os.path.join(script_path, 'ui.qml')))
    sys.exit(app.exec())
