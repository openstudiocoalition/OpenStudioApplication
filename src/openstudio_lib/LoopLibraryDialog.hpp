/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_LOOPLIBRARYDIALOG_HPP
#define OPENSTUDIO_LOOPLIBRARYDIALOG_HPP

#include "SOConstants.hpp"
#include <QDialog>
#include <QPixmap>
#include <QString>
#include <boost/optional.hpp>

class QScrollArea;

namespace openstudio {

class LoopLibraryDialog : public QDialog
{
  Q_OBJECT

 public:
  explicit LoopLibraryDialog(QWidget* parent = nullptr);

  virtual ~LoopLibraryDialog() {}

  void newItem(const AddToModelEnum& addToModelEnum, const QString& detailedText, const QPixmap& pixmap);

  boost::optional<AddToModelEnum> addToModelEnum() const;

 protected:
  void paintEvent(QPaintEvent* event) override;

 private slots:

  void onAddToModelClicked(AddToModelEnum addToModelEnum);

 private:
  QScrollArea* m_scrollArea;

  boost::optional<AddToModelEnum> m_addToModelEnum;
};

class LoopItemView : public QWidget
{
  Q_OBJECT

 public:
  LoopItemView(const AddToModelEnum& addToModelEnum, const QString& detailedText, const QPixmap& pixmap, QWidget* parent = nullptr);

  virtual ~LoopItemView() {}

  //void paintEvent ( QPaintEvent * event );

 signals:

  void addToModelClicked(AddToModelEnum);

 private slots:

  void emitAddToModelClicked();

 private:
  AddToModelEnum m_addToModelEnum;

  QString m_detailedText;

  QPixmap m_pixmap;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_LOOPLIBRARYDIALOG_HPP
