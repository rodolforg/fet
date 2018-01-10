//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "timetable.h"
#include "timetableexport.h"
#include "solution.h"

#include "randomknuth.h"

#include <iostream>
using namespace std;

#include "fetmainform.h"

#include "randomseeddialog.h"

#include "centerwidgetonscreen.h"

#include "timetablegenerateform.h"
#include "timetablegeneratemultipleform.h"

#include "timetableviewstudentsdayshorizontalform.h"
#include "timetableviewstudentstimehorizontalform.h"
#include "timetableviewteachersdayshorizontalform.h"
#include "timetableviewteacherstimehorizontalform.h"
#include "timetableviewroomsdayshorizontalform.h"
#include "timetableviewroomstimehorizontalform.h"
#include "timetableshowconflictsform.h"
#include "timetableprintform.h"
#include "statisticsprintform.h"

#include "export.h"
#include "import.h"

#include "institutionnameform.h"
#include "commentsform.h"
#include "daysform.h"
#include "hoursform.h"
#include "subjectsform.h"
#include "subjectsstatisticsform.h"
#include "activitytagsform.h"
#include "teachersform.h"
#include "teachersstatisticsform.h"
#include "yearsform.h"
#include "splityearform.h"
#include "groupsform.h"
#include "subgroupsform.h"
#include "studentsstatisticsform.h"
#include "activitiesroomsstatisticsform.h"
#include "activitiesform.h"
#include "subactivitiesform.h"
#include "roomsform.h"
#include "buildingsform.h"
#include "alltimeconstraintsform.h"
#include "allspaceconstraintsform.h"
#include "helpaboutform.h"
#include "helpfaqform.h"
#include "helptipsform.h"
#include "helpinstructionsform.h"

#include "fet.h"

#include "groupactivitiesininitialorderitemsform.h"
#include "constraintactivityendsstudentsdayform.h"
#include "constraintactivitiesendstudentsdayform.h"
#include "constrainttwoactivitiesconsecutiveform.h"
#include "constrainttwoactivitiesgroupedform.h"
#include "constraintthreeactivitiesgroupedform.h"
#include "constrainttwoactivitiesorderedform.h"
#include "constraintactivitiespreferredtimeslotsform.h"
#include "constraintactivitiespreferredstartingtimesform.h"

#include "constraintsubactivitiespreferredtimeslotsform.h"
#include "constraintsubactivitiespreferredstartingtimesform.h"

#include "constraintactivitiessamestartingtimeform.h"
#include "constraintactivitiessamestartinghourform.h"
#include "constraintactivitiessamestartingdayform.h"

#include "constraintactivitiesoccupymaxtimeslotsfromselectionform.h"
#include "constraintactivitiesmaxsimultaneousinselectedtimeslotsform.h"

#include "constraintteachernotavailabletimesform.h"
#include "constraintbasiccompulsorytimeform.h"
#include "constraintbasiccompulsoryspaceform.h"
#include "constraintroomnotavailabletimesform.h"
#include "constraintactivitypreferredroomform.h"
#include "constraintstudentssetnotavailabletimesform.h"
#include "constraintbreaktimesform.h"
#include "constraintteachermaxdaysperweekform.h"
#include "constraintteachersmaxdaysperweekform.h"

#include "constraintteachermindaysperweekform.h"
#include "constraintteachersmindaysperweekform.h"

#include "constraintteacherintervalmaxdaysperweekform.h"
#include "constraintteachersintervalmaxdaysperweekform.h"
#include "constraintstudentssetintervalmaxdaysperweekform.h"
#include "constraintstudentsintervalmaxdaysperweekform.h"

#include "constraintstudentssetmaxdaysperweekform.h"
#include "constraintstudentsmaxdaysperweekform.h"

#include "constraintteachermaxhoursdailyform.h"
#include "constraintteachersmaxhoursdailyform.h"
#include "constraintteachermaxhourscontinuouslyform.h"
#include "constraintteachersmaxhourscontinuouslyform.h"

#include "constraintteacheractivitytagmaxhourscontinuouslyform.h"
#include "constraintteachersactivitytagmaxhourscontinuouslyform.h"

#include "constraintteacheractivitytagmaxhoursdailyform.h"
#include "constraintteachersactivitytagmaxhoursdailyform.h"

#include "constraintteacherminhoursdailyform.h"
#include "constraintteachersminhoursdailyform.h"
#include "constraintactivitypreferredstartingtimeform.h"
#include "constraintstudentssetmaxgapsperweekform.h"
#include "constraintstudentsmaxgapsperweekform.h"

#include "constraintstudentssetmaxgapsperdayform.h"
#include "constraintstudentsmaxgapsperdayform.h"

#include "constraintteachersmaxgapsperweekform.h"
#include "constraintteachermaxgapsperweekform.h"
#include "constraintteachersmaxgapsperdayform.h"
#include "constraintteachermaxgapsperdayform.h"
#include "constraintstudentsearlymaxbeginningsatsecondhourform.h"
#include "constraintstudentssetearlymaxbeginningsatsecondhourform.h"
#include "constraintstudentssetmaxhoursdailyform.h"
#include "constraintstudentsmaxhoursdailyform.h"
#include "constraintstudentssetmaxhourscontinuouslyform.h"
#include "constraintstudentsmaxhourscontinuouslyform.h"

#include "constraintstudentssetactivitytagmaxhourscontinuouslyform.h"
#include "constraintstudentsactivitytagmaxhourscontinuouslyform.h"

#include "constraintstudentssetactivitytagmaxhoursdailyform.h"
#include "constraintstudentsactivitytagmaxhoursdailyform.h"

#include "constraintstudentssetminhoursdailyform.h"
#include "constraintstudentsminhoursdailyform.h"
#include "constraintactivitiesnotoverlappingform.h"
#include "constraintmindaysbetweenactivitiesform.h"
#include "constraintmaxdaysbetweenactivitiesform.h"
#include "constraintmingapsbetweenactivitiesform.h"
#include "constraintactivitypreferredtimeslotsform.h"
#include "constraintactivitypreferredstartingtimesform.h"

#include "constraintteachermaxspanperdayform.h"
#include "constraintteachersmaxspanperdayform.h"
#include "constraintstudentssetmaxspanperdayform.h"
#include "constraintstudentsmaxspanperdayform.h"

#include "constraintteacherminrestinghoursform.h"
#include "constraintteachersminrestinghoursform.h"
#include "constraintstudentssetminrestinghoursform.h"
#include "constraintstudentsminrestinghoursform.h"

#include "constraintteachermincontinuousgapinintervalform.h"
#include "constraintteachersmincontinuousgapinintervalform.h"
#include "constraintstudentssetmincontinuousgapinintervalform.h"
#include "constraintstudentsmincontinuousgapinintervalform.h"

#include "constraintactivitypreferredroomsform.h"

#include "constraintstudentssethomeroomform.h"
#include "constraintstudentssethomeroomsform.h"
#include "constraintteacherhomeroomform.h"
#include "constraintteacherhomeroomsform.h"

#include "constraintstudentssetmaxbuildingchangesperdayform.h"
#include "constraintstudentsmaxbuildingchangesperdayform.h"
#include "constraintstudentssetmaxbuildingchangesperweekform.h"
#include "constraintstudentsmaxbuildingchangesperweekform.h"
#include "constraintstudentssetmingapsbetweenbuildingchangesform.h"
#include "constraintstudentsmingapsbetweenbuildingchangesform.h"

#include "constraintteachermaxbuildingchangesperdayform.h"
#include "constraintteachersmaxbuildingchangesperdayform.h"
#include "constraintteachermaxbuildingchangesperweekform.h"
#include "constraintteachersmaxbuildingchangesperweekform.h"
#include "constraintteachermingapsbetweenbuildingchangesform.h"
#include "constraintteachersmingapsbetweenbuildingchangesform.h"

#include "constraintsubjectpreferredroomform.h"
#include "constraintsubjectpreferredroomsform.h"
#include "constraintsubjectactivitytagpreferredroomform.h"
#include "constraintsubjectactivitytagpreferredroomsform.h"

#include "constraintactivitytagpreferredroomform.h"
#include "constraintactivitytagpreferredroomsform.h"

#include "constraintactivitiesoccupymaxdifferentroomsform.h"
#include "constraintactivitiessameroomifconsecutiveform.h"

#include "settingstimetablehtmllevelform.h"

#include "activityplanningconfirmationform.h"
#include "activityplanningform.h"

#include "spreadconfirmationform.h"

#include "removeredundantconfirmationform.h"
#include "removeredundantform.h"

#include "savetimetableconfirmationform.h"

#include "timetablestowriteondiskform.h"

#include "lockunlock.h"
#include "advancedlockunlockform.h"

#include "longtextmessagebox.h"

#include <QMessageBox>

#include <QFileDialog>
#include <QString>
#include <QDir>
#include <QTranslator>
#include <QCloseEvent>
#include <QStatusBar>
#include <QMap>
#include <QWidget>
#include <QList>
#include <QHash>

#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <QMenu>
#include <QCursor>
#include <QSettings>

#include <QFileInfo>
#include <QFile>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslSocket>

#include <QRegExp>

//for the icons of not perfect constraints
#include <QIcon>

#include "spreadmindaysconstraintsfivedaysform.h"

#include "statisticsexport.h"

bool simulation_running; //true if the user started an allocation of the timetable

QString conflictsString; //the string that contains a log of the broken constraints
QString conflictsStringTitle;

bool USE_GUI_COLORS=false;

bool SHOW_SUBGROUPS_IN_COMBO_BOXES=true;
bool SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING=true;

bool SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES=false;

bool ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY=false;
bool ENABLE_STUDENTS_MAX_GAPS_PER_DAY=false;

bool ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=false;

bool ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER=false;

const int STATUS_BAR_MILLISECONDS=2500;

// Constraint dialog constructor helpers
template< typename T > QDialog* fCreate(QWidget* parent) {
	return new T(parent);
}
struct ConstraintDialogHelper {
	typedef QDialog* (*tConstructor)(QWidget*);

	static const struct Mapping{
		QString name;
		tConstructor constructor;
	} constraintActions[];
	static const int numConstraintActions;
};

