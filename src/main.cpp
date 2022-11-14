#include "ccnotepad.h"
#include "jsondeploy.h"
#include "styleset.h"


#include <QtWidgets/QApplication>
#include <QTextCodec>
#include <QMessageBox>
#include <QSharedMemory>
#include <QFile>
#include <QStatusBar>
#include <qobject.h>
#include <QThread>
#include <QDir>

#ifdef Q_OS_UNIX
#include <QStyleFactory>
#include <signal.h>
#include <unistd.h>
#include <QDebug>
#include <QWidget>
#endif

#ifdef Q_OS_WIN
#pragma comment(lib, "user32.lib")
#include <qt_windows.h>
const ULONG_PTR CUSTOM_TYPE = 10000;
const ULONG_PTR OPEN_NOTEPAD_TYPE = 10001;
bool s_isAdminAuth = false;
#endif

const QString c_strTitle = "notepad-- v1.17.1";


#ifdef Q_OS_UNIX
#if defined(Q_OS_MAC)
QSharedMemory shared("CCNotebook116");;//mac下面后面带一个版本号，避免新的打不开
#else
QSharedMemory shared("CCNotebook");
#endif
QSharedMemory nppShared("notepad--");

static void sig_usr(int signo)
{
    if(signo  == SIGUSR1)
    {
        qlonglong winId;
        shared.lock();
        memcpy(&winId,shared.data(),sizeof(qlonglong));
        shared.unlock();

         QWidget *pMain = QWidget::find((WId)winId);
         CCNotePad* pNotePad = dynamic_cast<CCNotePad*>(pMain);
         if(pNotePad != nullptr)
         {
             QString filePath((char*)nppShared.data()+4);
             if(!filePath.isEmpty())
             {
             pNotePad->openFile(filePath);
             }
             pNotePad->activateWindow();
             pNotePad->showNormal();
             qDebug() << "sig_usr" << filePath;
         }
    }
}
#endif

#ifdef Q_OS_MAC

static void openfile(QString filePath)
{

    qlonglong winId;
    shared.lock();
    memcpy(&winId,shared.data(),sizeof(qlonglong));
    shared.unlock();

     QWidget *pMain = QWidget::find((WId)winId);
     CCNotePad* pNotePad = dynamic_cast<CCNotePad*>(pMain);
     if(pNotePad != nullptr)
     {
         if(!filePath.isEmpty())
         {
            pNotePad->openFile(filePath);
         }
         pNotePad->activateWindow();
         pNotePad->showNormal();
     }
}

class MyApplication : public QApplication
{
   public:
       MyApplication(int &argc, char **argv)
           : QApplication(argc, argv)
       {
       }

       bool event(QEvent *event)
       {
           if (event->type() == QEvent::FileOpen) {
               QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
               qDebug() << "Open file" << openEvent->file();
               s_openfile = openEvent->file();
               openfile(s_openfile);
           }

           return QApplication::event(event);
       }
       QString s_openfile;
   };
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_MAC
    MyApplication a(argc, argv);
#else
	QApplication a(argc, argv);
#endif

	QDir::setCurrent(QCoreApplication::applicationDirPath());

#if defined(Q_OS_UNIX)
    QApplication::setStyle(QStyleFactory::create("fusion"));
#endif
	a.setApplicationDisplayName(c_strTitle);
	a.setApplicationName(c_strTitle);

	QStringList arguments = QCoreApplication::arguments();

#ifdef Q_OS_WIN
	QSharedMemory shared("ccnotepad");
	if (arguments.size() > 2)
	{
		//如果是多开请求，这种是从管理员权限申请后重开过来的
		if (arguments[1] == QString("-muti"))
		{
			s_isAdminAuth = true;

			QString title = QString(u8"%1 管理员").arg(c_strTitle);
			a.setApplicationDisplayName(title);
			//删除-muti这个参数
			arguments.removeAt(1);
			
			//管理员不占用共享标志。这样的目的是，当管理员窗口存在时
			//打开原来的文件，原来的文件可以占用共享标志，作为主窗口打开。
			//管理员窗口永远不做主窗口打开
			goto authAdmin;
			
		}
	}
