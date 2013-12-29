#include "savedialog.hpp"
#include "ui_savedialog.h"

#include <QPixmap>
#include <QDir>
#include <QFile>

#include <sstream>

#include "settings.hpp"

#include <iostream>

SaveDialog::SaveDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SaveDialog)
{
  ui->setupUi(this);
  adjustSize();
}

SaveDialog::~SaveDialog()
{
  delete ui;
}

void SaveDialog::setImage(QImage image)
{
  _image = image;
  ui->cellImg->setPixmap(QPixmap::fromImage(_image));
}

void SaveDialog::save()
{
  int digit = ui->digit->value();
  if (digit == 0)
    return;

  std::stringstream ss;
  ss << TRAINING_DATA_DIR << digit << "/";
  //ss << "/ACERDATA/UNI/Graz/Bildverarbeitung und Mustererkennung/2013/KU/Assignment 4/project/bvme_project/training_set/" << digit << "/";

  QString dirName = ss.str().c_str();
  QDir dir(dirName);

  if (! dir.exists())
    qWarning("Cannot find directory!");

  QStringList nameFilters;
  nameFilters << "*.png";
  dir.setFilter(QDir::Files);
  dir.setSorting(QDir::Time);
  dir.setSorting(QDir::Reversed);
  dir.setNameFilters(nameFilters);
  QFileInfoList files = dir.entryInfoList();
  qDebug("saveDialog>>save(): Ordnerinhalt: ");

  int newValue = 0;
  if (! files.empty())
  {
	QString lastFile = files.at(0).fileName();
    //int lastfilenr = lastFile.toInt();
	//newValue = lastFile.toInt() + 1;
    newValue = files.size();
    const char* outline;
    outline << "Digit: " << digit << ": " <<  "Newest File: " << lastFile.toStdString() << " - Number of Files: " << newValue;
    qDebug(outline);
  }
  QString newName = QString::number(newValue) + ".png";

  QImage img = _image.convertToFormat(QImage::Format_RGB888);
  img.save(dir.absoluteFilePath(newName));
}