const ConstraintDialogHelper::Mapping ConstraintDialogHelper::constraintActions[] = {
	{"AllTimeConstraints", fCreate<AllTimeConstraintsForm> },
	{"AllSpaceConstraints", fCreate<AllSpaceConstraintsForm> },

	{"SpaceConstraintsRoomNotAvailableTimes", fCreate<ConstraintRoomNotAvailableTimesForm> },
	{"SpaceConstraintsBasicCompulsorySpace", fCreate<ConstraintBasicCompulsorySpaceForm>},
	{"SpaceConstraintsActivityPreferredRoom", fCreate<ConstraintActivityPreferredRoomForm>},
	{"SpaceConstraintsActivityPreferredRooms", fCreate<ConstraintActivityPreferredRoomsForm>},

	{"SpaceConstraintsStudentsSetHomeRoom", fCreate<ConstraintStudentsSetHomeRoomForm>},
	{"SpaceConstraintsStudentsSetHomeRooms", fCreate<ConstraintStudentsSetHomeRoomsForm>},
	{"SpaceConstraintsTeacherHomeRoom", fCreate<ConstraintTeacherHomeRoomForm>},
	{"SpaceConstraintsTeacherHomeRooms", fCreate<ConstraintTeacherHomeRoomsForm>},

	{"SpaceConstraintsStudentsSetMaxBuildingChangesPerDay", fCreate<ConstraintStudentsSetMaxBuildingChangesPerDayForm>},
	{"SpaceConstraintsStudentsMaxBuildingChangesPerDay", fCreate<ConstraintStudentsMaxBuildingChangesPerDayForm>},
	{"SpaceConstraintsStudentsSetMaxBuildingChangesPerWeek", fCreate<ConstraintStudentsSetMaxBuildingChangesPerWeekForm>},
	{"SpaceConstraintsStudentsMaxBuildingChangesPerWeek", fCreate<ConstraintStudentsMaxBuildingChangesPerWeekForm>},
	{"SpaceConstraintsStudentsSetMinGapsBetweenBuildingChanges", fCreate<ConstraintStudentsSetMinGapsBetweenBuildingChangesForm>},
	{"SpaceConstraintsStudentsMinGapsBetweenBuildingChanges", fCreate<ConstraintStudentsMinGapsBetweenBuildingChangesForm>},

	{"SpaceConstraintsTeacherMaxBuildingChangesPerDay", fCreate<ConstraintTeacherMaxBuildingChangesPerDayForm>},
	{"SpaceConstraintsTeachersMaxBuildingChangesPerDay", fCreate<ConstraintTeachersMaxBuildingChangesPerDayForm>},
	{"SpaceConstraintsTeacherMaxBuildingChangesPerWeek", fCreate<ConstraintTeacherMaxBuildingChangesPerWeekForm>},
	{"SpaceConstraintsTeachersMaxBuildingChangesPerWeek", fCreate<ConstraintTeachersMaxBuildingChangesPerWeekForm>},
	{"SpaceConstraintsTeacherMinGapsBetweenBuildingChanges", fCreate<ConstraintTeacherMinGapsBetweenBuildingChangesForm>},
	{"SpaceConstraintsTeachersMinGapsBetweenBuildingChanges", fCreate<ConstraintTeachersMinGapsBetweenBuildingChangesForm>},

	{"SpaceConstraintsSubjectPreferredRoom", fCreate<ConstraintSubjectPreferredRoomForm>},
	{"SpaceConstraintsSubjectPreferredRooms", fCreate<ConstraintSubjectPreferredRoomsForm>},
	{"SpaceConstraintsSubjectActivityTagPreferredRoom", fCreate<ConstraintSubjectActivityTagPreferredRoomForm>},
	{"SpaceConstraintsSubjectActivityTagPreferredRooms", fCreate<ConstraintSubjectActivityTagPreferredRoomsForm>},

	{"SpaceConstraintsActivityTagPreferredRoom", fCreate<ConstraintActivityTagPreferredRoomForm>},
	{"SpaceConstraintsActivityTagPreferredRooms", fCreate<ConstraintActivityTagPreferredRoomsForm>},

	{"SpaceConstraintsActivitiesOccupyMaxDifferentRooms", fCreate<ConstraintActivitiesOccupyMaxDifferentRoomsForm>},
	{"SpaceConstraintsActivitiesSameRoomIfConsecutive", fCreate<ConstraintActivitiesSameRoomIfConsecutiveForm>},

	{"TimeConstraintsBasicCompulsoryTime", fCreate<ConstraintBasicCompulsoryTimeForm>},
	{"TimeConstraintsBreakTimes", fCreate<ConstraintBreakTimesForm>},

	{"TimeConstraintsTwoActivitiesConsecutive", fCreate<ConstraintTwoActivitiesConsecutiveForm>},
	{"TimeConstraintsTwoActivitiesGrouped", fCreate<ConstraintTwoActivitiesGroupedForm>},
	{"TimeConstraintsThreeActivitiesGrouped", fCreate<ConstraintThreeActivitiesGroupedForm>},
	{"TimeConstraintsTwoActivitiesOrdered", fCreate<ConstraintTwoActivitiesOrderedForm>},
	{"TimeConstraintsActivityPreferredStartingTime", fCreate<ConstraintActivityPreferredStartingTimeForm>},
	{"TimeConstraintsActivityPreferredTimeSlots", fCreate<ConstraintActivityPreferredTimeSlotsForm>},
	{"TimeConstraintsActivitiesPreferredTimeSlots", fCreate<ConstraintActivitiesPreferredTimeSlotsForm>},
	{"TimeConstraintsSubactivitiesPreferredTimeSlots", fCreate<ConstraintSubactivitiesPreferredTimeSlotsForm>},
	{"TimeConstraintsActivityPreferredStartingTimes", fCreate<ConstraintActivityPreferredStartingTimesForm>},
	{"TimeConstraintsActivitiesPreferredStartingTimes", fCreate<ConstraintActivitiesPreferredStartingTimesForm>},
	{"TimeConstraintsSubactivitiesPreferredStartingTimes", fCreate<ConstraintSubactivitiesPreferredStartingTimesForm>},
	{"TimeConstraintsActivitiesSameStartingTime", fCreate<ConstraintActivitiesSameStartingTimeForm>},
	{"TimeConstraintsActivitiesSameStartingHour", fCreate<ConstraintActivitiesSameStartingHourForm>},
	{"TimeConstraintsActivitiesSameStartingDay", fCreate<ConstraintActivitiesSameStartingDayForm>},
	{"TimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelection", fCreate<ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm>},
	{"TimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlots", fCreate<ConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm>},
	{"TimeConstraintsActivitiesNotOverlapping", fCreate<ConstraintActivitiesNotOverlappingForm>},
	{"TimeConstraintsMinDaysBetweenActivities", fCreate<ConstraintMinDaysBetweenActivitiesForm>},
	{"TimeConstraintsMaxDaysBetweenActivities", fCreate<ConstraintMaxDaysBetweenActivitiesForm>},
	{"TimeConstraintsMinGapsBetweenActivities", fCreate<ConstraintMinGapsBetweenActivitiesForm>},
	{"TimeConstraintsActivityEndsStudentsDay", fCreate<ConstraintActivityEndsStudentsDayForm>},
	{"TimeConstraintsActivitiesEndStudentsDay", fCreate<ConstraintActivitiesEndStudentsDayForm>},

	{"TimeConstraintsTeacherNotAvailableTimes", fCreate<ConstraintTeacherNotAvailableTimesForm>},
	{"TimeConstraintsTeacherMaxDaysPerWeek", fCreate<ConstraintTeacherMaxDaysPerWeekForm>},
	{"TimeConstraintsTeachersMaxDaysPerWeek", fCreate<ConstraintTeachersMaxDaysPerWeekForm>},

	{"TimeConstraintsTeacherMinDaysPerWeek", fCreate<ConstraintTeacherMinDaysPerWeekForm>},
	{"TimeConstraintsTeachersMinDaysPerWeek", fCreate<ConstraintTeachersMinDaysPerWeekForm>},

	{"TimeConstraintsTeachersMaxHoursDaily", fCreate<ConstraintTeachersMaxHoursDailyForm>},
	{"TimeConstraintsTeacherMaxHoursDaily", fCreate<ConstraintTeacherMaxHoursDailyForm>},
	{"TimeConstraintsTeachersMaxHoursContinuously", fCreate<ConstraintTeachersMaxHoursContinuouslyForm>},
	{"TimeConstraintsTeacherMaxHoursContinuously", fCreate<ConstraintTeacherMaxHoursContinuouslyForm>},

	{"TimeConstraintsTeachersActivityTagMaxHoursContinuously", fCreate<ConstraintTeachersActivityTagMaxHoursContinuouslyForm>},
	{"TimeConstraintsTeacherActivityTagMaxHoursContinuously", fCreate<ConstraintTeacherActivityTagMaxHoursContinuouslyForm>},

//	{"TimeConstraintsTeachersActivityTagMaxHoursDaily", fCreate<ConstraintTeachersActivityTagMaxHoursDailyForm>},
//	{"TimeConstraintsTeacherActivityTagMaxHoursDaily", fCreate<ConstraintTeacherActivityTagMaxHoursDailyForm>},

	{"TimeConstraintsTeachersMinHoursDaily", fCreate<ConstraintTeachersMinHoursDailyForm>},
	{"TimeConstraintsTeacherMinHoursDaily", fCreate<ConstraintTeacherMinHoursDailyForm>},
	{"TimeConstraintsTeachersMaxGapsPerWeek", fCreate<ConstraintTeachersMaxGapsPerWeekForm>},
	{"TimeConstraintsTeacherMaxGapsPerWeek", fCreate<ConstraintTeacherMaxGapsPerWeekForm>},
	{"TimeConstraintsTeachersMaxGapsPerDay", fCreate<ConstraintTeachersMaxGapsPerDayForm>},
	{"TimeConstraintsTeacherMaxGapsPerDay", fCreate<ConstraintTeacherMaxGapsPerDayForm>},

	{"TimeConstraintsTeacherIntervalMaxDaysPerWeek", fCreate<ConstraintTeacherIntervalMaxDaysPerWeekForm>},
	{"TimeConstraintsTeachersIntervalMaxDaysPerWeek", fCreate<ConstraintTeachersIntervalMaxDaysPerWeekForm>},
	{"TimeConstraintsStudentsSetIntervalMaxDaysPerWeek", fCreate<ConstraintStudentsSetIntervalMaxDaysPerWeekForm>},
	{"TimeConstraintsStudentsIntervalMaxDaysPerWeek", fCreate<ConstraintStudentsIntervalMaxDaysPerWeekForm>},

	{"TimeConstraintsStudentsSetMaxDaysPerWeek", fCreate<ConstraintStudentsSetMaxDaysPerWeekForm>},
	{"TimeConstraintsStudentsMaxDaysPerWeek", fCreate<ConstraintStudentsMaxDaysPerWeekForm>},

	{"TimeConstraintsStudentsSetNotAvailableTimes", fCreate<ConstraintStudentsSetNotAvailableTimesForm>},
	{"TimeConstraintsStudentsSetMaxGapsPerWeek", fCreate<ConstraintStudentsSetMaxGapsPerWeekForm>},
	{"TimeConstraintsStudentsMaxGapsPerWeek", fCreate<ConstraintStudentsMaxGapsPerWeekForm>},

//	{"TimeConstraintsStudentsSetMaxGapsPerDay", fCreate<ConstraintStudentsSetMaxGapsPerDayForm>},
//	{"TimeConstraintsStudentsMaxGapsPerDay", fCreate<ConstraintStudentsMaxGapsPerDayForm>},

	{"TimeConstraintsStudentsEarlyMaxBeginningsAtSecondHour", fCreate<ConstraintStudentsEarlyMaxBeginningsAtSecondHourForm>},
	{"TimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHour", fCreate<ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm>},
	{"TimeConstraintsStudentsMaxHoursDaily", fCreate<ConstraintStudentsMaxHoursDailyForm>},
	{"TimeConstraintsStudentsSetMaxHoursDaily", fCreate<ConstraintStudentsSetMaxHoursDailyForm>},
	{"TimeConstraintsStudentsMaxHoursContinuously", fCreate<ConstraintStudentsMaxHoursContinuouslyForm>},
	{"TimeConstraintsStudentsSetMaxHoursContinuously", fCreate<ConstraintStudentsSetMaxHoursContinuouslyForm>},

	{"TimeConstraintsStudentsActivityTagMaxHoursContinuously", fCreate<ConstraintStudentsActivityTagMaxHoursContinuouslyForm>},
	{"TimeConstraintsStudentsSetActivityTagMaxHoursContinuously", fCreate<ConstraintStudentsSetActivityTagMaxHoursContinuouslyForm>},

//	{"TimeConstraintsStudentsActivityTagMaxHoursDaily", fCreate<ConstraintStudentsActivityTagMaxHoursDailyForm>},
//	{"TimeConstraintsStudentsSetActivityTagMaxHoursDaily", fCreate<ConstraintStudentsSetActivityTagMaxHoursDailyForm>},

	{"TimeConstraintsStudentsMinHoursDaily", fCreate<ConstraintStudentsMinHoursDailyForm>},
	{"TimeConstraintsStudentsSetMinHoursDaily", fCreate<ConstraintStudentsSetMinHoursDailyForm>},

	{"TimeConstraintsTeacherMaxSpanPerDay", fCreate<ConstraintTeacherMaxSpanPerDayForm>},
	{"TimeConstraintsTeachersMaxSpanPerDay", fCreate<ConstraintTeachersMaxSpanPerDayForm>},
	{"TimeConstraintsStudentsSetMaxSpanPerDay", fCreate<ConstraintStudentsSetMaxSpanPerDayForm>},
	{"TimeConstraintsStudentsMaxSpanPerDay", fCreate<ConstraintStudentsMaxSpanPerDayForm>},

	{"TimeConstraintsTeacherMinRestingHours", fCreate<ConstraintTeacherMinRestingHoursForm>},
	{"TimeConstraintsTeachersMinRestingHours", fCreate<ConstraintTeachersMinRestingHoursForm>},
	{"TimeConstraintsStudentsSetMinRestingHours", fCreate<ConstraintStudentsSetMinRestingHoursForm>},
	{"TimeConstraintsStudentsMinRestingHours", fCreate<ConstraintStudentsMinRestingHoursForm>},

	{"TimeConstraintsTeacherMinContinuousGapInInterval", fCreate<ConstraintTeacherMinContinuousGapInIntervalForm>},
	{"TimeConstraintsTeachersMinContinuousGapInInterval", fCreate<ConstraintTeachersMinContinuousGapInIntervalForm>},
	{"TimeConstraintsStudentsSetMinContinuousGapInInterval", fCreate<ConstraintStudentsSetMinContinuousGapInIntervalForm>},
	{"TimeConstraintsStudentsMinContinuousGapInInterval", fCreate<ConstraintStudentsMinContinuousGapInIntervalForm>},
};

const int ConstraintDialogHelper::numConstraintActions = sizeof(constraintActions)/sizeof(constraintActions[0]);

