#ifndef SKINSMODEL_H
#define SKINSMODEL_H

#include <QAbstractListModel>
class SkinsModel : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit SkinsModel(QObject *parent = 0);

  enum OptionID {
    nSkinNumber,	//int
    bRoundButtons,	//bool
    bBackgroundImg,	//bool
    OptionIDRowCount,
  };

  void Init();

  /* Explicit getters */
  bool getSkin();

signals:
  void skinsChanged(bool);
};

#endif // SKINSMODEL_H
