#ifndef SKINSPAGE_H
#define SKINSPAGE_H

#include <QDialog>

namespace Ui {
  class SkinsPage;
}
//class AddressTableModel;
//class OptionsModel;

//QT_BEGIN_NAMESPACE
//class QTableView;
//class QItemSelection;
//class QSortFilterProxyModel;
//class QMenu;
//class QModelIndex;
//QT_END_NAMESPACE

/** Widget that shows a list of sending or receiving addresses.
  */
class SkinsPage : public QDialog
{
  Q_OBJECT
public:
  explicit SkinsPage(QWidget *parent = 0);
    ~SkinsPage();

//public slots:
//    void done(int retval);
//    void exportClicked();

private:
    Ui::SkinsPage *ui;
//    QMenu *contextMenu;
//    QAction *deleteAction;
//    QString newAddressToSelect;

//private slots:
//    void on_skin1_clicked();  // default
//    void on_skin2_clicked();  // gold daze
};

#endif // SKINSPAGE_H