FetMainForm::FetMainForm()
{
	setupUi(this);
	
	QIcon appIcon(":/images/appicon.png");
	QGuiApplication::setWindowIcon(appIcon);

	loadSettings();

	QSettings settings;
	int nRec=settings.value(QString("FetMainForm/number-of-recent-files"), 0).toInt();
	if(nRec>MAX_RECENT_FILES)
		nRec=MAX_RECENT_FILES;
	for(int i=0; i<nRec; i++)
		if(settings.contains(QString("FetMainForm/recent-file/")+CustomFETString::number(i+1)))
			recentFiles.append(settings.value(QString("FetMainForm/recent-file/")+CustomFETString::number(i+1)).toString());
	
	recentSeparatorAction=fileOpenRecentMenu->insertSeparator(fileClearRecentFilesListAction);
	for(int i=0; i<MAX_RECENT_FILES; i++){
		recentFileActions[i]=new QAction(this);
		recentFileActions[i]->setVisible(false);
		connect(recentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
		
		fileOpenRecentMenu->insertAction(recentSeparatorAction, recentFileActions[i]);
	}
	
	updateRecentFileActions();
	
	//statusBar()->showMessage(tr("FET started", "This is a message written in the status bar, saying that FET was started"), STATUS_BAR_MILLISECONDS);
	statusBar()->showMessage("", STATUS_BAR_MILLISECONDS); //to get the correct centralWidget for the logo, so we need status bar existing.
	
	INPUT_FILENAME_XML=QString("");
	setCurrentFile(INPUT_FILENAME_XML);

	//toolBox->setCurrentIndex(0);
	
	shortcutBasicMenu=new QMenu();
	shortcutBasicMenu->addMenu(menuInstitution_information);
	shortcutBasicMenu->addSeparator(); //added on 19 Dec. 2009
	shortcutBasicMenu->addMenu(menuDays_and_hours);
	
	shortcutAdvancedTimeMenu=new QMenu();
	shortcutAdvancedTimeMenu->addAction(spreadActivitiesAction);
	shortcutAdvancedTimeMenu->addSeparator();
	shortcutAdvancedTimeMenu->addAction(removeRedundantConstraintsAction);
	//shortcutAdvancedTimeMenu->addSeparator();
	//shortcutAdvancedTimeMenu->addAction(groupActivitiesInInitialOrderAction);
	
	shortcutDataSpaceMenu=new QMenu();
	shortcutDataSpaceMenu->addAction(dataBuildingsAction);
	shortcutDataSpaceMenu->addSeparator();
	shortcutDataSpaceMenu->addAction(dataRoomsAction);
	
	shortcutDataAdvancedMenu=new QMenu();
	shortcutDataAdvancedMenu->addAction(activityPlanningAction);
	shortcutDataAdvancedMenu->addSeparator();
	shortcutDataAdvancedMenu->addAction(dataTeachersStatisticsAction);
	shortcutDataAdvancedMenu->addAction(dataSubjectsStatisticsAction);
	shortcutDataAdvancedMenu->addAction(dataStudentsStatisticsAction);
	shortcutDataAdvancedMenu->addSeparator();
	shortcutDataAdvancedMenu->addAction(dataActivitiesRoomsStatisticsAction);
	shortcutDataAdvancedMenu->addSeparator();
	shortcutDataAdvancedMenu->addAction(dataTeachersSubjectsQualificationsStatisticsAction);
	
	//2014-07-01
	shortcutTimetableLockingMenu=new QMenu();
	shortcutTimetableLockingMenu->addAction(timetableLockAllActivitiesAction);
	shortcutTimetableLockingMenu->addAction(timetableUnlockAllActivitiesAction);
	shortcutTimetableLockingMenu->addSeparator();
	shortcutTimetableLockingMenu->addAction(timetableLockActivitiesDayAction);
	shortcutTimetableLockingMenu->addAction(timetableUnlockActivitiesDayAction);
	shortcutTimetableLockingMenu->addSeparator();
	shortcutTimetableLockingMenu->addAction(timetableLockActivitiesEndStudentsDayAction);
	shortcutTimetableLockingMenu->addAction(timetableUnlockActivitiesEndStudentsDayAction);
	
	shortcutTimetableAdvancedMenu=new QMenu();
	shortcutTimetableAdvancedMenu->addAction(groupActivitiesInInitialOrderAction);
	
	originalWindowSize = size();

	windowSettingsRect=settings.value("FetMainForm/geometry", QRect(0,0,0,0)).toRect();
	if(!windowSettingsRect.isValid()){
		forceCenterWidgetOnScreen(this);
	}
	else{
		this->setGeometry(windowSettingsRect);
	}

	//new data
	if(gt.rules.initialized)
		gt.rules.kill();
	gt.rules.init();

	connect(&gt.rules, SIGNAL(contentsChanged()), this, SLOT(rules_contentsChanged()));
	connect(&gt.rules, SIGNAL(basicDataResized()), this, SLOT(rules_basicDataResized()));

	bool tmp=gt.rules.addTimeConstraint(new ConstraintBasicCompulsoryTime(100));
	assert(tmp);
	tmp=gt.rules.addSpaceConstraint(new ConstraintBasicCompulsorySpace(100));
	assert(tmp);

	gt.rules.setModified(false);

	CachedSchedule::invalidate();
	
	settingsShowToolTipsForConstraintsWithTablesAction->setCheckable(true);
	settingsShowToolTipsForConstraintsWithTablesAction->setChecked(SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES);
	
	checkForUpdatesAction->setCheckable(true);
	checkForUpdatesAction->setChecked(checkForUpdates);
	
	settingsUseColorsAction->setCheckable(true);
	settingsUseColorsAction->setChecked(USE_GUI_COLORS);
	
	settingsShowSubgroupsInComboBoxesAction->setCheckable(true);
	settingsShowSubgroupsInComboBoxesAction->setChecked(SHOW_SUBGROUPS_IN_COMBO_BOXES);
	settingsShowSubgroupsInActivityPlanningAction->setCheckable(true);
	settingsShowSubgroupsInActivityPlanningAction->setChecked(SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING);
	
	settingsDivideTimetablesByDaysAction->setCheckable(true);
	settingsDivideTimetablesByDaysAction->setChecked(DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS);
	
	settingsDuplicateVerticalNamesAction->setCheckable(true);
	settingsDuplicateVerticalNamesAction->setChecked(TIMETABLE_HTML_REPEAT_NAMES);
	
	if(checkForUpdates){
		//After setGeometry(rect), so that we know the position of the main form, used for the parent 'this' of the QMessageBox below.
		if(!QSslSocket::supportsSsl()){
			QMessageBox::warning(this, tr("FET warning"), tr("SSL is not available (this might be caused by missing SSL libraries)."
			 " Because of this, FET cannot check for updates on startup (it cannot get the file %1).")
			 .arg("https://lalescu.ro/liviu/fet/crtversion/crtversion.txt"));
		}
		else{
			networkManager=new QNetworkAccessManager(this);
			connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
			QUrl url("https://lalescu.ro/liviu/fet/crtversion/crtversion.txt");
			if(VERBOSE){
				cout<<"New version checking host: "<<qPrintable(url.host())<<endl;
				cout<<"New version checking path: "<<qPrintable(url.path())<<endl;
			}
			networkManager->get(QNetworkRequest(url));
		}
	}
	
	settingsPrintActivityTagsAction->setCheckable(true);
	settingsPrintActivityTagsAction->setChecked(TIMETABLE_HTML_PRINT_ACTIVITY_TAGS);

	settingsPrintDetailedTimetablesAction->setCheckable(true);
	settingsPrintDetailedTimetablesAction->setChecked(PRINT_DETAILED_HTML_TIMETABLES);
	settingsPrintDetailedTeachersFreePeriodsTimetablesAction->setCheckable(true);
	settingsPrintDetailedTeachersFreePeriodsTimetablesAction->setChecked(PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS);

	settingsPrintNotAvailableSlotsAction->setCheckable(true);
	settingsPrintNotAvailableSlotsAction->setChecked(PRINT_NOT_AVAILABLE_TIME_SLOTS);

	settingsPrintBreakSlotsAction->setCheckable(true);
	settingsPrintBreakSlotsAction->setChecked(PRINT_BREAK_TIME_SLOTS);

	settingsPrintActivitiesWithSameStartingTimeAction->setCheckable(true);
	settingsPrintActivitiesWithSameStartingTimeAction->setChecked(PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME);

	//needed to sync the view table forms
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	LockUnlock::increaseCommunicationSpinBox();
	
	showWarningForSubgroupsWithTheSameActivitiesAction->setCheckable(true);
	
	enableActivityTagMaxHoursDailyAction->setCheckable(true);
	enableStudentsMaxGapsPerDayAction->setCheckable(true);
	showWarningForNotPerfectConstraintsAction->setCheckable(true);

	enableStudentsMinHoursDailyWithAllowEmptyDaysAction->setCheckable(true);
	showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction->setCheckable(true);
	
	enableGroupActivitiesInInitialOrderAction->setCheckable(true);
	showWarningForGroupActivitiesInInitialOrderAction->setCheckable(true);
	
	showWarningForSubgroupsWithTheSameActivitiesAction->setChecked(SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES);

	enableActivityTagMaxHoursDailyAction->setChecked(ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	enableStudentsMaxGapsPerDayAction->setChecked(ENABLE_STUDENTS_MAX_GAPS_PER_DAY);
	showWarningForNotPerfectConstraintsAction->setChecked(SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS);

	enableStudentsMinHoursDailyWithAllowEmptyDaysAction->setChecked(ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS);
	showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction->setChecked(SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS);

	enableGroupActivitiesInInitialOrderAction->setChecked(ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER);
	showWarningForGroupActivitiesInInitialOrderAction->setChecked(SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER);
	
	connect(showWarningForSubgroupsWithTheSameActivitiesAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForSubgroupsWithTheSameActivitiesToggled(bool)));

	connect(settingsShowSubgroupsInComboBoxesAction, SIGNAL(toggled(bool)), this, SLOT(showSubgroupsInComboBoxesToggled(bool)));
	connect(settingsShowSubgroupsInActivityPlanningAction, SIGNAL(toggled(bool)), this, SLOT(showSubgroupsInActivityPlanningToggled(bool)));
	
	connect(enableActivityTagMaxHoursDailyAction, SIGNAL(toggled(bool)), this, SLOT(enableActivityTagMaxHoursDailyToggled(bool)));
	connect(enableStudentsMaxGapsPerDayAction, SIGNAL(toggled(bool)), this, SLOT(enableStudentsMaxGapsPerDayToggled(bool)));
	connect(showWarningForNotPerfectConstraintsAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForNotPerfectConstraintsToggled(bool)));

	connect(enableStudentsMinHoursDailyWithAllowEmptyDaysAction, SIGNAL(toggled(bool)), this, SLOT(enableStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool)));
	connect(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool)));

	connect(enableGroupActivitiesInInitialOrderAction, SIGNAL(toggled(bool)), this, SLOT(enableGroupActivitiesInInitialOrderToggled(bool)));
	connect(showWarningForGroupActivitiesInInitialOrderAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForGroupActivitiesInInitialOrderToggled(bool)));

	dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction->setIconVisibleInMenu(true);
	dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction->setIconVisibleInMenu(true);
	dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction->setIconVisibleInMenu(true);
	dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction->setIconVisibleInMenu(true);

	dataTimeConstraintsStudentsSetMaxGapsPerDayAction->setIconVisibleInMenu(true);
	dataTimeConstraintsStudentsMaxGapsPerDayAction->setIconVisibleInMenu(true);

	groupActivitiesInInitialOrderAction->setIconVisibleInMenu(true);

	setEnabledIcon(dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);

	setEnabledIcon(dataTimeConstraintsStudentsSetMaxGapsPerDayAction, ENABLE_STUDENTS_MAX_GAPS_PER_DAY);
	setEnabledIcon(dataTimeConstraintsStudentsMaxGapsPerDayAction, ENABLE_STUDENTS_MAX_GAPS_PER_DAY);

	setEnabledIcon(groupActivitiesInInitialOrderAction, ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER);

	for (int i = 0; i < ConstraintDialogHelper::numConstraintActions; ++i) {
		QAction *action = findChild<QAction*>("data"+ConstraintDialogHelper::constraintActions[i].name+"Action");
		connect(action, &QAction::triggered, [this, i](){openConstraintDialog(i);});
	}
}

void FetMainForm::setEnabledIcon(QAction* action, bool enabled)
{
	static QIcon locked(":/images/locked.png");
	static QIcon unlocked(":/images/unlocked.png");
	
	if(enabled)
		action->setIcon(unlocked);
	else
		action->setIcon(locked);
}

//this is another place (out of two) in which you need to add a new language. The other one is in the file fet.cpp
void FetMainForm::populateLanguagesMap(QMap<QString, QString>& languagesMap)
{
	languagesMap.clear();
	languagesMap.insert("ar", tr("Arabic"));
	languagesMap.insert("ca", tr("Catalan"));
	languagesMap.insert("de", tr("German"));
	languagesMap.insert("el", tr("Greek"));
	languagesMap.insert("en_GB", tr("British English"));
	languagesMap.insert("en_US", tr("US English"));
	languagesMap.insert("es", tr("Spanish"));
	languagesMap.insert("fr", tr("French"));
	languagesMap.insert("hu", tr("Hungarian"));
	languagesMap.insert("id", tr("Indonesian"));
	languagesMap.insert("it", tr("Italian"));
	languagesMap.insert("lt", tr("Lithuanian"));
	languagesMap.insert("mk", tr("Macedonian"));
	languagesMap.insert("ms", tr("Malay"));
	languagesMap.insert("nl", tr("Dutch"));
	languagesMap.insert("pl", tr("Polish"));
	languagesMap.insert("ro", tr("Romanian"));
	languagesMap.insert("tr", tr("Turkish"));
	languagesMap.insert("ru", tr("Russian"));
	languagesMap.insert("fa", tr("Persian"));
	languagesMap.insert("uk", tr("Ukrainian"));
	languagesMap.insert("pt_BR", tr("Brazilian Portuguese"));
	languagesMap.insert("da", tr("Danish"));
	languagesMap.insert("si", tr("Sinhala"));
	languagesMap.insert("sk", tr("Slovak"));
	languagesMap.insert("he", tr("Hebrew"));
	languagesMap.insert("sr", tr("Serbian"));
	languagesMap.insert("gl", tr("Galician"));
	languagesMap.insert("vi", tr("Vietnamese"));
	languagesMap.insert("uz", tr("Uzbek"));
	languagesMap.insert("sq", tr("Albanian"));
	languagesMap.insert("zh_CN", tr("Chinese Simplified"));
	languagesMap.insert("zh_TW", tr("Chinese Traditional"));
	languagesMap.insert("eu", tr("Basque"));
	languagesMap.insert("cs", tr("Czech"));
}

bool FetMainForm::isValidFilepathForSaving(const QString &filepath)
{
	int tmp2=filepath.lastIndexOf(FILE_SEP);
	QString s2=filepath.right(filepath.length()-tmp2-1);

	if(s2.length()>=1){
		if(s2.at(0).isSpace()){
			QMessageBox::warning(this, tr("FET information"),
			 tr("Please do not use a filename starting with white space(s), the html css code does not work.")
			 +"\n\n"+tr("File was not saved."));
			return false;
		}
	}
	QString s3;
	bool ewf;
	if(s2.endsWith(".fet")){
		s3=s2.left(s2.length()-4);
		ewf=true;
	}
	else{
		s3=s2;
		ewf=false;
	}
	if(s3.length()>=1){
		if(s3.at(s3.length()-1).isSpace()){
			if(ewf)
				QMessageBox::warning(this, tr("FET information"),
				 tr("Please do not use a filename ending with white space(s) before the '.fet' termination, problems might arise.")
				 +"\n\n"+tr("File was not saved."));
			else
				QMessageBox::warning(this, tr("FET information"),
				 tr("Please do not use a filename ending with white space(s), problems might arise.")
				 +"\n\n"+tr("File was not saved."));
			return false;
		}
	}
	if(s2.indexOf("\"") >= 0){
		QMessageBox::warning(this, tr("FET information"), tr("Please do not use quotation marks \" in filename, the html css code does not work")
		 +"\n\n"+tr("File was not saved."));
		return false;
	}
	if(s2.indexOf(";") >= 0){
		QMessageBox::warning(this, tr("FET information"), tr("Please do not use semicolon ; in filename, the html css code does not work")
		 +"\n\n"+tr("File was not saved."));
		return false;
	}
	if(s2.indexOf("#") >= 0){
		QMessageBox::warning(this, tr("FET information"), tr("Please do not use # in filename, the html css code does not work")
		 +"\n\n"+tr("File was not saved."));
		return false;
	}
	return true;
}


void FetMainForm::enableNotPerfectMessage()
{
	QString s=tr("Constraint is not enabled. To use this type of constraint you must enable it from the Settings->Advanced menu.");
	s+="\n\n";
	s+=tr("Explanation:");
	s+=" ";
	s+=tr("Constraints of this type are good, working, but they are not perfectly optimized.");
	s+=" ";
	s+=tr("For some situations, the generation of the timetable may take too long or be impossible.");
	s+="\n\n";
	s+=tr("Use with caution.");

	QMessageBox::information(this, tr("FET information"), s);
}

void FetMainForm::on_checkForUpdatesAction_toggled()
{
	checkForUpdates=checkForUpdatesAction->isChecked();
}

void FetMainForm::on_settingsUseColorsAction_toggled()
{
	USE_GUI_COLORS=settingsUseColorsAction->isChecked();
	
	LockUnlock::increaseCommunicationSpinBox();
}

void FetMainForm::showSubgroupsInComboBoxesToggled(bool checked)
{
	if(checked==false){
		QString s=tr("Note: if you disable this option, you will not be able to add/modify constraints for subgroups,"
		 " and you will not be able to make filters using a particular subgroup, when selecting activities.");
		//s+="\n\n";
		//s+=tr("Continue only if you know what you are doing.");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
	
		if(b!=QMessageBox::Ok){
			disconnect(settingsShowSubgroupsInComboBoxesAction, SIGNAL(toggled(bool)), this, SLOT(showSubgroupsInComboBoxesToggled(bool)));
			settingsShowSubgroupsInComboBoxesAction->setChecked(true);
			connect(settingsShowSubgroupsInComboBoxesAction, SIGNAL(toggled(bool)), this, SLOT(showSubgroupsInComboBoxesToggled(bool)));
			return;
		}
	}
	
	SHOW_SUBGROUPS_IN_COMBO_BOXES=checked;
}

void FetMainForm::showSubgroupsInActivityPlanningToggled(bool checked)
{
	if(checked==false){
		QString s=tr("Note: if you disable this option, you will not be able to view activities for subgroups"
		 " in the activity planning dialog.");
		//s+="\n\n";
		//s+=tr("Continue only if you know what you are doing.");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
	
		if(b!=QMessageBox::Ok){
			disconnect(settingsShowSubgroupsInActivityPlanningAction, SIGNAL(toggled(bool)), this, SLOT(showSubgroupsInActivityPlanningToggled(bool)));
			settingsShowSubgroupsInActivityPlanningAction->setChecked(true);
			connect(settingsShowSubgroupsInActivityPlanningAction, SIGNAL(toggled(bool)), this, SLOT(showSubgroupsInActivityPlanningToggled(bool)));
			return;
		}
	}
	
	SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING=checked;
}

void FetMainForm::on_settingsShowShortcutsOnMainWindowAction_toggled(bool checked)
{
	tabWidget->setVisible(checked);
}

void FetMainForm::on_settingsShowToolTipsForConstraintsWithTablesAction_toggled()
{
	SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES=settingsShowToolTipsForConstraintsWithTablesAction->isChecked();
}

void FetMainForm::on_settingsDivideTimetablesByDaysAction_toggled()
{
	DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=settingsDivideTimetablesByDaysAction->isChecked();
}

void FetMainForm::on_settingsDuplicateVerticalNamesAction_toggled()
{
	TIMETABLE_HTML_REPEAT_NAMES=settingsDuplicateVerticalNamesAction->isChecked();
}

void FetMainForm::on_timetablesToWriteOnDiskAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	TimetablesToWriteOnDiskForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::replyFinished(QNetworkReply* networkReply)
{
	if(networkReply->error()!=QNetworkReply::NoError){
		QString s=QString("");
		s+=tr("Could not search for possible updates on the internet - error message is: %1.").arg(networkReply->errorString());
		s+=QString("\n\n");
		s+=tr("Searching for file %1.").arg("https://lalescu.ro/liviu/fet/crtversion/crtversion.txt");
		s+=QString("\n\n");
		s+=tr("Possible actions: check your network connection, try again later, try to visit FET homepage: %1, or"
		 " try to search for the new FET page on the internet (maybe it has changed).").arg("https://lalescu.ro/liviu/fet/");

		QMessageBox::warning(this, tr("FET warning"), s);
	}
	else{
		QString internetVersion;
		QString additionalComments;
		
		QRegExp regExp("^\\s*(\\S+)(.*)$");
		int t=regExp.indexIn(QString(networkReply->readAll()));
		if(t!=0){
			QString s=QString("");
			s+=tr("The file %1 from the FET homepage, indicating the current FET version, is incorrect.").arg("https://lalescu.ro/liviu/fet/crtversion/crtversion.txt");
			s+=QString("\n\n");
			s+=tr("Maybe the FET homepage has some temporary problems, so try again later."
			 " Or maybe the current structure on FET homepage was changed. You may visit FET homepage: %1, and get latest version or,"
			 " if it does not work, try to search for the new FET page on the internet (maybe it has changed).")
			  .arg("https://lalescu.ro/liviu/fet/");

			QMessageBox::warning(this, tr("FET warning"), s);
		}
		else{
			internetVersion=regExp.cap(1);
			additionalComments=regExp.cap(2).trimmed();

			if(VERBOSE){
				cout<<"Your current version: '";
				cout<<qPrintable(FET_VERSION)<<"'"<<endl;
				cout<<"Latest version: '";
				cout<<qPrintable(internetVersion)<<"'"<<endl;
			}
			if(internetVersion!=FET_VERSION){
				QString s=tr("Another version: %1, is available on the FET homepage: %2", "%1 is new version, %2 is FET homepage").arg(internetVersion).arg("https://lalescu.ro/liviu/fet/");
				s+=QString("\n\n");
				s+=tr("You have to manually download and install.")+QString(" ")+tr("You may need to hit Refresh in your web browser.")+QString("\n\n")+tr("Would you like to open the FET homepage now?");
				if(!additionalComments.isEmpty()){
					s+=QString("\n\n");
					s+=tr("Additional comments: %1").arg(additionalComments);
				}
			
				QMessageBox::StandardButton button=QMessageBox::information(this, tr("FET information"), s, QMessageBox::Yes|QMessageBox::No);
				
				if(button==QMessageBox::Yes){
					bool tds=QDesktopServices::openUrl(QUrl("https://lalescu.ro/liviu/fet/"));
					if(!tds){
						QMessageBox::warning(this, tr("FET warning"), tr("Could not start the default internet browser (trying to open the link %1)."
							" Maybe you can try to manually start your browser and open this link.").arg("https://lalescu.ro/liviu/fet/"));
					}
				}
			}
		}
	}

	networkReply->deleteLater();
}

