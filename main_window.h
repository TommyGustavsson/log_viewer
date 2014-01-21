#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QHostAddress>
#include <QModelIndex>
#include <QSettings>
#include <color_theme.h>

class QListWidgetItem;
class QFrame;
class QUdpSocket;

namespace Ui {
    class Main_window;
}

class Main_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Main_window(QWidget *parent = 0);
    ~Main_window();

private slots:
    void on_ftpFiles_listView_activated(QModelIndex index);
    void on_openFTP_pushButton_clicked();
    void on_actionPaste_from_Clipboard_triggered();
    void on_actionEnter_filter_triggered();
    void on_apply_highlight_pushButton_clicked();
    void on_actionToggle_full_text_view_triggered();
    void on_actionAbout_QT_triggered();
    void on_action_disable_type_highlight_triggered(bool checked);
    void on_neighbor_listView_activated(QModelIndex index);
    void on_server_port_lineEdit_textChanged(QString text);
    void on_tail_cleared();
    void on_tail_pushButton_clicked(bool checked);
    void on_log_tableView_customContextMenuRequested(QPoint pos);
    void on_highlight_regex_checkBox_clicked(bool checked);
    void on_filter_regex_checkBox_clicked(bool checked);
    void on_clear_all_highlight_pushButton_clicked();
    void on_open_server_pushButton_clicked(bool checked);
    void on_actionClear_Log_View_triggered();
    void on_actionOpen_File_triggered();
    void on_actionExit_triggered();
    void on_filter_lineEdit_textChanged(QString text);
    void on_clear_filter_pushButton_clicked();
    void on_filter_pushButton_clicked();
    void on_actionAutoscroll_triggered(bool checked);
    void on_client_connected(const QString client_address);
    void on_client_disconnected(const QString client_address);
    void on_client_log_found();
    void on_log_row_inserted ( const QModelIndex& parent, int start, int end );
    void on_header_resized(int logicalIndex, int oldSize, int newSize);
    void on_filter_type_changed(bool checked);
    void on_files_dropped(const QStringList& files);
    void on_file_opened(const QString file);
    void on_log_manager_error(const QString text);
    void on_log_items_empty();
    void on_log_items_not_empty();
    void on_log_format_selected(const QString& format);
    void on_proxy_filter_cleared();
    void on_enabel_scroll_lock(bool enable);
    void on_autoscroll_checkBox_stateChanged(int state);
    void on_selected_row(const QString& row,
                         const QString& text,
                         const QString& type,
                         const QString& file,
                         const QString& line,
                         const QString& origin);
    void on_color_frame_context_menu(QContextMenuEvent* event);
    void on_color_theme_change_A();
    void on_color_theme_change_B();
    void on_color_theme_change_C();
    void on_color_theme_change_D();
    void on_color_theme_change_E();
    void on_ftp_file_download_progress(qint64 readBytes, qint64 totalBytes);
    void on_ftp_file_downloaded(const QString &file_name);
    void on_ftp_message(const QString &message);
    void on_ftp_download_canceled();    

    void on_actionSave_to_file_triggered();

private:
    void closeEvent(QCloseEvent *);
    void read_settings();
    void write_settings();
    void connect_log_manager();
    void connect_gui();
    void promt_open_file();
    void open_neighbor_file(const QModelIndex& index);
    void toggle_view();
    bool is_full_text_view_visible();
    void set_color_theme(QFrame* frame, int index);
    void switch_color_theme(int index);
    void add_focus_filter_shortcut();

    Ui::Main_window *ui;

    // UI state
    bool get_autoscroll();
    void set_autoscroll(bool value);

    QSettings m_settings;
    QString m_unformated_text;
    QList<Color_theme> m_themes;
    int m_theme_index;
};



#endif // MAIN_WINDOW_H
