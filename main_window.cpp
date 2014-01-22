#include <main_window.h>

#include <QFileDialog>

#include <ui_main_window.h>
#include <log_manager.h>
#include <log_filter_proxy_model.h>
#include <log_items_model.h>
#include <clients_model.h>
#include <file_neighbor_model.h>
#include <ftp_files_model.h>
#include <ftp_files_proxy_model.h>
#include <QFileInfo>
#include <QListWidgetItem>
#include <QMenu>
#include <QSharedPointer>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QSettings>
#include <QContextMenuEvent>
#include <QKeySequence>
#include <QtConcurrentRun>

using namespace Log_viewer;



// do not translate ---------------------------------------------------
const QString CGEOMETRY = "mainwindow/geometry";
const QString CWINDOW_STATE = "mainwindow/windowState";
const QString CFULL_TEXT_SPLITTER = "mainwindow/fullTextSplitterSize";
const QString CPREVIEW_VISIBLE = "mainwindow/previewVisible";
const QString CPORT = "server/port";
const QString CLAST_FILE = "files/lastNeighbor";
const QString CCOLOR_THEME = "mainwindow/colorTheme";
const QString CFTP_USER = "ftp/userName";
const QString CFTP_PASSWORD = "ftp/password";
const QString CFTP_HOST = "ftp/host";
const QString CFTP_PORT = "ftp/port";
const QString CPROTOCOL = "server/protocol";
// do not translate ---------------------------------------------------

const QColor THEME_A[5] = {QColor(44,73,105),
                           QColor(51,63,69),
                           QColor(255,135,46),
                           QColor(255,69,59),
                           QColor(168,20,81)};

const QColor THEME_B[5] = {QColor(71,16,69),
                           QColor(189,32,86),
                           QColor(255,112,0),
                           QColor(255,211,0),
                           QColor(100,140,2)};

const QColor THEME_C[5] = {QColor(76,3,60),
                           QColor(158,3,0),
                           QColor(255,216,0),
                           QColor(67,176,0),
                           QColor(84,11,255)};

const QColor THEME_D[5] = {QColor(232,219,71),
                           QColor(74,255,90),
                           QColor(127,60,255),
                           QColor(232,89,151),
                           QColor(65,255,216)};

const QColor THEME_E[5] = {QColor(176,232,131),
                           QColor(151,226,255),
                           QColor(255,206,143),
                           QColor(254,128,255),
                           QColor(0,150,255)};


Main_window::Main_window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Main_window),
    m_settings("LogViewer", "LogViewer"), // do not translate
    m_unformated_text(""),
    m_theme_index(0)
{
    ui->setupUi(this);

    qRegisterMetaType<Log_item_ptr>("Log_item_ptr");
    qRegisterMetaType<Log_file_parser::Log_items>("Log_file_parser::Log_items");

    connect_log_manager();
    connect_gui();

    m_themes.append(THEME_A);
    m_themes.append(THEME_B);
    m_themes.append(THEME_C);
    m_themes.append(THEME_D);
    m_themes.append(THEME_E);

    switch_color_theme(4);

    ui->ftpDownload_progressBar->setVisible(false);
    ui->menuView->addAction(ui->tools_dockWidget->toggleViewAction());
    ui->log_tableView->setModel(Log_manager::instance->log_filter_proxy_model());
    ui->log_tableView->setAutoScrolling(true);
    ui->actionAutoscroll->setChecked(true);
    ui->actionAutoscroll->setEnabled(false);
    ui->autoscroll_checkBox->setEnabled(false);
    ui->autoscroll_checkBox->setChecked(Qt::Checked);
    ui->actionClear_Log_View->setEnabled(false);
    ui->clients_listView->setModel(Log_manager::instance->clients_model());
    ui->neighbor_listView->setModel(Log_manager::instance->file_neighbor_model());
    ui->ftpFiles_listView->setModel(Log_manager::instance->ftp_proxy_model());
    ui->tailFTP_pushButton->setEnabled(false);
    ui->ftpFiles_listView->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->index_lineEdit->setToolTip("Ex. 100-150, will show line 100 to 150");
    add_focus_filter_shortcut();

    read_settings();

}

// ----------------------------------------------------------------------------

Main_window::~Main_window()
{
    delete ui;
}

// ----------------------------------------------------------------------------

