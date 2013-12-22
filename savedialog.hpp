#ifndef SAVEDIALOG_HPP
#define SAVEDIALOG_HPP

#include <QDialog>
#include <QImage>

namespace Ui {
  class SaveDialog;
}

class SaveDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SaveDialog(QWidget *parent = 0);
  ~SaveDialog();

  void setImage(QImage image);

  void save();

private:
  Ui::SaveDialog *ui;

  QImage _image;
};

#endif // SAVEDIALOG_HPP
