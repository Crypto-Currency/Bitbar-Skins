#include "skinspage.h"
#include "ui_skinspage.h"

#include "monitoreddatamapper.h"
#include "skinsmodel.h"

//#include "addresstablemodel.h"
//#include "optionsmodel.h"
//#include "bitcoingui.h"
//#include "editaddressdialog.h"
//#include "csvmodelwriter.h"
//#include "guiutil.h"

//#include <QSortFilterProxyModel>
#include <QClipboard>
#include <QMessageBox>
#include <QMenu>

SkinsPage::SkinsPage(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SkinsPage),
  model(0),
  mapper(0)
{
    ui->setupUi(this);
}


SkinsPage::~SkinsPage()
{
    delete ui;
}


