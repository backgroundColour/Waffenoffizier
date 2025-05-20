#include "mainwindow.h"
#include "ipdialog.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    IpDialog ipDialog;
    if (ipDialog.exec() == QDialog::Accepted) {
        QString ipAddress = ipDialog.getIpAddress();

        MainWindow w(ipAddress);
        w.showFullScreen();
        return a.exec();
    }else {
        return 0;
    }
}
