/****************************************************************************
** Meta object code from reading C++ file 'ModelObjectInspectorView.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ModelObjectInspectorView.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ModelObjectInspectorView.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10openstudio24ModelObjectInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::ModelObjectInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio24ModelObjectInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::ModelObjectInspectorView",
        "toggleUnitsClicked",
        "",
        "displayIP",
        "toggleDisplayAdditionalPropsClicked",
        "displayAdditionalProps",
        "itemSelected",
        "OSItem*",
        "item",
        "selectionCleared",
        "itemsRequested",
        "dropZoneItemClicked",
        "onChange",
        "selectModelObject",
        "openstudio::model::ModelObject",
        "modelObject",
        "toggleUnits",
        "toggleDisplayAdditionalProps"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'toggleUnitsClicked'
        QtMocHelpers::SignalData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
        // Signal 'toggleDisplayAdditionalPropsClicked'
        QtMocHelpers::SignalData<void(bool)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 5 },
        }}),
        // Signal 'itemSelected'
        QtMocHelpers::SignalData<void(OSItem *)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Signal 'selectionCleared'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'itemsRequested'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'dropZoneItemClicked'
        QtMocHelpers::SignalData<void(OSItem *)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Signal 'onChange'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'selectModelObject'
        QtMocHelpers::SlotData<void(const openstudio::model::ModelObject &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 15 },
        }}),
        // Slot 'toggleUnits'
        QtMocHelpers::SlotData<void(bool)>(16, 2, QMC::AccessProtected, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
        // Slot 'toggleDisplayAdditionalProps'
        QtMocHelpers::SlotData<void(bool)>(17, 2, QMC::AccessProtected, QMetaType::Void, {{
            { QMetaType::Bool, 5 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ModelObjectInspectorView, qt_meta_tag_ZN10openstudio24ModelObjectInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::ModelObjectInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<OSInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio24ModelObjectInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio24ModelObjectInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio24ModelObjectInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::ModelObjectInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ModelObjectInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->toggleUnitsClicked((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->toggleDisplayAdditionalPropsClicked((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->itemSelected((*reinterpret_cast<std::add_pointer_t<OSItem*>>(_a[1]))); break;
        case 3: _t->selectionCleared(); break;
        case 4: _t->itemsRequested(); break;
        case 5: _t->dropZoneItemClicked((*reinterpret_cast<std::add_pointer_t<OSItem*>>(_a[1]))); break;
        case 6: _t->onChange(); break;
        case 7: _t->selectModelObject((*reinterpret_cast<std::add_pointer_t<openstudio::model::ModelObject>>(_a[1]))); break;
        case 8: _t->toggleUnits((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->toggleDisplayAdditionalProps((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ModelObjectInspectorView::*)(bool )>(_a, &ModelObjectInspectorView::toggleUnitsClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModelObjectInspectorView::*)(bool )>(_a, &ModelObjectInspectorView::toggleDisplayAdditionalPropsClicked, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModelObjectInspectorView::*)(OSItem * )>(_a, &ModelObjectInspectorView::itemSelected, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModelObjectInspectorView::*)()>(_a, &ModelObjectInspectorView::selectionCleared, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModelObjectInspectorView::*)()>(_a, &ModelObjectInspectorView::itemsRequested, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModelObjectInspectorView::*)(OSItem * )>(_a, &ModelObjectInspectorView::dropZoneItemClicked, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModelObjectInspectorView::*)()>(_a, &ModelObjectInspectorView::onChange, 6))
            return;
    }
}

const QMetaObject *openstudio::ModelObjectInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::ModelObjectInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio24ModelObjectInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return OSInspectorView::qt_metacast(_clname);
}

int openstudio::ModelObjectInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = OSInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void openstudio::ModelObjectInspectorView::toggleUnitsClicked(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void openstudio::ModelObjectInspectorView::toggleDisplayAdditionalPropsClicked(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void openstudio::ModelObjectInspectorView::itemSelected(OSItem * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void openstudio::ModelObjectInspectorView::selectionCleared()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void openstudio::ModelObjectInspectorView::itemsRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void openstudio::ModelObjectInspectorView::dropZoneItemClicked(OSItem * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void openstudio::ModelObjectInspectorView::onChange()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
namespace {
struct qt_meta_tag_ZN10openstudio20DefaultInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::DefaultInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio20DefaultInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::DefaultInspectorView"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DefaultInspectorView, qt_meta_tag_ZN10openstudio20DefaultInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::DefaultInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<ModelObjectInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio20DefaultInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio20DefaultInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio20DefaultInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::DefaultInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DefaultInspectorView *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *openstudio::DefaultInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::DefaultInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio20DefaultInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return ModelObjectInspectorView::qt_metacast(_clname);
}

int openstudio::DefaultInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ModelObjectInspectorView::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
