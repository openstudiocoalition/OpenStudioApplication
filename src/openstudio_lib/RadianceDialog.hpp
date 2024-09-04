/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_RADIANCEDIALOG_HPP
#define OPENSTUDIO_RADIANCEDIALOG_HPP

#include "../shared_gui_components/OSDialog.hpp"

class QWidget;

namespace openstudio {

class RadianceDialog : public OSDialog
{
  Q_OBJECT

 public:
  RadianceDialog(bool isIP, QWidget* radianceInspector, QWidget* parent = nullptr);

  virtual ~RadianceDialog() = default;

 private:
  virtual void createLayout() override;
  void createLayoutInternal();

  QWidget* m_radianceInspector;

 protected slots:

  void toggleUnits(bool displayIP) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_RADIANCEDIALOG_HPP
