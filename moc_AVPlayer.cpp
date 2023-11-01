/****************************************************************************
** Meta object code from reading C++ file 'AVPlayer.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "AVPlayer.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AVPlayer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.2. It"
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

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSAVPlayerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSAVPlayerENDCLASS = QtMocHelpers::stringData(
    "AVPlayer",
    "onVideoFrameDecoded",
    "",
    "cv::Mat",
    "frame",
    "int64_t",
    "pts",
    "onAudioFrameDecoded",
    "uint8_t*",
    "data",
    "nb_samples"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSAVPlayerENDCLASS_t {
    uint offsetsAndSizes[22];
    char stringdata0[9];
    char stringdata1[20];
    char stringdata2[1];
    char stringdata3[8];
    char stringdata4[6];
    char stringdata5[8];
    char stringdata6[4];
    char stringdata7[20];
    char stringdata8[9];
    char stringdata9[5];
    char stringdata10[11];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSAVPlayerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSAVPlayerENDCLASS_t qt_meta_stringdata_CLASSAVPlayerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 8),  // "AVPlayer"
        QT_MOC_LITERAL(9, 19),  // "onVideoFrameDecoded"
        QT_MOC_LITERAL(29, 0),  // ""
        QT_MOC_LITERAL(30, 7),  // "cv::Mat"
        QT_MOC_LITERAL(38, 5),  // "frame"
        QT_MOC_LITERAL(44, 7),  // "int64_t"
        QT_MOC_LITERAL(52, 3),  // "pts"
        QT_MOC_LITERAL(56, 19),  // "onAudioFrameDecoded"
        QT_MOC_LITERAL(76, 8),  // "uint8_t*"
        QT_MOC_LITERAL(85, 4),  // "data"
        QT_MOC_LITERAL(90, 10)   // "nb_samples"
    },
    "AVPlayer",
    "onVideoFrameDecoded",
    "",
    "cv::Mat",
    "frame",
    "int64_t",
    "pts",
    "onAudioFrameDecoded",
    "uint8_t*",
    "data",
    "nb_samples"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSAVPlayerENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   26,    2, 0x0a,    1 /* Public */,
       7,    3,   31,    2, 0x0a,    4 /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 8, QMetaType::Int, 0x80000000 | 5,    9,   10,    6,

       0        // eod
};

Q_CONSTINIT const QMetaObject AVPlayer::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSAVPlayerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSAVPlayerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSAVPlayerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<AVPlayer, std::true_type>,
        // method 'onVideoFrameDecoded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cv::Mat, std::false_type>,
        QtPrivate::TypeAndForceComplete<int64_t, std::false_type>,
        // method 'onAudioFrameDecoded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint8_t *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int64_t, std::false_type>
    >,
    nullptr
} };

void AVPlayer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AVPlayer *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onVideoFrameDecoded((*reinterpret_cast< std::add_pointer_t<cv::Mat>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int64_t>>(_a[2]))); break;
        case 1: _t->onAudioFrameDecoded((*reinterpret_cast< std::add_pointer_t<uint8_t*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int64_t>>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject *AVPlayer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AVPlayer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSAVPlayerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AVPlayer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
