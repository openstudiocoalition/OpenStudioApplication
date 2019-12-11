/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2008-2019, Alliance for Sustainable Energy, LLC, and other contributors. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************************************************************/

#include "SpecialDayGridView.hpp"

#include "../shared_gui_components/OSGridView.hpp"

#include "ModelObjectItem.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "OSDropZone.hpp"

#include <openstudio/src/model/RunPeriodControlSpecialDays.hpp>
#include <openstudio/src/model/RunPeriodControlSpecialDays_Impl.hpp>

#include <openstudio/src/utilities/time/Date.hpp>

#include <openstudio/src/utilities/idd/IddEnums.hxx>
#include <openstudio/src/utilities/idd/RunPeriodControl_SpecialDays_FieldEnums.hxx>

#include <QBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QScrollArea>
#include <QSettings>
#include <QTimer>

// These defines provide a common area for field display names
// used on column headers, and other grid widgets

#define NAME "Special Day Name"
#define SELECTED "All"
#define DATESTRING "Date String"

#define NTHDAYOFMONTH "Nth Day of Month"
#define DAYOFMONTH "Day Of Month"
#define MONTH "Month"
#define SPECIALDAYTYPE "Special Day Type"
#define DURATION "Duration"

namespace openstudio {

struct ModelObjectNameSorter{
  // sort by name
  bool operator()(const model::ModelObject & lhs, const model::ModelObject & rhs){
    return (lhs.name() < rhs.name());
  }
};

SpecialDayGridView::SpecialDayGridView(bool isIP, const model::Model & model, QWidget * parent)
  : QWidget(parent),
  m_isIP(isIP)
{
  auto layout = new QVBoxLayout();
  layout->setSpacing(0);
  layout->setContentsMargins(0,0,0,0);
  setLayout(layout);

  auto specialDays = model.getConcreteModelObjects<model::RunPeriodControlSpecialDays>();
  auto specialDayModelObjects = subsetCastVector<model::ModelObject>(specialDays);

  m_gridController = new SpecialDayGridController(m_isIP, "Special Days", IddObjectType::OS_RunPeriodControl_SpecialDays, model, specialDayModelObjects);
  auto gridView = new OSGridView(m_gridController, "Special Days", "Drop\nZone", true, parent);

  bool isConnected = false;

  isConnected = connect(gridView, SIGNAL(dropZoneItemClicked(OSItem*)), this, SIGNAL(dropZoneItemClicked(OSItem*)));
  OS_ASSERT(isConnected);

  //isConnected = connect(this, SIGNAL(itemSelected(OSItem *)), gridView, SIGNAL(itemSelected(OSItem*)));
  //OS_ASSERT(isConnected);
  //isConnected = connect(this, SIGNAL(selectionCleared()), gridView, SLOT(onSelectionCleared()));
  //OS_ASSERT(isConnected);
  //isConnected = connect(gridView, SIGNAL(gridRowSelected(OSItem*)), this, SIGNAL(gridRowSelected(OSItem*)));
  //OS_ASSERT(isConnected);

  gridView->m_dropZone->hide();

  layout->addWidget(gridView, 0, Qt::AlignTop);

  layout->addStretch(1);

  isConnected = connect(this, SIGNAL(toggleUnitsClicked(bool)), m_gridController, SIGNAL(toggleUnitsClicked(bool)));
  OS_ASSERT(isConnected);

  isConnected = connect(this, SIGNAL(toggleUnitsClicked(bool)), m_gridController, SLOT(toggleUnits(bool)));
  OS_ASSERT(isConnected);

}

void SpecialDayGridView::onAddClicked()
{
  // TODO: make a popup to facilitate entry of dates either by Date or by NThDayOfMonth (similar to daylight savings time)

  openstudio::NthDayOfWeekInMonth nth(3);
  openstudio::DayOfWeek dayOfWeek = openstudio::DayOfWeek::Friday;
  openstudio::MonthOfYear month = openstudio::MonthOfYear::Jan;
  //
  // Always make at least one
  if (!m_gridController->selectedObjects().size()) {
    model::RunPeriodControlSpecialDays(nth, dayOfWeek, month, m_gridController->model());
  }
  else {
    for (auto &obj : m_gridController->selectedObjects())
    {
      addObject(obj);
    }
  }

}

void SpecialDayGridView::onCopyClicked()
{
  for (auto &obj : m_gridController->selectedObjects())
  {
    if (!obj.handle().isNull()){
      copyObject(obj);
    }
  }
}

void SpecialDayGridView::onRemoveClicked()
{
  for (auto &obj : m_gridController->selectedObjects())
  {
    removeObject(obj);
  }
}

void SpecialDayGridView::onPurgeClicked()
{
  purgeObjects(IddObjectType::OS_SizingPeriod_DesignDay);
}

void SpecialDayGridView::addObject(const model::ModelObject& modelObject)
{
  // TODO:
  openstudio::NthDayOfWeekInMonth nth(3);
  openstudio::DayOfWeek dayOfWeek = openstudio::DayOfWeek::Friday;
  openstudio::MonthOfYear month = openstudio::MonthOfYear::Jan;
  model::RunPeriodControlSpecialDays(nth, dayOfWeek, month, m_gridController->model());
}

void SpecialDayGridView::copyObject(const openstudio::model::ModelObject& modelObject)
{
  modelObject.clone(m_gridController->model());
}

void SpecialDayGridView::removeObject(openstudio::model::ModelObject modelObject)
{
  modelObject.remove();
}

// TODO: that's dangerous, probably should remove
void SpecialDayGridView::purgeObjects(const IddObjectType& iddObjectType)
{
  for (auto mo : m_gridController->model().getConcreteModelObjects<model::RunPeriodControlSpecialDays>()){
    mo.remove();
  }
}

std::vector<model::ModelObject> SpecialDayGridView::selectedObjects() const
{
  return m_gridController->selectedObjects();
}

SpecialDayGridController::SpecialDayGridController(bool isIP,
  const QString & headerText,
  IddObjectType iddObjectType,
  model::Model model,
  std::vector<model::ModelObject> modelObjects) :
  OSGridController(isIP, headerText, iddObjectType, model, modelObjects)
{
  setCategoriesAndFields();
}

void SpecialDayGridController::setCategoriesAndFields()
{
  {
    std::vector<QString> fields;
    fields.push_back(DATESTRING);
    fields.push_back(NTHDAYOFMONTH);
    fields.push_back(DAYOFMONTH);
    fields.push_back(MONTH);
    fields.push_back(SPECIALDAYTYPE);
    fields.push_back(DURATION);
    std::pair<QString,std::vector<QString> > categoryAndFields = std::make_pair(QString("General"),fields);
    m_categoriesAndFields.push_back(categoryAndFields);
  }

  // Ideally one could filter the days by whether they have a specific date set or not...
  {
    std::vector<QString> fields;
    fields.push_back(DAYOFMONTH);
    fields.push_back(MONTH);
    fields.push_back(SPECIALDAYTYPE);
    fields.push_back(DURATION);
    std::pair<QString, std::vector<QString> > categoryAndFields = std::make_pair(QString("By Specific Date"), fields);
    m_categoriesAndFields.push_back(categoryAndFields);
  }

  {
    std::vector<QString> fields;
    fields.push_back(NTHDAYOFMONTH);
    fields.push_back(MONTH);
    fields.push_back(SPECIALDAYTYPE);
    fields.push_back(DURATION);
    std::pair<QString, std::vector<QString> > categoryAndFields = std::make_pair(QString("By Nth Day"), fields);
    m_categoriesAndFields.push_back(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();

}

void SpecialDayGridController::addColumns(const QString &/*category*/, std::vector<QString> & fields)
{
  // always show name column
  fields.insert(fields.begin(), { NAME, SELECTED });

  m_baseConcepts.clear();

  // auto getNthDayOfMonth =

  for (const QString& field : fields) {

    if (field == SELECTED){
      auto checkbox = QSharedPointer<QCheckBox>(new QCheckBox());
      checkbox->setToolTip("Check to select all rows");
      connect(checkbox.data(), &QCheckBox::stateChanged, this, &SpecialDayGridController::selectAllStateChanged);
      connect(checkbox.data(), &QCheckBox::stateChanged, this->gridView(), &OSGridView::gridRowSelectionChanged);

      addSelectColumn(Heading(QString(SELECTED), false, false, checkbox),
        "Check to select this row"
        );
    }
    // INTEGER
    else if (field == DAYOFMONTH) {
      addComboBoxColumn<std::string, model::RunPeriodControlSpecialDays>(Heading(QString(SPECIALDAYTYPE)),
        static_cast<std::string(*)(const std::string&)>(&openstudio::toString),
        std::function<std::vector<std::string>()>(&model::RunPeriodControlSpecialDays::validSpecialDayTypeValues),
        CastNullAdapter<model::RunPeriodControlSpecialDays>(&model::RunPeriodControlSpecialDays::specialDayType),
        CastNullAdapter<model::RunPeriodControlSpecialDays>(&model::RunPeriodControlSpecialDays::setSpecialDayType),
        boost::none, // reset
        boost::none, // defaulted,
        boost::optional<DataSource>()
      );

      addValueEditColumn(Heading(QString(DAYOFMONTH)),
        NullAdapter(&model::DesignDay::dayOfMonth),
        NullAdapter(&model::DesignDay::setDayOfMonth),
        boost::optional<std::function<void(model::DesignDay*)>>(CastNullAdapter<model::DesignDay>(&model::DesignDay::resetDayOfMonth)),
        boost::optional<std::function<bool(model::DesignDay*)>>(CastNullAdapter<model::DesignDay>(&model::DesignDay::isDayOfMonthDefaulted)),
        boost::optional<DataSource>()
        );
    }
    else if (field == MONTH) {
      addValueEditColumn(Heading(QString(MONTH)),
        NullAdapter(&model::DesignDay::month),
        NullAdapter(&model::DesignDay::setMonth),
        boost::optional<std::function<void(model::DesignDay*)>>(CastNullAdapter<model::DesignDay>(&model::DesignDay::resetMonth)),
        boost::optional<std::function<bool(model::DesignDay*)>>(CastNullAdapter<model::DesignDay>(&model::DesignDay::isMonthDefaulted)),
        boost::optional<DataSource>()
        );
    }
    else if (field == DURATION) {
      addValueEditColumn(Heading(QString(DURATION)),
        NullAdapter(&model::RunPeriodControlSpecialDays::duration),
        NullAdapter(&model::DesignDay::setDuration),
        boost::none,
        boost::none,
        boost::optional<DataSource>()
        );
    }
        // STRING
    else if (field == NAME) {
      addNameLineEditColumn(Heading(QString(NAME), false, false),
        false,
        false,
        CastNullAdapter<model::RunPeriodControlSpecialDays>(&model::RunPeriodControlSpecialDays::name),
        CastNullAdapter<model::RunPeriodControlSpecialDays>(&model::RunPeriodControlSpecialDays::setName)
        );
    }
    else if (field == DATESTRING){
     // TODO
    }
    else if (field == SPECIALDAYTYPE) {
      addComboBoxColumn<std::string, model::RunPeriodControlSpecialDays>(Heading(QString(SPECIALDAYTYPE)),
        static_cast<std::string(*)(const std::string&)>(&openstudio::toString),
        std::function<std::vector<std::string>()>(&model::RunPeriodControlSpecialDays::validSpecialDayTypeValues),
        CastNullAdapter<model::RunPeriodControlSpecialDays>(&model::RunPeriodControlSpecialDays::specialDayType),
        CastNullAdapter<model::RunPeriodControlSpecialDays>(&model::RunPeriodControlSpecialDays::setSpecialDayType),
        boost::none, // reset
        boost::none, // defaulted,
        boost::optional<DataSource>()
        );
    }
    else{
      // unhandled
      OS_ASSERT(false);
    }

  }

}

QString SpecialDayGridController::getColor(const model:: ModelObject & modelObject)
{
  QColor defaultColor(Qt::lightGray);
  QString color(defaultColor.name());

  return color;
}

void SpecialDayGridController::checkSelectedFields()
{
  if(!this->m_hasHorizontalHeader) return;

  OSGridController::checkSelectedFields();
}

void SpecialDayGridController::onItemDropped(const OSItemId& itemId)
{
  boost::optional<model::ModelObject> modelObject = OSAppBase::instance()->currentDocument()->getModelObject(itemId);
  if (modelObject){
    if (modelObject->optionalCast<model::DesignDay>()){
      modelObject->clone(m_model);
      emit modelReset();
    }
  }
}

void SpecialDayGridController::refreshModelObjects()
{
  auto designDays = m_model.getConcreteModelObjects<model::DesignDay>();
  m_modelObjects = subsetCastVector<model::ModelObject>(designDays);
  std::sort(m_modelObjects.begin(), m_modelObjects.end(), ModelObjectNameSorter());
}

void SpecialDayGridController::onComboBoxIndexChanged(int index)
{
  // Note: find the correct system color on RACK change,
  // but currently unable to know which row changed.
  for(unsigned index = 0; index < m_horizontalHeader.size(); index++){
  HorizontalHeaderWidget * horizontalHeaderWidget = qobject_cast<HorizontalHeaderWidget *>(m_horizontalHeader.at(index));
    if(horizontalHeaderWidget->m_label->text() == "RACK"){
      // NOTE required due to a race condition
      // Code below commented out due to a very infrequent crash in the bowels of Qt appears to be exasperated by this refresh.
      // A new refresh scheme with finer granularity may eliminate the problem, and restore rack colors.
      //QTimer::singleShot(0, this, SLOT(reset())); TODO
      break;
    }
  }
}

} // openstudio
