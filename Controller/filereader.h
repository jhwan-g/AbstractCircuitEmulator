#ifndef FILEREADER_H
#define FILEREADER_H

// stores file path
#include <QObject>
#include <QDebug>
#include <QUrl>

class fileReader : public QObject {
    Q_OBJECT
public:
    explicit fileReader();
    Q_INVOKABLE void FileSelect(const QString &file_path);

    std::string GetFilePath();
    bool IsFilePathValid();
    void PrintFilePath();
protected:
    std::string file_path;
    bool is_file_path_valid;

};
#endif // FILEREADER_H