void Main_window::add_focus_filter_shortcut()
{
    this->addAction(ui->actionEnter_filter);
    QKeySequence focus_filter_key_sequence(Qt::CTRL + Qt::Key_E);
    ui->actionEnter_filter->setShortcut(focus_filter_key_sequence);
    QString key_text = focus_filter_key_sequence.toString(QKeySequence::NativeText);
    ui->filter_lineEdit->setToolTip(QString("Enter text to filter - (%1)").arg(key_text));
}

// ----------------------------------------------------------------------------

void Main_window::switch_color_theme(int index)
{
    m_theme_index = index;
    set_color_theme(ui->h1_frame, 0);
    set_color_theme(ui->h2_frame, 1);
    set_color_theme(ui->h3_frame, 2);
    set_color_theme(ui->h4_frame, 3);
    set_color_theme(ui->h5_frame, 4);

    ui->actionColor_theme_A->setChecked(false);
    ui->actionColor_theme_B->setChecked(false);
    ui->actionColor_theme_C->setChecked(false);
    ui->actionColor_theme_D->setChecked(false);
    ui->actionColor_theme_E->setChecked(false);

    switch(index)
    {
    case 0: ui->actionColor_theme_A->setChecked(true); break;
    case 1: ui->actionColor_theme_B->setChecked(true); break;
    case 2: ui->actionColor_theme_C->setChecked(true); break;
    case 3: ui->actionColor_theme_D->setChecked(true); break;
    case 4: ui->actionColor_theme_E->setChecked(true); break;
    }

    on_apply_highlight_pushButton_clicked();
}

// ----------------------------------------------------------------------------

void Main_window::set_color_theme(QFrame* frame, int index)
{
    QString stylesheet = QString(
                "QFrame#%0 { background-color: %1; border: 2px solid rgb(120,120,140); }")
            .arg(frame->objectName())
            .arg(m_themes.at(m_theme_index).c[index].name());
            //.arg(m_themes.at(m_theme_index).c[index].darker(200).name());

    frame->setStyleSheet(stylesheet);
}

// ----------------------------------------------------------------------------

void Main_window::connect_log_manager()
{
    connect(Log_manager::instance,
            SIGNAL(client_connected(const QString)),
            this,
            SLOT(on_client_connected(const QString)));
    connect(Log_manager::instance,
            SIGNAL(client_disconnected(const QString)),
            this,
            SLOT(on_client_disconnected(const QString)));
    connect(Log_manager::instance->log_items_model(),
            SIGNAL(rowsInserted(QModelIndex, int, int)),
            this,
            SLOT(on_log_row_inserted(QModelIndex, int, int)));
    connect(Log_manager::instance,
            SIGNAL(file_opened(QString)),
            this,
            SLOT(on_file_opened(QString)));
    connect(Log_manager::instance,
            SIGNAL(error(QString)),
            this,
            SLOT(on_log_manager_error(QString)));
    connect(Log_manager::instance,
            SIGNAL(tail_cleared()),
            this,
            SLOT(on_tail_cleared()));
    connect(Log_manager::instance,
            SIGNAL(ftp_tail_cleared()),
            this,
            SLOT(on_ftp_tail_cleared()));
    connect(Log_manager::instance,
            SIGNAL(log_items_empty()),
            this,
            SLOT(on_log_items_empty()));
    connect(Log_manager::instance,
            SIGNAL(log_items_not_empty()),
            this,
            SLOT(on_log_items_not_empty()));
    connect(Log_manager::instance,
            SIGNAL(log_format_selected(QString)),
            this,
            SLOT(on_log_format_selected(QString)));
    connect(Log_manager::instance,
            SIGNAL(log_proxy_filter_cleared()),
            this,
            SLOT(on_proxy_filter_cleared()));
    connect(Log_manager::instance,
            SIGNAL(file_transfer_progress(double, double)),
            this,
            SLOT(on_file_transfer_progress(double, double)));
    connect(Log_manager::instance,
            SIGNAL(ftp_file_downloaded(QString)),
            this,
            SLOT(on_ftp_file_downloaded(QString)));
    connect(Log_manager::instance->ftp_model(),
            SIGNAL(ftpStatusMessage(QString)),
            this,
            SLOT(on_ftp_message(QString)));
    connect(Log_manager::instance->ftp_model(),
            SIGNAL(ftpErrorMessage(QString)),
            this,
            SLOT(on_ftp_message(QString)));
    connect(Log_manager::instance->ftp_model(),
            SIGNAL(downloadCanceled()),
            this,
            SLOT(on_ftp_download_canceled()));
    connect(Log_manager::instance,
            SIGNAL(local_file_opened()),
            this,
            SLOT(on_local_file_opened()));
    connect(Log_manager::instance->ftp_model(),
            SIGNAL(ftpFileListUpdated()),
            this,
            SLOT(on_ftp_file_list_updated()));
    connect(Log_manager::instance,
            SIGNAL(downloading_file()),
            this,
            SLOT(on_downloading_file()));
}