#endif

	//attach成功表示已经存在该内存了，表示当前存在实例
	if (shared.attach())//共享内存被占用则直接返回
	{
		//发现在文件中如果存在空格时，参数不止1个，所以不能单纯用2个参数表示
		if (arguments.size() > 1)
		{
        #if defined(Q_OS_WIN)

			qlonglong hwndId;
			shared.lock();
			memcpy(&hwndId, shared.data(), sizeof(qlonglong));
			shared.unlock();

			HWND hwnd = (HWND)hwndId;

			if (::IsWindow(hwnd))
			{
				//去掉第一个参数，后续的参数拼接起来。其实参数中间有空格还是需要使用""引用起来，避免空格参数分隔为多个
				arguments.takeFirst();

				QString filename = arguments.join("");
				QByteArray data = filename.toUtf8();

				COPYDATASTRUCT copydata;
				copydata.dwData = CUSTOM_TYPE; //自定义类型
				copydata.lpData = data.data();  //数据大小
				copydata.cbData = data.size();  // 指向数据的指针

				::SendMessage(hwnd, WM_COPYDATA, reinterpret_cast<WPARAM>(nullptr), reinterpret_cast<LPARAM>(&copydata));
			}
			else
			{
				//失败了，此时说明前一个窗口极可能状态错误了。如果不处理，则再也打不开程序了
				goto drop_old;
			}
        #elif defined (Q_OS_MAC)
        {
               //mac下面不需要，有他自身的机制保证
        }
        #else
            pid_t pid;

            arguments.takeFirst();
            QString filename = arguments.join("");
            QByteArray data = filename.toUtf8();

            nppShared.attach();
            nppShared.lock();
            memcpy(&pid, nppShared.data(), sizeof(pid_t));
            memset((char*)nppShared.data()+sizeof(pid_t),0, 1024-sizeof(pid_t));
            memcpy((char*)nppShared.data()+sizeof(pid_t),data.data(),data.size());
            nppShared.unlock();

            kill(pid,SIGUSR1);
        #endif
		}
		else if (arguments.size() == 1)
		{
#if defined(Q_OS_WIN)
			//把窗口设置到最前
			qlonglong hwndId;
			shared.lock();
			memcpy(&hwndId, shared.data(), sizeof(qlonglong));
			shared.unlock();
			HWND hwnd = (HWND)hwndId;
			if (::IsWindow(hwnd))
			{
				QString filename("open");
				QByteArray data = filename.toUtf8();

				COPYDATASTRUCT copydata;
				copydata.dwData = OPEN_NOTEPAD_TYPE; //自定义类型
				copydata.lpData = data.data();  //数据大小
				copydata.cbData = data.size();  // 指向数据的指针

				::SendMessage(hwnd, WM_COPYDATA, reinterpret_cast<WPARAM>(nullptr), reinterpret_cast<LPARAM>(&copydata));
			}
			else
			{
				//失败了，此时说明前一个窗口极可能状态错误了。如果不处理，则再也打不开程序了
				//继续新开一个窗口，放弃之前的旧内容
				goto drop_old;
			}
#elif defined (Q_OS_MAC)
{
       //mac下面不需要，有他自身的机制保证
}
#else
            pid_t pid;
            nppShared.attach();
            nppShared.lock();
            memcpy(&pid, nppShared.data(), sizeof(pid_t));
            memset((char*)nppShared.data()+sizeof(pid_t),0, 1024-sizeof(pid_t));
            nppShared.unlock();
            qDebug()<<"empty file send";

            kill(pid,SIGUSR1);
#endif
		}
		return 0;
	}
#if defined(Q_OS_WIN)
	shared.create(32);
#elif defined (Q_OS_MAC)
{
       //mac下面不需要，有他自身的机制保证。当程序已经在线时，再打开程序，系统会自动调用已经存在的程序出现
        //不需要使用类似linux下面的机制。
     shared.create(32);
     nppShared.create(32);
}
#else
    shared.create(32);
    nppShared.create(2048);

    if(signal(SIGUSR1,sig_usr) == SIG_ERR)
    {
       qDebug()<<"linux create sign failed";
    }
#endif

#if defined(Q_OS_WIN)
authAdmin:
drop_old:
#endif

	//20221009发现有小概率出现窗口没有，但是进程还在的诡异问题，加个保护一下
	QApplication::setQuitOnLastWindowClosed(true);

	JsonDeploy::init();

	int id = JsonDeploy::getKeyValueFromNumSets(SKIN_KEY);
	StyleSet::setSkin(id);

	CCNotePad *pMainNotepad = new CCNotePad(true);
	pMainNotepad->setAttribute(Qt::WA_DeleteOnClose);
	pMainNotepad->setShareMem(&shared);
	pMainNotepad->show();

	pMainNotepad->syncCurSkinToMenu(id);

#ifdef uos
	QFont font("Noto Sans CJK JP,9,-1,5,50,0,0,0,0,0,Regular", 9);
	QApplication::setFont(font);
#endif
#ifdef Q_OS_MAC
	//这里的字体大小，务必要和查找结果框的高度匹配，否则会结构字体拥挤
	QFont font("Courier New,11,-1,5,50,0,0,0,0,0,Regular", 11);
	QApplication::setFont(font);
#endif

#ifdef Q_OS_WIN
	//HWND hwnd = ::FindWindowA("Qt5QWindowIcon", "CCNotebook");
	//发现hwnd就是和effectiveWinId相等的，不需要查询了
	//管理员可以多开，暂时不把管理员的权限作为主窗口，因为其他用户没有权限右键菜单发送消息给管理员窗口去打开文件
	if (!s_isAdminAuth)
	{
	qlonglong winId = (qlonglong)pMainNotepad->effectiveWinId();
	shared.lock();
	memcpy(shared.data(), &winId, sizeof(qlonglong));
	shared.unlock();
	}
#else
    qlonglong winId = (qlonglong)pMainNotepad->effectiveWinId();
    shared.lock();
    memcpy(shared.data(), &winId, sizeof(qlonglong));
    shared.unlock();
    nppShared.attach();
    //get proceess id to share memory
    pid_t pid = getpid();
    nppShared.lock();
    memcpy(nppShared.data(), &pid, sizeof(pid_t));
    nppShared.unlock();
#endif // Q_OS_WIN

	//else
	//{
	//	pMainNotepad->initTabNewOne();
	//}
	//恢复上次关闭时的文件
#ifdef Q_OS_WIN
	if (!s_isAdminAuth)
	{
		if (0 == pMainNotepad->restoreLastFiles())
		{
		pMainNotepad->initTabNewOne();
	}
	}
#else
    if (0 == pMainNotepad->restoreLastFiles())
    {
    pMainNotepad->initTabNewOne();
    }
#endif

	if (arguments.size() == 2)
	{
		pMainNotepad->openFile(arguments[1]);
		pMainNotepad->showNormal();
	}
#ifdef Q_OS_WIN
	pMainNotepad->checkAppFont();
#endif

	a.exec();

	JsonDeploy::close();

	return 0;
}
