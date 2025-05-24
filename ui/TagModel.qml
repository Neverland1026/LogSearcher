import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs

// TagModel
Rectangle {
    id: root
    width: contentRect.width + 20
    height: 50
    color: "transparent"
    //border.color: "black"; border.width: 1;

    property alias keywordColor: colorRect.color
    property alias keyword: textInput.text
    property alias delBtnVisible: delBtnRect.visible

    signal sigInsert();
    signal sigRemove();
    signal sigAccepted();

    QtObject {
        id: privateObject

        // 生成随机色
        function generateRandomColor() {
            var r = Math.random();
            var g = Math.random();
            var b = Math.random();
            return Qt.rgba(r, g, b, 1.0);
        }
    }

    // 内容区域
    Rectangle {
        id: contentRect
        width: colorRect.width + 20 + textInput.width + 20
        height: parent.height - 10
        anchors.centerIn: parent
        color: "#1E1E1E"
        radius: 5
        border.color: "#7F000000"; border.width: 1;

        // 选择颜色
        Rectangle {
            id: colorRect
            width: parent.height * 0.4
            height: width
            radius: width
            color: "yellow"
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            border.color: "#000000"; border.width: 1;

            MouseArea {
                anchors.fill: parent
                onPressed: colorDialog.open()
            }
        }

        // 查询关键字
        TextInput {
            id: textInput
            height: contentRect.height
            anchors.left: colorRect.right
            anchors.leftMargin: 10
            anchors.verticalCenter: colorRect.verticalCenter
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 20
            font.bold: true
            color: "white"
            selectByMouse: true
            selectionColor: "lightblue"
            selectedTextColor: "black"
            onAccepted: root.sigAccepted()
            onTextEdited: root.sigInsert()
        }
    }

    // 删除标签
    Rectangle {
        id: delBtnRect
        width: 20
        height: width
        visible: false
        color: contentRect.color
        radius: contentRect.radius
        anchors.left: contentRect.right
        anchors.leftMargin: -width / 3 * 2
        anchors.bottom: contentRect.top
        anchors.bottomMargin: -width / 3 * 2
        border.color: contentRect.border.color; border.width: contentRect.border.width;

        MouseArea {
            anchors.fill: parent
            onPressed: root.sigRemove()

            Image {
                id: image
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                source: "qrc:/image/cancel.svg"
            }
        }
    }

    // 颜色选择对话框
    ColorDialog {
        id: colorDialog
        title: "请选择颜色"
        //color: "red"
        //showAlphaChannel: false  // 显示透明度调节滑块
        modality: Qt.WindowModal // 设置为模态对话框
        onAccepted: {
            console.log("你选择了颜色: " + colorDialog.selectedColor);
            colorRect.color = colorDialog.selectedColor;
            root.sigInsert();
        }
        onRejected: {}
    }

    Component.onCompleted: {
        colorRect.color = privateObject.generateRandomColor();
    }
}
