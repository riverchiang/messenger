#ifndef MESSENGER_H
#define MESSENGER_H

#include "messengertab.h"
#include "clickablelabel.h"

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
#include <QTimer>
#include <QSignalMapper>
#include <QFileInfo>

#include <QDebug>

class QTcpSocket;
class QNetworkSession;

namespace Ui {
class Messenger;
}

struct friendInfo
{
    QString name;
    int uid;
    bool hasClientIcon;
    ClickableLabel *friendLabel;
};

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
    void pollingServer();
    void handleCallFriend(int param);

private:
    Ui::Messenger *ui;
    QGridLayout *infoLayout;

    enum MessengerCmd{None = 0, Register, Login, FriendList, TalkSend, TalkRecv, PicSend, PicMeta, PicRecv};

    // Message information
    QTextEdit *inputArea;
    QPushButton *sendMsgBtn;
    MessengerTab *friendTabs = nullptr;
    QVector<QVBoxLayout *> friendList;

    // Login information
    QLabel *userIcon;
    QLabel *comboboxLabel;
    QLabel *nameLabel;
    QLabel *passwdLabel;
    QLineEdit *nameLine;
    QLineEdit *passwdLine;
    QPushButton *enterBtn;
    QPushButton *clearBtn;
    QComboBox *registerLoginComboBox;
    QLabel *statusLabel;
    QScrollArea *friendInfoList;
    QVBoxLayout *friendListVBLayout;

    // Function
    void loginPage();
    void messagePage();
    void clearLoginPage();
    int callFriend(QString name);
    void authUser(bool isAuth);
    void sendNetworkCmd(quint64 cmdID, QString message);
    void sendNetworkfile(QString filePath);
    void sendGetFriendList();
    void sendGetMessage();
    void sendGetIconMeta();
    void sendGetIconByUid(int uid);
    bool checkfriendVectorExist(QString name);
    void addFriendList();
    void putMsgOnTab(int tabId, QString text, bool isFriend);
    QString findNameByUid(int uid);
    bool fileExists(QString path);

    QMenu *fileMenu;
    QAction *openAct;

    QTcpSocket *tcpSocket = nullptr;
    QDataStream in;

    quint64 cmdID = None;
    quint64 blockSize = 0;
    int clientUid = 0;

    QString picFolder = "C:/Users/A60013/Pictures/temp/client/";
    //QString picFolder = "L:/Users/admin/Pictures/temp/client/";
    QVector<struct friendInfo> friendVector;
    QVector<struct friendInfo> friendVectorNew;
    QTimer *pollingTimer;
    QSignalMapper *signalMapper;
    QLabel *clientIcon;
    QVector<int> picUidVector;

    quint64 friendUid = 0;

    // Recv network function
    void recvCmdRegister(QString recvData);
    void recvCmdLogin(QString recvData);
    void recvCmdFriendList(QString recvData);
    void recvCmdTalkRecv(QString recvData);
    void recvCmdPicMeta(QString recvData);
    void recvCmdPicRecv();
};

#endif // MESSENGER_H
