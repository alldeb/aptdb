#ifndef APTDB_H
#define APTDB_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QString>

namespace Ui {
class Aptdb;
}

class Aptdb : public QMainWindow
{
    Q_OBJECT

public:
    explicit Aptdb(QWidget *parent = 0);
    ~Aptdb();

private slots:
    void on_actionQuit_triggered();

    void on_actionAbout_triggered();

    void on_actionAbout_Qt_triggered();

    void on_actionPanduan_triggered();
    void buatDaftar(QString folder);

    void on_buttonBuat_clicked();

    void on_buttonAptList_clicked();

    void on_buttonSimpan_clicked();
    void ubahKeCsv(QString filename, QString location);

    void on_listView_clicked(const QModelIndex &index);
    void setMaksimumProgres(int maks);

private:
    Ui::Aptdb *ui;
    QFileSystemModel *ramban;
};

#endif // APTDB_H
