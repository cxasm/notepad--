#pragma once

#include <QObject>
#include <QMap>
#include <QVariant>

//这个类供插件中调用，务必要保证该类的ABI兼容性。
//这意味着，不能随意增加删除该类中的函数和成员变量;否则会导致插件无法兼容。
//不使用除虚函数之外的任何虚函数
//理论上最好一个插件，对应一个该对象。这样不会冲突。

class QsciScintilla;

class NddPluginApi  : public QObject
{
	Q_OBJECT

public:
	NddPluginApi(QObject *parent);
	~NddPluginApi();

	//获取当前编辑框的对象这个一定要每次动态获取。这里也有问题，一旦QsciScintilla修改，还是不能起到ABI兼容。
	//尽量依赖不需要改动的部分。

	void setMainNotePad(QWidget* pWidget);

	//有了这个当前编辑框后，就可以动态做许多事情了。但是前提是QsciScintilla基类不能随意修改。
	QsciScintilla* getCurrentEidtHandle();



	//这个里面的成员函数要特别小心，一旦给定，则顺序和参数不能随意修改。只能依次往后增加、不能删除。
public:
	//使用动态参数进行参数的传递。使用public传递成员参数
	QMap<QString, QVariant> m_parameter;
	QWidget* m_mainNotePad;
};
