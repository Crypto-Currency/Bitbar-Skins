#include "skinspage.h"
#include "ui_skinspage.h"

#include <QClipboard>
#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>
#include <QProgressDialog>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QSettings>

SkinsPage::SkinsPage(QWidget *parent) : QWidget(parent), ui(new Ui::SkinsPage)
{
  ui->setupUi(this);
  browseButton = createButton(tr("&Browse..."), SLOT(browse()));
  findButton = createButton(tr("&Find"), SLOT(find()));

//  QAction *b1Change =new QAction(tr("Show or Hide background image"),this);

  connect(ui->rbBackground, SIGNAL(toggled(bool)), this, SLOT(optionChanged()));

  fileComboBox = createComboBox(tr("*"));
  textComboBox = createComboBox();
  directoryComboBox = createComboBox(QDir::currentPath());

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
//  setLayout(mainLayout);

    setWindowTitle(tr("Find Files"));
#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_SIMULATOR)
    resize(700, 300);
#endif

// load settings
  inipath=QApplication::applicationDirPath()+("/");
  IniFile = inipath+("skins.ini");
  loadSettings();
}

void SkinsPage::browse()
{
  QString directory = QFileDialog::getExistingDirectory(this,
                               tr("Find Files"), QDir::currentPath());

  if (!directory.isEmpty())
  {
    if (directoryComboBox->findText(directory) == -1)
      directoryComboBox->addItem(directory);
    directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
  }
// save settings
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
                                 QDir::Files | QDir::NoSymLinks);

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
  for (int i = 0; i < files.size(); ++i)
  {
    QFile file(currentDir.absoluteFilePath(files[i]));
//    qint64 size = QFileInfo(file).size();

    QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
    fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);
    QTableWidgetItem *descriptionItem = new QTableWidgetItem("description goes here");


//    QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB").arg(int((size + 1023) / 1024)));
//    sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
//    sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);
//    QTableWidgetItem *descriptionItem= new QTableWidgetItem(tr(""));
//    QString *temp="description goes here";
//    descriptionItem=temp;
    int row = filesTable->rowCount();
    filesTable->insertRow(row);
    filesTable->setItem(row, 0, fileNameItem);
    filesTable->setItem(row, 1, descriptionItem);
  }
  filesFoundLabel->setText(tr("%1 file(s) found").arg(files.size()) +
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
                             (" (Select file to open it)"));
#else
                             (" (Double click on a file to open it)"));
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
  filesTable = new QTableWidget(0, 2);
  filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

  QStringList labels;
  labels << tr("Filename") << tr("Description");// << tr("Size");
  filesTable->setHorizontalHeaderLabels(labels);
//  filesTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
  filesTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
//  filesTable->setColumnWidth(2,60);
  filesTable->verticalHeader()->hide();
  filesTable->setShowGrid(false);

  connect(filesTable, SIGNAL(cellActivated(int,int)),
            this, SLOT(openFileOfItem(int,int)));
}

void SkinsPage::openFileOfItem(int row, int /* column */)
{
  QTableWidgetItem *item = filesTable->item(row, 0);
  
QMessageBox::information(this,tr("Open File:"),tr("=%1").arg(item->text()));

//  QDesktopServices::openUrl(QUrl::fromLocalFile(currentDir.absoluteFilePath(item->text())));
}

void SkinsPage::loadSettings()
{
  QSettings settings(IniFile, QSettings::NativeFormat);
//  QString sText = settings.value("text", "").toString();
  inipath=settings.value("path", "").toString();
  inib1=settings.value("b1", false).toBool();
  inib2=settings.value("b2", false).toBool();
  inib3=settings.value("b3", false).toBool();
}
 
void SkinsPage::saveSettings()
{
  QSettings settings(IniFile, QSettings::NativeFormat);
//  QString sText = (m_pEdit) ? m_pEdit->text() : "";
  settings.setValue("path", IniFile);
  settings.setValue("b1", ui->rbBackground->isChecked());
  settings.setValue("b2", ui->rbRound->isChecked());
  settings.setValue("b3", inib3);
//  if (m_pLabel)
//    m_pLabel->setText(sText);
  QMessageBox::information(this,tr("inipath:"),tr("=%1").arg(inipath));
}

void SkinsPage::optionChanged()
{
  QMessageBox::information(this,tr("IniFile:"),tr("=%1").arg(IniFile));
  saveSettings();
}

