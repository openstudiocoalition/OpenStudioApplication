/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_ZONECHOOSERVIEW_HPP
#define OPENSTUDIO_ZONECHOOSERVIEW_HPP

#include <QWidget>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/AirLoopHVACZoneSplitter.hpp>
#include <openstudio/model/AirLoopHVACZoneMixer.hpp>
#include <openstudio/model/ThermalZone.hpp>
#include <openstudio/utilities/core/String.hpp>

class QCheckBox;

class QVBoxLayout;

namespace openstudio {

class ZoneChooserItem;

class ZoneChooserView : public QWidget
{
  Q_OBJECT;

 public:
  explicit ZoneChooserView(QWidget* parent = nullptr);

  virtual ~ZoneChooserView() {}

  ZoneChooserItem* zoneChooserItemForZone(const std::string& zoneName);

  void layoutView();

 public slots:

  void layoutModelObject(model::ModelObject& modelObject);

 signals:

  void addZoneClicked(model::ThermalZone&);

  void removeZoneClicked(model::ThermalZone&);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  model::OptionalAirLoopHVACZoneSplitter m_splitter;

  model::OptionalAirLoopHVACZoneMixer m_mixer;

  QVBoxLayout* m_vLayout;

  std::vector<ZoneChooserItem*> m_zoneChooserItems;
};

class ZoneChooserItem : public QWidget
{
  Q_OBJECT;

 public:
  explicit ZoneChooserItem(model::ThermalZone&, ZoneChooserView* parent = nullptr);

  virtual ~ZoneChooserItem() {}

  std::string zoneName();

 signals:

  void addZoneClicked(model::ThermalZone&);

  void removeZoneClicked(model::ThermalZone&);

 public slots:

  void setChecked(bool checked);

 private slots:

  void sendClickedSignal(bool checked);

 private:
  QCheckBox* m_checkBox;

  ZoneChooserView* m_zoneChooserView;

  model::ThermalZone m_thermalZone;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_ZONECHOOSERVIEW_HPP
