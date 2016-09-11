/***************************************************************************
                          helpinstructionsform.cpp  -  description
                             -------------------
    begin                : July 19, 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#include "helpinstructionsform.h"

#include "timetable_defs.h"

HelpInstructionsForm::HelpInstructionsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);
	
	plainTextEdit->setReadOnly(true);

	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	setText();
}

HelpInstructionsForm::~HelpInstructionsForm()
{
	saveFETDialogGeometry(this);
}

void HelpInstructionsForm::setText()
{
	QString s;

	s+=tr("Instructions.");
	s+="\n\n";
	s+=tr("Last modified on %1.").arg(tr("12 October 2009"));
	s+="\n\n";
	s+=tr("These are some small instructions which you have to follow in order to input a solvable data set.");
	s+="\n\n";
	s+=tr("If you get an impossible timetable and your institution allows beginning later for students, please reconsider your students (set) early "
		"constraints to allow more beginnings at second hour. Also, removing or weakening other constraints might help. If FET cannot find "
		"a good timetable for your school, make sure to report this, as FET needs to be improved. Maybe small changes to your datafile "
		"can bring good solutions, or maybe there are aspects which have to be changed in FET.");
	s+="\n\n";
	s+=tr("An impossible timetable might also be caused by incorrect years division. Please check statistics/students for all subgroups, "
		"each subgroup should have the necessary hours, not less. If you have for instance some subgroups with less than say 20 hours per "
		"week you might get an impossible timetable, probably because of incorrect division of years and incorrectly added activities. "
		"All the subgroups are independent and should have an amount of working hours per week close to the average for all "
		"subgroups. Please see FAQ for more information.");
	s+="\n\n";
	s+=tr("0) Please select correctly the days and hours (working periods). To ensure best performance, it is best to keep the "
		"number of hours (periods) per day to your required needs, not larger. It is a good practice to choose a value for the number "
		"of days and number of hours from the beginning (you can change their names anytime you want afterwards).");
	s+="\n\n";
	s+=tr("1) To say that component activities of a larger split activity are not in the same day, FET automatically adds a "
		"constraint min days between activities with default 95% weight. If you want to ensure no two activities of same type "
		"are in the same day, you will have to modify the weight from 95% to higher values (see these constraints in the Time constraints "
		"menu->Activities->Min days between a set of activities constraints. You have there a powerful filter to change many "
		"constraints with a few clicks).");
	s+="\n\n";
	s+=tr("To specify that some activities must be in different days, the min days between activities "
		"must be 1. To specify that some activities must be separated even more, you can increase the min days to 2 (or even more, "
		"but probably not needed). Min days = 2 means that activities will be at least 2 days apart from each other (so there is "
		"another day between them). It is recommended that when inputting activities which are split into only 2 subactivities per week, "
		"add min days = 2. Or you can, after introducing the activities, to modify the min days value in more constraints at "
		"once by using the powerful filter in constraints min days between activities dialog (select old number of activities 2 and "
		"new min days 2, maybe also raise weight percentage to 100%).");
	s+="\n\n";
	s+=tr("You can use for the constraint min days the weight you want. It can be 95%, 99.75% or even 100%. If you specify a value below "
		"100% (even 99.75% or more) for an impossible constraint, FET will be able to detect that it is impossible and obtain a "
		"timetable in the end, so the weight is subjective. So, the best way would be to choose a 99.75% for all constraints, "
		"but the time of generation might be larger than with 95%. You could try at first the 95% minimum recommended value, then "
		"highten the weight percentage up to maybe 99.75% or 100%. I am not sure here, I have not enough sample files (please contribute with advice).");
	s+="\n\n";
	s+=tr("If you would like to change the default 95% for another value for some constraints of type min days, the easiest way "
		"is in Data/Time constraints/Min days between activities dialog, where starting with version 5.6.1 there is a "
		"very powerful dialog, where you can specify a filter (old values) and new values for the selected constraints.");
	s+="\n\n";
	s+=tr("You might want to choose different weights for different constraint min days (for instance, higher on subjects with less activities per week)");
	s+="\n\n";
	s+=tr("In versions 5.5.8 and higher, you can add 2 constraints min days for the same split activity. In add activity "
		"dialog, select min days 2 (or 3) and you get the chance to add another constraint min 1 days (or 2). For instance, if you have 3 "
		"activities per week from the same group, you can add 2 constraints, min 2 days and min 1 day, both with 95%. This will ensure that "
		"in 99.75% of cases the min 1 day will be respected.");
	s+="\n\n";
	s+=tr("2) If you have a course lesson with say 4-5 activities per week, difficult lessons (Maths), which you would like to schedule "
		"in the beginning of the day (say 3 out of 4-5 need to be in the first half of the day), the best approach is to use constraint "
		"subactivities preferred time slots or starting times, specifying possibly the subject and component number: 1, 2 and maybe 3"
		", adding 3 constraints for this subject (see FAQ for more details). You can also add constraints for component numbers 3, 4 "
		"and 5 for subject Maths (this way, if activity is split into 4 it will get 2 early, if it is split into 5 it will get 3 "
		"early - a nice trick). Similarly, if you have say Bio with 1 or 2 splits per week, you can add constraint with component number = 2.");
	s+="\n\n";
	s+=tr("3) From the way the algorithm is conceived, the automatic generation is faster if you use students set (or teacher) "
		"not available and/or constraint activity(ies) preferred time slots or preferred starting times to specify impossible slots, "
		"in addition to possible use of students (set) or teacher(s) max hours daily, whenever it is possible. For instance, "
		"if you know that year 5 will only have hours from 8:00 to 13:00, it is better to add students set not available in "
		"addition to students set max hours daily. So, if possible, try to follow this advice. This trick helps guide FET better towards a solution.");
	s+="\n\n";
	s+=tr("Each constraint of type not available or preferred times which filters out impossible slots might bring an improvement in speed.");
	s+="\n\n";
	s+=tr("4) Constraint students (set) early: if you input only partial data, you might get impossible timetables (see FAQ for details).");
	s+="\n\n";
	s+=tr("5) For teacher(s) gaps: please note that FET will take care of teacher(s) gaps only if you tell it so, by adding constraint "
		"teacher(s) max gaps per week. It is normal to add such constraint(s) after you saw that the data is solvable, using for max "
		"gaps a value which seems acceptable, lowering it as you find timetables.");
	s+="\n\n";
	s+=tr("6) Rooms: You might firstly try to generate a timetable without rooms, to see if it is possible, then add rooms and rooms related constraints.");
	s+="\n\n";
	s+=tr("7) It is possible to work with institutions in which the students work in shifts (for instance, lowest forms in the morning and "
		"highest forms in the afternoon). Please see in the Help/Frequently Asked Questions the solution to how to do that "
		"(the essence is to add corresponding constraints students set not available).");
	s+="\n\n";
	s+=tr("8) The new algorithm (after version 5.0.0) does not accept fortnightly activities. But you might apply a trick."
		" Please see menu Help/Frequently Asked Questions menu (it has the description of a possible trick) "
		"(the essence is to make a weekly activity with the sum of teachers and students of both fortnightly activities)");
	s+="\n\n";
	s+=tr("9) Even if it takes a long time to generate your timetable with a set of constraints, strengthening the constraints might not "
		"slow the generation too much. You are advised to try with stronger constraints after you obtain timetables, because you can "
		"obtain better timetables this way.");
	s+="\n\n";
	s+=tr("10) If a constraint is allowed values under 100%, you can use any weight, even fractional numbers like 99.75%. "
		"It might help in constraints like min days between activities, preferred rooms or max hours daily.");
	s+="\n\n";
	s+=tr("11) Each room can host a single activity at the same time. If you have a large room which can host more "
		"activities at the same time, you need to use a small trick. You can add more rooms representing this large room "
		"and add corresponding constraints preferred rooms. There is an entry in the FAQ about that.");
	s+="\n\n";
	s+=tr("12) If teachers are getting days with only one hour of work, you might want to add constraint teacher(s) "
		"min hours daily (probably with 2 hours), but please make sure your timetable is possible. This constraint is "
		"smart, it only considers non-empty days.");
	s+="\n\n";
	s+=tr("13) If you have for instance 7 hours of Maths on a 5 days week (more lessons than days), "
		"please respect the correct way to add these as a split activity. See question Q-1-27-March-2008 from FAQ. It is important!");
	s+="\n\n";
	s+=tr("14) It is recommended to remove redundant min days between activities constraints after adding constraints same starting day or time "
		"(read Help/Important tips).");
	s+="\n\n";
	s+=tr("15) If you want for instance teachers (or students) not to have more than 2 times per week "
		"activities in the last hours, you have to use the constraint teacher(s) or students (set) hourly interval "
		"max days per week. Please read FAQ for more details.");
	s+="\n\n";
	s+=tr("16) If you have activities which you want to put in the last slots of a day (like say "
		"the meetings with the class master), please use the constraint a set of activities end students day (or singular activity ends students day).");
	s+="\n\n";
	s+=tr("17) If you have activities split into 3 activities per week and need them to be not "
		"in 3 consecutive days, there is an entry in the FAQ explaining how to specify this (question Q1-5-September-2008).");
	s+="\n\n";
	s+=tr("18) If you use the not perfect constraints activity tag max hours daily or students max gaps per day (there are 4+2"
		" types of constraints in this category), use them with caution not to obtain an impossible timetable. If the timetable is impossible, it may be because of them."
		" You are advised to add such constraints only in the end, after you are sure that the other constraints are good and the timetable is possible.");
		
	plainTextEdit->setPlainText(s);
}