// ----------------------------------------------------------------------------

void Main_window::connect_gui()
{
    connect(ui->actionAutoscroll,
            SIGNAL(triggered(bool)),
            this,
            SLOT(on_actionAutoscroll_triggered(bool)));
    connect(ui->log_tableView->horizontalHeader(),
            SIGNAL(sectionResized(int, int, int)),
            this,
            SLOT(on_header_resized(int, int, int)));
    connect(ui->filter_lineEdit,
            SIGNAL(returnPressed()),
            this,
            SLOT(on_filter_pushButton_clicked()));
    connect(ui->file_lineEdit,
            SIGNAL(returnPressed()),
            this,
            SLOT(on_filter_pushButton_clicked()));
    connect(ui->origin_lineEdit,
            SIGNAL(returnPressed()),
            this,
            SLOT(on_filter_pushButton_clicked()));
    connect(ui->index_lineEdit,
            SIGNAL(returnPressed()),
            this,
            SLOT(on_filter_pushButton_clicked()));
    connect(ui->module_lineEdit,
            SIGNAL(returnPressed()),
            this,
            SLOT(on_filter_pushButton_clicked()));
    connect(ui->info_toolButton,
            SIGNAL(toggled(bool)),
            this,
            SLOT(on_filter_type_changed(bool)));
    connect(ui->warn_toolButton,
            SIGNAL(toggled(bool)),
            this,
            SLOT(on_filter_type_changed(bool)));
    connect(ui->error_toolButton,
            SIGNAL(toggled(bool)),
            this,
            SLOT(on_filter_type_changed(bool)));
    connect(ui->fatal_toolButton,
            SIGNAL(toggled(bool)),
            this,
            SLOT(on_filter_type_changed(bool)));
    connect(ui->log_tableView,
            SIGNAL(files_dropped(QStringList)),
            this,
            SLOT(on_files_dropped(QStringList)));
    connect(ui->log_tableView,
            SIGNAL(current_changed(QModelIndex,QModelIndex)),
            this,
            SLOT(on_log_tableView_currentChanged(QModelIndex,QModelIndex)));
    connect(ui->log_tableView,
            SIGNAL(enable_scroll_lock(bool)),
            this,
            SLOT(on_enabel_scroll_lock(bool)));
    connect(ui->log_tableView,
            SIGNAL(selected_row(QString, QString, QString, QString, QString, QString)),
            this,
            SLOT(on_selected_row(QString, QString, QString, QString, QString, QString)));
    connect(ui->FTP_radioButton,
            SIGNAL(toggled(bool)),
            this,
            SLOT(on_FTP_radioButton_toggled(bool)));
    connect(ui->SFTP_radioButton,
            SIGNAL(toggled(bool)),
            this,
            SLOT(on_SFTP_radioButton_toggled(bool)));
    connect(ui->h1_frame,
            SIGNAL(context_menu_event(QContextMenuEvent*)),
            this,
            SLOT(on_color_frame_context_menu(QContextMenuEvent*))
            );
    connect(ui->h2_frame,
            SIGNAL(context_menu_event(QContextMenuEvent*)),
            this,
            SLOT(on_color_frame_context_menu(QContextMenuEvent*))
            );
    connect(ui->h3_frame,
            SIGNAL(context_menu_event(QContextMenuEvent*)),
            this,
            SLOT(on_color_frame_context_menu(QContextMenuEvent*))
            );
    connect(ui->h4_frame,
            SIGNAL(context_menu_event(QContextMenuEvent*)),
            this,
            SLOT(on_color_frame_context_menu(QContextMenuEvent*))
            );
    connect(ui->h5_frame,
            SIGNAL(context_menu_event(QContextMenuEvent*)),
            this,
            SLOT(on_color_frame_context_menu(QContextMenuEvent*))
            );
    connect(ui->actionColor_theme_A,
            SIGNAL(triggered()),
            this,
            SLOT(on_color_theme_change_A()));
    connect(ui->actionColor_theme_B,
            SIGNAL(triggered()),
            this,
            SLOT(on_color_theme_change_B()));
    connect(ui->actionColor_theme_C,
            SIGNAL(triggered()),
            this,
            SLOT(on_color_theme_change_C()));
    connect(ui->actionColor_theme_D,
            SIGNAL(triggered()),
            this,
            SLOT(on_color_theme_change_D()));
    connect(ui->actionColor_theme_E,
            SIGNAL(triggered()),
            this,
            SLOT(on_color_theme_change_E()));
    connect(ui->ftpFiles_listView,
            SIGNAL(customContextMenuRequested(const QPoint)),
            this,
            SLOT(on_ftpFiles_listView_context_menu(const QPoint)));
}

