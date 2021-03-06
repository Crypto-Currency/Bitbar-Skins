#include "skinspage.h"
#include "ui_skinspage.h"
#include "util.h"
//#include "settings.h"

#include <QClipboard>
#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>
#include <QProgressDialog>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QSettings>
#include <QSize>
#include <boost/filesystem.hpp>
#include <string>
#include <QMainWindow>

#include <QDebug>
using namespace std;

SkinsPage::SkinsPage(QWidget *parent) : QWidget(parent), ui(new Ui::SkinsPage)
{
  ui->setupUi(this);
  browseButton = createButton(tr("&Browse..."), SLOT(browse()));
  findButton = createButton(tr("&Find"), SLOT(find()));

// load settings - do before connecting signals or loading will trigger optionchanged
  QSettings settings("Bitbar", "settings");
  inipath=settings.value("path", "").toString();
//  IniFile = GetDataDir() / "skins.ini";
  loadSettings();
  loadSkin();

  connect(ui->CB1, SIGNAL(toggled(bool)), this, SLOT(optionChanged()));
  connect(ui->CB2, SIGNAL(toggled(bool)), this, SLOT(optionChanged()));
  connect(ui->CB3, SIGNAL(toggled(bool)), this, SLOT(optionChanged()));

  fileComboBox = createComboBox(tr("*"));
  textComboBox = createComboBox();
  
//qDebug() << "from settings inipath:" <<inipath;
  if(inipath!="")
    directoryComboBox = createComboBox(inipath);
  else
  {
    inipath=GetDataDir().string().c_str();
    inipath=inipath+"/skins";
    directoryComboBox=createComboBox(inipath);
    //directoryComboBox = createComboBox(GetDataDir().string().c_str()+("/skins"));skins

  }
//    directoryComboBox = createComboBox(QDir::currentPath());
//qDebug() << "from getdatadir inipath:" <<inipath;

  fileLabel = new QLabel(tr("Named:"));
  textLabel = new QLabel(tr("Description search:"));
  directoryLabel = new QLabel(tr("In directory:"));
  filesFoundLabel = new QLabel;

  createFilesTable();

//  QGridLayout *mainLayout = new QGridLayout;
  ui->mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
  ui->mainLayout->addWidget(fileLabel, 0, 0);
  ui->mainLayout->addWidget(fileComboBox, 0, 1, 1, 2);
  ui->mainLayout->addWidget(textLabel, 1, 0);
  ui->mainLayout->addWidget(textComboBox, 1, 1, 1, 2);
  ui->mainLayout->addWidget(directoryLabel, 2, 0);
  ui->mainLayout->addWidget(directoryComboBox, 2, 1);
  ui->mainLayout->addWidget(browseButton, 2, 2);
  ui->mainLayout->addWidget(filesTable, 3, 0, 1, 3);
  ui->mainLayout->addWidget(filesFoundLabel, 4, 0, 1, 2);
  ui->mainLayout->addWidget(findButton, 4, 2);

  //force find
  find();
}

void SkinsPage::browse()
{
  QString directory=QFileDialog::getExistingDirectory(this,tr("Find Files"),inipath);

  if (!directory.isEmpty())
  {
    if (directoryComboBox->findText(directory) == -1)
      directoryComboBox->addItem(directory);
    directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
  }
  inipath=directory;
  // save settings
  saveSettings();
}

void SkinsPage::find()
{
  filesTable->setRowCount(0);

  QString fileName = fileComboBox->currentText();
  QString text = textComboBox->currentText();
  QString path = directoryComboBox->currentText();

  currentDir = QDir(path);
  QStringList files;
  if (fileName.isEmpty())
    fileName = "*";
  files = currentDir.entryList(QStringList(fileName),
                                 QDir::Files | QDir::NoSymLinks |QDir::Hidden);

  if (!text.isEmpty())
    files = findFiles(files, text);
  showFiles(files);
}

QStringList SkinsPage::findFiles(const QStringList &files, const QString &text)
{
  QProgressDialog progressDialog(this);
  progressDialog.setCancelButtonText(tr("&Cancel"));
  progressDialog.setRange(0, files.size());
  progressDialog.setWindowTitle(tr("Find Files"));

  QStringList foundFiles;
  for (int i = 0; i < files.size(); ++i)
  {
    progressDialog.setValue(i);
    progressDialog.setLabelText(tr("Searching file number %1 of %2...")
                                    .arg(i).arg(files.size()));
    qApp->processEvents();
    QFile file(currentDir.absoluteFilePath(files[i]));

    if (file.open(QIODevice::ReadOnly))
    {
      QString line;
      QTextStream in(&file);
      while (!in.atEnd())
      {
        if (progressDialog.wasCanceled())
          break;
        line = in.readLine();
        if (line.contains(text))
        {
          foundFiles << files[i];
          break;
        }
      }
    }
  }
  return foundFiles;
}