void FetMainForm::closeOtherWindows()
{
	QList<QWidget*> tlwl=qApp->topLevelWidgets();
	
	foreach(QWidget* wi, tlwl)
		if(wi->isVisible() && wi!=this)
			wi->close();
}

void FetMainForm::closeEvent(QCloseEvent* event)
{
	windowSettingsRect=this->geometry();
	
	if(gt.rules.isModified()){
		QMessageBox::StandardButton res=QMessageBox::question( this, tr("FET - exiting"),
		 tr("Your data file has been modified - do you want to save it?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

		if(res==QMessageBox::Yes){
			bool t=this->fileSave();
			if(!t){
				event->ignore();
			}
			else{
				closeOtherWindows();
				event->accept();
			}
		}
		else if(res==QMessageBox::No){
			closeOtherWindows();
			event->accept();
		}
		else{
			assert(res==QMessageBox::Cancel);
			event->ignore();
		}
	}
	else{
		closeOtherWindows();
		event->accept();
	}
}

FetMainForm::~FetMainForm()
{
	QSettings settings;
	settings.setValue(QString("FetMainForm/number-of-recent-files"), recentFiles.count());
	settings.remove(QString("FetMainForm/recent-file"));
	for(int i=0; i<recentFiles.count(); i++)
		settings.setValue(QString("FetMainForm/recent-file/")+CustomFETString::number(i+1), recentFiles.at(i));

	settings.setValue("FetMainForm/geometry", windowSettingsRect);

	shortcutBasicMenu->clear();
	delete shortcutBasicMenu;

	shortcutAdvancedTimeMenu->clear();
	delete shortcutAdvancedTimeMenu;

	shortcutDataSpaceMenu->clear();
	delete shortcutDataSpaceMenu;

	shortcutDataAdvancedMenu->clear();
	delete shortcutDataAdvancedMenu;
	
	//2014-07-01
	shortcutTimetableLockingMenu->clear();
	delete shortcutTimetableLockingMenu;
	
	shortcutTimetableAdvancedMenu->clear();
	delete shortcutTimetableAdvancedMenu;
}

void FetMainForm::on_fileExitAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	close();
}

QString FetMainForm::strippedName(const QString& fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}

void FetMainForm::setCurrentFile(const QString& fileName)
{
	QString currentFile=fileName;
	QString shownName=tr("Untitled");
	
	if(!currentFile.isEmpty()){
		shownName=strippedName(currentFile);
		recentFiles.removeAll(currentFile);
		recentFiles.prepend(currentFile);
		if(recentFiles.count()>MAX_RECENT_FILES){
			assert(recentFiles.count()==MAX_RECENT_FILES+1);
			assert(!recentFiles.isEmpty());
			recentFiles.removeLast();
		}
		updateRecentFileActions();
	}
	
	setWindowTitle(tr("%1[*] - FET", "The title of the main window, %1 is the name of the current file. "
	 "Please keep the string [*] unmodified (three characters) - it is used to make the difference between modified files and unmodified files.")
	 .arg(shownName));
}

void FetMainForm::updateRecentFileActions()
{
	QMutableStringListIterator i(recentFiles);
	while(i.hasNext()){
		if(!QFile::exists(i.next()))
			i.remove();
	}
	
	for(int j=0; j<MAX_RECENT_FILES; j++){
		if(j<recentFiles.count()){
			QString text=strippedName(recentFiles[j]);
			recentFileActions[j]->setText(text);
			recentFileActions[j]->setData(recentFiles[j]);
			recentFileActions[j]->setVisible(true);
		}
		else{
			recentFileActions[j]->setVisible(false);
		}
	}
	
	recentSeparatorAction->setVisible(!recentFiles.isEmpty());
}

void FetMainForm::on_fileClearRecentFilesListAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	recentFiles.clear();
	updateRecentFileActions();
}

void FetMainForm::on_fileNewAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	int confirm=0;
	
	if(gt.rules.isModified()){
		switch( QMessageBox::question(
		 this,
		 tr("FET application"),
		 tr("Your current data file has been modified. Are you sure you want to reset to new empty data?"),
		 QMessageBox::Yes|QMessageBox::No
		 ) ){
		case QMessageBox::Yes: // Yes
			confirm=1;
			break;
		case QMessageBox::No: // No
			confirm=0;
			break;
		default:
			assert(0);
			break;
		}
	}
	else
		confirm=1;
	
	if(confirm){
		INPUT_FILENAME_XML=QString("");
		setCurrentFile(INPUT_FILENAME_XML);
	
		if(gt.rules.initialized)
			gt.rules.kill();
		gt.rules.init();
		
		bool tmp=gt.rules.addTimeConstraint(new ConstraintBasicCompulsoryTime(100));
		assert(tmp);
		tmp=gt.rules.addSpaceConstraint(new ConstraintBasicCompulsorySpace(100));
		assert(tmp);
		
		gt.rules.setModified(false);

		CachedSchedule::invalidate();

		LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
		LockUnlock::increaseCommunicationSpinBox();

		statusBar()->showMessage(tr("New file generated"), STATUS_BAR_MILLISECONDS);
	}
}

void FetMainForm::openRecentFile()
{
	QAction* action=qobject_cast<QAction*>(sender());
	if(action)
		openFile(action->data().toString());
}

void FetMainForm::on_fileOpenAction_triggered()
{
	openFile(QString());
}

void FetMainForm::openFile(const QString& fileName)
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	int confirm=0;
	
	if(gt.rules.isModified()){
		switch( QMessageBox::question(
		 this,
		 tr("FET application"),
		 tr("Your current data file has been modified. Are you sure you want to open another data file?"),
		 QMessageBox::Yes|QMessageBox::No
		 ) ){
		case QMessageBox::Yes: // Yes
			confirm=1;
			break;
		case QMessageBox::No: // No
			confirm=0;
			break;
		default:
			assert(0);
			break;
		}
	}
	else
		confirm=1;
	
	if(confirm){
		QString s=fileName;
		
		if(s.isNull()){
			s = QFileDialog::getOpenFileName(this, tr("Choose a file to open"),
			 WORKING_DIRECTORY, 
			 tr("FET XML files", "Instructions for translators: FET XML is a type of file format (using text mode). "
			 "So this field means files in the FET XML format")+" (*.fet)"+";;"+tr("All files")+" (*)");

			if(s.isNull())
				return;
		}

		int tmp2=s.lastIndexOf(FILE_SEP);
		QString s2=s.right(s.length()-tmp2-1);
		
		if(s2.length()>=1){
			if(s2.at(0).isSpace()){
				QMessageBox::warning(this, tr("FET information"),
				 tr("Please do not use a filename starting with white space(s), the html css code does not work."
				  " File was not loaded. Please rename it, removing the white space(s) from the beginning and open it after that with FET."));
				return;
			}
		}
		QString s3;
		if(s2.endsWith(".fet"))
			s3=s2.left(s2.length()-4);
		else
			s3=s2;
		if(s3.length()>=1){
			if(s3.at(s3.length()-1).isSpace()){
				QMessageBox::warning(this, tr("FET information"),
				 tr("Please do not use a filename ending with white space(s) before the '.fet' termination or at the end of the filename,"
				 " problems might arise. File was not loaded. Please rename it, removing the white space(s) at the end and open it after"
				 " that with FET."));
				return;
			}
		}
		if(s2.indexOf("\"") >= 0){
			QMessageBox::warning(this, tr("FET information"),
			 tr("Please do not use quotation marks \" in filename, the html css code does not work."
			  " File was not loaded. Please rename it, removing not allowed characters and open it after that with FET."));
			return;
		}		
		if(s2.indexOf(";") >= 0){
			QMessageBox::warning(this, tr("FET information"), 
			 tr("Please do not use semicolon ; in filename, the html css code does not work."
			  " File was not loaded. Please rename it, removing not allowed characters and open it after that with FET."));
			return;
		}
		if(s2.indexOf("#") >= 0){
			QMessageBox::warning(this, tr("FET information"), 
			 tr("Please do not use # in filename, the html css code does not work."
			  " File was not loaded. Please rename it, removing not allowed characters and open it after that with FET."));
			return;
		}
		/*if(s2.indexOf("(") >= 0 || s2.indexOf(")")>=0){
			QMessageBox::information(this, tr("FET information"), tr("Please do not use parentheses () in filename, the html css code does not work"));
			return;
		}*/
		else{
			//QCursor orig=this->cursor();
			//this->setCursor(Qt::WaitCursor);
			statusBar()->showMessage(tr("Loading...", "This is a message in the status bar, that we are loading the file"), 0);
			QCoreApplication::processEvents();
		
			//bool before=gt.rules.modified;

			if(gt.rules.read(this, s)){
				CachedSchedule::invalidate();

				INPUT_FILENAME_XML = s;
				
				LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
				LockUnlock::increaseCommunicationSpinBox();

				statusBar()->showMessage(tr("File opened"), STATUS_BAR_MILLISECONDS);
				
				gt.rules.setModified(false);
				
				setCurrentFile(INPUT_FILENAME_XML);
			}
			else{
				//incorrect code - the old file may be broken - so we generate a new file.
				/*gt.rules.modified=before;
			
				statusBar()->showMessage("", STATUS_BAR_MILLISECONDS);
				
				setCurrentFile(INPUT_FILENAME_XML);*/
				
				assert(!simulation_running);
				gt.rules.setModified(false);
				on_fileNewAction_triggered();
			}
			
			//this->setCursor(orig);
		}
		//get the directory
		int tmp=s.lastIndexOf(FILE_SEP);
		WORKING_DIRECTORY=s.left(tmp);
	}
}