// ----------------------------------------------------------------------------

void Main_window::on_ftp_download_canceled()
{
    ui->ftpDownload_progressBar->setVisible(false);
    ui->tailFTP_pushButton->setEnabled(false);
}

// ----------------------------------------------------------------------------

void Main_window::on_client_connected(const QString client_address)
{
    Q_UNUSED(client_address);
    ui->tailFTP_pushButton->setEnabled(false);
}

// ----------------------------------------------------------------------------

void Main_window::on_ftp_message(const QString &message)
{
    ui->statusBar->showMessage(message, 5000);
}

// ----------------------------------------------------------------------------

void Main_window::on_file_transfer_progress(double readBytes, double totalBytes)
{
    ui->ftpDownload_progressBar->setVisible(true);
    if((readBytes == 0) && (totalBytes == 0)) {
        ui->ftpDownload_progressBar->setMaximum(0);
        ui->ftpDownload_progressBar->setMinimum(0);
    }
    else {
        ui->ftpDownload_progressBar->setMaximum(100);
        ui->ftpDownload_progressBar->setMinimum(0);
        double part = (readBytes / totalBytes);
        int value = part * 100;
        ui->ftpDownload_progressBar->setValue(value);
        qDebug(QString("Transfer value: %1, %2, %3").arg(value).arg(readBytes).arg(totalBytes).toStdString().c_str());
    }
}

// ----------------------------------------------------------------------------

void Main_window::on_ftp_file_downloaded(const QString &file_name)
{
    ui->ftpDownload_progressBar->setVisible(false);
    ui->tailFTP_pushButton->setEnabled(true);
}

// ----------------------------------------------------------------------------

void Main_window::on_color_frame_context_menu(QContextMenuEvent *event)
{
    QMenu context_menu(this);
    context_menu.addAction(ui->actionColor_theme_A);
    context_menu.addAction(ui->actionColor_theme_B);
    context_menu.addAction(ui->actionColor_theme_C);
    context_menu.addAction(ui->actionColor_theme_D);
    context_menu.addAction(ui->actionColor_theme_E);
    context_menu.exec(event->globalPos());
}

// ----------------------------------------------------------------------------

void Main_window::on_color_theme_change_A()
{
    switch_color_theme(0);
}

// ----------------------------------------------------------------------------

void Main_window::on_color_theme_change_B()
{
    switch_color_theme(1);
}

// ----------------------------------------------------------------------------

void Main_window::on_color_theme_change_C()
{
    switch_color_theme(2);
}

// ----------------------------------------------------------------------------

void Main_window::on_color_theme_change_D()
{
    switch_color_theme(3);
}

// ----------------------------------------------------------------------------

void Main_window::on_color_theme_change_E()
{
    switch_color_theme(4);
}

// ----------------------------------------------------------------------------

void Main_window::on_client_disconnected(const QString client_address)
{
    Q_UNUSED(client_address);
}

// ----------------------------------------------------------------------------

void Main_window::on_client_log_found()
{

}

// ----------------------------------------------------------------------------

void Main_window::on_actionAutoscroll_triggered(bool checked)
{
    set_autoscroll(checked);
}

// ----------------------------------------------------------------------------

void Main_window::on_log_row_inserted ( const QModelIndex& parent, int start, int end )
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
}

// ----------------------------------------------------------------------------

void Main_window::on_header_resized(int logicalIndex, int oldSize, int newSize)
{
    Q_UNUSED(logicalIndex);
    Q_UNUSED(oldSize);
    Q_UNUSED(newSize);
}

// ----------------------------------------------------------------------------

void Main_window::on_filter_pushButton_clicked()
{
    Log_manager::instance->apply_filter(
            ui->filter_lineEdit->text(),
            ui->file_lineEdit->text(),
            ui->module_lineEdit->text(),
            ui->index_lineEdit->text(),
            ui->origin_lineEdit->text());
}

// ----------------------------------------------------------------------------

