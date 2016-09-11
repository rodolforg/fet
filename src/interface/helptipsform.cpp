/***************************************************************************
                          helptipsform.cpp  -  description
                             -------------------
    begin                : Feb 20, 2005
    copyright            : (C) 2005 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "helptipsform.h"

#include "timetable_defs.h"

HelpTipsForm::HelpTipsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);
	
	plainTextEdit->setReadOnly(true);

	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	setText();
}

HelpTipsForm::~HelpTipsForm()
{
	saveFETDialogGeometry(this);
}

void HelpTipsForm::setText()
{
	QString s;
	
	s+=tr("Important tips.");
	s+="\n\n";
	s+=tr("Last modified on %1.").arg(tr("19 August 2009"));
	
	s+="\n\n";
	
	s+=tr("1) Say you add a split activity or you add a constraint min days between activities (say na activities),"
	 " when your week has nd days. Do not add a constraint with na>nd (number of activities strictly greater than number of days per week).");
	s+="\n\n";
	s+=tr("The correct way: if you want consecutive if same day, consider activities having a longer duration."
	 " Instead of adding activities 1+1+1+1+1+1+1 in a 5 days week, add them as 2+2+1+1+1. If you don't want "
	 "consecutive if same day, add 1+1+1+1+1 firstly and 1+1 after that.");
	s+="\n\n";
	s+=tr("The generation will be much faster and easier using this approach (more than 10 times faster sometimes).");
	
	s+="\n\n";
	
	s+=tr("2) When adding constraints activities same starting time (or day), please remove redundant min days constraints. "
	 "For instance, A1 (at same time with B1 and C1), A2 (same time with B2 and C2) and A3 (same time with B3 and C3)."
	 " You will normally have 3 or 6 constraints min days between activities (first one or two with A1,A2,A3 and then "
	 "with B1,B2,B3 and then with C1,C2,C3). These 3 or 6 constraints min days will result in a much stronger constraint, "
	 "which will make the timetable much harder to find. If you have say 4 groups of activities (A, B, C, D), things will be even harder.");
	s+="\n\n";
	s+=tr("The correct way is to leave only the constraints min days referring to the first group of activities (A)."
	 " You will have a much faster generation (maybe 10 times faster or more).");
	s+="\n\n";
	s+=tr("PS: Suppose there are 2 constraints for activities A1,2,3 and 2 constraints for activities B1,2,3 and 2 "
	 "constraints for activities C1,2,3. If A and B and C are simultaneous, remove only the constraints referring to B "
	 "and C (but leave both for A, if for instance you have min 2 days with 95% and min 1 day with 100%).");
	s+="\n\n";
	s+=tr("More details: the combination of 2 or more redundant constraints min days between activities gives another "
	 "resultant constraint, much stronger. From 3 redundant constraints with 95%, you get one with 100%-5%*5%*5%=99.9875%, which is not what you want.");
	s+="\n\n";
	s+=tr("Do not consider redundant constraints with the same activities but different number of days "
	 "(you may want to add min 2 days 95% and min 1 day 95%, which gives in 95% cases the 2 days will be "
	 "respected and in 99.75% cases the 1 day will be respected).");
	s+="\n\n";
	s+=tr("The other constraints (like preferred time(s) constraints) do not have the problem of redundancy like min "
	 "days constraints, so no need to take care about them.");
	s+="\n\n";
	s+=tr("It is IMPORTANT to remove redundant min days constraints after you inputted data and before generating "
	 "(for instance, apply this after adding more constraints of type same starting time/day and before generating)."
	 " Any modification of the min days constraints should be followed by this removal of redundant min days "
	 "constraints (well, not all modifications, but better to do it than not). If you modify more constraints "
	 "at once or apply the balancing of activities, it is important to remove redundant constraints. If you have "
	 "no redundant constraints, it is no need to remove the redundant constraints, but better to check again than "
	 "let some redundant constraints active.");
	s+="\n\n";
	s+=tr("Note: redundant constraints min days with weight 100.0% do not really affect the generation. This is because the resultant of more constraints"
	 " with weight 100.0% is also 100.0%. So, if your data contains only min days between activities constraints with 100.0% weight, there "
	 "is no imperative need to remove redundant constraints."
	 " But, again, better to remove redundant constraints as a precaution.");
	 
	s+="\n\n";
	s+=tr("3) About constraints two activities grouped, two activities consecutive and three activities grouped:"
	 " It is a bad practice to add such constraints if the involved activities are also constrained not to be in the same day"
	 " by constraints min days between activities.");
	s+="\n\n";
	s+=tr("If A1 and A2 are constrained not to be in the same day with 95% weight or any other weight, it is a bad practice "
	"to add a constraint grouped or consecutive to them. If they are constrained with weight 100% not to be in the same day, "
	"the timetable is impossible; if the weight is below 100%, the timetable is more difficult to find than using the correct way."
	" The correct way would probably be to consider A1 and A2 = a single activity A12', or to modify the related constraint "
	"min days between activities. Or maybe you can find other ways.");
	
	plainTextEdit->setPlainText(s);
}
