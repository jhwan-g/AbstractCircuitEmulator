#include "filereader.h"
#include "ModelView/consoleglobal.h"

fileReader::fileReader(): is_file_path_valid(false){

}

void fileReader::FileSelect(const QString &file_path){
    QUrl url(file_path);
    QString local_file_path = url.toLocalFile(); // URL 형식에서 로컬 경로 추출
    this->file_path = local_file_path.toStdString();
    this->is_file_path_valid = true;
    this->PrintFilePath();
}

std::string fileReader::GetFilePath(){
    return this->file_path;
}

bool fileReader::IsFilePathValid(){
    return this->is_file_path_valid;
}

void fileReader::PrintFilePath(){
    debug_console<<"File: "<<this->file_path<<"loaded\n";
}
