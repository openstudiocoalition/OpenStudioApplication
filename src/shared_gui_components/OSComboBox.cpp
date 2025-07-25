/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSComboBox.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>

#include <openstudio/utilities/idf/WorkspaceObject.hpp>
#include <openstudio/utilities/idf/WorkspaceObject_Impl.hpp>
#include <openstudio/utilities/core/Assert.hpp>

#include <QCompleter>
#include <QEvent>

#include <bitset>

namespace openstudio {

OSObjectListCBDS::OSObjectListCBDS(const IddObjectType& type, const model::Model& model)
  : OSComboBoxDataSource(), m_allowEmptySelection(false), m_model(model) {
  m_types.push_back(type);

  initialize();
}

OSObjectListCBDS::OSObjectListCBDS(const std::vector<IddObjectType>& types, const model::Model& model)
  : OSComboBoxDataSource(), m_allowEmptySelection(true), m_types(types), m_model(model) {
  initialize();
}

void OSObjectListCBDS::initialize() {
  std::vector<model::ModelObject> modelObjects = m_model.getModelObjects<model::ModelObject>();

  for (const auto& modelObject : modelObjects) {
    if (std::find(m_types.begin(), m_types.end(), modelObject.iddObjectType()) != m_types.end()) {
      m_workspaceObjects << modelObject;

      modelObject.getImpl<model::detail::ModelObject_Impl>()->onChange.connect<OSObjectListCBDS, &OSObjectListCBDS::onObjectChanged>(this);
    }
  }

  m_model.getImpl<model::detail::Model_Impl>()->addWorkspaceObject.connect<OSObjectListCBDS, &OSObjectListCBDS::onObjectAdded>(this);

  m_model.getImpl<model::detail::Model_Impl>()->removeWorkspaceObject.connect<OSObjectListCBDS, &OSObjectListCBDS::onObjectWillBeRemoved>(this);
}

int OSObjectListCBDS::numberOfItems() {
  if (m_allowEmptySelection) {
    return m_workspaceObjects.count() + 1;
  } else {
    return m_workspaceObjects.count();
  }
}

QString OSObjectListCBDS::valueAt(int i) {
  if (m_allowEmptySelection) {
    if (i > 0) {
      return QString::fromStdString(m_workspaceObjects[i - 1].name().get());
    } else {
      return QString("");
    }
  } else {
    return QString::fromStdString(m_workspaceObjects[i].name().get());
  }
}

void OSObjectListCBDS::onObjectAdded(const WorkspaceObject& workspaceObject, const openstudio::IddObjectType& type, const openstudio::UUID& uuid) {
  if (std::find(m_types.begin(), m_types.end(), workspaceObject.cast<model::ModelObject>().iddObjectType()) != m_types.end()) {
    m_workspaceObjects << workspaceObject;

    workspaceObject.getImpl<model::detail::ModelObject_Impl>()->onChange.connect<OSObjectListCBDS, &OSObjectListCBDS::onObjectChanged>(this);
    if (m_allowEmptySelection) {
      emit itemAdded(m_workspaceObjects.size());
    } else {
      emit itemAdded(m_workspaceObjects.size() - 1);
    }
  }
}

void OSObjectListCBDS::onObjectWillBeRemoved(const WorkspaceObject& workspaceObject, const openstudio::IddObjectType& type,
                                             const openstudio::UUID& uuid) {
  if (std::find(m_types.begin(), m_types.end(), workspaceObject.cast<model::ModelObject>().iddObjectType()) != m_types.end()) {
    int i = m_workspaceObjects.indexOf(workspaceObject);

    m_workspaceObjects.removeAt(i);

    if (m_allowEmptySelection) {
      emit itemRemoved(i + 1);
    } else {
      emit itemRemoved(i);
    }
  }
}

void OSObjectListCBDS::onObjectChanged() {
  // WorkspaceObject workspaceObject = qobject_cast<detail::WorkspaceObject_Impl *>(sender())->getObject<WorkspaceObject>();

  // if(std::find(m_types.begin(),m_types.end(),workspaceObject.cast<model::ModelObject>().iddObjectType()) != m_types.end())
  // {

  //   int i = m_workspaceObjects.indexOf(workspaceObject);

  //   if( m_allowEmptySelection )
  //   {
  //     emit itemChanged(i + 1);
  //   }
  //   else
  //   {
  //     emit itemChanged(i);
  //   }
  // }
}

OSComboBox2::OSComboBox2(QWidget* parent, bool editable) : QComboBox(parent), m_editable(editable) {
  this->setAcceptDrops(false);
  setEditable(m_editable);
  if (m_editable) {
    auto* completer = new QCompleter();
    this->setCompleter(completer);
  }
  setEnabled(false);  // default locked

  // if multiple qss rules apply with same specificity then the last one is chosen
  this->setStyleSheet("QComboBox[style=\"0000\"] { color:black; background:#e6e6e6; } "  // Locked=0, Editable=0, Focused=0, Defaulted=0
                      "QComboBox[style=\"0001\"] { color:green; background:#e6e6e6; } "  // Locked=0, Editable=0, Focused=0, Defaulted=1
                      "QComboBox[style=\"0010\"] { color:black; background:#e6e6e6; } "  // Locked=0, Editable=0, Focused=1, Defaulted=0
                      "QComboBox[style=\"0011\"] { color:green; background:#e6e6e6; } "  // Locked=0, Editable=0, Focused=1, Defaulted=1
                      "QComboBox[style=\"0100\"] { color:black; background:white;   } "  // Locked=0, Editable=1, Focused=0, Defaulted=0
                      "QComboBox[style=\"0101\"] { color:green; background:white;   } "  // Locked=0, Editable=1, Focused=0, Defaulted=1
                      "QComboBox[style=\"0110\"] { color:black; background:#ffc627; } "  // Locked=0, Editable=1, Focused=1, Defaulted=0
                      "QComboBox[style=\"0111\"] { color:green; background:#ffc627; } "  // Locked=0, Editable=1, Focused=1, Defaulted=1
                      "QComboBox[style=\"1000\"] { color:black; background:#e6e6e6; } "  // Locked=1, Editable=0, Focused=0, Defaulted=0
                      "QComboBox[style=\"1001\"] { color:green; background:#e6e6e6; } "  // Locked=1, Editable=0, Focused=0, Defaulted=1
                      "QComboBox[style=\"1010\"] { color:black; background:#cc9a00; } "  // Locked=1, Editable=0, Focused=1, Defaulted=0
                      "QComboBox[style=\"1011\"] { color:green; background:#cc9a00; } "  // Locked=1, Editable=0, Focused=1, Defaulted=1
                      "QComboBox[style=\"1100\"] { color:black; background:#e6e6e6; } "  // Locked=1, Editable=1, Focused=0, Defaulted=0
                      "QComboBox[style=\"1101\"] { color:green; background:#e6e6e6; } "  // Locked=1, Editable=1, Focused=0, Defaulted=1
                      "QComboBox[style=\"1110\"] { color:black; background:#cc9a00; } "  // Locked=1, Editable=1, Focused=1, Defaulted=0
                      "QComboBox[style=\"1111\"] { color:green; background:#cc9a00; } "  // Locked=1, Editable=1, Focused=1, Defaulted=1
  );

  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}

OSComboBox2::~OSComboBox2() = default;

bool OSComboBox2::event(QEvent* e) {
  if (e->type() == QEvent::Wheel) {
    return false;
  } else if (e->type() == QEvent::FocusIn && m_hasClickFocus) {
    m_focused = true;
    updateStyle();

    emit inFocus(m_focused, hasData());

    return QComboBox::event(e);
  } else if (e->type() == QEvent::FocusOut && m_hasClickFocus) {
    m_focused = false;
    updateStyle();

    emit inFocus(m_focused, false);

    return QComboBox::event(e);
  } else {
    return QComboBox::event(e);
  }
}

void OSComboBox2::enableClickFocus() {
  m_hasClickFocus = true;
  this->setFocusPolicy(Qt::ClickFocus);
}

void OSComboBox2::disableClickFocus() {
  m_hasClickFocus = false;
  this->setFocusPolicy(Qt::NoFocus);
  clearFocus();
}

bool OSComboBox2::hasData() {
  return !this->currentText().isEmpty();
}

bool OSComboBox2::locked() const {
  return m_locked;
}

void OSComboBox2::setLocked(bool locked) {
  if (m_locked != locked) {
    m_locked = locked;
    setEnabled(!locked);
    updateStyle();
  }
}

void OSComboBox2::bind(std::shared_ptr<OSComboBoxDataSource> dataSource) {
  m_dataSource = dataSource;

  clear();
  completeBind();
}

void OSComboBox2::unbind() {
  if (m_modelObject) {
    // disconnect( m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get() );

    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.disconnect<OSComboBox2, &OSComboBox2::onModelObjectChanged>(this);
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
      ->onRemoveFromWorkspace.disconnect<OSComboBox2, &OSComboBox2::onModelObjectRemoved>(this);
    // m_modelObject->model().getImpl<openstudio::model::detail::Model_Impl>().get()->onChange.disconnect<OSComboBox2, &OSComboBox2::onChoicesRefreshTrigger>(this);

    m_modelObject.reset();
    m_choiceConcept.reset();
  }

  if (m_dataSource) {
    disconnect(m_dataSource.get());

    m_dataSource.reset();
  }

  this->blockSignals(true);
  clear();
  setLocked(true);
  this->blockSignals(false);
}

void OSComboBox2::onModelObjectChanged() {
  OS_ASSERT(m_modelObject);

  if (m_choiceConcept) {
    std::string value = m_choiceConcept->get();

    auto it = std::find(m_values.begin(), m_values.end(), value);

    int i = int(it - m_values.begin());
    this->blockSignals(true);
    setCurrentIndex(i);
    updateStyle();
    this->blockSignals(false);
  }
}

void OSComboBox2::onModelObjectRemoved(const Handle& handle) {
  unbind();
}

void OSComboBox2::onActivated(int index) {
  if (m_choiceConcept) {
    if (index == this->currentIndex() && m_choiceConcept->isDefaulted()) {
      this->onCurrentIndexChanged(this->currentText());
    }
  }
}

void OSComboBox2::onCurrentIndexChanged(const QString& text) {
  emit inFocus(m_focused, hasData());

  OS_ASSERT(m_modelObject);

  if (m_choiceConcept) {
    std::string value = text.toStdString();

    this->blockSignals(true);
    m_choiceConcept->set(value);
    onModelObjectChanged();  // will be sure to display actual value
    this->blockSignals(false);
  }
}

void OSComboBox2::onEditTextChanged(const QString& text) {
  OS_ASSERT(m_modelObject);

  if (m_choiceConcept) {
    std::string value = text.toStdString();

    this->blockSignals(true);
    m_choiceConcept->set(value);
    // We need to trigger repopulating of the ComboBox items so we can actually set the index...
    // No need to check if m_choiceConcept->editable(), because this slot is connected in completeBind
    // only if it's editable
    onChoicesRefreshTrigger();
    this->blockSignals(false);
  }
}

void OSComboBox2::onChoicesRefreshTrigger() {
  if (m_choiceConcept) {
    m_values = m_choiceConcept->choices();
    this->blockSignals(true);

    clear();
    for (const auto& value : m_values) {
      QString qvalue = QString::fromStdString(value);
      addItem(qvalue);
      setItemData(count() - 1, qvalue, Qt::ToolTipRole);
    }

    // re-initialize
    onModelObjectChanged();

    this->blockSignals(false);
    setLocked(false);
  }
}

void OSComboBox2::onDataSourceChange(int i) {
  this->setItemText(i, m_dataSource->valueAt(i));
}

void OSComboBox2::onDataSourceAdd(int i) {
  int oldIndex = this->currentIndex();

  this->insertItem(i, m_dataSource->valueAt(i));

  if (oldIndex == -1) {
    setCurrentIndex(oldIndex);
  }
}

void OSComboBox2::onDataSourceRemove(int i) {
  if (currentIndex() == i) {
    this->setCurrentIndex(-1);
  }

  this->removeItem(i);
}

void OSComboBox2::updateStyle() {
  // Locked, Focused, Defaulted
  std::bitset<4> style;
  style[0] = m_choiceConcept ? m_choiceConcept->isDefaulted() : false;
  style[1] = m_focused;
  style[2] = m_editable;
  style[3] = m_locked;
  QString thisStyle = QString::fromStdString(style.to_string());

  QVariant currentStyle = property("style");
  if (currentStyle.isNull() || currentStyle.toString() != thisStyle) {
    this->setProperty("style", thisStyle);
    this->style()->unpolish(this);
    this->style()->polish(this);
  }
}

void OSComboBox2::completeBind() {
  if (m_modelObject) {
    // connections
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->onChange.connect<OSComboBox2, &OSComboBox2::onModelObjectChanged>(this);
    m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()
      ->onRemoveFromWorkspace.connect<OSComboBox2, &OSComboBox2::onModelObjectRemoved>(this);

    connect(this, static_cast<void (OSComboBox2::*)(int)>(&OSComboBox2::activated), this, &OSComboBox2::onActivated);
    connect(this, static_cast<void (OSComboBox2::*)(const QString&)>(&OSComboBox2::currentTextChanged), this, &OSComboBox2::onCurrentIndexChanged);

    if (isEditable()) {
      connect(this, &OSComboBox2::editTextChanged, this, &OSComboBox2::onEditTextChanged);
    }

    // isConnected = connect( m_modelObject->model().getImpl<openstudio::model::detail::Model_Impl>().get(),
    //                        SIGNAL(addWorkspaceObject(const WorkspaceObject&, const openstudio::IddObjectType&, const openstudio::UUID&)),
    //                        this,
    //                        SLOT(onChoicesRefreshTrigger()) );
    // OS_ASSERT(isConnected);

    // isConnected = connect( m_modelObject->model().getImpl<openstudio::model::detail::Model_Impl>().get(),
    //                        SIGNAL(removeWorkspaceObject(const WorkspaceObject&, const openstudio::IddObjectType&, const openstudio::UUID&)),
    //                        this,
    //                        SLOT(onChoicesRefreshTrigger()) );
    // OS_ASSERT(isConnected);

    // if this is too burdensome, implement Workspace_Impl onNameChange() signal and uncomment the above two connections.
    // (IdfObject_Impl already has onNameChange(); Workspace_Impl::onChange() includes object addition and removal.)
    // m_modelObject->model().getImpl<openstudio::model::detail::Model_Impl>().get()->onChange.connect<OSComboBox2, &OSComboBox2::onChoicesRefreshTrigger>(this);

    // populate choices
    // ETH@20140228 - With extension of this class to choices of ModelObjects, and beyond,
    // do we need to figure out some way to signal when the choices have changed? Or maybe
    // controllers will be able to sense that and trigger an unbind(), (re-)bind?
    m_values = m_choiceConcept->choices();
    this->blockSignals(true);

    for (const auto& value : m_values) {
      QString qvalue = QString::fromStdString(value);
      addItem(qvalue);
      setItemData(count() - 1, qvalue, Qt::ToolTipRole);
    }

    // initialize
    onModelObjectChanged();
  } else if (m_dataSource) {

    // connections
    connect(m_dataSource.get(), &OSComboBoxDataSource::itemChanged, this, &OSComboBox2::onDataSourceChange);
    connect(m_dataSource.get(), &OSComboBoxDataSource::itemAdded, this, &OSComboBox2::onDataSourceAdd);
    connect(m_dataSource.get(), &OSComboBoxDataSource::itemRemoved, this, &OSComboBox2::onDataSourceRemove);

    this->blockSignals(true);

    // populate choices
    for (int i = 0; i < m_dataSource->numberOfItems(); i++) {
      QString qvalue = m_dataSource->valueAt(i);
      addItem(qvalue);
      setItemData(count() - 1, qvalue, Qt::ToolTipRole);
    }

    // initialize
    setCurrentIndex(-1);
  } else {
    OS_ASSERT(false);
  }

  this->blockSignals(false);
  setLocked(false);
  updateStyle();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// OSComboBox::OSComboBox( QWidget * parent )
//   : QComboBox(parent)
// {
//   this->setAcceptDrops(false);
//   setEnabled(false);
// }

// bool OSComboBox::event( QEvent * e )
// {
//   if( e->type() == QEvent::Wheel )
//   {
//     return false;
//   }
//   else
//   {
//     return QComboBox::event(e);
//   }
// }

// void OSComboBox::bind(model::ModelObject & modelObject, const char * property)
// {
//   m_modelObject = modelObject;

//   m_property = property;

//   m_dataSource.reset();

//   clear();

//   // Connections

//   m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get()->onChange.connect<OSComboBox, &OSComboBox::onModelObjectChanged>(this);

//   m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get()->onRemoveFromWorkspace.connect<OSComboBox, &OSComboBox::onModelObjectRemoved>(this);

//   connect(this, static_cast<void (OSComboBox::*)(const QString &)>(&OSComboBox::currentIndexChanged), this, &OSComboBox::onCurrentIndexChanged);

//   // Populate choices

//   std::string valuesPropertyName = m_property;
//   valuesPropertyName.append("Values");

//   QVariant variant = m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->property(valuesPropertyName.c_str());

//   OS_ASSERT( variant.canConvert<std::vector<std::string> >() );

//   m_values = variant.value<std::vector<std::string> >();

//   this->blockSignals(true);

//   for( const auto & value : m_values )
//   {
//     addItem(QString::fromStdString(value));
//   }

//   // Initialize

//   onModelObjectChanged();

//   this->blockSignals(false);

//   setEnabled(true);
// }

// void OSComboBox::onModelObjectChanged()
// {
//   OS_ASSERT(m_modelObject);

//   QVariant variant = m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->property(m_property.c_str());

//   OS_ASSERT( variant.canConvert<std::string>() );

//   std::string value = variant.value<std::string>();

//   int i = 0;
//   for( const auto & v : m_values )
//   {
//     if( istringEqual(v,value) )
//     {
//       this->blockSignals(true);
//       setCurrentIndex(i);
//       this->blockSignals(false);
//       break;
//     }

//     i++;
//   }
// }

// void OSComboBox::onModelObjectRemoved(const Handle& handle)
// {
//   unbind();
// }

// void OSComboBox::onCurrentIndexChanged(const QString & text)
// {
//   OS_ASSERT(m_modelObject);

//   QVariant variant = m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->property(m_property.c_str());
//   QVariant textString;
//   if (variant.canConvert<QString>()) {
//     textString = QVariant::fromValue(text);
//   } else if (variant.canConvert<std::string>()) {
//     textString = QVariant::fromValue(toString(text));
//   }
//   m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->setProperty(m_property.c_str(), textString);

//   // test if property changed
//   variant = m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>()->property(m_property.c_str());
//   std::string value;
//   if (variant.canConvert<QString>()) {
//     value = variant.toString().toStdString();
//   } else if (variant.canConvert<std::string>()) {
//     value = variant.value<std::string>();
//   }

//   if (!istringEqual(value, toString(text))){
//     // failed, reset combo box
//     onModelObjectChanged();
//   }
// }

// void OSComboBox::unbind()
// {
//   if (m_modelObject){
//     disconnect( m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get() );
//   }
//   m_modelObject = boost::none;

//   m_property = "";

//   m_dataSource.reset();

//   this->blockSignals(true);

//   clear();

//   this->blockSignals(false);

//   setEnabled(false);
// }

// void OSComboBox::onDataSourceChange(int i)
// {
//   this->setItemText(i,m_dataSource->valueAt(i));
// }

// void OSComboBox::onDataSourceAdd(int i)
// {
//   int oldIndex = this->currentIndex();

//   this->insertItem(i,m_dataSource->valueAt(i));

//   if( oldIndex == -1 )
//   {
//     setCurrentIndex(oldIndex);
//   }
// }

// void OSComboBox::onDataSourceRemove(int i)
// {
//   if( currentIndex() == i )
//   {
//     this->setCurrentIndex(-1);
//   }

//   this->removeItem(i);
// }

// void OSComboBox::setDataSource(std::shared_ptr<OSComboBoxDataSource> dataSource)
// {
//   unbind();

//   if( m_dataSource )
//   {
//     disconnect(m_dataSource.get(), &OSComboBoxDataSource::itemChanged, this, &OSComboBox::onDataSourceChange);
//     disconnect(m_dataSource.get(), &OSComboBoxDataSource::itemAdded, this, &OSComboBox::onDataSourceAdd);
//     disconnect(m_dataSource.get(), &OSComboBoxDataSource::itemRemoved, this, &OSComboBox::onDataSourceRemove);
//   }

//   m_dataSource = dataSource;

//   connect(m_dataSource.get(), &OSComboBoxDataSource::itemChanged, this, &OSComboBox::onDataSourceChange);
//   connect(m_dataSource.get(), &OSComboBoxDataSource::itemAdded, this, &OSComboBox::onDataSourceAdd);
//   connect(m_dataSource.get(), &OSComboBoxDataSource::itemRemoved, this, &OSComboBox::onDataSourceRemove);
//   this->clear();

//   for( int i = 0;
//        i < m_dataSource->numberOfItems();
//        i++ )
//   {
//     this->addItem(m_dataSource->valueAt(i));
//   }

//   setCurrentIndex(-1);

//   setEnabled(true);
// }

}  // namespace openstudio
