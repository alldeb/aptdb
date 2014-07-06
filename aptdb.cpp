#include "aptdb.h"
#include "ui_aptdb.h"
#include <QMessageBox>
#include <QPixmap>
#include <QImage>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QDebug>

Aptdb::Aptdb(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Aptdb)
{
    ui->setupUi(this);
    ui->lokasiFileList->setText("/var/lib/apt/lists");
    ui->lokasiSimpan->setText(QDir::homePath());
    ui->progressBar->hide();
    buatDaftar(ui->lokasiFileList->text());
}

Aptdb::~Aptdb()
{
    delete ui;
}

void Aptdb::on_actionQuit_triggered()
{
    qApp->quit();
}

void Aptdb::on_actionAbout_triggered()
{
    QMessageBox tentang;
    tentang.setText(tr("Pengubah basis data APT menjadi CSV"));
    tentang.setInformativeText(tr("Mengubah berkas tembolok daftar paket APT menjadi CSV untuk di-dump ke SQL\n\n"
                                  "(c) 2014 Slamet Badwi\n\nIkon adalah bagian dari tema Evolvere untuk KDE"));
    tentang.setIconPixmap(QPixmap::fromImage(QImage(":/res/icon-apt.svg"),Qt::AutoColor));
    tentang.exec();
}

void Aptdb::on_actionAbout_Qt_triggered()
{
    qApp->aboutQt();
}

void Aptdb::on_actionPanduan_triggered()
{

}

void Aptdb::buatDaftar(QString folder)
{
    ramban = new QFileSystemModel;
    ramban->setRootPath(folder);
    ramban->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    ramban->setNameFilters(QStringList() << "*Packages");
    ramban->setNameFilterDisables(false);
    ui->listView->setModel(ramban);
    ui->listView->setRootIndex(ramban->index(folder));
}

void Aptdb::on_buttonBuat_clicked()
{
    //ui->statusBar->showMessage(ramban->fileName(ui->listView->currentIndex()));
    //qDebug() << ramban->fileName(ui->listView->currentIndex());
    QDir::setCurrent(ui->lokasiFileList->text());
    if(ui->progressBar->maximum() == 100)
    {
        ui->statusBar->showMessage("Pilih dulu satu file");
    }
    else
    {
        ubahKeCsv(ramban->fileName(ui->listView->currentIndex()),ui->lokasiSimpan->text());
    }
}

void Aptdb::on_buttonAptList_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Location"),
                                                    ui->lokasiFileList->text(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    //qDebug() << dir;
    if(!dir.isEmpty()){
        ui->lokasiFileList->setText(dir);
        buatDaftar(dir);
    }
}

void Aptdb::on_buttonSimpan_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Location"),
                                                    ui->lokasiSimpan->text(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    //qDebug() << dir;
    if(!dir.isEmpty())
        ui->lokasiSimpan->setText(dir);
}

void Aptdb::ubahKeCsv(QString filename, QString location)
{
    QFile teksSumber(filename);
    QFileInfo nama(filename);
    filename = nama.fileName();
    QFile simpanSbg(filename.right(filename.length()-filename.indexOf("dists",1)-6)+".csv");
    QString baris;
    //QString arsi = filename.mid(filename.lastIndexOf("-")+1,filename.lastIndexOf("_")-filename.lastIndexOf("-")-1);
    QString componen;
    if(filename.contains("_main")){
        componen = "Main";
    }
    else if(filename.contains("restricted")){
        componen = "Restricted";
    }
    else if(filename.contains("universe")){
        componen = "Universe";
    }
    else if(filename.contains("multiverse")){
        componen = "Multiverse";
    }
    QStringList kriteria;
    kriteria << "Package: " << "Depends: " << "Recommends: " << "Filename: " << "Size: ";
    QByteArray tulisBaru;
    if(teksSumber.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->progressBar->show();
        QTextStream baca(&teksSumber);
        while(!baca.atEnd())
        {
            baris = baca.readLine();
            bool versi;
            bool depen;
            if(baris.startsWith("Version"))
            {
                versi = true;
                depen = true;
            }

            foreach(QString kunci, kriteria)
            {
                QString kalimat;
                if(baris.startsWith(kunci,Qt::CaseSensitive))
                {
                    //qDebug() << baris.remove(kunci); //ini yang benar

                    kalimat.append(baris.remove(kunci).replace(QRegExp(" [^,]*(, |$)|, ")," "));

                    if(kunci == kriteria.at(1))
                    {
                        versi = false;
                        depen = true;
                    }

                    if(kunci == kriteria.at(2))
                    {
                        depen = false;
                        if(versi)
                        {
                            kalimat.prepend(";");
                        }
                    }

                    if(kunci == kriteria.at(3))
                    {
                        if(versi && depen)
                        {
                            kalimat.prepend(";;");
                        }
                        else if(depen)
                        {
                            kalimat.prepend(";");
                        }
                    }

                    if(kunci == kriteria.at(4))
                    {
                        kalimat.append(";");
                        kalimat.append(componen);
                        kalimat.append("\n");
                        //qDebug() << kalimat;
                    }
                    else
                    {
                        kalimat.append(";");
                    }

                    //qDebug() << kalimat;

                    tulisBaru.append(kalimat);
                }
                kalimat.clear();
            }
            ui->progressBar->setValue(ui->progressBar->value()+1);
        }
        //qDebug() << tulisBaru;
        QDir::setCurrent(location);
        if(simpanSbg.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            simpanSbg.write(tulisBaru);
            simpanSbg.close();
            ui->statusBar->showMessage("Berhasil menyimpan di: "+QFileInfo(simpanSbg).absoluteFilePath());
        }
        else
        {
            ui->statusBar->showMessage("Tidak bisa menyimpan file");
        }

        //qDebug() << QFileInfo(simpanSbg).fileName();
        teksSumber.close();
        ui->progressBar->hide();

    }
}

void Aptdb::on_listView_clicked(const QModelIndex &index)
{
    QFile file(ramban->fileInfo(index).absoluteFilePath());
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    int jumlah = 0;
    QTextStream in(&file);
    while( !in.atEnd())
    {
        in.readLine();
        jumlah++;
    }
    //qDebug() << jumlah;
    setMaksimumProgres(jumlah);
    ui->progressBar->setValue(0);
    file.close();
}

void Aptdb::setMaksimumProgres(int maks)
{
    ui->progressBar->setMaximum(maks);
}
