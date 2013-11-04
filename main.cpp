#include <QtGui/QApplication>
#include <main_window.h>

#include <QFile>
#include <QWindowsStyle>

void load_stylesheet()
{
    QFile file(":/stylesheet/Resources/skin.css");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());

    qApp->setStyleSheet(styleSheet);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setStyle(new QWindowsStyle);
    load_stylesheet();

    Main_window w;
    w.show();

    return a.exec();
}
