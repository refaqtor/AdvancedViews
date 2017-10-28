import QtQuick 2.8
import AdvancedViews 1.0

Flickable {
    id: root

    property alias cellDelegate: view.cellDelegate

    contentWidth: view.width
    contentHeight: view.height

    TableViewPrivate {
        id: view
        visibleArea: Qt.rect(root.contentX, root.contentY, root.width, root.height)
        cellDelegate: root.cellDelegate
    }
}