bool FetMainForm::fileSaveAs()
{
	QString predefFileName=INPUT_FILENAME_XML;
	if(predefFileName.isEmpty())
		predefFileName=WORKING_DIRECTORY+FILE_SEP+QString("untitled.fet");

	QString s = QFileDialog::getSaveFileName(this, tr("Choose a filename to save under"),
		predefFileName, tr("FET XML files", "Instructions for translators: FET XML is a type of file format (using text mode). "
		"So this field means files in the FET XML format")+" (*.fet)"+";;"+tr("All files")+" (*)",
		0, QFileDialog::DontConfirmOverwrite);
	if(s.isEmpty())
		return false;

	if (!isValidFilepathForSaving(s))
		return false;
	
	if(s.right(4)!=".fet")
		s+=".fet";

	int tmp=s.lastIndexOf(FILE_SEP);
	WORKING_DIRECTORY=s.left(tmp);

	if(QFile::exists(s))
		if(QMessageBox::warning( this, tr("FET"),
		 tr("File %1 exists - are you sure you want to overwrite it?").arg(s),
		 QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
		 	return false;
			
	bool t=gt.rules.write(this, s);
	if(t){
		INPUT_FILENAME_XML = s;

		gt.rules.setModified(false);
	
		setCurrentFile(INPUT_FILENAME_XML);
	
		statusBar()->showMessage(tr("File saved"), STATUS_BAR_MILLISECONDS);
		
		return true;
	}
	else{
		return false;
	}
}

void FetMainForm::resetSettings()
{
	settingsShowShortcutsOnMainWindowAction->setChecked(true);

	settingsConfirmActivityPlanningAction->setChecked(true);
	settingsConfirmSpreadActivitiesAction->setChecked(true);
	settingsConfirmRemoveRedundantAction->setChecked(true);
	settingsConfirmSaveTimetableAction->setChecked(true);
}

void FetMainForm::loadSettings()
{
	QSettings settings;
	settingsShowShortcutsOnMainWindowAction->setChecked(settings.value("FetMainForm/show-shortcuts", true).toBool());

	settingsConfirmActivityPlanningAction->setChecked(settings.value("confirm-activity-planning", true).toBool());
	settingsConfirmSpreadActivitiesAction->setChecked(settings.value("confirm-spread-activities", true).toBool());
	settingsConfirmRemoveRedundantAction->setChecked(settings.value("confirm-remove-redundant", true).toBool());
	settingsConfirmSaveTimetableAction->setChecked(settings.value("confirm-save-data-and-timetable", true).toBool());
}

void FetMainForm::saveSettings()
{
	QSettings settings;
	settings.setValue("FetMainForm/show-shortcuts", settingsShowShortcutsOnMainWindowAction->isChecked());

	settings.setValue("confirm-activity-planning", settingsConfirmActivityPlanningAction->isChecked());
	settings.setValue("confirm-spread-activities", settingsConfirmSpreadActivitiesAction->isChecked());
	settings.setValue("confirm-remove-redundant", settingsConfirmRemoveRedundantAction->isChecked());
	settings.setValue("confirm-save-data-and-timetable", settingsConfirmSaveTimetableAction->isChecked());
}

void FetMainForm::on_fileSaveAsAction_triggered()
{
	fileSaveAs();
}

// Start of code contributed by Volker Dirr
void FetMainForm::on_fileImportCSVRoomsBuildingsAction_triggered(){
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVRoomsAndBuildings(this);
}

void FetMainForm::on_fileImportCSVSubjectsAction_triggered(){
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVSubjects(this);
}

void FetMainForm::on_fileImportCSVTeachersAction_triggered(){
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVTeachers(this);
}

void FetMainForm::on_fileImportCSVActivitiesAction_triggered(){
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVActivities(this);

	//TODO: if the import takes care of locked activities, then we need
	//to do:
	//LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	//LockUnlock::increaseCommunicationSpinBox();
	//after the importing
}

void FetMainForm::on_fileImportCSVActivityTagsAction_triggered(){
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVActivityTags(this);
}

void FetMainForm::on_fileImportCSVYearsGroupsSubgroupsAction_triggered(){
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVStudents(this);
}

void FetMainForm::on_fileExportCSVAction_triggered(){
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Export exp(gt, CachedSchedule::getCachedSolution());
	exp.exportCSV(this);
}
// End of code contributed by Volker Dirr

void FetMainForm::on_timetableSaveTimetableAsAction_triggered()
{
	if(!CachedSchedule::isValid()){
		QMessageBox::warning(this, tr("FET - Warning"), tr("You have not yet generated a timetable - please generate firstly"));
		return;
	}

	const Solution* tc=&CachedSchedule::getCachedSolution();
	
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		//Activity* act=&gt.rules.internalActivitiesList[ai];
		int time=tc->time(ai);
		if(time==UNALLOCATED_TIME){
			QMessageBox::warning(this, tr("FET - Warning"), tr("It seems that you have an incomplete timetable."
			 " Saving of timetable does not work for incomplete timetables. Please generate a complete timetable"));
			 //.arg(act->id));
			return;	
		}
		
		int ri=tc->room(ai);
		if(ri==UNALLOCATED_SPACE){
			QMessageBox::warning(this, tr("FET - Warning"), tr("It seems that you have an incomplete timetable."
			 " Saving of timetable does not work for incomplete timetables. Please generate a complete timetable"));
			 //.arg(act->id));
			return;	
		}
	}

	if(settingsConfirmSaveTimetableAction->isChecked()){
		int confirm;
		
		SaveTimetableConfirmationForm c_form(this);
		setParentAndOtherThings(&c_form, this);
		confirm=c_form.exec();
		
		if(confirm==QDialog::Accepted){
			if(c_form.dontShowAgain)
				settingsConfirmSaveTimetableAction->setChecked(false);
		} else {
			return;
		}
	}
		
	if(1){
		QString s;

		for(;;){
			s = QFileDialog::getSaveFileName(this, tr("Choose a filename for data and timetable"),
				INPUT_FILENAME_XML, tr("FET XML files", "Instructions for translators: FET XML is a type of file format (using text mode). "
				"So this field means files in the FET XML format")+" (*.fet)"+";;"+tr("All files")+" (*)",
				0, QFileDialog::DontConfirmOverwrite);
			if(s.isEmpty())
				return;

			if (!isValidFilepathForSaving(s))
				continue;
			
			if(s.right(4)!=".fet")
				s+=".fet";

			int tmp=s.lastIndexOf(FILE_SEP);
			WORKING_DIRECTORY=s.left(tmp);

			if(QFile::exists(s)){
				QString t=tr("File exists");
				t+="\n\n";
				t+=tr("For safety (so you don't lose work), it is not allowed to overwrite an existing file with"
					" locking and saving a current data+timetable");
				t+="\n\n";
				t+=tr("Please choose a non-existing name");
		
				QMessageBox::warning( this, tr("FET warning"), t);
			}
			else
				break;
		}

		Rules rules2;
		rules2.initialized=true;
		
		rules2.setInstitutionName(gt.rules.getInstitutionName());
		rules2.setComments(gt.rules.getComments());
		
		rules2.nHoursPerDay=gt.rules.nHoursPerDay;
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			rules2.hoursOfTheDay[i]=gt.rules.hoursOfTheDay[i];

		rules2.nDaysPerWeek=gt.rules.nDaysPerWeek;
		for(int i=0; i<gt.rules.nDaysPerWeek; i++)
			rules2.daysOfTheWeek[i]=gt.rules.daysOfTheWeek[i];
			
		rules2.yearsList=gt.rules.yearsList;
		
		rules2.teachersList=gt.rules.teachersList;
		
		rules2.subjectsList=gt.rules.subjectsList;
		
		rules2.activityTagsList=gt.rules.activityTagsList;

		rules2.activitiesList=gt.rules.activitiesList;

		rules2.buildingsList=gt.rules.buildingsList;

		rules2.roomsList=gt.rules.roomsList;

		rules2.timeConstraintsList=gt.rules.timeConstraintsList;
		
		rules2.spaceConstraintsList=gt.rules.spaceConstraintsList;

		rules2.apstHash=gt.rules.apstHash;
		rules2.aprHash=gt.rules.aprHash;

		rules2.groupActivitiesInInitialOrderList=gt.rules.groupActivitiesInInitialOrderList;

		//add locking constraints
		TimeConstraintsList lockTimeConstraintsList;
		SpaceConstraintsList lockSpaceConstraintsList;

		//bool report=true;
		
		int addedTime=0, duplicatesTime=0;
		int addedSpace=0, duplicatesSpace=0;
		
		QString constraintsString=QString("");

		//lock selected activities
		for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			int time=tc->time(ai);
			if(time>=0 && time<gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay){
				int hour=time/gt.rules.nDaysPerWeek;
				int day=time%gt.rules.nDaysPerWeek;

				ConstraintActivityPreferredStartingTime* ctr=new ConstraintActivityPreferredStartingTime(100.0, act->id, day, hour, false); //permanently locked is false
				bool t=rules2.addTimeConstraint(ctr);
							
				if(t){
					addedTime++;
					lockTimeConstraintsList.append(ctr);
				}
				else
					duplicatesTime++;

				QString s;

				if(t)
					s=tr("Added to the saved file:", "It refers to a constraint")+QString("\n")+ctr->getDetailedDescription(gt.rules);
				else{
					s=tr("NOT added to the saved file (already existing):", "It refers to a constraint")+QString("\n")+ctr->getDetailedDescription(gt.rules);
					delete ctr;
				}
				
				constraintsString+=QString("\n");
				constraintsString+=s;
			}
						
			int ri=tc->room(ai);
			if(ri!=UNALLOCATED_SPACE && ri!=UNSPECIFIED_ROOM && ri>=0 && ri<gt.rules.nInternalRooms){
				ConstraintActivityPreferredRoom* ctr=new ConstraintActivityPreferredRoom(100, act->id, (gt.rules.internalRoomsList[ri])->name, false); //false means not permanently locked
				bool t=rules2.addSpaceConstraint(ctr);

				QString s;
							
				if(t){
					addedSpace++;
					lockSpaceConstraintsList.append(ctr);
				}
				else
					duplicatesSpace++;

				if(t)
					s=tr("Added to the saved file:", "It refers to a constraint")+QString("\n")+ctr->getDetailedDescription(gt.rules);
				else{
					s=tr("NOT added to the saved file (already existing):", "It refers to a constraint")+QString("\n")+ctr->getDetailedDescription(gt.rules);
					delete ctr;
				}
				
				constraintsString+=QString("\n");
				constraintsString+=s;
			}
		}

		LongTextMessageBox::largeInformation(this, tr("FET information"), tr("Added %1 locking time constraints and %2 locking space constraints to saved file,"
		" ignored %3 activities which were already fixed in time and %4 activities which were already fixed in space.").arg(addedTime).arg(addedSpace).arg(duplicatesTime).arg(duplicatesSpace)
		+QString("\n\n")+tr("Detailed information about each locking constraint which was added or not (if already existing) to the saved file:")+QString("\n")+constraintsString
		+QString("\n")+tr("Your current data file remained untouched (no locking constraints were added), so you can save it also, or generate different timetables."));
			
		bool result=rules2.write(this, s);
		
		Q_UNUSED(result);
		
		while(!lockTimeConstraintsList.isEmpty())
			delete lockTimeConstraintsList.takeFirst();
		while(!lockSpaceConstraintsList.isEmpty())
			delete lockSpaceConstraintsList.takeFirst();

		rules2.nHoursPerDay=0;
		rules2.nDaysPerWeek=0;

		rules2.yearsList.clear();
		
		rules2.teachersList.clear();
		
		rules2.subjectsList.clear();
		
		rules2.activityTagsList.clear();

		rules2.activitiesList.clear();

		rules2.buildingsList.clear();

		rules2.roomsList.clear();

		rules2.timeConstraintsList.clear();
		
		rules2.spaceConstraintsList.clear();

		rules2.apstHash.clear();
		rules2.aprHash.clear();

		rules2.groupActivitiesInInitialOrderList.clear();
	}
}

bool FetMainForm::fileSave()
{
	if(INPUT_FILENAME_XML.isEmpty())
		return fileSaveAs();
	else{
		bool t=gt.rules.write(this, INPUT_FILENAME_XML);
		
		if(t){
			gt.rules.setModified(false);
		
			setCurrentFile(INPUT_FILENAME_XML);
	
			statusBar()->showMessage(tr("File saved"), STATUS_BAR_MILLISECONDS);
			return true;
		}
		else{
			return false;
		}
	}
}

void FetMainForm::on_fileSaveAction_triggered()
{
	fileSave();
}

