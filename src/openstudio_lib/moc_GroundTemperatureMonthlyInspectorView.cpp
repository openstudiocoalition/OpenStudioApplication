/****************************************************************************
** Meta object code from reading C++ file 'GroundTemperatureMonthlyInspectorView.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "GroundTemperatureMonthlyInspectorView.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GroundTemperatureMonthlyInspectorView.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10openstudio34SiteGroundTemperatureMonthlyWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::SiteGroundTemperatureMonthlyWidget::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio34SiteGroundTemperatureMonthlyWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::SiteGroundTemperatureMonthlyWidget",
        "toggleUnitsClicked",
        "",
        "displayIP"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'toggleUnitsClicked'
        QtMocHelpers::SignalData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SiteGroundTemperatureMonthlyWidget, qt_meta_tag_ZN10openstudio34SiteGroundTemperatureMonthlyWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::SiteGroundTemperatureMonthlyWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio34SiteGroundTemperatureMonthlyWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio34SiteGroundTemperatureMonthlyWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio34SiteGroundTemperatureMonthlyWidgetE_t>.metaTypes,
    nullptr
} };

void openstudio::SiteGroundTemperatureMonthlyWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SiteGroundTemperatureMonthlyWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->toggleUnitsClicked((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SiteGroundTemperatureMonthlyWidget::*)(bool )>(_a, &SiteGroundTemperatureMonthlyWidget::toggleUnitsClicked, 0))
            return;
    }
}

const QMetaObject *openstudio::SiteGroundTemperatureMonthlyWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::SiteGroundTemperatureMonthlyWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio34SiteGroundTemperatureMonthlyWidgetE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Nano::Observer"))
        return static_cast< Nano::Observer*>(this);
    return QWidget::qt_metacast(_clname);
}

int openstudio::SiteGroundTemperatureMonthlyWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void openstudio::SiteGroundTemperatureMonthlyWidget::toggleUnitsClicked(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN10openstudio42SiteGroundTemperatureBuildingSurfaceWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::SiteGroundTemperatureBuildingSurfaceWidget::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio42SiteGroundTemperatureBuildingSurfaceWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::SiteGroundTemperatureBuildingSurfaceWidget"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SiteGroundTemperatureBuildingSurfaceWidget, qt_meta_tag_ZN10openstudio42SiteGroundTemperatureBuildingSurfaceWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::SiteGroundTemperatureBuildingSurfaceWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<SiteGroundTemperatureMonthlyWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio42SiteGroundTemperatureBuildingSurfaceWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio42SiteGroundTemperatureBuildingSurfaceWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio42SiteGroundTemperatureBuildingSurfaceWidgetE_t>.metaTypes,
    nullptr
} };

void openstudio::SiteGroundTemperatureBuildingSurfaceWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SiteGroundTemperatureBuildingSurfaceWidget *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *openstudio::SiteGroundTemperatureBuildingSurfaceWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::SiteGroundTemperatureBuildingSurfaceWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio42SiteGroundTemperatureBuildingSurfaceWidgetE_t>.strings))
        return static_cast<void*>(this);
    return SiteGroundTemperatureMonthlyWidget::qt_metacast(_clname);
}

int openstudio::SiteGroundTemperatureBuildingSurfaceWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SiteGroundTemperatureMonthlyWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN10openstudio34SiteGroundTemperatureShallowWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::SiteGroundTemperatureShallowWidget::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio34SiteGroundTemperatureShallowWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::SiteGroundTemperatureShallowWidget"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SiteGroundTemperatureShallowWidget, qt_meta_tag_ZN10openstudio34SiteGroundTemperatureShallowWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::SiteGroundTemperatureShallowWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<SiteGroundTemperatureMonthlyWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio34SiteGroundTemperatureShallowWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio34SiteGroundTemperatureShallowWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio34SiteGroundTemperatureShallowWidgetE_t>.metaTypes,
    nullptr
} };

void openstudio::SiteGroundTemperatureShallowWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SiteGroundTemperatureShallowWidget *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *openstudio::SiteGroundTemperatureShallowWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::SiteGroundTemperatureShallowWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio34SiteGroundTemperatureShallowWidgetE_t>.strings))
        return static_cast<void*>(this);
    return SiteGroundTemperatureMonthlyWidget::qt_metacast(_clname);
}

int openstudio::SiteGroundTemperatureShallowWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SiteGroundTemperatureMonthlyWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN10openstudio31SiteGroundTemperatureDeepWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::SiteGroundTemperatureDeepWidget::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio31SiteGroundTemperatureDeepWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::SiteGroundTemperatureDeepWidget"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SiteGroundTemperatureDeepWidget, qt_meta_tag_ZN10openstudio31SiteGroundTemperatureDeepWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::SiteGroundTemperatureDeepWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<SiteGroundTemperatureMonthlyWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio31SiteGroundTemperatureDeepWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio31SiteGroundTemperatureDeepWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio31SiteGroundTemperatureDeepWidgetE_t>.metaTypes,
    nullptr
} };

void openstudio::SiteGroundTemperatureDeepWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SiteGroundTemperatureDeepWidget *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *openstudio::SiteGroundTemperatureDeepWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::SiteGroundTemperatureDeepWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio31SiteGroundTemperatureDeepWidgetE_t>.strings))
        return static_cast<void*>(this);
    return SiteGroundTemperatureMonthlyWidget::qt_metacast(_clname);
}

int openstudio::SiteGroundTemperatureDeepWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SiteGroundTemperatureMonthlyWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN10openstudio41SiteGroundTemperatureFCfactorMethodWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::SiteGroundTemperatureFCfactorMethodWidget::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio41SiteGroundTemperatureFCfactorMethodWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::SiteGroundTemperatureFCfactorMethodWidget"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SiteGroundTemperatureFCfactorMethodWidget, qt_meta_tag_ZN10openstudio41SiteGroundTemperatureFCfactorMethodWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::SiteGroundTemperatureFCfactorMethodWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<SiteGroundTemperatureMonthlyWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio41SiteGroundTemperatureFCfactorMethodWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio41SiteGroundTemperatureFCfactorMethodWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio41SiteGroundTemperatureFCfactorMethodWidgetE_t>.metaTypes,
    nullptr
} };

void openstudio::SiteGroundTemperatureFCfactorMethodWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SiteGroundTemperatureFCfactorMethodWidget *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *openstudio::SiteGroundTemperatureFCfactorMethodWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::SiteGroundTemperatureFCfactorMethodWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio41SiteGroundTemperatureFCfactorMethodWidgetE_t>.strings))
        return static_cast<void*>(this);
    return SiteGroundTemperatureMonthlyWidget::qt_metacast(_clname);
}

int openstudio::SiteGroundTemperatureFCfactorMethodWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SiteGroundTemperatureMonthlyWidget::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
