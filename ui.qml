import QtQuick 2.7
import QtQuick.Controls 2.2 as C2

C2.ApplicationWindow {
    width: 200
    height: 200

    TableView {
        id: view
        anchors.fill: parent
        cellDelegate: Rectangle {
            width: 10
            height: 10
            color: "yellow"
            border.color: "black"
            border.width: 1
        }
    }

    Component.onCompleted: visible = true
}