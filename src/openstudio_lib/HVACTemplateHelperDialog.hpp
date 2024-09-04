/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_HVACTEMPLATEHELPERDIALOG_HPP
#define OPENSTUDIO_HVACTEMPLATEHELPERDIALOG_HPP

#include <QDialog>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/ThermalZone.hpp>

class QButtonGroup;

namespace openstudio {

class HVACTemplateHelperDialog : public QDialog
{
  Q_OBJECT;

 public:
  explicit HVACTemplateHelperDialog(const model::Model& model, QWidget* parent = nullptr);

  virtual ~HVACTemplateHelperDialog() {}

  std::vector<model::ThermalZone> selectedZones();

 private:
  model::Model m_model;

  std::vector<model::ThermalZone> m_zones;

  QButtonGroup* m_buttonGroup;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_HVACTEMPLATEHELPERDIALOG_HPP
