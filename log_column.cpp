#include "log_column.h"

#include <log_item.h>

namespace Log_viewer
{

    Log_column::Log_column(Column_type a_type) : type(a_type)
    {
        switch(type)
        {
            case column_index :
                caption = "Index";
                size_hint = QSize(70, 20);
                break;
            case column_type :
                caption = "Type";
                size_hint = QSize(80, 20);
                break;
            case column_text :
                caption = "Text";
                size_hint = QSize(450, 20);
                break;
            case column_date :
                caption = "Date";
                size_hint = QSize(180, 20);
                break;
            case column_time :
                caption = "Time";
                size_hint = QSize(120, 20);
                break;
            case column_file :
                caption = "File";
                size_hint = QSize(120, 20);
                break;
            case column_method :
                caption = "Method";
                size_hint = QSize(220, 20);
                break;
            case column_line :
                caption = "Line";
                size_hint = QSize(50, 20);
                break;
            case column_application :
                caption = "Application";
                size_hint = QSize(200, 20);
                break;
            case column_module :
                caption = "Module";
                size_hint = QSize(100, 20);
                break;
            case column_thread :
                caption = "Thread";
                size_hint = QSize(80, 20);
                break;
            case column_origin :
                caption = "Origin";
                size_hint = QSize(100, 20);
                break;
            default : throw;
        }
    }    

    QString Log_column::get_value_for_log_item(const Log_item& log_item) const
    {
        switch(type)
        {
            case column_type : return log_item.get_log_type_as_string();
            case column_text : return log_item.get_text();
            case column_date : return log_item.get_date();
            case column_time : return log_item.get_time();
            case column_file : return log_item.get_file();
            case column_method : return log_item.get_method();
            case column_line : return log_item.get_line();
            case column_application : return log_item.get_application();
            case column_module : return log_item.get_module();
            case column_thread : return log_item.get_thread();
            case column_origin : return log_item.get_origin();
            default : throw;
        }
    }
}