void Main_window::on_clear_filter_pushButton_clicked()
{
    ui->filter_lineEdit->setText("");
    ui->file_lineEdit->setText("");
    ui->module_lineEdit->setText("");
    ui->index_lineEdit->setText("");
    ui->origin_lineEdit->setText("");
    Log_manager::instance->clear_filter();
}

// ----------------------------------------------------------------------------

void Main_window::on_filter_lineEdit_textChanged(QString text)
{
    //ui->filter_pushButton->setEnabled(!text.isEmpty());
}

// ----------------------------------------------------------------------------

void Main_window::on_filter_type_changed(bool checked)
{
    Q_UNUSED(checked);

    Log_manager::instance->log_filter_proxy_model()->clear_type_filter();
    if(ui->info_toolButton->isChecked())
    {
        Log_manager::instance->log_filter_proxy_model()->add_type_filter(lt_info);
    }
    if(ui->warn_toolButton->isChecked())
    {
        Log_manager::instance->log_filter_proxy_model()->add_type_filter(lt_warning);
    }
    if(ui->error_toolButton->isChecked())
    {
        Log_manager::instance->log_filter_proxy_model()->add_type_filter(lt_error);
    }
    if(ui->fatal_toolButton->isChecked())
    {
        Log_manager::instance->log_filter_proxy_model()->add_type_filter(lt_fatal);
    }
    Log_manager::instance->log_filter_proxy_model()->apply();
}

// ----------------------------------------------------------------------------

void Main_window::on_files_dropped(const QStringList& files)
{
    set_autoscroll(false);
    Log_manager::instance->open_log_files(files);
}

// ----------------------------------------------------------------------------

void Main_window::on_actionOpen_File_triggered()
{
    promt_open_file();
}

// ----------------------------------------------------------------------------

void Main_window::on_actionExit_triggered()
{
    this->close();
}

// ----------------------------------------------------------------------------

void Main_window::promt_open_file()
{
    set_autoscroll(false);
    QStringList files = QFileDialog::getOpenFileNames(
                             this,
                             "Select one or more log files to open",
                             "",
                             "");
    if(files.size() > 0)
        Log_manager::instance->open_log_files(files);
}

// ----------------------------------------------------------------------------

bool Main_window::get_autoscroll()
{
    return ui->actionAutoscroll->isChecked();
}

// ----------------------------------------------------------------------------

void Main_window::set_autoscroll(bool value)
{
    ui->actionAutoscroll->setChecked(value);
    ui->autoscroll_checkBox->setChecked(value);
    ui->log_tableView->setAutoScrolling(value);
}

// ----------------------------------------------------------------------------

void Main_window::on_actionClear_Log_View_triggered()
{
    Log_manager::instance->clear_log_items();
}

// ----------------------------------------------------------------------------

void Main_window::on_file_opened(const QString file)
{
    //QFileInfo file_info(file);
    //ui->opended_file_lineEdit->setText(file_info.fileName());
    ui->tail_pushButton->setEnabled(true);
}

// ----------------------------------------------------------------------------

void Main_window::on_log_manager_error(const QString text)
{
    ui->statusBar->showMessage(text, 5000);
}

// ----------------------------------------------------------------------------

void Main_window::on_open_server_pushButton_clicked(bool checked)
{
    if(checked)
    {
        bool is_int;
        int port = ui->server_port_lineEdit->text().toInt(&is_int);
        if(is_int)
        {
            Log_manager::instance->listen(port, ui->udpRadioButton->isChecked() ? stUDP : stTCP);
        }
        else
        {
            ui->open_server_pushButton->setChecked(false);
        }        
    }
    else
    {
        Log_manager::instance->close();
    }
}

// ----------------------------------------------------------------------------

void Main_window::on_clear_all_highlight_pushButton_clicked()
{
    Log_manager::instance->log_items_model()->begin_highlight();

    Log_manager::instance->log_items_model()->clear_all_highlights();

    ui->h1_lineEdit->setText("");
    ui->h1_lineEdit->clearFocus();
    ui->h2_lineEdit->setText("");
    ui->h2_lineEdit->clearFocus();
    ui->h3_lineEdit->setText("");
    ui->h3_lineEdit->clearFocus();
    ui->h4_lineEdit->setText("");
    ui->h4_lineEdit->clearFocus();
    ui->h5_lineEdit->setText("");
    ui->h5_lineEdit->clearFocus();

    Log_manager::instance->log_items_model()->end_highlight();

}


// ----------------------------------------------------------------------------

