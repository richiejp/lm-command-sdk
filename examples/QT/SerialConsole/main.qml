import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import lm 1.0;

Window {
    visible: true
    title: qsTr("LM Serial Console")
    width: 800
    height: 600

    SerialController {
        id: sc

        onFoundPorts: {
            portsModel.clear();
            for(var i = 0; i < ports.length; ++i){
                var text = ports[i].portName;
                if(ports[i].isBusy === true){
                    text += ' (busy)';
                }

                portsModel.append({
                    text: text,
                    value: ports[i].portName
                });
            }

            requestBauds();
        }

        onFoundBauds: {
            baudsModel.clear();
            for(var i = 0; i < rates.length; ++i){
                baudsModel.append({
                    text: "" + rates[i],
                    value: rates[i]
                });
            }
        }

        onError: {
            messageText.text = message;
            messageArea.state = "error";
        }

        onOpenedPort: {
            wrapper.state = "opened"
        }

        onClosedPort: {
            wrapper.state = "default"
        }
    }

    Component.onCompleted: {
        sc.requestPorts();
    }

    Item {
        id: wrapper
        anchors.fill: parent

    Column {
        id: headerColumn
        spacing: 5
        anchors.left: parent.left
        anchors.right: parent.right

        Row {
            id: menuBar
            spacing: 5

            Text {
                text: qsTr("Port")
            }
            ComboBox {
                id: portsCombo
                width: 100
                model: ListModel {
                    id: portsModel
                }
            }

            Text {
                text: qsTr("Baud")
            }
            ComboBox {
                id: baudCombo
                width: 100
                model: ListModel {
                    id: baudsModel
                }
            }

            Text {
                text: qsTr("Flow Control")
            }
            ComboBox {
                id: flowCombo
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

        Row {
            id: actionBar
            spacing: 5
            layoutDirection: Qt.RightToLeft

            Button {
                id: openButton
                text: qsTr("Open")
                visible: false

                onClicked: {
                    var selectedPort =
                            portsModel.get(portsCombo.currentIndex);
                    var selectedBaud =
                            baudsModel.get(baudCombo.currentIndex);
                    sc.requestOpenPort(selectedPort.value,
                                       selectedBaud.value);
                }
            }

            Button {
                id: closeButton
                text: qsTr("Close")
                visible: false

                onClicked: {
                    sc.requestClosePort();
                }
            }

            Button {
                id: reloadButton
                text: qsTr("Reload Port Info")
                enabled: false

                onClicked: {
                    sc.requestPorts();
                }
            }

            Button {
                text: "cause error"
                onClicked: {
                    messageText.text = "A test error";
                    messageArea.state = "error";
                }
            }

        }

        Rectangle {
            id: messageArea
            anchors.left: parent.left
            anchors.right: parent.right
           height: 5

           Text {
               id: messageText
               anchors.bottom: parent.bottom
               visible: false
           }

           Button {
               id: messageCloseButton
               anchors.bottom: parent.bottom
               anchors.right: parent.right
               text: "Close"
               visible: false

               onClicked: {
                   messageArea.state = "empty";
               }
           }

           state: "empty"
           states: [
               State {
                   name: "error"
                   PropertyChanges {
                       target: messageArea
                       color: "Red"
                       height: 80
                   }
                   PropertyChanges {
                       target: messageText
                       visible: true
                   }
                   PropertyChanges {
                       target: messageCloseButton
                       visible: true
                   }
               }
           ]
        }
    }

    Console {
        anchors.top: headerColumn.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    state: "default"
    states: [
        State {
            name: "default"
            PropertyChanges {
                target: openButton
                visible: true
            }

            PropertyChanges {
                target: reloadButton
                enabled: true
            }
        },
        State {
            name: "opened"
            PropertyChanges {
                target: closeButton
                visible: true
            }
        }
    ]
    }
}
