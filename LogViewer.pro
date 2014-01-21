#-------------------------------------------------
#
# Project created by QtCreator 2010-12-18T14:15:18
#
#-------------------------------------------------

QT       += core gui\
         network

TARGET = LogViewer
TEMPLATE = app

QMAKE_CXXFLAGS +=   -DCURL_STATICLIB \
                    #-DQT_NO_DEBUG_OUTPUT

QMAKE_LFLAGS += -static-libgcc

INCLUDEPATH += curl-7.25.0-devel-mingw32/include/
LIBS        += -L"$$PWD/Redist" -lcurl-4 -lwldap32 -lws2_32

SOURCES += main.cpp\
        main_window.cpp \
    start_stop_buffer.cpp \
    log_manager.cpp \
    log_item.cpp \
    log_client.cpp \
    g3_log_item.cpp \
    nrg_log_item.cpp \
    log_table_view.cpp \
    nrg_legacy_log_item.cpp \
    app_log_item.cpp \
    log_items_model.cpp \
    log_format.cpp \
    Log_format_factory.cpp \
    log_filter_proxy_model.cpp \
    log_file_parser.cpp \
    clients_model.cpp \
    tail.cpp \
    file_neighbor_model.cpp \
    log_column.cpp \
    plain_text_log_item.cpp \
    log_item_preview_textedit.cpp \
    color_theme.cpp \
    colored_frame.cpp \
    ftp_files_model.cpp \
    ftp_tail_libcurl.cpp \
    ftp_files_proxy_model.cpp

HEADERS  += main_window.h \
    start_stop_buffer.h \
    log_manager.h \
    log_item.h \
    log_client.h \
    singleton.h \
    g3_log_item.h \
    nrg_log_item.h \
    log_table_view.h \
    nrg_legacy_log_item.h \
    app_log_item.h \
    unknown_log_format.h \
    nrg_legacy_log_format.h \
    nrg_log_format.h \
    log_items_model.h \
    log_format.h \
    Log_format_factory.h \
    log_filter_proxy_model.h \
    log_file_parser.h \
    g3_log_format.h \
    clients_model.h \
    tail.h \
    file_neighbor_model.h \
    log_column.h \
    plain_text_log_item.h \
    log_item_preview_textedit.h \
    color_theme.h \
    colored_frame.h \
    ftp_files_model.h \
    ftp_tail_libcurl.h \
    ftp_files_proxy_model.h \
    protocol.h

FORMS    += main_window.ui

OTHER_FILES += \
    spec.txt \
    doc_main.h \
    Resources/arrows_north_south.png \
    Resources/uparrow.png \
    Resources/rightarrow.png \
    Resources/leftarrow.png \
    Resources/downarrow.png \
    Resources/skin.css \
    Resources/app.rc \
    Resources/app.ico

RESOURCES += \
    resources.qrc

win32:RC_FILE = Resources/app.rc

