import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Dialogs
import Qt.labs.platform

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Abstract Circuit Emulator")

    ColumnLayout{
        anchors.fill: parent
        RowLayout{
            height: 30
            Button {
                text: "파일 열기"
                Layout.fillWidth: true
                Layout.preferredWidth: 300
                onClicked: fileDialog.open()
            }

            Button {
                text: "Assemble"
                Layout.fillWidth: true;
                Layout.preferredWidth: 150
                onClicked: assemble_clicked.Run()

            }

            Button{
                text: "Next"
                Layout.fillWidth: true;
                Layout.preferredWidth: 100
                onClicked: run_clicked.Run()
            }
        }
        Text{
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredHeight: 400
            text: stat_view.consoleMsg
            wrapMode: Text.WordWrap
        }
        ScrollView{
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredHeight: 400
            Text{
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.preferredHeight: 400
                text: console_view.consoleMsg
                wrapMode: Text.WordWrap
            }
        }
    }

    FileDialog { // 파일 선택창
        id: fileDialog
        title: "파일을 선택하세요"
        nameFilters: ["모든 파일 (*)", "LC3 소스 파일 (*.asm)"]
        onAccepted: {
            file_reader.FileSelect(fileDialog.file) // 선택된 파일 정보 백엔드로 전파
        }
    }
}
