/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_LOOPCHOOSERVIEW_HPP
#define OPENSTUDIO_LOOPCHOOSERVIEW_HPP

#include <QWidget>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/WaterToAirComponent.hpp>
#include <openstudio/model/HVACComponent.hpp>
#include <openstudio/model/Loop.hpp>
#include <openstudio/utilities/core/String.hpp>

class QCheckBox;

class QVBoxLayout;

namespace openstudio {

class LoopChooserItem;

class LoopChooserView : public QWidget
{
  Q_OBJECT;

 public:
  explicit LoopChooserView(QWidget* parent = nullptr);

  virtual ~LoopChooserView() {}

  LoopChooserItem* loopChooserItemForLoop(const std::string& loopName);

  void layoutView();

 public slots:

  void layoutModelObject(boost::optional<model::ModelObject>& modelObject);

  void onAddToLoopClicked(model::Loop&);

  void onRemoveFromLoopClicked(model::Loop&);

 signals:

  void addToLoopClicked(model::Loop&, boost::optional<model::HVACComponent>&);

  void removeFromLoopClicked(model::Loop&, boost::optional<model::HVACComponent>&);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  boost::optional<model::HVACComponent> m_component;

  QVBoxLayout* m_vLayout;

  std::vector<LoopChooserItem*> m_loopChooserItems;
};

class LoopChooserItem : public QWidget
{
  Q_OBJECT;

 public:
  explicit LoopChooserItem(model::Loop&, LoopChooserView* parent = nullptr);

  virtual ~LoopChooserItem() {}

  std::string loopName();

 signals:

  void addToLoopClicked(model::Loop&);

  void removeFromLoopClicked(model::Loop&);

 public slots:

  void setChecked(bool checked);

 private slots:

  void sendClickedSignal(bool checked);

 private:
  QCheckBox* m_checkBox;

  LoopChooserView* m_loopChooserView;

  model::Loop m_loop;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_LOOPCHOOSERVIEW_HPP
