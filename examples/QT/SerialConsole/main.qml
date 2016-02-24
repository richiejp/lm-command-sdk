import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Window {
    visible: true
    title: qsTr("LM Serial Console")
    width: 800
    height: 600

    Row {
        id: menuBar
        spacing: 5

        Text {
            text: qsTr("Port")
        }
        ComboBox {
            model: ListModel {
                ListElement { text: "COM1" }
                ListElement { text: "COM2" }
            }
        }

        Text {
            text: qsTr("Baud")
        }
        ComboBox {
            width: 100
            model: ListModel {
                ListElement { text: qsTr("192,000"); value: 192000 }
            }
        }

        Text {
            text: qsTr("Flow Control")
        }
        ComboBox {
            model: ListModel {
                ListElement { text: qsTr("None") }
                ListElement { text: qsTr("Hardware") }
                ListElement { text: qsTr("Software") }
            }
        }

        Text {
            text: qsTr("Parity")
        }
        ComboBox {
            model: ListModel {
                ListElement { text: qsTr("None") }
            }
        }

        Text {
            text: qsTr("Stop Bits")
        }
        ComboBox {
            model: ListModel {
                ListElement { text: qsTr("One") }
            }
        }
    }

    Console {
        anchors.top: menuBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

    }

}
