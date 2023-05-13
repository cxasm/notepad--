#include "ccnotepad.h"
#include "nddsetting.h"
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
#if _DEBUG
#pragma comment(lib, "qmyedit_qt5d.lib")
#else
#pragma comment(lib, "qmyedit_qt5.lib")
#endif
#include <qt_windows.h>
const ULONG_PTR CUSTOM_TYPE = 10000;
const ULONG_PTR OPEN_NOTEPAD_TYPE = 10001;
const ULONG_PTR CUSTOM_TYPE_FILE_LINENUM = 10002;
bool s_isAdminAuth = false;
#endif

const QString c_strTitle = "Ndd";


#ifdef Q_OS_UNIX
#if defined(Q_OS_MAC)
QSharedMemory shared("CCNotebook123");;//mac下面后面带一个版本号，避免新的打不开
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
	//可以防止某些屏幕下的字体拥挤重叠问题。暂时屏蔽，不使用qt方法，使用windows自带方案
	// 发现windows自带方案模糊。//发现下面打开后，在win10上反而效果不好，界面会变得很大，默认还是不开启的好。
	//QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	//QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
	//QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::RoundPreferFloor);

#ifdef Q_OS_MAC
    MyApplication a(argc, argv);
#else
	QApplication a(argc, argv);
#endif

	//不能开启，开启后相对路径打开文件失败
	//QDir::setCurrent(QCoreApplication::applicationDirPath());

#if defined(Q_OS_UNIX)
    QApplication::setStyle(QStyleFactory::create("fusion"));
#endif
	a.setApplicationDisplayName(c_strTitle);
	a.setApplicationName(c_strTitle);

	QStringList arguments = QCoreApplication::arguments();

	//目前就三种
	//1) ndd filepath
	//2) ndd filepath -n linenum
	//3) ndd -multi filepath
	//只有 1  2 需要处理短路径
	if ((arguments.size() == 2) || (arguments.size() == 4))
	{
		QFileInfo fi(arguments[1]);
		if (fi.isRelative())
		{
			QString absDir = QDir::currentPath();
			//获取绝对路径
			arguments[1] = QString("%1/%2").arg(absDir).arg(arguments.at(1));
		}
	}

#ifdef uos
	QFont font("Noto Sans CJK SC,9,-1,5,50,0,0,0,0,0,Regular", 9);
	QApplication::setFont(font);
#endif
#ifdef Q_OS_MAC
	//这里的字体大小，务必要和查找结果框的高度匹配，否则会结构字体拥挤
	QFont font("Courier New,11,-1,5,50,0,0,0,0,0,Regular", 11);
	// qDebug() << "font name mac";
	QApplication::setFont(font);
	// qDebug() << QApplication::font().toString();
#endif

bool isGotoLine = false;

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
		else if ((arguments.size() == 4) && arguments[2] == QString("-n"))
		{
			//使用的是 file -n lineNums 方式。目前只有windows下支持 xxxfile -n linenum的格式
			isGotoLine = true;
	}
		
	}
#else
if ((arguments.size() == 4) && (arguments[2] == QString("-n")))
{
      //使用的是 file -n lineNums 方式。目前只有windows下支持 xxxfile -n linenum的格式
      isGotoLine = true;
}
#endif

	//attach成功表示已经存在该内存了，表示当前存在实例
	if (shared.attach())//共享内存被占用则直接返回
	{
		//发现在文件中如果存在空格时，参数不止1个，所以不能单纯用2个参数表示
		if (arguments.size() > 1)
		{
        #if defined(Q_OS_WIN)
			int tryTimes = 0;
			do {
			qlonglong hwndId;
			shared.lock();
			memcpy(&hwndId, shared.data(), sizeof(qlonglong));
			shared.unlock();

			HWND hwnd = (HWND)hwndId;

			if (::IsWindow(hwnd))
			{
					if (!isGotoLine)
					{
						//就是ndd filepath的命令行格式
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
						//是 filepath -n linenums 方式。不考虑filepath含有空格的情况，因为前面做了严格判断
						
						QString para = QString("%1|%2").arg(arguments[1]).arg(arguments[3]);
						QByteArray data = para.toUtf8();

						COPYDATASTRUCT copydata;
						copydata.dwData = CUSTOM_TYPE_FILE_LINENUM; //自定义类型
						copydata.lpData = data.data();  //数据大小
						copydata.cbData = data.size();  // 指向数据的指针

						::SendMessage(hwnd, WM_COPYDATA, reinterpret_cast<WPARAM>(nullptr), reinterpret_cast<LPARAM>(&copydata));
					}

					break;
				}
				else
				{
					
					//20230304 右键多个文件同时打开，比如3个。此时只有第1个可获取锁，其余2个均走这里。
					//因为第个还没有来的及写入hwnd。此时不要goto drop_old。等一下再重试
					QThread::sleep(1); 
					++tryTimes;

					//2次识别后，没法了，只能通过继续往下走。
				//失败了，此时说明前一个窗口极可能状态错误了。如果不处理，则再也打不开程序了
					if (tryTimes > 2)
					{
				goto drop_old;
			}

				}
			} while (true);

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

            //if kill failed, then open a new process
            if(0 != kill(pid,SIGUSR1))
            {
                goto unix_goon;
            }
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

            if(0 != kill(pid,SIGUSR1))
            {
                goto unix_goon;
            }
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

unix_goon:
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

	NddSetting::init();

	int id = NddSetting::getKeyValueFromNumSets(SKIN_KEY);
	StyleSet::setSkin(id);

	CCNotePad *pMainNotepad = new CCNotePad(true);
	pMainNotepad->setAttribute(Qt::WA_DeleteOnClose);
	pMainNotepad->setShareMem(&shared);
	pMainNotepad->quickshow();

	pMainNotepad->syncCurSkinToMenu(id);


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
	//恢复上次关闭时的文件
#ifdef Q_OS_WIN
	if (!s_isAdminAuth)
	{
		if (0 == pMainNotepad->restoreLastFiles() && (arguments.size() == 1))
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
#ifdef Q_OS_WIN
		if (!s_isAdminAuth)
		{
			pMainNotepad->openFile(arguments[1]);
		}
		else
		{
			//如果是管理员，还不能直接打开文件，需要恢复之前文件的修改内容
			//恢复不了，再直接打开
			pMainNotepad->tryRestoreFile(arguments[1]);
		}
#else
		pMainNotepad->openFile(arguments[1]);
#endif
	}
	else if (isGotoLine)
	{
		//是filepath -n xxx 格式。
		bool ok = true;
		int lineNum = arguments[3].toInt(&ok);
		if (!ok)
		{
			lineNum = -1;
		}
		pMainNotepad->openFile(arguments[1], lineNum);
	}
#ifdef Q_OS_WIN
	pMainNotepad->checkAppFont();
#endif

	a.exec();

	NddSetting::close();

	return 0;
}
