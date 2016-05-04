#ifndef SKINSPAGE_H
#define SKINSPAGE_H

#include <QDialog>

namespace Ui {
  class SkinsPage;
}

class SkinsModel;
class MonitoredDataMapper;


class SkinsPage : public QDialog
{
  Q_OBJECT

public:
  explicit SkinsPage(QWidget *parent = 0);
  ~SkinsPage();

  void setModel(SkinsModel *model);
  void setMapper();

//public slots:
//    void done(int retval);
//    void exportClicked();

private:
  Ui::SkinsPage *ui;
  SkinsModel *model;
  MonitoredDataMapper *mapper;
//    QMenu *contextMenu;
//    QAction *deleteAction;
//    QString newAddressToSelect;

//private slots:
//    void on_skin1_clicked();  // default
//    void on_skin2_clicked();  // gold daze
};

#endif // SKINSPAGE_H
