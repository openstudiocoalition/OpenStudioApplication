/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_LIFECYCLECOSTSTABVIEW_HPP
#define OPENSTUDIO_LIFECYCLECOSTSTABVIEW_HPP

#include "MainTabView.hpp"

#include <openstudio/model/LifeCycleCostParameters.hpp>
#include <openstudio/model/Model.hpp>

#include <QWidget>

class QButtonGroup;
class QLabel;
class QStackedWidget;

namespace openstudio {

class OSComboBox2;
class OSDoubleEdit2;
class OSIntegerEdit2;

namespace model {
class Model;
}

class LifeCycleCostsView : public QWidget
{
  Q_OBJECT

 public:
  explicit LifeCycleCostsView(const model::Model& model);

  virtual ~LifeCycleCostsView();

 private:
  void createWidgets();

  QWidget* createInflationRatesWidget();

  QWidget* createNistWidget();

  void attach(openstudio::model::LifeCycleCostParameters& lifeCycleCostParameters);

  void detach();

  model::Model m_model;

  boost::optional<model::LifeCycleCostParameters> m_lifeCycleCostParameters;

  OSIntegerEdit2* m_analysisLengthIntegerEdit;

  OSDoubleEdit2* m_realDiscountRateDoubleEdit;

  OSDoubleEdit2* m_electricityDoubleEdit;
  OSDoubleEdit2* m_naturalGasDoubleEdit;
  OSDoubleEdit2* m_steamDoubleEdit;
  OSDoubleEdit2* m_gasolineDoubleEdit;

  OSDoubleEdit2* m_dieselDoubleEdit;
  OSDoubleEdit2* m_propaneDoubleEdit;
  OSDoubleEdit2* m_coalDoubleEdit;
  OSDoubleEdit2* m_fuelOil_1DoubleEdit;

  OSDoubleEdit2* m_fuelOil_2DoubleEdit;
  OSDoubleEdit2* m_waterDoubleEdit;

  QButtonGroup* m_fempGroup;
  QButtonGroup* m_nistGroup;

  OSComboBox2* m_nistRegionComboBox;
  OSComboBox2* m_nistSectorComboBox;

  QStackedWidget* m_stackedWidget;

  QLabel* m_analysisLengthLabel;
  QLabel* m_realDiscountRateLabel;

 private slots:

  void fempGroupClicked(int index);

  void nistGroupClicked(int index);
};

//class LifeCycleCostsTabView : public MainTabView
//{

//Q_OBJECT

//public:
//LifeCycleCostsTabView(const model::Model& model, const QString& modelTempDir, QWidget* parent = nullptr);

//virtual ~LifeCycleCostsTabView();

//private:
//LifeCycleCostsView* m_lifeCycleCostsView;
//};

}  // namespace openstudio

#endif  // OPENSTUDIO_LIFECYCLECOSTSTABVIEW_HPP