void FetMainForm::on_dataInstitutionNameAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	InstitutionNameForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataCommentsAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	CommentsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataDaysAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	DaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataHoursAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	HoursForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataTeachersAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	TeachersForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataTeachersStatisticsAction_triggered()
{
	TeachersStatisticsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataSubjectsAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	SubjectsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataSubjectsStatisticsAction_triggered()
{
	SubjectsStatisticsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataActivityTagsAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ActivityTagsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataYearsAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	YearsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataGroupsAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	GroupsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataSubgroupsAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	SubgroupsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataStudentsStatisticsAction_triggered()
{
	StudentsStatisticsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataActivitiesRoomsStatisticsAction_triggered()
{
	ActivitiesRoomsStatisticsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataTeachersSubjectsQualificationsStatisticsAction_triggered()
{
	QHash<QString, Teacher*> teachersHash;
	
	foreach(Teacher* tch, gt.rules.teachersList)
		teachersHash.insert(tch->name, tch);
		
	bool unqualifiedExist=false;

	QString s=tr("The teachers who are not qualified to teach a certain activity (in activities order):");
	s+="\n\n";
	
	bool begin=true;

	foreach(Activity* act, gt.rules.activitiesList){
		bool alreadyAdded=false;
		QString subject=act->subjectName;
		foreach(QString teacher, act->teachersNames){
			Teacher* tch=teachersHash.value(teacher, NULL);
			assert(tch!=NULL);
			if(!tch->qualifiedSubjectsHash.contains(subject)){
				unqualifiedExist=true;
				if(!alreadyAdded){
					if(!begin)
						s+="\n";
					else
						begin=false;
					s+=tr("For activity: %1").arg(act->getDescription());
					s+="\n";
					alreadyAdded=true;
				}
				s+=QString(4, ' ');
				s+=tr("Teacher %1 is not qualified to teach subject %2.").arg(teacher).arg(subject);
				s+="\n";
			}
		}
	}

	if(!unqualifiedExist)
		s=tr("All the teachers are qualified to teach their activities.");
	s+="\n";

	LongTextMessageBox::largeInformation(this, tr("FET information"), s);
}

void FetMainForm::on_helpSettingsAction_triggered()
{
	QString s;
	
	s+=tr("Probably some settings which are more difficult to understand are these ones:");
	s+="\n\n";
	s+=tr("Option 'Divide html timetables with time-axis by days':"
	" This means simply that the html timetables of type 'time horizontal' or 'time vertical' (see the generated html timetables)"
	" should be or not divided according to the days.");
	s+=" ";
	s+=tr("If the 'time horizontal' or 'time vertical' html timetables are too large for you, then you might need to select this option");
	
	s+="\n\n";
	s+=tr("Option 'Print activities with same starting time in timetables': selecting it means that the html timetables will contain for"
	 " each slot all the activities which have the same starting time (fact specified by your constraints) as the activity(ies) which are normally shown in this slot."
	 " If you don't use constraints activities same starting time, this option has no effect for you.");
	 
	s+="\n\n";
	s+=tr("Seed of random number generator: please read the help in the dialog of this option");
	
	s+="\n\n";
	s+=tr("Interface - use colors: the places with colors in FET interface are in:");
	s+="\n";
	s+=" -";
	s+=tr("add/modify constraints break, not available, preferred starting times or time slots, occupy max time slots from selection or"
		" max simultaneous in selected time slots (the table cells will have green or red colors).");
	s+="\n";
	s+=" -";
	s+=tr("activities and subactivities dialogs, the inactive activities will have a distinctive background color");
	s+="\n";
	s+=" -";
	s+=tr("all time constraints and all space constraints dialogs, the inactive constraints will have a distinctive background color");
	s+="\n";
	s+=" -";
	s+=tr("group activities in initial order items, the inactive items will have a distinctive background color");
	s+="\n";
	s+=" -";
	s+=tr("the three timetable view dialogs");
	
	s+="\n\n";
	s+=tr("Enable activity tag max hours daily:");
	s+="\n";
	s+=tr("This will enable the menu for 4 constraints: teacher(s) or students (set) activity tag max hours daily. These 4 constraints are good, but not perfect and"
		" may bring slow down of generation or impossible timetables if used unproperly. Select only if you know what you're doing.");
	s+="\n\n";
	s+=tr("Enable students max gaps per day:");
	s+="\n";
	s+=tr("This will enable the menu for 2 constraints: students (set) max gaps per day. These 2 constraints are good, but not perfect and"
		" may bring slow down of generation or impossible timetables if used unproperly. Select only if you know what you're doing.");
		
	s+="\n\n";
	s+=tr("Warn if using not perfect constraints:", "this is a warning if user uses not perfect constraints");
	s+="\n";
	s+=tr("If you use a not perfect constraint (activity tag max hours daily or students max gaps per day), you'll get a warning before generating"
		". Uncheck this option to get rid of that warning (it is recommended to keep the warning).");
	
	s+="\n\n";
	s+=tr("Enable students min hours daily with empty days:");
	s+="\n";
	s+=tr("This will enable you to modify the students min hours daily constraints, to allow empty days. It is IMPERATIVE (for performance reasons) to allow empty days for students min hours daily only"
		" if your institution allows empty days for students and if a solution with empty days is possible. Select only if you know what you're doing.");
		
	s+="\n\n";
	s+=tr("Warn if using students min hours daily with empty days:", "this is a warning if user has constraints min hours daily for students with allowed empty days");
	s+="\n";
	s+=tr("If you use constraints students min hours daily with allowed empty days, you'll get a warning before generating"
		". Uncheck this option to get rid of that warning (it is recommended to keep the warning).");
		
	s+="\n\n";
	
	s+=tr("Confirmations: unselect the corresponding check boxes if you want to skip introduction and confirmation to various advanced dialogs.");
	
	s+="\n\n";
	
	s+=tr("Duplicate vertical headers to the right (in timetable settings) - select this if you want the timetables to duplicate the table left vertical headers to the right"
		" part of the tables");
		
	s+="\n\n";
	
	s+=tr("If you have many subgroups and you don't explicitly use them, it is recommended to use the three global settings: hide subgroups"
		" in combo boxes, hide subgroups in activity planning, and do not write subgroups timetables on hard disk.");
	s+="\n";
	s+=tr("Note that using the global menu setting to hide subgroups in activity planning is a different thing from the check box in the activity"
		" planning dialog, and the global setting works better and faster.");
	s+="\n";
	s+=tr("If you hide subgroups in combo boxes, the affected dialogs (like the activities dialog or the add constraint students set not available times dialog)"
		" will load much faster if you have many subgroups.");
		
	s+="\n\n";
	
	s+=tr("If you are only working on a timetable, and you do not need to publish it, you may want to disable writing some categories of timetables"
		" on the hard disk, for operativity (the generation speed is not affected, only the overhead to write the partial/complete timetables"
		" when stopping/finishing the simulation). The timetables taking the longest time are the subgroups, groups AND years ones.");
	s+=" ";
	s+=tr("(Also the conflicts timetable might take long to write, if the file is large.)");
	s+=" ";
	s+=tr("After that, you can re-enable writing of the timetables and re-generate.");

	s+="\n\n";
	s+=tr("Show tool tips for constraints with tables: in the add/modify constraint dialogs which use tables, like the 'not available times' ones,"
		" each table cell will have a tool tip to show the current day/hour (column/row name).");
	
	LongTextMessageBox::largeInformation(this, tr("FET information"), s);
}

void FetMainForm::on_dataHelpOnStatisticsAction_triggered()
{
	QString s;
	
	s+=tr("You will find in the statistics only active activities count. The inactive ones are not counted.");
	
	s+="\n\n";
	
	s+=tr("Statistics for students might be the most difficult to understand."
	 " If you are using divisions of years: probably the most relevant statistics"
	 " are the ones for each subgroup (so you may check only subgroups check box)."
	 " You may see more hours for the years or groups, but these are not significant, please ignore them,"
	 " because each year or group will count also activities of all contained subgroups."
	 "\n\n"
	 "Each subgroup should have a number of hours per week close to the average of"
	 " all subgroups and close to the normal number of working hours of each students set."
	 " If a subgroup has a much lower value, maybe you used incorrectly"
	 " the years/groups/subgroups for activities."
	 "\n\n"
	 "Please read FAQ for detailed description"
	 " on how divisions work. The key is that the subgroups are independent and represent the smallest unit of students."
	 " Each subgroup receives the activities of the parent year and parent group and of itself."
	 "\n\n"
	 "Having a subgroup with too little working hours per week means that you inputted activities in a wrong manner,"
	 " and also that some constraints like no gaps, early or min hours daily for this subgroup"
	 " are interpreted in a wrong manner (if subgroup has only 2 activities, then these must"
	 " be placed in the first hours, which is too hard and wrong)."
	 );
	
	s+="\n\n";
	s+=tr("Students' statistics form contains a check box named '%1'"
	 ". This has effect only if you have overlapping groups/years, and means that FET will show the complete tree structure"
	 ", even if that means that some subgroups/groups will appear twice or more in the table, with the same information."
	 " For instance, if you have year Y1, groups G1 and G2, subgroups S1, S2, S3, with structure: Y1 (G1 (S1, S2), G2 (S1, S3)),"
	 " S1 will appear twice in the table").arg(tr("Show duplicates"));

	s+="\n\n";
	s+=tr("Activities rooms statistics: this menu will show the activities which may be scheduled in an unspecified room,"
	 " if they are referred to by space constraints with weight under 100.0%, and the activities which will certainly be scheduled in an"
	 " unspecified room, if they are not referred to by any space constraints. Remember that home rooms constraints are effective only"
	 " on activities which have only the exact specified teacher or students set (activities with more teachers or students"
	 " sets are not affected by home rooms constraints, you need to have preferred rooms constraints for such activities to ensure"
	 " they will not end up in an unspecified room).");
	
	LongTextMessageBox::largeInformation(this, tr("FET - information about statistics"), s);
}

void FetMainForm::on_dataActivitiesAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ActivitiesForm form(this, "", "", "", "");
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataSubactivitiesAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	SubactivitiesForm form(this, "", "", "", "");
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataRoomsAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	RoomsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataBuildingsAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	BuildingsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::openConstraintDialog(int dialogIdx)
{
	assert(dialogIdx >= 0 && dialogIdx < ConstraintDialogHelper::numConstraintActions);

	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	QDialog *form = ConstraintDialogHelper::constraintActions[dialogIdx].constructor(this);
	setParentAndOtherThings(form, this);
	form->exec();
	delete form;
}

void FetMainForm::openConstraintDialog(const QString& name)
{
	for (int i=0; i < ConstraintDialogHelper::numConstraintActions; i++) {
		if (ConstraintDialogHelper::constraintActions[i].name == name) {
			openConstraintDialog(i);
			return;
		}
	}
	if (VERBOSE)
		cerr << "Constraint Dialog not found: " << name.toStdString() << endl;
}

void FetMainForm::on_dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	
	if(!ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintTeachersActivityTagMaxHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	if(!ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintTeacherActivityTagMaxHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_groupActivitiesInInitialOrderAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	if(!ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER){
		QString s=tr("Feature is not enabled. To use this type of feature you must enable it from the Settings->Advanced menu.");
		s+="\n\n";
		s+=tr("Explanation:");
		s+=" ";
		s+=tr("The feature must be used with caution, by experienced users.");

		QMessageBox::information(this, tr("FET information"), s);

		return;
	}

	GroupActivitiesInInitialOrderItemsForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsSetMaxGapsPerDayAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	if(!ENABLE_STUDENTS_MAX_GAPS_PER_DAY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintStudentsSetMaxGapsPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsMaxGapsPerDayAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	if(!ENABLE_STUDENTS_MAX_GAPS_PER_DAY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintStudentsMaxGapsPerDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	if(!ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintStudentsSetActivityTagMaxHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	if(!ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY){
		enableNotPerfectMessage();
		return;
	}

	ConstraintStudentsActivityTagMaxHoursDailyForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_helpAboutAction_triggered()
{
	HelpAboutForm* form=new HelpAboutForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::on_helpHomepageAction_triggered()
{
	bool tds=QDesktopServices::openUrl(QUrl("https://lalescu.ro/liviu/fet/"));

	if(!tds){
		QMessageBox::warning(this, tr("FET warning"), tr("Could not start the default internet browser (trying to open the link %1)."
		" Maybe you can try to manually start your browser and open this link.").arg("https://lalescu.ro/liviu/fet/"));
	}
}

void FetMainForm::on_helpContentsAction_triggered()
{
	bool tds=QDesktopServices::openUrl(QUrl("https://lalescu.ro/liviu/fet/doc/"));

	if(!tds){
		QMessageBox::warning(this, tr("FET warning"), tr("Could not start the default internet browser (trying to open the link %1)."
		" Maybe you can try to manually start your browser and open this link.").arg("https://lalescu.ro/liviu/fet/doc/"));
	}
}

void FetMainForm::on_helpForumAction_triggered()
{
	bool tds=QDesktopServices::openUrl(QUrl("https://lalescu.ro/liviu/fet/forum/"));

	if(!tds){
		QMessageBox::warning(this, tr("FET warning"), tr("Could not start the default internet browser (trying to open the link %1)."
		" Maybe you can try to manually start your browser and open this link.").arg("https://lalescu.ro/liviu/fet/forum/"));
	}
}

void FetMainForm::on_helpAddressesAction_triggered()
{
	QString s="";
	s+=tr("In case the Help/Online menus do not function, please write down these addresses and open them in an internet browser:");
	s+="\n\n";
	s+=tr("FET homepage: %1", "%1 is FET homepage, begins with https://...").arg("https://lalescu.ro/liviu/fet/");
	s+="\n";
	s+=tr("Documentation (online help contents): %1", "%1 is web page of FET Doc(umentation), which is the contents of the online help, it begins with https://...").arg("https://lalescu.ro/liviu/fet/doc/");
	s+="\n";
	s+=tr("Forum: %1", "%1 is web page of FET forum, begins with https://...").arg("https://lalescu.ro/liviu/fet/forum/");
	s+="\n\n";
	s+=tr("Additionally, you may find on the FET homepage other contact information.");
	s+="\n\n";
	s+=tr("In case these addresses do not function, maybe the FET webpage has temporary problems, so try again later. Or maybe the FET webpage has changed, so search for the new page on the internet.");

	LongTextMessageBox::largeInformation(this, tr("FET web addresses"), s);
}

void FetMainForm::on_helpFAQAction_triggered()
{
	HelpFaqForm* form=new HelpFaqForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::on_helpTipsAction_triggered()
{
	HelpTipsForm* form=new HelpTipsForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::on_helpInstructionsAction_triggered()
{
	HelpInstructionsForm* form=new HelpInstructionsForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::on_timetableGenerateAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	int count=0;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		if(act->active)
			count++;
	}
	if(count<1){
		QMessageBox::information(this, tr("FET information"), tr("Please input at least one active activity before generating"));
		return;
	}
	TimetableGenerateForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
	
	LockUnlock::increaseCommunicationSpinBox();
}

void FetMainForm::on_timetableGenerateMultipleAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	if(INPUT_FILENAME_XML.isEmpty()){
		QMessageBox::information(this, tr("FET information"),
			tr("Your current data has no name. Please save it as a file with a certain name before proceeding."));
		return;
	}

	int count=0;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		if(act->active)
			count++;
	}
	if(count<1){
		QMessageBox::information(this, tr("FET information"), tr("Please input at least one active activity before generating multiple"));
		return;
	}
	TimetableGenerateMultipleForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	LockUnlock::increaseCommunicationSpinBox();
}

void FetMainForm::on_timetableViewStudentsDaysHorizontalAction_triggered()
{
	if(!CachedSchedule::isValid()){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	TimetableViewStudentsDaysHorizontalForm *form=new TimetableViewStudentsDaysHorizontalForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
	form->resizeRowsAfterShow();
}

void FetMainForm::on_timetableViewStudentsTimeHorizontalAction_triggered()
{
	if(!CachedSchedule::isValid()){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	TimetableViewStudentsTimeHorizontalForm *form=new TimetableViewStudentsTimeHorizontalForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
	form->resizeRowsAfterShow();
}

void FetMainForm::on_timetableViewTeachersDaysHorizontalAction_triggered()
{
	if(!CachedSchedule::isValid()){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}
	if(gt.rules.nInternalTeachers!=gt.rules.teachersList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some teachers. Please regenerate the timetable and then view it"));
		return;
	}
	
	TimetableViewTeachersDaysHorizontalForm *form=new TimetableViewTeachersDaysHorizontalForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
	form->resizeRowsAfterShow();
}

void FetMainForm::on_timetableViewTeachersTimeHorizontalAction_triggered()
{
	if(!CachedSchedule::isValid()){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}
	if(gt.rules.nInternalTeachers!=gt.rules.teachersList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some teachers. Please regenerate the timetable and then view it"));
		return;
	}
	
	TimetableViewTeachersTimeHorizontalForm *form=new TimetableViewTeachersTimeHorizontalForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
	form->resizeRowsAfterShow();
}

void FetMainForm::on_timetableShowConflictsAction_triggered()
{
	if(!CachedSchedule::isValid()){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	TimetableShowConflictsForm *form=new TimetableShowConflictsForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
}

void FetMainForm::on_timetableViewRoomsDaysHorizontalAction_triggered()
{
	if(!CachedSchedule::isValid()){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	TimetableViewRoomsDaysHorizontalForm* form=new TimetableViewRoomsDaysHorizontalForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
	form->resizeRowsAfterShow();
}

void FetMainForm::on_timetableViewRoomsTimeHorizontalAction_triggered()
{
	if(!CachedSchedule::isValid()){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	TimetableViewRoomsTimeHorizontalForm* form=new TimetableViewRoomsTimeHorizontalForm(this);
	form->setWindowFlags(Qt::Window);
	form->setAttribute(Qt::WA_DeleteOnClose);
	forceCenterWidgetOnScreen(form);
	restoreFETDialogGeometry(form);
	form->show();
	form->resizeRowsAfterShow();
}

void FetMainForm::on_timetablePrintAction_triggered()
{
	if(!CachedSchedule::isValid()){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}
	if(gt.rules.nInternalTeachers!=gt.rules.teachersList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some teachers. Please regenerate the timetable and then view it"));
		return;
	}
	
	StartTimetablePrint::startTimetablePrint(this);
}

void FetMainForm::on_statisticsPrintAction_triggered()
{
	StartStatisticsPrint::startStatisticsPrint(this);
}

void FetMainForm::on_timetableLockAllActivitiesAction_triggered()
{
	if(!CachedSchedule::isValid()){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	AdvancedLockUnlockForm::lockAll(this);
}

void FetMainForm::on_timetableUnlockAllActivitiesAction_triggered()
{
	if(!CachedSchedule::isValid()){
		//QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		QMessageBox::information(this, tr("FET information"), tr("The timetable is not generated, but anyway FET will proceed now"));

		AdvancedLockUnlockForm::unlockAllWithoutTimetable(this);
	
		return;
	}

	AdvancedLockUnlockForm::unlockAll(this);
}

void FetMainForm::on_timetableLockActivitiesDayAction_triggered()
{
	if(!CachedSchedule::isValid()){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	AdvancedLockUnlockForm::lockDay(this);
}

void FetMainForm::on_timetableUnlockActivitiesDayAction_triggered()
{
	if(!CachedSchedule::isValid()){
		//QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		QMessageBox::information(this, tr("FET information"), tr("The timetable is not generated, but anyway FET will proceed now"));
		
		AdvancedLockUnlockForm::unlockDayWithoutTimetable(this);
		
		return;
	}

	AdvancedLockUnlockForm::unlockDay(this);
}

void FetMainForm::on_timetableLockActivitiesEndStudentsDayAction_triggered()
{
	if(!CachedSchedule::isValid()){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	AdvancedLockUnlockForm::lockEndStudentsDay(this);
}

void FetMainForm::on_timetableUnlockActivitiesEndStudentsDayAction_triggered()
{
	if(!CachedSchedule::isValid()){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	AdvancedLockUnlockForm::unlockEndStudentsDay(this);
}

void FetMainForm::on_languageAction_triggered()
{
	QDialog dialog(this);
	dialog.setWindowTitle(tr("Please select FET language"));
	
	QVBoxLayout* taMainLayout=new QVBoxLayout(&dialog);

	QPushButton* tapb1=new QPushButton(tr("Cancel"));
	QPushButton* tapb2=new QPushButton(tr("OK"));
				
	QHBoxLayout* buttons=new QHBoxLayout();
	buttons->addStretch();
	buttons->addWidget(tapb1);
	buttons->addWidget(tapb2);
	
	QComboBox* languagesComboBox=new QComboBox();
	
	QSize tmp=languagesComboBox->minimumSizeHint();
	Q_UNUSED(tmp);
	
	QMap<QString, QString> languagesMap;
	populateLanguagesMap(languagesMap);
	
	//assert(languagesMap.count()==N_LANGUAGES);
	
	QMapIterator<QString, QString> it(languagesMap);
	int i=0;
	int j=-1;
	int eng=-1;
	while(it.hasNext()){
		it.next();
		//: Locale_Code (Language_Name). Ex.: en_US (US English)
		QString text = tr("%1 (%2)").arg(it.key()).arg(tr(it.value().toStdString().c_str()));
		languagesComboBox->addItem(text, it.key());
		if(it.key()==FET_LANGUAGE)
			j=i;
		if(it.key()=="en_US")
			eng=i;
		i++;
	}
	assert(eng>=0);
	if(j==-1){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid current language - making it en_US (US English)"));
		FET_LANGUAGE="en_US";
		j=eng;
	}
	languagesComboBox->setCurrentIndex(j);
	
	QLabel* label=new QLabel(tr("Please select FET language"));
	
	QHBoxLayout* languagesLayout=new QHBoxLayout();
	languagesLayout->addWidget(languagesComboBox);
	//languagesLayout->addStretch();
	
	taMainLayout->addStretch();
	taMainLayout->addWidget(label);
	//taMainLayout->addWidget(languagesComboBox);
	taMainLayout->addLayout(languagesLayout);
	taMainLayout->addStretch();
	taMainLayout->addLayout(buttons);

	QObject::connect(tapb2, SIGNAL(clicked()), &dialog, SLOT(accept()));
	QObject::connect(tapb1, SIGNAL(clicked()), &dialog, SLOT(reject()));
	
	tapb2->setDefault(true);
	tapb2->setFocus();
	
	const QString settingsName=QString("LanguageSelectionForm");

	int w=dialog.sizeHint().width();
	if(w<350)
		w=350;
	int h=dialog.sizeHint().height();
	if(h<180)
		h=180;
	dialog.resize(w,h);
	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, settingsName);
	
	setParentAndOtherThings(&dialog, this);
	bool ok=dialog.exec();
	saveFETDialogGeometry(&dialog, settingsName);
	if(!ok)
		return;
		
	FET_LANGUAGE = languagesComboBox->currentData().toString();
	
	setLanguage(this);
	setCurrentFile(INPUT_FILENAME_XML);
}

void FetMainForm::on_settingsRestoreDefaultsAction_triggered()
{
	QString default_working_directory="examples";
	QDir d2(default_working_directory);
	if(!d2.exists())
		default_working_directory=QDir::homePath();
	else
		default_working_directory=d2.absolutePath();

	QString s=tr("Are you sure you want to reset all settings to defaults?");
	s+="\n\n";
	
	s+=tr("That means:");
	s+="\n";

	s+=tr("1")+QString(". ")+tr("The geometry and any other saved settings of all the windows and dialogs will be reset to default");
	s+="\n";

	s+=tr("2")+QString(". ")+tr("Show shortcut buttons in main window will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";

	s+=tr("3")+QString(". ")+tr("In the shortcuts tab from the main window, the first section will be selected/shown", "Option refers to the main window tab widget for shortcuts, which currently contains 5 tabs: File, Data, "
		"Time, Space, Timetable (so it will select/show File tab).");
	s+="\n";

	s+=tr("4")+QString(". ")+tr("Check for updates at startup will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("5")+QString(". ")+tr("Use colors in FET graphical user interface will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

#ifndef USE_SYSTEM_LOCALE
	s+=tr("6")+QString(". ")+tr("Language will be %1", "%1 is the default language").arg(QString("en_US")+QString(" (")+tr("US English")+QString(")"));
#else
	QMap<QString, QString> languagesMap;
	populateLanguagesMap(languagesMap);

	QString NEW_FET_LANGUAGE=QLocale::system().name();
	
	bool ok=false;
	QMapIterator<QString, QString> i(languagesMap);
	while(i.hasNext()){
		i.next();
		if(NEW_FET_LANGUAGE.left(i.key().length())==i.key()){
			NEW_FET_LANGUAGE=i.key();
			ok=true;
			break;
		}
	}
	if(!ok)
		NEW_FET_LANGUAGE="en_US";
		
	s+=tr("6")+QString(". ")+tr("Language will be %1", "%1 is the default language").arg(tr("%1 (%2)").arg(NEW_FET_LANGUAGE).arg(languagesMap().value(NEW_FET_LANGUAGE)));
#endif
	s+="\n";

	s+=tr("7")+QString(". ")+tr("The list of recently used files will be cleared");
	s+="\n";
	
	s+=tr("8")+QString(". ")+tr("Working directory will be %1", "%1 is the directory").arg(QDir::toNativeSeparators(default_working_directory));
	s+="\n";

	s+=tr("9")+QString(". ")+tr("Output directory will be %1", "%1 is the directory").arg(QDir::toNativeSeparators(QDir::homePath()+FILE_SEP+"fet-results"));
	s+="\n";

	s+=tr("10")+QString(". ")+tr("Import directory will be %1", "%1 is the directory").arg(QDir::toNativeSeparators(QDir::homePath()+FILE_SEP+"fet-results"));
	s+="\n";

	s+=tr("11")+QString(". ")+tr("Html level of the timetables will be %1", "%1 is default html level").arg(2);
	s+="\n";

	s+=tr("12")+QString(". ")+tr("Mark not available slots with -x- in timetables will be %1", "%1 is true or false. Lowercase -x-").arg(tr("true"));
	s+="\n";

	s+=tr("13")+QString(". ")+tr("Mark break slots with -X- in timetables will be %1", "%1 is true or false. Uppercase -X-").arg(tr("true"));
	s+="\n";

	s+=tr("14")+QString(". ")+tr("Divide html timetables with time-axis by days will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("15")+QString(". ")+tr("Duplicate vertical headers to the right will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("16")+QString(". ")+tr("Print activities with same starting time will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("17")+QString(". ")+tr("Print activities tags will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";

	s+=tr("18")+QString(". ")+tr("Enable activity tag max hours daily will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("19")+QString(". ")+tr("Enable students max gaps per day will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("20")+QString(". ")+tr("Warn if using not perfect constraints will be %1", "%1 is true or false. This is a warning if user uses not perfect constraints").arg(tr("true"));
	s+="\n";

	s+=tr("21")+QString(". ")+tr("Enable constraints students min hours daily with empty days will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";

	s+=tr("22")+QString(". ")+tr("Warn if using constraints students min hours daily with empty days will be %1", "%1 is true or false. This is a warning if user uses a nonstandard constraint"
		" students min hours daily with allowed empty days").arg(tr("true"));
	s+="\n";

	///////////////confirmations
	s+=tr("23")+QString(". ")+tr("Confirm activity planning will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("24")+QString(". ")+tr("Confirm spread activities over the week will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("25")+QString(". ")+tr("Confirm remove redundant constraints will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("26")+QString(". ")+tr("Confirm save data and timetable as will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	///////////////
	
	s+=tr("27")+QString(". ")+tr("Enable group activities in the initial order of generation will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";
	s+=tr("28")+QString(". ")+tr("Warn if using group activities in the initial order of generation will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	///////////////

	s+=tr("29")+QString(". ")+tr("Show subgroups in combo boxes will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("30")+QString(". ")+tr("Show subgroups in activity planning will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	///////////////

	s+=tr("31")+QString(". ")+tr("Write on disk the %1 timetable will be %2", "%1 is a category of timetables, like conflicts, %2 is true or false")
	 .arg(tr("conflicts")).arg(tr("true"));
	s+="\n";

	s+=tr("32")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("statistics")).arg(tr("true"));
	s+="\n";
	s+=tr("33")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("XML")).arg(tr("true"));
	s+="\n";
	s+=tr("34")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("days horizontal")).arg(tr("true"));
	s+="\n";
	s+=tr("35")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("days vertical")).arg(tr("true"));
	s+="\n";
	s+=tr("36")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("time horizontal")).arg(tr("true"));
	s+="\n";
	s+=tr("37")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("time vertical")).arg(tr("true"));
	s+="\n";

	s+=tr("38")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("subgroups")).arg(tr("true"));
	s+="\n";
	s+=tr("39")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("groups")).arg(tr("true"));
	s+="\n";
	s+=tr("40")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("years")).arg(tr("true"));
	s+="\n";
	s+=tr("41")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("teachers")).arg(tr("true"));
	s+="\n";
	s+=tr("42")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("teachers free periods")).arg(tr("true"));
	s+="\n";
	s+=tr("43")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("rooms")).arg(tr("true"));
	s+="\n";
	s+=tr("44")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("subjects")).arg(tr("true"));
	s+="\n";
	s+=tr("45")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("activity tags")).arg(tr("true"));
	s+="\n";
	s+=tr("46")+QString(". ")+tr("Write on disk the %1 timetables will be %2", "%1 is a category of timetables, like XML or subgroups timetables, %2 is true or false")
	 .arg(tr("activities")).arg(tr("true"));
	s+="\n";
	s+=tr("47")+QString(". ")+tr("Show tool tips for constraints with tables will be %1", "%1 is true or false").arg(tr("false"));
	s+="\n";
	s+=tr("48")+QString(". ")+tr("Show warning for subgroups with the same activities will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("49")+QString(". ")+tr("Print detailed timetables will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	s+=tr("50")+QString(". ")+tr("Print detailed teachers' free periods timetables will be %1", "%1 is true or false").arg(tr("true"));
	s+="\n";
	
	switch( LongTextMessageBox::largeConfirmation( this, tr("FET confirmation"), s,
	 tr("&Yes"), tr("&No"), QString(), 0 , 1 ) ) {
	case 0: // Yes
		break;
	case 1: // No
		return;
	}

	QSettings settings;
	settings.clear();
	
	recentFiles.clear();
	updateRecentFileActions();

	resize(originalWindowSize);
	forceCenterWidgetOnScreen(this);
	
#ifndef USE_SYSTEM_LOCALE
	FET_LANGUAGE=QString("en_US");
#else
	FET_LANGUAGE=NEW_FET_LANGUAGE;
#endif
	
	checkForUpdatesAction->setChecked(false);
	checkForUpdates=false;
	
	tabWidget->setCurrentIndex(0);
	
	SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES=false;
	settingsShowToolTipsForConstraintsWithTablesAction->setChecked(SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES);
	
	USE_GUI_COLORS=false;
	settingsUseColorsAction->setChecked(USE_GUI_COLORS);
	
	SHOW_SUBGROUPS_IN_COMBO_BOXES=true;
	settingsShowSubgroupsInComboBoxesAction->setChecked(SHOW_SUBGROUPS_IN_COMBO_BOXES);
	
	SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING=true;
	settingsShowSubgroupsInActivityPlanningAction->setChecked(SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING);

	///
	WRITE_TIMETABLE_CONFLICTS=true;

	WRITE_TIMETABLES_STATISTICS=true;
	WRITE_TIMETABLES_XML=true;
	WRITE_TIMETABLES_DAYS_HORIZONTAL=true;
	WRITE_TIMETABLES_DAYS_VERTICAL=true;
	WRITE_TIMETABLES_TIME_HORIZONTAL=true;
	WRITE_TIMETABLES_TIME_VERTICAL=true;

	WRITE_TIMETABLES_SUBGROUPS=true;
	WRITE_TIMETABLES_GROUPS=true;
	WRITE_TIMETABLES_YEARS=true;
	WRITE_TIMETABLES_TEACHERS=true;
	WRITE_TIMETABLES_TEACHERS_FREE_PERIODS=true;
	WRITE_TIMETABLES_ROOMS=true;
	WRITE_TIMETABLES_SUBJECTS=true;
	WRITE_TIMETABLES_ACTIVITY_TAGS=true;
	WRITE_TIMETABLES_ACTIVITIES=true;
	//
	
	resetSettings();

	///////////
	SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES=true;
	showWarningForSubgroupsWithTheSameActivitiesAction->setChecked(SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES);
	
	ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY=false;
	enableActivityTagMaxHoursDailyAction->setChecked(ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);

	ENABLE_STUDENTS_MAX_GAPS_PER_DAY=false;
	enableStudentsMaxGapsPerDayAction->setChecked(ENABLE_STUDENTS_MAX_GAPS_PER_DAY);
	
	SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS=true;
	showWarningForNotPerfectConstraintsAction->setChecked(SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS);
	
	ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER=false;
	SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER=true;
	enableGroupActivitiesInInitialOrderAction->setChecked(ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER);
	showWarningForGroupActivitiesInInitialOrderAction->setChecked(SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER);
	
	setEnabledIcon(dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);

	setEnabledIcon(dataTimeConstraintsStudentsSetMaxGapsPerDayAction, ENABLE_STUDENTS_MAX_GAPS_PER_DAY);
	setEnabledIcon(dataTimeConstraintsStudentsMaxGapsPerDayAction, ENABLE_STUDENTS_MAX_GAPS_PER_DAY);

	setEnabledIcon(groupActivitiesInInitialOrderAction, ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER);

	ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=false;
	enableStudentsMinHoursDailyWithAllowEmptyDaysAction->setChecked(ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS);

	SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=true;
	showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction->setChecked(SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS);

	///////////
	
	settingsDivideTimetablesByDaysAction->setChecked(false);
	DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=false;
	
	settingsDuplicateVerticalNamesAction->setChecked(false);
	TIMETABLE_HTML_REPEAT_NAMES=false;
	
	WORKING_DIRECTORY=default_working_directory;
	
	OUTPUT_DIR=QDir::homePath()+FILE_SEP+"fet-results";
	QDir dir;
	if(!dir.exists(OUTPUT_DIR))
		dir.mkpath(OUTPUT_DIR);
	IMPORT_DIRECTORY=OUTPUT_DIR;
	
	TIMETABLE_HTML_LEVEL=2;
	
	settingsPrintActivityTagsAction->setChecked(true);
	TIMETABLE_HTML_PRINT_ACTIVITY_TAGS=true;
	
	settingsPrintDetailedTimetablesAction->setChecked(true);
	PRINT_DETAILED_HTML_TIMETABLES=true;
	settingsPrintDetailedTeachersFreePeriodsTimetablesAction->setChecked(true);
	PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS=true;

	settingsPrintNotAvailableSlotsAction->setChecked(true);
	PRINT_NOT_AVAILABLE_TIME_SLOTS=true;

	settingsPrintBreakSlotsAction->setChecked(true);
	PRINT_BREAK_TIME_SLOTS=true;

	settingsPrintActivitiesWithSameStartingTimeAction->setChecked(false);
	PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME=false;

	setLanguage(this);
	setCurrentFile(INPUT_FILENAME_XML);

	LockUnlock::increaseCommunicationSpinBox(); //for GUI colors in timetables
}

void FetMainForm::on_settingsTimetableHtmlLevelAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	SettingsTimetableHtmlLevelForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_settingsPrintActivityTagsAction_toggled()
{
	TIMETABLE_HTML_PRINT_ACTIVITY_TAGS=settingsPrintActivityTagsAction->isChecked();
}

void FetMainForm::on_settingsPrintDetailedTimetablesAction_toggled()
{
	PRINT_DETAILED_HTML_TIMETABLES=settingsPrintDetailedTimetablesAction->isChecked();
}

void FetMainForm::on_settingsPrintDetailedTeachersFreePeriodsTimetablesAction_toggled()
{
	PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS=settingsPrintDetailedTeachersFreePeriodsTimetablesAction->isChecked();
}

void FetMainForm::on_settingsPrintNotAvailableSlotsAction_toggled()
{
	PRINT_NOT_AVAILABLE_TIME_SLOTS=settingsPrintNotAvailableSlotsAction->isChecked();
}

void FetMainForm::on_settingsPrintBreakSlotsAction_toggled()
{
	PRINT_BREAK_TIME_SLOTS=settingsPrintBreakSlotsAction->isChecked();
}

void FetMainForm::on_settingsPrintActivitiesWithSameStartingTimeAction_toggled()
{
	PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME=settingsPrintActivitiesWithSameStartingTimeAction->isChecked();
}

void FetMainForm::on_activityPlanningAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	if(settingsConfirmActivityPlanningAction->isChecked()){
		int confirm;
	
		ActivityPlanningConfirmationForm c_form(this);
		setParentAndOtherThings(&c_form, this);
		confirm=c_form.exec();

		if(confirm==QDialog::Accepted){
			if(c_form.dontShowAgain)
				settingsConfirmActivityPlanningAction->setChecked(false);
		} else {
			return;
		}
	}

	StartActivityPlanning::startActivityPlanning(this);
}

void FetMainForm::on_spreadActivitiesAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	
	if(gt.rules.nDaysPerWeek>=7){
		QString s;
		s=tr("You have more than 6 days per week, so probably you won't need this feature. Do you still want to continue?");
		
		int cfrm=0;
		switch( QMessageBox::question( this, tr("FET question"),
		 s,
		 tr("&Continue"), tr("&Cancel"), 0 , 1 ) ) {
		case 0: // Yes - continue
			cfrm=1;
			break;
		case 1: // No - cancel
			cfrm=0;
			break;
		}

		if(!cfrm){
			return;
		}
	}
	
	if(gt.rules.nDaysPerWeek<=4){
		QString s;
		s=tr("You have less than 5 days per week, so probably you won't need this feature. Do you still want to continue?");
		
		int cfrm=0;
		switch( QMessageBox::question( this, tr("FET question"),
		 s,
		 tr("&Continue"), tr("&Cancel"), 0 , 1 ) ) {
		case 0: // Yes - continue
			cfrm=1;
			break;
		case 1: // No - cancel
			cfrm=0;
			break;
		}

		if(!cfrm){
			return;
		}
	}
	
	if(settingsConfirmSpreadActivitiesAction->isChecked()){
		int confirm;
	
		SpreadConfirmationForm c_form(this);
		setParentAndOtherThings(&c_form, this);
		confirm=c_form.exec();

		if(confirm==QDialog::Accepted){
			if(c_form.dontShowAgain)
				settingsConfirmSpreadActivitiesAction->setChecked(false);
		} else {
			return;
		}
	}

	SpreadMinDaysConstraintsFiveDaysForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_statisticsExportToDiskAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	StatisticsExport::exportStatistics(this);
}

void FetMainForm::on_removeRedundantConstraintsAction_triggered()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	
	if(settingsConfirmRemoveRedundantAction->isChecked()){
		int confirm;
	
		RemoveRedundantConfirmationForm c_form(this);
		setParentAndOtherThings(&c_form, this);
		confirm=c_form.exec();

		if(confirm==QDialog::Accepted){
			if(c_form.dontShowAgain)
				settingsConfirmRemoveRedundantAction->setChecked(false);
		} else {
			return;
		}
	}

	RemoveRedundantForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();
}

void FetMainForm::on_selectOutputDirAction_triggered()
{
	QString od;
	
	od = QFileDialog::getExistingDirectory(this, tr("Choose results (output) directory"), OUTPUT_DIR);
	
	if(!od.isNull()){
		QFile test(od+FILE_SEP+"test_write_permissions_3.tmp");
		bool existedBefore=test.exists();
		bool t=test.open(QIODevice::ReadWrite);
		//if(!test.exists())
		//	t=false;
		if(!t){
			QMessageBox::warning(this, tr("FET warning"), tr("You don't have write permissions in this directory"));
			return;
		}
		test.close();
		if(!existedBefore)
			test.remove();
	
		OUTPUT_DIR=od;
	}
}

void FetMainForm::on_randomSeedAction_triggered()
{
	RandomSeedDialog dialog(this);
	
	setParentAndOtherThings(&dialog, this);
	int te=dialog.exec();
	
	if(te==QDialog::Accepted){
		int tx=dialog.lineEditX->text().toInt();
		if(!(tx>=1 && tx<RandomKnuth::getMM())){
			assert(0);
			//QMessageBox::warning(this, tr("FET warning"), tr("The random seed X component must be at least %1 and at most %2").arg(1).arg(MM-1));
			//return;
		}

		int ty=dialog.lineEditY->text().toInt();
		if(!(ty>=1 && ty<RandomKnuth::getMMM())){
			assert(0);
			//QMessageBox::warning(this, tr("FET warning"), tr("The random seed Y component must be at least %1 and at most %2").arg(1).arg(MMM-1));
			//return;
		}

		RandomKnuth::XX=tx;
		RandomKnuth::YY=ty;
	}
}

void FetMainForm::enableActivityTagMaxHoursDailyToggled(bool checked)
{
	if(checked==true){
		QString s=tr("These kinds of constraints are good, but not perfectly optimized. Adding such constraints may make your"
		 " timetable solve too slow or even impossible.");
		s+=" ";
		s+=tr("It is recommended to use such constraints only at the end, after you generated successfully with all the other constraints.");
		s+="\n\n";
		s+=tr("If your timetable is too difficult, it may be from these kinds of constraints, so you may need to remove them and retry.");
		s+="\n\n";
		s+=tr("Continue only if you know what you are doing.");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
	
		if(b!=QMessageBox::Ok){
			disconnect(enableActivityTagMaxHoursDailyAction, SIGNAL(toggled(bool)), this, SLOT(enableActivityTagMaxHoursDailyToggled(bool)));
			enableActivityTagMaxHoursDailyAction->setChecked(false);
			connect(enableActivityTagMaxHoursDailyAction, SIGNAL(toggled(bool)), this, SLOT(enableActivityTagMaxHoursDailyToggled(bool)));
			return;
		}
	}
	
	ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY=checked;

	setEnabledIcon(dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	setEnabledIcon(dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction, ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
}

void FetMainForm::enableStudentsMaxGapsPerDayToggled(bool checked)
{
	if(checked==true){
		QString s=tr("These kinds of constraints are good, but not perfectly optimized. Adding such constraints may make your"
		 " timetable solve too slow or even impossible.");
		s+=" ";
		s+=tr("It is recommended to use such constraints only at the end, after you generated successfully with all the other constraints.");
		s+="\n\n";
		s+=tr("If your timetable is too difficult, it may be from these kinds of constraints, so you may need to remove them and retry.");
		s+="\n\n";
		s+=tr("Continue only if you know what you are doing.");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
	
		if(b!=QMessageBox::Ok){
			disconnect(enableStudentsMaxGapsPerDayAction, SIGNAL(toggled(bool)), this, SLOT(enableStudentsMaxGapsPerDayToggled(bool)));
			enableStudentsMaxGapsPerDayAction->setChecked(false);
			connect(enableStudentsMaxGapsPerDayAction, SIGNAL(toggled(bool)), this, SLOT(enableStudentsMaxGapsPerDayToggled(bool)));
			return;
		}
	}
	
	ENABLE_STUDENTS_MAX_GAPS_PER_DAY=checked;
	
	setEnabledIcon(dataTimeConstraintsStudentsSetMaxGapsPerDayAction, ENABLE_STUDENTS_MAX_GAPS_PER_DAY);
	setEnabledIcon(dataTimeConstraintsStudentsMaxGapsPerDayAction, ENABLE_STUDENTS_MAX_GAPS_PER_DAY);
}

void FetMainForm::showWarningForSubgroupsWithTheSameActivitiesToggled(bool checked)
{
	if(checked==false){
		QString s=tr("It is recommended to keep this warning active, but if you really want, you can disable it.");
		s+="\n\n";
		s+=tr("Disable it only if you know what you are doing.");
		s+="\n\n";
		s+=tr("Are you sure you want to disable it?");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
	
		if(b!=QMessageBox::Yes){
			disconnect(showWarningForSubgroupsWithTheSameActivitiesAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForSubgroupsWithTheSameActivitiesToggled(bool)));
			showWarningForSubgroupsWithTheSameActivitiesAction->setChecked(true);
			connect(showWarningForSubgroupsWithTheSameActivitiesAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForSubgroupsWithTheSameActivitiesToggled(bool)));
			return;
		}
	}
	
	SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES=checked;
}

void FetMainForm::showWarningForNotPerfectConstraintsToggled(bool checked)
{
	if(checked==false){
		QString s=tr("It is recommended to keep this warning active, but if you really want, you can disable it.");
		s+="\n\n";
		s+=tr("Disable it only if you know what you are doing.");
		s+="\n\n";
		s+=tr("Are you sure you want to disable it?");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
	
		if(b!=QMessageBox::Yes){
			disconnect(showWarningForNotPerfectConstraintsAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForNotPerfectConstraintsToggled(bool)));
			showWarningForNotPerfectConstraintsAction->setChecked(true);
			connect(showWarningForNotPerfectConstraintsAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForNotPerfectConstraintsToggled(bool)));
			return;
		}
	}
	
	SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS=checked;
}


void FetMainForm::enableStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool checked)
{
	if(checked==true){
		QString s=tr("This kind of constraint is good, but only in the right case. Adding such constraints in the wrong circumstance may make your"
		 " timetable solve too slow or even impossible.");
		s+="\n\n";
		s+=tr("Please use such constraints only if you are sure that there exists a timetable with empty days for students. If your instution requires"
			" each day for the students to be not empty, or if there does not exist a solution with empty days for students, "
			"it is IMPERATIVE (for performance reasons) that you use the standard constraint which does not allow empty days."
			" Otherwise, the timetable may be impossible to find.");
		s+="\n\n";
		s+=tr("Continue only if you know what you are doing.");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
	
		if(b!=QMessageBox::Ok){
			disconnect(enableStudentsMinHoursDailyWithAllowEmptyDaysAction, SIGNAL(toggled(bool)), this, SLOT(enableStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool)));
			enableStudentsMinHoursDailyWithAllowEmptyDaysAction->setChecked(false);
			connect(enableStudentsMinHoursDailyWithAllowEmptyDaysAction, SIGNAL(toggled(bool)), this, SLOT(enableStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool)));
			return;
		}
	}
	
	ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=checked;
}

void FetMainForm::showWarningForStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool checked)
{
	if(checked==false){
		QString s=tr("It is recommended to keep this warning active, but if you really want, you can disable it.");
		s+="\n\n";
		s+=tr("Disable it only if you know what you are doing.");
		s+="\n\n";
		s+=tr("Are you sure you want to disable it?");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
	
		if(b!=QMessageBox::Yes){
			disconnect(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool)));
			showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction->setChecked(true);
			connect(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForStudentsMinHoursDailyWithAllowEmptyDaysToggled(bool)));
			return;
		}
	}
	
	SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=checked;
}

void FetMainForm::enableGroupActivitiesInInitialOrderToggled(bool checked)
{
	if(checked==true){
		QString s=tr("This kind of option is good, but only in the right case. Adding such an option in the wrong circumstance may make your"
		 " timetable solve too slow or even impossible.");
		s+="\n\n";
		s+=tr("Continue only if you know what you are doing.");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
	
		if(b!=QMessageBox::Ok){
			disconnect(enableGroupActivitiesInInitialOrderAction, SIGNAL(toggled(bool)), this, SLOT(enableGroupActivitiesInInitialOrderToggled(bool)));
			enableGroupActivitiesInInitialOrderAction->setChecked(false);
			connect(enableGroupActivitiesInInitialOrderAction, SIGNAL(toggled(bool)), this, SLOT(enableGroupActivitiesInInitialOrderToggled(bool)));
			return;
		}
	}

	ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER=checked;

	setEnabledIcon(groupActivitiesInInitialOrderAction, ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER);
}

void FetMainForm::showWarningForGroupActivitiesInInitialOrderToggled(bool checked)
{
	if(checked==false){
		QString s=tr("It is recommended to keep this warning active, but if you really want, you can disable it.");
		s+="\n\n";
		s+=tr("Disable it only if you know what you are doing.");
		s+="\n\n";
		s+=tr("Are you sure you want to disable it?");
	
		QMessageBox::StandardButton b=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
	
		if(b!=QMessageBox::Yes){
			disconnect(showWarningForGroupActivitiesInInitialOrderAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForGroupActivitiesInInitialOrderToggled(bool)));
			showWarningForGroupActivitiesInInitialOrderAction->setChecked(true);
			connect(showWarningForGroupActivitiesInInitialOrderAction, SIGNAL(toggled(bool)), this, SLOT(showWarningForGroupActivitiesInInitialOrderToggled(bool)));
			return;
		}
	}
	
	SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER=checked;
}


//time constraints
void FetMainForm::on_shortcutAllTimeConstraintsPushButton_clicked()
{
	openConstraintDialog("AllTimeConstraints");
}

void FetMainForm::on_shortcutAdvancedTimeConstraintsPushButton_clicked()
{
	shortcutAdvancedTimeMenu->popup(QCursor::pos());
}

void FetMainForm::on_shortcutBreakTimeConstraintsPushButton_clicked()
{
	openConstraintDialog("TimeConstraintsBreakTimes");
}

void FetMainForm::on_shortcutTeachersTimeConstraintsPushButton_clicked()
{
	menuTeachers_time_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutStudentsTimeConstraintsPushButton_clicked()
{
	menuStudents_time_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutActivitiesTimeConstraintsPushButton_clicked()
{
	menuActivities_time_constraints->popup(QCursor::pos());
}


//space constraints
void FetMainForm::on_shortcutAllSpaceConstraintsPushButton_clicked()
{
	openConstraintDialog("AllSpaceConstraints");
}

void FetMainForm::on_shortcutRoomsSpaceConstraintsPushButton_clicked()
{
	menuRooms_space_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutTeachersSpaceConstraintsPushButton_clicked()
{
	menuTeachers_space_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutStudentsSpaceConstraintsPushButton_clicked()
{
	menuStudents_space_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutSubjectsSpaceConstraintsPushButton_clicked()
{
	menuSubjects_space_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutActivityTagsSpaceConstraintsPushButton_clicked()
{
	menuActivity_tags_space_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutSubjectsAndActivityTagsSpaceConstraintsPushButton_clicked()
{
	menuSubjects_and_activity_tags_space_constraints->popup(QCursor::pos());
}

void FetMainForm::on_shortcutActivitiesSpaceConstraintsPushButton_clicked()
{
	menuActivities_space_constraints->popup(QCursor::pos());
}

//timetable
void FetMainForm::on_shortcutGeneratePushButton_clicked()
{
	on_timetableGenerateAction_triggered();
}

void FetMainForm::on_shortcutGenerateMultiplePushButton_clicked()
{
	on_timetableGenerateMultipleAction_triggered();
}

void FetMainForm::on_shortcutViewTeachersPushButton_clicked()
{
	menuView_teachers->popup(QCursor::pos());
	//old
	//on_timetableViewTeachersAction_triggered();
}

void FetMainForm::on_shortcutViewStudentsPushButton_clicked()
{
	menuView_students->popup(QCursor::pos());
	//old
	//on_timetableViewStudentsAction_triggered();
}

void FetMainForm::on_shortcutViewRoomsPushButton_clicked()
{
	menuView_rooms->popup(QCursor::pos());
	//old
	//on_timetableViewRoomsAction_triggered();
}

void FetMainForm::on_shortcutShowSoftConflictsPushButton_clicked()
{
	on_timetableShowConflictsAction_triggered();
}

//2014-07-01
void FetMainForm::on_shortcutsTimetableAdvancedPushButton_clicked()
{
	shortcutTimetableAdvancedMenu->popup(QCursor::pos());
}

void FetMainForm::on_shortcutsTimetablePrintPushButton_clicked()
{
	on_timetablePrintAction_triggered();
}

void FetMainForm::on_shortcutsTimetableLockingPushButton_clicked()
{
	shortcutTimetableLockingMenu->popup(QCursor::pos());
}

//data shortcut
void FetMainForm::on_shortcutBasicPushButton_clicked()
{
	shortcutBasicMenu->popup(QCursor::pos());
}

void FetMainForm::on_shortcutSubjectsPushButton_clicked()
{
	on_dataSubjectsAction_triggered();
}

void FetMainForm::on_shortcutActivityTagsPushButton_clicked()
{
	on_dataActivityTagsAction_triggered();
}

void FetMainForm::on_shortcutTeachersPushButton_clicked()
{
	on_dataTeachersAction_triggered();
}

void FetMainForm::on_shortcutStudentsPushButton_clicked()
{
	menuStudents->popup(QCursor::pos());
}

void FetMainForm::on_shortcutActivitiesPushButton_clicked()
{
	on_dataActivitiesAction_triggered();
}

void FetMainForm::on_shortcutSubactivitiesPushButton_clicked()
{
	on_dataSubactivitiesAction_triggered();
}

void FetMainForm::on_shortcutDataSpacePushButton_clicked()
{
	shortcutDataSpaceMenu->popup(QCursor::pos());
}

void FetMainForm::on_shortcutDataAdvancedPushButton_clicked()
{
	shortcutDataAdvancedMenu->popup(QCursor::pos());
}

//file shortcut
void FetMainForm::on_shortcutNewPushButton_clicked()
{
	on_fileNewAction_triggered();
}

void FetMainForm::on_shortcutOpenPushButton_clicked()
{
	on_fileOpenAction_triggered();
}

void FetMainForm::on_shortcutOpenRecentPushButton_clicked()
{
	fileOpenRecentMenu->popup(QCursor::pos());
}

void FetMainForm::on_shortcutSavePushButton_clicked()
{
	on_fileSaveAction_triggered();
}

void FetMainForm::on_shortcutSaveAsPushButton_clicked()
{
	on_fileSaveAsAction_triggered();
}

void FetMainForm::rules_contentsChanged()
{
	setWindowModified(gt.rules.isModified());
}

void FetMainForm::rules_basicDataResized()
{
	// TODO Put this directly in Solution (connect Rules::contentsChanged signal internally and update these properties)
	CachedSchedule::invalidate();
}