void Main_window::on_filter_regex_checkBox_clicked(bool checked)
{
    if(!ui->filter_lineEdit->text().isEmpty())
    {
        Log_manager::instance->log_filter_proxy_model()->set_use_regex(checked);
        Log_manager::instance->apply_filter(ui->filter_lineEdit->text(),
                                            ui->file_lineEdit->text(),
                                            ui->module_lineEdit->text(),
                                            ui->index_lineEdit->text(),
                                            ui->origin_lineEdit->text());
    }
}

// ----------------------------------------------------------------------------

void Main_window::on_highlight_regex_checkBox_clicked(bool checked)
{
    Log_manager::instance->log_items_model()->begin_highlight();
    Log_manager::instance->log_items_model()->set_use_regex_for_highlight(checked);
    Log_manager::instance->log_items_model()->end_highlight();
}


// ----------------------------------------------------------------------------

void Main_window::open_neighbor_file(const QModelIndex &index)
{
    QFileInfo file_info = Log_manager::instance->file_neighbor_model()->get_item(index);
    QStringList files;
    files.push_back(file_info.filePath());
    Log_manager::instance->open_log_files(files);
}

// ----------------------------------------------------------------------------

void Main_window::on_log_tableView_customContextMenuRequested(QPoint pos)
{
    QMenu context_menu(this);
    context_menu.addAction(ui->actionClear_Log_View);
    context_menu.addAction(ui->actionAutoscroll);
    context_menu.addAction(ui->action_disable_type_highlight);
    context_menu.addSeparator();
    context_menu.addAction(ui->actionOpen_File);
    context_menu.exec(ui->log_tableView->mapToGlobal(pos));
}

// ----------------------------------------------------------------------------

void Main_window::on_tail_pushButton_clicked(bool checked)
{
    if(checked)
    {
        Log_manager::instance->tail_current_file();
        Log_manager::instance->clear_ftp_tail();
    }
    else
    {
        Log_manager::instance->clear_tail();
    }
    set_autoscroll(checked);
}

// ----------------------------------------------------------------------------

void Main_window::on_tail_cleared()
{
    ui->tail_pushButton->setChecked(false);
}

// ----------------------------------------------------------------------------

void Main_window::on_ftp_tail_cleared()
{
    ui->tailFTP_pushButton->setChecked(false);
}

// ----------------------------------------------------------------------------

void Main_window::on_log_items_empty()
{
    ui->actionAutoscroll->setEnabled(false);
    ui->actionClear_Log_View->setEnabled(false);
    ui->autoscroll_checkBox->setEnabled(false);
    ui->full_text_textEdit->clear_item();
}

// ----------------------------------------------------------------------------

void Main_window::on_log_items_not_empty()
{
    ui->actionAutoscroll->setEnabled(true);
    ui->actionClear_Log_View->setEnabled(true);
    ui->autoscroll_checkBox->setEnabled(true);
    ui->log_tableView->selectRow(0);
}

// ----------------------------------------------------------------------------

void Main_window::on_server_port_lineEdit_textChanged(QString text)
{
    bool is_int;
    text.toInt(&is_int);
    ui->open_server_pushButton->setEnabled(is_int);
}

// ----------------------------------------------------------------------------

