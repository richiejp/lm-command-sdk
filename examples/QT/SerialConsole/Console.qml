import QtQuick 2.3
import QtQuick.Controls 1.4

Item {
    TextArea {
        id: consoleOutput
        //anchors.left: parent.left
        //anchors.right: parent.right
        width: parent.width
        height: parent.height - 20

        text: qsTr("Hello World")
    }

    Item {
        anchors.top: consoleOutput.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        Text {
            id: promptSymbol
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            text: qsTr(">: ")
        }

        TextInput {
            anchors.left: promptSymbol.right
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
        }
    }
}
