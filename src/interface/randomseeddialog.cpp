#include "randomseeddialog.h"

#include "timetable_defs.h"
#include "randomknuth.h"
#include "longtextmessagebox.h"

RandomSeedDialog::RandomSeedDialog(QWidget* parent): QDialog(parent)
{
	setWindowTitle(tr("Random seed"));

	labelX=new QLabel(tr("Random seed X component:", "Means the X component of the random seed (random seed has 2 components, X and Y). Please keep translation short"));
	lineEditX=new QLineEdit(CustomFETString::number(RandomKnuth::XX));
	labelY=new QLabel(tr("Random seed Y component:", "Means the Y component of the random seed (random seed has 2 components, X and Y). Please keep translation short"));
	lineEditY=new QLineEdit(CustomFETString::number(RandomKnuth::YY));
	okPB=new QPushButton(tr("OK"));
	okPB->setDefault(true);
	helpPB=new QPushButton(tr("Help"));
	cancelPB=new QPushButton(tr("Cancel"));

	valuesLabelX=new QLabel(tr("Allowed minimum %1 to maximum %2").arg(1).arg(RandomKnuth::getMM()-1));
	valuesLabelY=new QLabel(tr("Allowed minimum %1 to maximum %2").arg(1).arg(RandomKnuth::getMMM()-1));

	seedLayoutX=new QGridLayout();
	seedLayoutX->addWidget(labelX,0,0);
	seedLayoutX->addWidget(lineEditX,0,1);
	seedLayoutX->addWidget(valuesLabelX,1,0,1,2);

	seedLayoutY=new QGridLayout();
	seedLayoutY->addWidget(labelY,0,0);
	seedLayoutY->addWidget(lineEditY,0,1);
	seedLayoutY->addWidget(valuesLabelY,1,0,1,2);

	buttonsLayout=new QHBoxLayout();
	buttonsLayout->addWidget(helpPB);
	buttonsLayout->addWidget(okPB);
	buttonsLayout->addWidget(cancelPB);

	mainLayout=new QVBoxLayout(this);
	mainLayout->addLayout(seedLayoutX);
	mainLayout->addStretch();
	mainLayout->addLayout(seedLayoutY);
	mainLayout->addStretch();
	mainLayout->addLayout(buttonsLayout);

	connect(helpPB, SIGNAL(clicked()), this, SLOT(help()));
	connect(okPB, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPB, SIGNAL(clicked()), this, SLOT(reject()));

	int w=this->sizeHint().width();
	int h=this->sizeHint().height();
	if(w<370)
		w=370;
	if(h<220)
		h=220;
	this->resize(w, h);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
}

RandomSeedDialog::~RandomSeedDialog()
{
	saveFETDialogGeometry(this);
}

void RandomSeedDialog::help()
{
	LongTextMessageBox::largeInformation(this, tr("FET information"),
		tr("You can control the random behaviour of FET with this function")+".\n\n"+
		tr("The random seed is the state of the random number generator.")+" "+
		tr("It has two components, X and Y.")+" "+
		tr("X is a value at least %1 and at most %2.").arg(1).arg(RandomKnuth::getMM()-1)+" "+
		tr("Y is a value at least %1 and at most %2.").arg(1).arg(RandomKnuth::getMMM()-1)+" "+
		+"\n\n"+tr("The random seed before the generation of a timetable (the X and Y components) is saved on disk in the corresponding timetables directory,"
		" so that you can simulate again the same generation after that.")+"\n\n"+
		tr("Mode of operation: to obtain the same timetable twice, give the random seed"
		" two values (say X=1234, Y=12345), then generate single, then make it again the same values (X=1234 and Y=12345),"
		" then generate single again. The timetables will be the same. If you generate multiple instead of single,"
		" the first set of timetables will be the same as the second set (if you generate the same number of timetables)"
		" but of course timetables inside each set will be different. If you enter the same random seed (X and Y) on different computers"
		" (but using the same FET version, see note below)"
		" and generate single, the timetables will be the same (if you generate multiple, the sets of timetables will correspond, "
		"the first timetable from simulation 1 with first timetable from simulation 2, etc.)")
		+"\n\n"
		+tr("Note: of course you need exactly the same conditions to duplicate the same simulations (so, you need exactly the same data - activities, constraints, etc.).")
		+"\n\n"
		+tr("Note: for different versions of FET, the behavior of generation may be totally different, even for the same data and the same random seed (same X and same Y), "
		"due to possible changes in the algorithm. You need to consider this as a precaution.")
		+"\n\n"
		+tr("Note: when you start FET, each time, the random seed X is the number of seconds elapsed since 1 January 1970 and the random seed Y is the next number"
		" in the Y series, after the number of seconds elapsed since 1 January 1970")
		+". "
		+tr("After you generate (even partially), the random seed (X and Y components) will change (each call of the random number generator updates the random seed components to the next numbers"
		" in the sequence, and there are many calls to this random generating routine in the generate function)")
		+"\n\n"
		+tr("This setting is useful for more things, maybe one thing is bug report: send you file along with the random seed at the start of generating"
		" (the two components, X and Y, are saved in the timetable directory at the start of generation)")
	);
}

void RandomSeedDialog::ok()
{
	int tx=lineEditX->text().toInt();
	if(!(tx>0 && tx<RandomKnuth::getMM())){
		QMessageBox::warning(this, tr("FET warning"), tr("The random seed X component must be at least %1 and at most %2").arg(1).arg(RandomKnuth::getMM()-1));
		return;
	}

	int ty=lineEditY->text().toInt();
	if(!(ty>0 && ty<RandomKnuth::getMMM())){
		QMessageBox::warning(this, tr("FET warning"), tr("The random seed Y component must be at least %1 and at most %2").arg(1).arg(RandomKnuth::getMMM()-1));
		return;
	}

	accept();
}