void Main_window::on_log_format_selected(const QString& format)
{
    Q_UNUSED(format);
    ui->log_tableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

// ----------------------------------------------------------------------------

void Main_window::on_neighbor_listView_activated(QModelIndex index)
{
    open_neighbor_file(index);
}

// ----------------------------------------------------------------------------

void Main_window::on_action_disable_type_highlight_triggered(bool checked)
{
    Log_manager::instance->log_items_model()->set_type_highlight(!checked);
}

// ----------------------------------------------------------------------------

void Main_window::on_actionAbout_QT_triggered()
{
    QMessageBox::aboutQt(this);
}

// ----------------------------------------------------------------------------

void Main_window::toggle_view()
{
    /*
    QPropertyAnimation* animation = new QPropertyAnimation(ui->full_text_groupBox, "maximumHeight", this);
    animation->setDuration(100);

    if(ui->full_text_groupBox->maximumHeight() == 0)
    {
        animation->setStartValue(0);
        animation->setEndValue(150);
        ui->toggle_view_toolButton->setArrowType(Qt::DownArrow);
    }
    else
    {
        animation->setStartValue(150);
        animation->setEndValue(0);
        ui->toggle_view_toolButton->setArrowType(Qt::UpArrow);
    }
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    */
}

// ----------------------------------------------------------------------------

void Main_window::on_actionToggle_full_text_view_triggered()
{
    toggle_view();
}

// ----------------------------------------------------------------------------

bool Main_window::is_full_text_view_visible()
{
    return (ui->full_text_textEdit->maximumHeight() > 0);
}

// ----------------------------------------------------------------------------

void Main_window::on_apply_highlight_pushButton_clicked()
{
    Log_manager::instance->log_items_model()->begin_highlight();

    Log_manager::instance->log_items_model()->clear_all_highlights();

    Log_manager::instance->log_items_model()->add_highlight(m_themes.at(m_theme_index).c[0], ui->h1_lineEdit->text());
    Log_manager::instance->log_items_model()->add_highlight(m_themes.at(m_theme_index).c[1], ui->h2_lineEdit->text());
    Log_manager::instance->log_items_model()->add_highlight(m_themes.at(m_theme_index).c[2], ui->h3_lineEdit->text());
    Log_manager::instance->log_items_model()->add_highlight(m_themes.at(m_theme_index).c[3], ui->h4_lineEdit->text());
    Log_manager::instance->log_items_model()->add_highlight(m_themes.at(m_theme_index).c[4], ui->h5_lineEdit->text());

    Log_manager::instance->log_items_model()->end_highlight();
}

// ----------------------------------------------------------------------------

void Main_window::on_proxy_filter_cleared()
{
    ui->info_toolButton->setChecked(false);
    ui->warn_toolButton->setChecked(false);
    ui->error_toolButton->setChecked(false);
    ui->fatal_toolButton->setChecked(false);

    /*ui->filter_lineEdit->setText("");
    ui->index_lineEdit->setText("");
    ui->file_lineEdit->setText("");
    ui->module_lineEdit->setText("");*/
}

// ----------------------------------------------------------------------------

void Main_window::on_enabel_scroll_lock(bool enable)
{
    set_autoscroll(enable);
}
// ----------------------------------------------------------------------------

void Main_window::on_autoscroll_checkBox_stateChanged(int state)
{
    if(state == Qt::Unchecked)
    {
        set_autoscroll(false);
    } else if (state == Qt::Checked) {
        set_autoscroll(true);
    }
}

// ----------------------------------------------------------------------------

void Main_window::on_selected_row(const QString& row,
                                  const QString& text,
                                  const QString& type,
                                  const QString& file,
                                  const QString& line,
                                  const QString& origin)
{
    ui->full_text_textEdit->show_item(row,
                                      text,
                                      type,
                                      file,
                                      line,
                                      origin,
                                      Log_manager::instance->log_items_model()->get_highlights(),
                                      Log_manager::instance->log_filter_proxy_model()->get_text_filter());
    //format_text(str);
}

// ----------------------------------------------------------------------------

void Main_window::closeEvent(QCloseEvent *event)
{
    write_settings();

    QMainWindow::closeEvent(event);
}

// ----------------------------------------------------------------------------

void Main_window::write_settings()
{
    m_settings.setValue(CGEOMETRY, saveGeometry());
    m_settings.setValue(CWINDOW_STATE, saveState());
    m_settings.setValue(CFULL_TEXT_SPLITTER, ui->full_text_splitter->saveState());
    m_settings.setValue(CPORT, ui->server_port_lineEdit->text());
    m_settings.setValue(CLAST_FILE, Log_manager::instance->get_current_file());
    m_settings.setValue(CCOLOR_THEME, m_theme_index);
    m_settings.setValue(CFTP_HOST, ui->ftpHost_lineEdit->text());
    m_settings.setValue(CFTP_PASSWORD, ui->ftpPassword_lineEdit->text());
    m_settings.setValue(CFTP_PORT, ui->ftpPort_lineEdit->text().toInt());
    m_settings.setValue(CFTP_USER, ui->ftpUser_lineEdit->text());
    m_settings.setValue(CPROTOCOL, m_protocol);
}

// ----------------------------------------------------------------------------

void Main_window::read_settings()
{
    restoreGeometry(m_settings.value(CGEOMETRY).toByteArray());
    restoreState(m_settings.value(CWINDOW_STATE).toByteArray());
    ui->full_text_splitter->restoreState(m_settings.value(CFULL_TEXT_SPLITTER).toByteArray());
    ui->server_port_lineEdit->setText(m_settings.value(CPORT).toString());
    Log_manager::instance->file_neighbor_model()->populate(m_settings.value(CLAST_FILE).toString());
    switch_color_theme(m_settings.value(CCOLOR_THEME).toInt());
    ui->ftpHost_lineEdit->setText(m_settings.value(CFTP_HOST).toString());
    ui->ftpPassword_lineEdit->setText(m_settings.value(CFTP_PASSWORD).toString());
    ui->ftpPort_lineEdit->setText(m_settings.value(CFTP_PORT).toString());
    ui->ftpUser_lineEdit->setText(m_settings.value(CFTP_USER).toString());
    if (m_settings.value(CPROTOCOL, m_protocol).toInt() == 0)
    {
        ui->FTP_radioButton->setChecked(true);
        m_protocol = FTP;
    }
    else
    {
        ui->SFTP_radioButton->setChecked(true);
        m_protocol = SFTP;
    }
}

// ----------------------------------------------------------------------------

void Main_window::on_actionEnter_filter_triggered()
{
    ui->filter_lineEdit->setFocus();
}

// ----------------------------------------------------------------------------

void Main_window::on_actionPaste_from_Clipboard_triggered()
{
    Log_manager::instance->open_log_from_clipboard();
    Log_manager::instance->clear_ftp_tail();
}

// ----------------------------------------------------------------------------

void Main_window::on_openFTP_pushButton_clicked()
{
    Log_manager::instance->open_ftp(
            ui->ftpHost_lineEdit->text(),
            ui->ftpPort_lineEdit->text().toInt(),
            ui->ftpUser_lineEdit->text(),
            ui->ftpPassword_lineEdit->text(),
            m_protocol);
}

// ----------------------------------------------------------------------------

void Main_window::on_tailFTP_pushButton_clicked(bool checked)
{
    if(checked)
    {
        Log_manager::instance->tail_ftp(
                ui->ftpHost_lineEdit->text(),
                ui->ftpPort_lineEdit->text().toInt(),
                ui->ftpUser_lineEdit->text(),
                ui->ftpPassword_lineEdit->text(),
                m_protocol);
    }
    else
    {
        Log_manager::instance->clear_ftp_tail();
    }
    set_autoscroll(checked);

}

// ----------------------------------------------------------------------------

void Main_window::on_ftpFiles_listView_activated(QModelIndex index)
{
    QModelIndex source_index = Log_manager::instance->ftp_proxy_model()->mapToSource(index);
    Log_manager::instance->ftp_model()->openFileFromModelIndex(source_index);
}

// ----------------------------------------------------------------------------

void Main_window::on_local_file_opened()
{
    ui->tailFTP_pushButton->setEnabled(false);
}

// ----------------------------------------------------------------------------

void Main_window::on_ftp_file_list_updated()
{
    qDebug("---ftp_list_updated");
    Log_manager::instance->ftp_proxy_model()->sort(0);
}

// ----------------------------------------------------------------------------

void Main_window::on_FTP_radioButton_toggled(bool toggled)
{
    if (toggled)
        m_protocol = FTP;
}

// ----------------------------------------------------------------------------

void Main_window::on_SFTP_radioButton_toggled(bool toggled)
{
    if (toggled)
        m_protocol = SFTP;
}



void Main_window::on_ftpFiles_listView_context_menu(const QPoint position)
{
    /*QMenu context_menu(this);
    context_menu.addAction(ui->actionSet_log_rotation_file);
    context_menu.addSeparator();
    context_menu.addAction(ui->actionDelete_file);
    context_menu.exec(ui->ftpFiles_listView->mapToGlobal(position));*/
}

// ----------------------------------------------------------------------------

void Main_window::on_actionDelete_file_triggered()
{
    qDebug("Delete triggered");
    QModelIndex source_index = ui->ftpFiles_listView->currentIndex();
    source_index = Log_manager::instance->ftp_proxy_model()->mapToSource(source_index);
    QtConcurrent::run(Log_manager::instance->ftp_model(), &ftp_files_model::deleteFileFromModelIndex, source_index);
}

// ----------------------------------------------------------------------------

void Main_window::on_actionSet_log_rotation_file_triggered()
{

}

// ----------------------------------------------------------------------------

void Main_window::on_downloading_file()
{
    ui->tailFTP_pushButton->setChecked(false);
    ui->tailFTP_pushButton->setEnabled(false);
}

void Main_window::on_actionSave_to_file_triggered()
{
    QString file_name = QFileDialog::getSaveFileName(this,
         tr("Save file as"), "/home", tr("CSV file (*.txt)"));

    if (file_name.isNull())
    {
        return;
    }

    Log_manager::instance->log_items_model()->save_to_file(file_name);
}
