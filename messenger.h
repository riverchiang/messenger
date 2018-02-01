#ifndef MESSENGER_H
#define MESSENGER_H

#include "messengertab.h"


#include <QMainWindow>
#include <QTextEdit>
#include <QDebug>
#include <QPushButton>
#include <QMessageBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QVector>
#include <QComboBox>

#include <QDataStream>
#include <QTcpSocket>

#include <QDebug>

class QTcpSocket;
class QNetworkSession;

namespace Ui {
class Messenger;
}

class Messenger : public QMainWindow
{
    Q_OBJECT

public:
    explicit Messenger(QWidget *parent = 0);
    ~Messenger();

private slots:
    void clickSendMsg();
    void clickLogin();
    void clickClear();

    void newFile();
    void readNetwork();

private:
    Ui::Messenger *ui;
    QGridLayout *infoLayout;

    // Message information
    QTextEdit *inputArea;

    //QVBoxLayout *tabelLayout;
    //QScrollArea *messageArea;
    //QFrame *scrollFrame;
    QPushButton *sendMsgBtn;

    MessengerTab *firendTabs;
    QWidget *messageArea;
    QVector<QVBoxLayout *> friendList;

    // Login information
    QLabel *comboboxLabel;
    QLabel *nameLabel;
    QLabel *passwdLabel;
    QLineEdit *nameLine;
    QLineEdit *passwdLine;
    QPushButton *enterBtn;
    QPushButton *clearBtn;
    QComboBox *registerLoginComboBox;
    QLabel *statusLabel;

    // Function
    void loginPage();
    void messagePage();
    void clearLoginPage();
    void callFriend(QString name);
    void authUser(bool isAuth);
    void sendNetworkCmd(quint64 cmdID, QString message);
    void sendNetworkfile(QString filePath);

    int dialog_num = 0;

    QMenu *fileMenu;
    QAction *openAct;

    QTcpSocket *tcpSocket = nullptr;
    QDataStream in;

    quint64 cmdID = 0;
    quint64 blockSize = 0;
    int clientUid = 0;
};

#endif // MESSENGER_H
