#ifndef RANDOMSEEDDIALOG_H
#define RANDOMSEEDDIALOG_H

#include <QtGlobal>

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

class RandomSeedDialog: public QDialog
{
	Q_OBJECT

public:
	QLabel* labelX;
	QLabel* labelY;

	QLabel* valuesLabelX;
	QLabel* valuesLabelY;

	QLineEdit* lineEditX;
	QLineEdit* lineEditY;
	QPushButton* helpPB;
	QPushButton* okPB;
	QPushButton* cancelPB;
	QGridLayout* seedLayoutX;
	QGridLayout* seedLayoutY;
	QHBoxLayout* buttonsLayout;
	QVBoxLayout* mainLayout;

	RandomSeedDialog(QWidget* parent);
	~RandomSeedDialog();

public slots:
	void help();
	void ok();
};

#endif // RANDOMSEEDDIALOG_H
