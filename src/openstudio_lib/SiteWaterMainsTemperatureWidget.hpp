/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SITEWATERMAINSTEMPERATUREWIDGET_HPP
#define OPENSTUDIO_SITEWATERMAINSTEMPERATUREWIDGET_HPP

#include <openstudio/model/SiteWaterMainsTemperature.hpp>

#include <QWidget>

#include <boost/optional.hpp>

class QLabel;

namespace openstudio {

class OSComboBox2;
class OSDropZone2;
class OSQuantityEdit2;

/** Inspector widget for OS:Site:WaterMainsTemperature. */
class SiteWaterMainsTemperatureWidget : public QWidget
{
  Q_OBJECT

 public:
  explicit SiteWaterMainsTemperatureWidget(bool isIP, QWidget* parent = nullptr);

  void attach(const model::ModelObject& obj);
  void detach();

 signals:
  void toggleUnitsClicked(bool displayIP);

 private slots:
  void refreshVisibility();

 private:
  bool m_isIP;
  boost::optional<model::SiteWaterMainsTemperature> m_obj;

  OSComboBox2* m_calculationMethod = nullptr;

  // Visible only when method == "Schedule"
  QLabel* m_scheduleLabel = nullptr;
  OSDropZone2* m_scheduleDropZone = nullptr;

  // Visible only when method == "Correlation"
  QLabel* m_annualAvgLabel = nullptr;
  OSQuantityEdit2* m_annualAvgTemp = nullptr;
  QLabel* m_maxDiffLabel = nullptr;
  OSQuantityEdit2* m_maxDiffTemp = nullptr;

  // Hidden when method == "Schedule"
  QLabel* m_multiplierLabel = nullptr;
  OSQuantityEdit2* m_multiplier = nullptr;
  QLabel* m_offsetLabel = nullptr;
  OSQuantityEdit2* m_offset = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SITEWATERMAINSTEMPERATUREWIDGET_HPP
