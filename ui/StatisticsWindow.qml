import QtQuick 2.15
import QtCharts 2.15
import QtQuick.Controls 2.15
import CustomModels 1.0

// FindResultWindow
Window {
    id: root
    width: Screen.width / 2
    height: Screen.height / 2
    title: "Statistics"

    modality: Qt.NonModal

    ChartView {
        id: chartView
        anchors.fill: parent
        antialiasing: true
        animationOptions: ChartView.SeriesAnimations
        theme: ChartView.ChartThemeLight

        // X轴（类别轴）
        BarCategoryAxis {
            id: axisX
            titleText: "Time Point"
        }

        // Y轴（数值轴）
        ValueAxis {
            id: axisY
            min: 0
//            labelFormat: "%.1f"
            titleText: "Memory"
        }

        // 折线系列
        LineSeries {
            id: lineSeries
            name: "Remain Memory"
            color: $LogSearcher.majorLogoColor
            capStyle: Qt.RoundCap
            axisX: axisX
            axisY: axisY
            pointsVisible: true

//            pointLabelsFormat: "(@xPoint, @yPoint)"
//            pointLabelsVisible: true

//            // 点击交互
//            onClicked: (point) => {
//                infoText.text = `选中点: X=${point.x.toFixed(2)}, Y=${point.y.toFixed(2)}`;
//            }
        }
    }

    // 连接数据模型
    Connections {
        target: $ChartDataModel
        function onDataChanged() {
            updateChart();
        }
    }

    function updateChart() {
        // 清空现有数据
        lineSeries.clear();
        var categories = [];

        // 填充新数据
        console.log("$ChartDataModel =", $ChartDataModel.count)
        console.log("$ChartDataModel.count =", $ChartDataModel.count)
        for (var i = 0; i < $ChartDataModel.count; ++i) {
            var item = $ChartDataModel.get(i);
            lineSeries.append(i, item.value);
            categories.push(item.label);
        }

        // 更新X轴类别
        axisX.categories = categories;

        // 自动调整Y轴范围
        if ($ChartDataModel.count > 0) {
            var maxValue = 0;
            for (i = 0; i < $ChartDataModel.count; ++i) {
                maxValue = Math.max(maxValue, $ChartDataModel.get(i).value);
            }
            axisY.max = maxValue * 1.2; // 留20%余量
        }
    }

    Component.onCompleted: updateChart()

    // 控制按钮
    //    Row {
    //        anchors {
    //            bottom: parent.bottom
    //            horizontalCenter: parent.horizontalCenter
    //            margins: 20
    //        }
    //        spacing: 10

    //        Button {
    //            text: "Add Random Data"
    //            onClicked: {
    //                var months = ["Jun", "Jul", "Aug", "Sep", "Oct"];
    //                var randomMonth = months[Math.floor(Math.random() * months.length)];
    //                var randomValue = Math.floor(Math.random() * 100);
    //                chartDataModel.appendData(randomMonth, randomValue);
    //            }
    //        }

    //        Button {
    //            text: "Clear"
    //            onClicked: chartDataModel.clearData()
    //        }
    //    }
}
