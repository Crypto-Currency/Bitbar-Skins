#include "skinsmodel.h"
#include <QSettings>

SkinsModel::SKINSModel(QObject *parent) :
  QAbstractListModel(parent)
{
  Init();
}

void SkinsModel::Init()
{
  QSettings settings;

  // These are Qt-only settings:
  nSkinNumber = settings.value("nSkinNumber", 1).toInt();
  bRoundButtons = settings.value("bRoundButtons", true).toBool();
  bBackgroundImg = settings.value("bBackgroundImg", true).toBool();
}
