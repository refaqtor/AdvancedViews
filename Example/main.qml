import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import AdvancedViews 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    TableView {
        anchors.fill: parent
        cellDelegate: Rectangle {
            color: "yellow"
            width: 100
            height: 100
            Label {
                anchors.centerIn: parent
                text: row + " " + column
            }
        }
    }
}
