import QtQuick 2.15
import QtQuick.Controls 2.15

// FindResultWindow
Window {
    id: root
    width: Screen.width / 6 * 5
    height: Screen.height / 5 * 4
    title: "Log Summary"

    modality: Qt.NonModal

    function setLineNumWidth(lineNumWidth) {
        logPanel.lineNumWidth = lineNumWidth;
    }

    signal sigPositionViewAtIndex(var lineNumber);

    signal sigClose();

    // 背景
    Rectangle { anchors.fill: parent; color: $LogSearcher.eyeProtectionMode ? $LogSearcher.eyeProtectionColor : "white"; }

    // 日志摘要
    LogPanel {
        id: logPanel
        anchors.fill: parent
        summaryMode: true
        logModel: $SummaryModel
        onSigDoubleClicked: sigPositionViewAtIndex(lineNumber)
    }

    onClosing: { sigClose(); }
}
