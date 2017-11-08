/*
    This file is part of AdvancedViews.

    AdvancedViews is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AdvancedViews is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with AdvancedViews.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import AdvancedViews 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    RowLayout {
        anchors.fill: parent
        TableView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: testModel
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
        ScrollView {
            Layout.fillHeight: true
            ColumnLayout {
                RowLayout {
                    SpinBox { id: rowPos }
                    Button { text: "Add row"; onClicked: testModel.insertRowAt(rowPos.value) }
                }
                RowLayout {
                    SpinBox { id: columnPos }
                    Button { text: "Add column"; onClicked: testModel.insertColumnAt(columnPos.value) }
                }
                RowLayout {
                    SpinBox { id: rowReset }
                    SpinBox { id: columnReset }
                    Button { text: "Reset"; onClicked: testModel.reset(rowReset.value, columnReset.value) }
                }
                RowLayout {
                    SpinBox { id: rowMoveFrom }
                    SpinBox { id: rowMoveTo }
                    Button { text: "Move row"; onClicked: testModel.moveRow(rowMoveFrom.value, rowMoveTo.value) }
                }
                RowLayout {
                    SpinBox { id: columnMoveFrom }
                    SpinBox { id: columnMoveTo }
                    Button { text: "Move column"; onClicked: testModel.moveColumn(columnMoveFrom.value, columnMoveTo.value) }
                }
            }
        }
    }
}
