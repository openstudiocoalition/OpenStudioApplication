/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_UTILITYBILLSVIEW_HPP
#define OPENSTUDIO_UTILITYBILLSVIEW_HPP

#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement
#include "ModelObjectInspectorView.hpp"
#include "ModelSubTabView.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/UtilityBill.hpp>

#include <openstudio/utilities/data/DataEnums.hpp>

#include <QWidget>

class QButtonGroup;
class QDate;
class QDateEdit;
class QLabel;
class QLineEdit;
class QPushButton;
class QString;
class QVBoxLayout;

namespace openstudio {

class BillFormatDialog;
class BillingPeriod;
class Date;
class FuelType;
class OSComboBox2;
class OSDoubleEdit2;
class OSIntegerEdit2;
class OSLineEdit2;
class OSUnsignedEdit2;
class UtilityBillListView;

class BillingPeriodWidget;

enum BillFormat
{
  STARTDATE_ENDDATE,
  STARTDATE_NUMDAYS,
  ENDDATE_NUMDAYS
};

class UtilityBillsView : public ModelSubTabView
{
  Q_OBJECT

 public:
  explicit UtilityBillsView(const model::Model& model, QWidget* parent = nullptr);

  virtual ~UtilityBillsView();

 private:
  void resetUtilityBills();

  static std::vector<std::pair<FuelType, std::string>> utilityBillFuelTypesAndNames();

 signals:

  void enableAddNewObjectButton(bool enable);
};

class UtilityBillsInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  explicit UtilityBillsInspectorView(const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~UtilityBillsInspectorView();

  boost::optional<QString> runPeriodDates();

 protected:
  virtual void onSelectItem(OSItem* item) override;
  virtual void onClearSelection() override;
  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;
  virtual void onUpdate() override;

 protected slots:

  //virtual void toggleUnits(bool displayIP);

 private:
  void createWidgets();
  void getSortedBills();
  void showAddButton();
  void hideAddButton();
  void enableAddButton();
  void disableAddButton();
  void attach(const openstudio::model::UtilityBill& utilityBill);
  void detach();
  void refresh();

  void createBillingPeriodHeaderWidget();
  void addBillingPeriodWidget(model::BillingPeriod& billingPeriod);
  void addBillingPeriodWidgets();

  void deleteBillingPeriodWidgets();

  QString getEnergyUseLabelText();
  QString getPeakLabelText();

  void updateEnergyUseLabelText();
  void updatePeakLabelText();

  boost::optional<model::UtilityBill> m_utilityBill;

  std::vector<BillingPeriodWidget*> m_billingPeriodWidgets;

  BillFormat m_billFormat;

  bool m_showPeak;

  QWidget* m_billingPeriodHeaderWidget;

  QButtonGroup* m_buttonGroup;

  OSLineEdit2* m_name;

  QLabel* m_consumptionUnitsLabel;
  OSComboBox2* m_consumptionUnits;
  QLabel* m_peakDemandUnitsLabel;
  OSComboBox2* m_peakDemandUnits;
  QLabel* m_windowTimestepsLabel;
  OSUnsignedEdit2* m_windowTimesteps;

  QPushButton* m_addBillingPeriod;

  QVBoxLayout* m_billPeriodLayout;

  QWidget* m_billPeriodLayoutWidget;

  QLabel* m_runPeriodDatesLabel;

  QLabel* m_energyUseLabel;
  QLabel* m_peakLabel;

  QString m_energyUseUnits;
  QString m_peakUnits;

  int m_hiddenWidgetIndex;
  int m_visibleWidgetIndex;

 signals:
  void showSubTabView(bool showSubTabView);
  void enableAddNewObjectButton(bool enable);

 private slots:
  void addBillingPeriod(bool checked);
  void deleteBillingPeriod(int idx);
  void setBillFormat(int index);
  void setBillFormat(BillFormat billFormat);
  void updateEnergyUseLabelText(const QString& text);
  void updatePeakLabelText(const QString& text);
  void updateRunPeriodDates();
  void deleteAllWidgetsAndLayoutItems(QLayout* layout, bool deleteWidgets = true);
};

class BillingPeriodWidget
  : public QWidget
  , public Nano::Observer
{

  Q_OBJECT

 public:
  BillingPeriodWidget(model::BillingPeriod billingPeriod, const openstudio::FuelType& fuelType, BillFormat billFormat, QWidget* parent = nullptr);

  virtual ~BillingPeriodWidget() = default;

  void attach(const openstudio::model::BillingPeriod& billingPeriod);
  void detach();

  QDateEdit* m_startDateEdit;
  QDateEdit* m_endDateEdit;

  OSIntegerEdit2* m_billingPeriodIntEdit;
  OSDoubleEdit2* m_energyUseDoubleEdit;
  OSDoubleEdit2* m_peakDoubleEdit;
  OSDoubleEdit2* m_costDoubleEdit;

  QPushButton* m_deleteBillWidget;

 private:
  void createWidgets(const FuelType& fuelType, BillFormat billFormat);

  boost::optional<model::BillingPeriod> m_billingPeriod;

 private slots:

  void modelObjectChanged();
  void startDateChanged(const QDate& newdate);
  void endDateChanged(const QDate& newdate);
};

}  // namespace openstudio

#endif  // OPENSTUDIO_UTILITYBILLSVIEW_HPP
