#include "DrivesBox.h"

#include <QIcon>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QString>
#include <QVariant>
#include <QVector>
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QFileIconProvider>
#include <QStyle>

#ifdef Q_OS_WIN32

#include <qt_windows.h>

struct DrivesBoxModelItem
{
	QString name;
	QIcon icon;
};

class DrivesBoxModel : public QAbstractItemModel
{
public:
	DrivesBoxModel();
	
	int columnCount(const QModelIndex & parent = QModelIndex()) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex & index) const;
	int rowCount(const QModelIndex & parent = QModelIndex()) const;

private:
	QVector< DrivesBoxModelItem > drives;
};
#endif // Q_OS_WIN32

DrivesBox::DrivesBox(QWidget * parent) : QComboBox(parent)
{
#ifdef Q_OS_WIN32
	setModel(new DrivesBoxModel);
#endif // Q_OS_WIN32
}

#ifdef Q_OS_WIN32
DrivesBoxModel::DrivesBoxModel()
{
	QFileIconProvider iconProvider;
	
	foreach(QFileInfo fi, QDir::drives())
	{
		QString drivePath = fi.absoluteFilePath();
		
		char name[MAX_PATH+1] = {0};
		char fileSystemNameBuffer[MAX_PATH+1] = {0};
		GetVolumeInformationA(drivePath.toAscii().data(), name, MAX_PATH, NULL, NULL, NULL, fileSystemNameBuffer, MAX_PATH);
		
		DrivesBoxModelItem item;
		item.icon = iconProvider.icon(fi);
		
		if(name[0])
			item.name = QString("%1 (%2)").arg(name).arg(drivePath.left(2));
		else
			item.name = drivePath.left(2);
		
		drives.append(item);
	}
}

int DrivesBoxModel::columnCount(const QModelIndex & parent) const
{
	return 1;
}

QVariant DrivesBoxModel::data(const QModelIndex & index, int role) const
{
	if(!index.isValid() || index.row() >= drives.size())
		return QVariant();
	
	const DrivesBoxModelItem * node = &drives.at(index.row());
	switch(role)
	{
		case Qt::DisplayRole: return node->name;
		case Qt::DecorationRole: return node->icon;
		default:
			;
	}
	
	return QVariant();
}

QModelIndex DrivesBoxModel::index(int row, int column, const QModelIndex & parent) const
{
	if(column != 0)
		return QModelIndex();
	
	if(parent.isValid())
		return QModelIndex();
	
	if(row < drives.size())
		return createIndex(row, 0);
	return QModelIndex();
}

QModelIndex DrivesBoxModel::parent(const QModelIndex & index) const
{
	return QModelIndex();
}

int DrivesBoxModel::rowCount(const QModelIndex & parent) const
{
	return drives.count();
}
#endif // Q_OS_WIN32