void SkinsPage::showFiles(const QStringList &files)
{
  inipath=currentDir.absolutePath();
//QMessageBox::information(this,tr("currentDir:"),tr("=%1").arg(inipath));
  
  QString line="";// first line of file;
  QString desc="";// descrition goes here;
  QString vers="";// version info;
  int fcount=0;
  for (int i = 0; i < files.size(); ++i)
  {
    line="";
    desc="";
    vers="";
    QFile file(currentDir.absoluteFilePath(files[i]));

    QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
    fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);

// read first line of file to get desc, version
    QFile inputFile(currentDir.absoluteFilePath(files[i]));
    QFileInfo fi(inputFile);

    if(fi.suffix() == "qss")
    {
      if (inputFile.open(QIODevice::ReadOnly))
      {
        QTextStream in(&inputFile);
        if(!in.atEnd())
          line = in.readLine();
      }
      inputFile.close();

// parse line
    int x=line.indexOf("desc=");
    int e=0;
    if(x>0)
    {
      e=line.indexOf(QChar('"'),x);
      if(e>0) // if we found a "
      {
        x=e+1;
        e=line.indexOf(QChar('"'),x); //find the next "
        e=e-x; 
        desc=line.mid(x,e);
      }
    }

    x=line.indexOf("vers=");
    e=0;
    if(x>0)
    {
      e=line.indexOf(QChar('"'),x);
      if(e>0) // if we found a "
      {
        x=e+1;
        e=line.indexOf(QChar('"'),x); //find the next "
        e=e-x; 
        vers=line.mid(x,e);
      }
    }
//qDebug() <<tr("index of x= %1 e=%2").arg(x).arg(e);

    QTableWidgetItem *descriptionItem = new QTableWidgetItem(desc);
    QTableWidgetItem *versionItem = new QTableWidgetItem(vers);

    fcount++;
    int row = filesTable->rowCount();
    filesTable->insertRow(row);
    filesTable->setItem(row, 0, fileNameItem);
    filesTable->setItem(row, 1, descriptionItem);
    filesTable->setItem(row, 2, versionItem);
  }
    }
  filesFoundLabel->setText(tr("%1 file(s) found").arg(fcount) +
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
    (" (Select file to load it)"));
#else
    (" (Double click on a file to load it)"));
#endif
  filesFoundLabel->setWordWrap(true);
}

QPushButton *SkinsPage::createButton(const QString &text, const char *member)
{
  QPushButton *button = new QPushButton(text);
  connect(button, SIGNAL(clicked()), this, member);
  return button;
}

QComboBox *SkinsPage::createComboBox(const QString &text)
{
  QComboBox *comboBox = new QComboBox;
  comboBox->setEditable(false);  //  was true
  comboBox->addItem(text);
  comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_SIMULATOR)
  comboBox->setMinimumContentsLength(3);
#endif
  return comboBox;
}

void SkinsPage::createFilesTable()
{
  filesTable = new QTableWidget(0, 3);
  filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

  QStringList labels;
  labels << tr("Filename") << tr("Description") << tr("Version");
  filesTable->setHorizontalHeaderLabels(labels);
//  filesTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

  filesTable->setColumnWidth(0,160);// last column get resized automatically by qt
  filesTable->setColumnWidth(2,100);
  filesTable->setColumnWidth(1,260);
  filesTable->verticalHeader()->hide();
  filesTable->setShowGrid(false);

  connect(filesTable, SIGNAL(cellActivated(int,int)),
            this, SLOT(openFileOfItem(int,int)));
}

void SkinsPage::openFileOfItem(int row, int /* column */)
{
  QTableWidgetItem *item = filesTable->item(row, 0);
  
  inifname=item->text();
  saveSettings();
  loadSkin();
//QMessageBox::information(this,tr("from getdatadir:"),tr("inipath=%1").arg(inifname));
//QMessageBox::information(this,tr("Open File:"),tr("=%1").arg(inifname));
//QDesktopServices::openUrl(QUrl::fromLocalFile(currentDir.absoluteFilePath(item->text())));
}


void SkinsPage::optionChanged()
{
//  QMessageBox::information(this,tr("IniFile:"),tr("=%1").arg(IniFile));
  saveSettings();
  loadSettings(); // to resize the window
  loadSkin();
}

void SkinsPage::saveSettings()
{
  QSettings settings("Bitbar", "settings");
  settings.setValue("path", inipath);
  settings.setValue("filename", inifname);
  settings.setValue("BackgroundImg", ui->CB1->isChecked());
  settings.setValue("RoundCorners", ui->CB2->isChecked());
  settings.setValue("CB3", ui->CB3->isChecked());
//QMessageBox::information(this,tr("saveSettings:"),tr("=%1").arg(IniFile.string().c_str()));
//qDebug() << "saving IniFile path:" <<IniFile.string().c_str();
}

void SkinsPage::loadSettings()
{
  QSettings settings("Bitbar", "settings");
  inipath=settings.value("path", "").toString();
  inifname=settings.value("filename", "").toString();
  inib1=settings.value("BackgroundImg", false).toBool();
  inib2=settings.value("RoundCorners", false).toBool();
  inib3=settings.value("CB3", false).toBool();
  if(inib1)
    ui->CB1->setCheckState(Qt::Checked);
  if(inib2)
    ui->CB2->setCheckState(Qt::Checked);
  if(inib3)
    ui->CB3->setCheckState(Qt::Checked);
//QMessageBox::information(this,tr("loadSettings:"),tr("path=%1, filename=%2").arg(inipath).arg(inifname));
//qDebug() << "loading IniFile path:" <<IniFile.string().c_str();
}
 
void SkinsPage::loadSkin()
{
  QFile styleFile(inipath+"/"+inifname);
  styleFile.open(QFile::ReadOnly);
  QByteArray bytes = styleFile.readAll();
  QString newStyleSheet(bytes);
  QApplication *app = (QApplication*)QApplication::instance();
  app->setStyleSheet(newStyleSheet);
}

void SkinsPage::resizeEvent(QResizeEvent* event)
{
  int ww=width();
  filesTable->setColumnWidth(1,ww-278);
}
