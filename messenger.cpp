#include "messenger.h"
#include "ui_messenger.h"

Messenger::Messenger(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Messenger)
{
    ui->setupUi(this);
    setMinimumSize(800,800);
    setMaximumSize(800,800);
    infoLayout = new QGridLayout;

    setCentralWidget(new QWidget);
    centralWidget()->setLayout(infoLayout);


    openAct = new QAction(tr("&New"), this);
    openAct->setStatusTip(tr("Create a new file"));
    connect(openAct, &QAction::triggered, this, &Messenger::newFile);
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);


    loginPage();



    /*
    QWidget *centralWidget = new QWidget;
    MessageTab *tabs = new MessageTab(centralWidget());

    tabs->setFixedSize(245, 245);
    tabs->addTab(new QWidget(),"TAB 1");
    tabs->addTab(new QWidget(),"TAB 2");

    tabs->setTabsClosable(true);
    setCentralWidget(centralWidget);
    */

    /*
    QPushButton *button1 = new QPushButton("button1");
    mylineedit = new QTextEdit(this);
    mylineedit->setFocus();


    scrollArea = new QScrollArea;

    my = new QFrame;
    my->setStyleSheet("background-color:white;");
    tabelLayout = new QVBoxLayout(my);

    scrollArea->setWidget(my);
    scrollArea->setWidgetResizable(true);   // important


    infoLayout->addWidget(mylineedit, 1, 0);
    infoLayout->addWidget(button1, 1, 1);
    infoLayout->addWidget(scrollArea, 0, 0);

    QObject::connect(button1, SIGNAL(clicked()),this, SLOT(clickedSlot()));
    */
}


void MessengerTab::tabChange(int index)
{
    qDebug() << index;
}

void Messenger::newFile()
{
    QString s = QFileDialog::getOpenFileName(this,tr("開啟檔案"),"/","Head files(*.jpg)");
    test->setText(s);
}

void Messenger::loginPage()
{
    nameLabel = new QLabel("Enter name: ");
    passwdLabel = new QLabel("Enter password: ");
    nameLine = new QLineEdit;
    passwdLine = new QLineEdit;
    loginBtn = new QPushButton("Login");
    clearBtn = new QPushButton("clear input");

    infoLayout->addWidget(nameLabel, 1, 0);
    infoLayout->addWidget(nameLine, 1, 1);
    infoLayout->addWidget(passwdLabel, 2, 0);
    infoLayout->addWidget(passwdLine, 2, 1);
    infoLayout->addWidget(loginBtn, 3, 0);
    infoLayout->addWidget(clearBtn, 3, 1);

    QLabel *my = new QLabel();
    QPixmap *p = new QPixmap("L:/Users/admin/Documents/icon.jpg");
    QPixmap p1(p->scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    my->setPixmap(p1);
    //infoLayout->addWidget(my, 0, 0);

    test = new QLabel();
    //infoLayout->addWidget(test, 0, 1);

    QObject::connect(loginBtn, SIGNAL(clicked()), this, SLOT(clickLogin()));
    QObject::connect(clearBtn, SIGNAL(clicked()), this, SLOT(clickClear()));

}

bool authUser(QString name, QString passwd)
{
    if (name == "" && passwd == "")
        return true;
    else
        return false;
}

void Messenger::clearLoginPage()
{
    infoLayout->removeWidget(nameLabel);
    infoLayout->removeWidget(nameLine);
    infoLayout->removeWidget(passwdLabel);
    infoLayout->removeWidget(passwdLine);
    infoLayout->removeWidget(loginBtn);
    infoLayout->removeWidget(clearBtn);

    nameLabel->hide();
    nameLine->hide();
    passwdLabel->hide();
    passwdLine->hide();
    loginBtn->hide();
    clearBtn->hide();
}

void Messenger::clickLogin()
{
    QString name = nameLine->text();
    QString passwd = passwdLine->text();
    if (authUser(name, passwd) == true)
    {
        clearLoginPage();
        messagePage();
    }
    else
    {
        QMessageBox::warning(this,"System information", "Invalid User or Password");
    }
}

void Messenger::clickClear()
{
    nameLine->clear();
    passwdLine->clear();
}

void Messenger::callFriend(QString name)
{
    QScrollArea *myScroll = new QScrollArea;
    QFrame *scrollFrame = new QFrame;
    scrollFrame->setStyleSheet("background-color:white;");
    scrollFrame->setFixedHeight(800);
    QVBoxLayout *tabelLayout = new QVBoxLayout(scrollFrame);
    friendList.push_back(tabelLayout);

    myScroll->setWidget(scrollFrame);
    myScroll->setWidgetResizable(true);
    firendTabs->addTab(myScroll, name);
}

void Messenger::messagePage()
{
    sendMsgBtn = new QPushButton("send");
    inputArea = new QTextEdit(this);
    inputArea->setFixedSize(300, 300);
    inputArea->setFocus();

    /*
    messageArea = new QScrollArea;
    scrollFrame = new QFrame;
    scrollFrame->setStyleSheet("background-color:white;");
    tabelLayout = new QVBoxLayout(scrollFrame);

    messageArea->setWidget(scrollFrame);
    messageArea->setWidgetResizable(true);   // important

    infoLayout->addWidget(messageArea, 0, 0);
    */

    messageArea = new QWidget;
    firendTabs = new MessengerTab(messageArea);

    callFriend("John");
    callFriend("Max");

    infoLayout->addWidget(messageArea, 0, 0);
    infoLayout->addWidget(inputArea, 1, 0);
    infoLayout->addWidget(sendMsgBtn, 1, 1);

    QObject::connect(sendMsgBtn, SIGNAL(clicked()),this, SLOT(clickSendMsg()));
}

void Messenger::clickSendMsg()
{
    QLabel *new_label1 = new QLabel;
    QLabel *new_label2 = new QLabel;

    new_label1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    new_label2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    new_label2->setText(inputArea->toPlainText());
    new_label2->setStyleSheet("background-color:gray;");

    QHBoxLayout *labelLayout = new QHBoxLayout;
    labelLayout->addWidget(new_label1);
    labelLayout->addWidget(new_label2);

    //tabelLayout->addLayout(labelLayout);
    qDebug() << firendTabs->currentIndex();
    friendList.at(firendTabs->currentIndex())->addLayout(labelLayout);
    dialog_num++;
}

Messenger::~Messenger()
{
    delete ui;
}
