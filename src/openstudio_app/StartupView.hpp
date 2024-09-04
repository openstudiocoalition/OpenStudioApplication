/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_STARTUPVIEW_HPP
#define OPENSTUDIO_STARTUPVIEW_HPP

#include "../openstudio_lib/SOConstants.hpp"
#include <boost/smart_ptr.hpp>
#include <QWidget>
#include <QPoint>
#include <QAbstractListModel>

class QListView;

namespace openstudio {

class TemplateListModel;

class StartupView : public QWidget
{
  Q_OBJECT

 public:
  explicit StartupView(QWidget* parent = nullptr);

  virtual ~StartupView() {}

  QSize sizeHint() const override;

 signals:

  void newFromTemplate(NewFromTemplateEnum);

  void openClicked();

  void importClicked();

  void importSDDClicked();

 protected:
  void mouseMoveEvent(QMouseEvent* event) override;

  void mousePressEvent(QMouseEvent* event) override;

  void resizeEvent(QResizeEvent* event) override;

  void paintEvent(QPaintEvent* event) override;

 private slots:

  void newFromTemplateSlot();

  void showDetailsForItem(const QModelIndex& index);

 private:
  QPoint dragPosition;

  bool _move;

  std::shared_ptr<TemplateListModel> m_templateListModel;

  QWidget* m_projectDetailView;

  QListView* m_listView;
};

class TemplateListModel : public QAbstractListModel
{
 public:
  explicit TemplateListModel(QObject* parent = nullptr);

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  Qt::ItemFlags flags(const QModelIndex& index) const override;

 private:
};

}  // namespace openstudio

#endif  // OPENSTUDIO_STARTUPVIEW_HPP
