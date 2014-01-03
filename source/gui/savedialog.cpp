#include "../../include/gui/savedialog.hpp"
#include "ui_savedialog.h"

#include <QPixmap>
#include <QDir>
#include <QFile>

#include <sstream>

#include "settings.hpp"

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

  QString dirName = ss.str().c_str();
  QDir dir(dirName);

  if (! dir.exists())
    qWarning("Cannot find directory!");

  QStringList nameFilters;
  nameFilters << "*.png";
  QFileInfoList files = dir.entryInfoList(nameFilters, QDir::Files, QDir::Time);

  int newValue = 0;
  if (! files.empty())
  {
    QString lastFile = files.at(0).baseName();
    newValue = lastFile.toInt() + 1;
  }
  QString newName = QString::number(newValue) + ".png";

  QImage img = _image.convertToFormat(QImage::Format_RGB888);
  img.save(dir.absoluteFilePath(newName));
}
