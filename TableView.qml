import QtQuick 2.7
import AdvancedViews 1.0

Flickable {
    id: root

    readonly property rect __visibleArea: Qt.rect(contentX, contentY, width, height)
    property Component cellDelegate

    contentWidth: view.boundingRect.width
    contentHeight: view.boundingRect.height


    TableViewPrivate {
        id: view
        visibleArea: __visibleArea
        cellDelegate: root.cellDelegate
    }
}