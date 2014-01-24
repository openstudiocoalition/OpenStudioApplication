/**********************************************************************
 *  Copyright (c) 2008-2013, Alliance for Sustainable Energy.  
 *  All rights reserved.
 *  
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *  
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *  
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **********************************************************************/

#include "OSGridView.hpp"
#include <shared_gui_components/OSCollapsibleView.hpp>
#include <shared_gui_components/HeaderViews.hpp>

#include <shared_gui_components/FieldMethodTypedefs.hpp>
#include <shared_gui_components/OSCheckBox.hpp>
#include <shared_gui_components/OSComboBox.hpp>
#include <shared_gui_components/OSDoubleEdit.hpp>
#include <shared_gui_components/OSIntegerEdit.hpp>
#include <shared_gui_components/OSLineEdit.hpp>
#include <shared_gui_components/OSQuantityEdit.hpp>
#include <shared_gui_components/OSUnsignedEdit.hpp>

#include <model/Model_impl.hpp>
#include <model/ModelObject_impl.hpp>
#include <model/RefrigerationCase.hpp>
#include <model/RefrigerationCase_impl.hpp>

#include <utilities/core/Assert.hpp>
#include <utilities/idd/IddObject.hpp>

#include <QAbstractButton>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>

namespace openstudio {

OSGridView::OSGridView(IddObjectType iddObjectType, const model::Model & model, QWidget * parent)
  : QWidget(parent),
  m_model(model),
  m_gridController(0)
{
  QVBoxLayout * layout = 0;
  
  layout = new QVBoxLayout();
  layout->setSpacing(0);
  layout->setContentsMargins(0,0,0,0);
  setLayout(layout);

  OSCollapsibleView * collabsibleView = new OSCollapsibleView(this);
  layout->addWidget(collabsibleView);

  DarkGradientHeader * header = new DarkGradientHeader(); 
  header->label->setText("Display Cases");
  collabsibleView->setHeader(header);

  QWidget * widget = new QWidget;
  collabsibleView->setContent(widget);

  collabsibleView->setExpanded(true);
  
  QVBoxLayout * m_contentLayout = 0;
  m_contentLayout = new QVBoxLayout();
  m_contentLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_contentLayout->setSpacing(0);
  m_contentLayout->setContentsMargins(0,0,0,0);
  widget->setLayout(m_contentLayout);

  std::vector<QString> fields;
  fields.push_back("a");
  fields.push_back("b");
  fields.push_back("c");
  this->setCategoryFields("1",fields);
  this->setCategoryFields("2",fields);

  QButtonGroup * buttonGroup = new QButtonGroup();
  bool isConnected = false;
  isConnected = connect(buttonGroup, SIGNAL(buttonClicked(int)),
    this, SLOT(selectCategory(int)));
  OS_ASSERT(isConnected);
  
  QHBoxLayout * buttonLayout = new QHBoxLayout();
  buttonLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_contentLayout->addLayout(buttonLayout);

  std::vector<QString> categories = this->categories();
  QPushButton * button = 0;
  for(unsigned i=0; i<categories.size(); i++){
    button = new QPushButton(categories.at(i));
    buttonLayout->addWidget(button,0,Qt::AlignLeft);
    buttonGroup->addButton(button,buttonGroup->buttons().size());
  }
  buttonLayout->addStretch();

  m_gridLayout = new QGridLayout();
  m_gridLayout->setSizeConstraint(QLayout::SetMinimumSize);
  m_gridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_contentLayout->addLayout(m_gridLayout);

  setContentsMargins(5,5,5,5);

  setGridController(new OSGridController(iddObjectType, model));

  refreshAll();
}

OSGridView::OSGridView(std::vector<model::ModelObject> modelObjects, QWidget * parent)
  : QWidget(parent)
{
  m_gridLayout = new QGridLayout();
  m_gridLayout->setSizeConstraint(QLayout::SetMinimumSize);
  m_gridLayout->setAlignment(Qt::AlignTop);
  setLayout(m_gridLayout);

  setContentsMargins(5,5,5,5);
}

void OSGridView::setGridController(OSGridController * gridController)
{
  if( m_gridController )
  {
    m_gridController->disconnect(this);
  }

  m_gridController = gridController;

  bool isConnected = false;

  isConnected = connect(m_gridController,SIGNAL(itemInserted(int,int)),this,SLOT(addWidget(int,int)));
  OS_ASSERT(isConnected);

  isConnected = connect(m_gridController,SIGNAL(itemRemoved(int,int)),this,SLOT(removeWidget(int,int)));
  OS_ASSERT(isConnected);

  isConnected = connect(m_gridController,SIGNAL(itemChanged(int,int)),this,SLOT(refresh(int,int)));
  OS_ASSERT(isConnected);

  isConnected = connect(m_gridController,SIGNAL(modelReset()),this,SLOT(refreshAll()));
  OS_ASSERT(isConnected);
    
  refreshAll();
}

OSGridController * OSGridView::gridController() const
{
  return m_gridController;
}

void OSGridView::refresh(int row, int column)
{
  removeWidget(row,column);

  addWidget(row,column);
}

void OSGridView::setCategories(std::vector<QString> categories)
{
  m_categoriesAndFields.clear();

  Q_FOREACH(QString category, categories) {
     m_categoriesAndFields[category] = std::vector<QString>();
  }
}

std::vector<QString> OSGridView::categories()
{
  std::vector<QString> categories;

  for( std::map<QString,std::vector<QString>>::iterator iter = m_categoriesAndFields.begin(); iter != m_categoriesAndFields.end(); iter++ ) {
    categories.push_back( iter->first );
  }

  return categories;
}

void OSGridView::setCategoryFields(const QString & category, std::vector<QString> fields)
{
  m_categoriesAndFields[category] = fields;
}

std::vector<QString> OSGridView::categoryFields(const QString & category)
{
  return m_categoriesAndFields[category];
}

void OSGridView::removeWidget(int row, int column)
{
  QLayoutItem * item = m_gridLayout->itemAtPosition(row,column);

  OS_ASSERT(item);

  QWidget * widget = item->widget();

  OS_ASSERT(widget);

  delete widget;

  delete item;
}

void OSGridView::refreshAll()
{
  QLayoutItem * child;
  while((child = m_gridLayout->takeAt(0)) != 0)
  {
      QWidget * widget = child->widget();

      OS_ASSERT(widget);

      delete widget;

      delete child;
  }

  if( m_gridController )
  {
    for( int i = 0; i < m_gridController->rowCount(); i++ )
    {
      for( int j = 0; j < m_gridController->columnCount(); j++ )
      {
        addWidget(i,j);
      }
    }
  }
}

void OSGridView::addWidget(int row, int column)
{
  OS_ASSERT(m_gridController);

  QWidget * widget = m_gridController->widgetAt(row,column);

  OS_ASSERT(m_gridLayout);
  
  m_gridLayout->addWidget(widget,row,column);
}

void OSGridView::setVerticalHeader(bool visible, QString title)
{
  OS_ASSERT(m_gridLayout);

  QLabel * label = new QLabel(title);
  label->setVisible(visible);
  m_gridLayout->addWidget(label,0,0);
}

void OSGridView::setHorizontalHeader(std::vector<QWidget *> widgets)
{
  OS_ASSERT(m_gridLayout);

  int column = 0;
  Q_FOREACH(QWidget * widget, widgets){
    m_gridLayout->addWidget(widget,0,column++);
  }
}

void OSGridView::setHorizontalHeader(std::vector<QString> names)
{
  OS_ASSERT(m_gridLayout);

  QLabel * label = 0;
  int column = 0;
  Q_FOREACH(QString name, names){
    label = new QLabel(name);
    m_gridLayout->addWidget(new QLabel(name),0,column++);
  }
}

void OSGridView::selectCategory(int category)
{
}

} // openstudio

