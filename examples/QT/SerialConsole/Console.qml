import QtQuick 2.3
import QtQuick.Controls 1.4

Item {
    function write(data){
        consoleOutput.text += data;
    }

    signal inputtedLine(string text)

    TextArea {
        id: consoleOutput
        width: parent.width
        height: parent.height - 20
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
            id: lineInput
            anchors.left: promptSymbol.right
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            enabled: false

            onAccepted: {
                inputtedLine(text);
                text = "";
            }

            onTextChanged: {

            }
        }
    }

    state: "default"
    states: [
        State {
            name: "singleLineInput"

            PropertyChanges {
                target: lineInput
                enabled: true
            }
        }
    ]
}
