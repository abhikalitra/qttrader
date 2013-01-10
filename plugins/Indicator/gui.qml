// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import Qt 4.7

Rectangle {
    id: rectangle1
    width: 400
    height: 400
    color: "#6b3939"

    Text {
        id: text1
        x: 110
        y: 125
        text: qsTr("text")
        font.pixelSize: 12
        opacity: 0
    }

    TextInput {
        id: text_input1
        x: 21
        y: 28
        width: 80
        height: 20
        text: qsTr("text")
        font.pixelSize: 12
    }

    TextEdit {
        id: text_edit1
        x: 21
        y: 106
        width: 80
        height: 20
        text: qsTr("text edit")
        font.pixelSize: 12
    }
}
