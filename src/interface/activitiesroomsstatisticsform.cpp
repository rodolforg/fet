/***************************************************************************
                          activitiesroomsstatisticform.cpp  -  description
                             -------------------
    begin                : October 6, 2011
    copyright            : (C) 2011 by Lalescu Liviu
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

#include "activitiesroomsstatisticsform.h"

#include "timetable_defs.h"
#include "timetable.h"

#include "fet.h"

#include <QString>
#include <QStringList>

#include <QSet>
#include <QList>
#include <QStringList>

ActivitiesRoomsStatisticsForm::ActivitiesRoomsStatisticsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closeButton->setDefault(true);

	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	possiblyPlainTextEdit->setReadOnly(true);
	certainlyPlainTextEdit->setReadOnly(true);
	
	QSet<int> activitiesWith100;
	QSet<int> activitiesWithUnder100;
	
	foreach(Activity* act, gt.rules.activitiesList){
		if(!act->active)
			continue;
	
		QSet<QString> currentActivityTagsSet=act->activityTagsNames.toSet();
	
		foreach(SpaceConstraint* genericConstraint, gt.rules.spaceConstraintsList){
			if(!genericConstraint->active)
				continue;
		
			if(genericConstraint->type==CONSTRAINT_STUDENTS_SET_HOME_ROOM){
				ConstraintStudentsSetHomeRoom* ctr=(ConstraintStudentsSetHomeRoom*)genericConstraint;
				
				if(act->studentsNames.count()==1){
					if(act->studentsNames.at(0)==ctr->studentsName){
						if(ctr->weightPercentage==100.0){
							if(!activitiesWith100.contains(act->id)){
								activitiesWith100.insert(act->id);
							}
						}
						else if(ctr->weightPercentage>=0.0 && ctr->weightPercentage<100.0){
							if(!activitiesWithUnder100.contains(act->id)){
								activitiesWithUnder100.insert(act->id);
							}
						}
					}
				}
			}

			else if(genericConstraint->type==CONSTRAINT_STUDENTS_SET_HOME_ROOMS){
				ConstraintStudentsSetHomeRooms* ctr=(ConstraintStudentsSetHomeRooms*)genericConstraint;
				
				if(act->studentsNames.count()==1){
					if(act->studentsNames.at(0)==ctr->studentsName){
						if(ctr->weightPercentage==100.0){
							if(!activitiesWith100.contains(act->id)){
								activitiesWith100.insert(act->id);
							}
						}
						else if(ctr->weightPercentage>=0.0 && ctr->weightPercentage<100.0){
							if(!activitiesWithUnder100.contains(act->id)){
								activitiesWithUnder100.insert(act->id);
							}
						}
					}
				}
			}

			else if(genericConstraint->type==CONSTRAINT_TEACHER_HOME_ROOM){
				ConstraintTeacherHomeRoom* ctr=(ConstraintTeacherHomeRoom*)genericConstraint;
				
				if(act->teachersNames.count()==1){
					if(act->teachersNames.at(0)==ctr->teacherName){
						if(ctr->weightPercentage==100.0){
							if(!activitiesWith100.contains(act->id)){
								activitiesWith100.insert(act->id);
							}
						}
						else if(ctr->weightPercentage>=0.0 && ctr->weightPercentage<100.0){
							if(!activitiesWithUnder100.contains(act->id)){
								activitiesWithUnder100.insert(act->id);
							}
						}
					}
				}
			}

			else if(genericConstraint->type==CONSTRAINT_TEACHER_HOME_ROOMS){
				ConstraintTeacherHomeRooms* ctr=(ConstraintTeacherHomeRooms*)genericConstraint;
				
				if(act->teachersNames.count()==1){
					if(act->teachersNames.at(0)==ctr->teacherName){
						if(ctr->weightPercentage==100.0){
							if(!activitiesWith100.contains(act->id)){
								activitiesWith100.insert(act->id);
							}
						}
						else if(ctr->weightPercentage>=0.0 && ctr->weightPercentage<100.0){
							if(!activitiesWithUnder100.contains(act->id)){
								activitiesWithUnder100.insert(act->id);
							}
						}
					}
				}
			}

			else if(genericConstraint->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
				ConstraintSubjectPreferredRoom* ctr=(ConstraintSubjectPreferredRoom*)genericConstraint;
				
				if(act->subjectName==ctr->subjectName){
					if(ctr->weightPercentage==100.0){
						if(!activitiesWith100.contains(act->id)){
							activitiesWith100.insert(act->id);
						}
					}
					else if(ctr->weightPercentage>=0.0 && ctr->weightPercentage<100.0){
						if(!activitiesWithUnder100.contains(act->id)){
							activitiesWithUnder100.insert(act->id);
						}
					}
				}
			}

			else if(genericConstraint->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
				ConstraintSubjectPreferredRooms* ctr=(ConstraintSubjectPreferredRooms*)genericConstraint;
				
				if(act->subjectName==ctr->subjectName){
					if(ctr->weightPercentage==100.0){
						if(!activitiesWith100.contains(act->id)){
							activitiesWith100.insert(act->id);
						}
					}
					else if(ctr->weightPercentage>=0.0 && ctr->weightPercentage<100.0){
						if(!activitiesWithUnder100.contains(act->id)){
							activitiesWithUnder100.insert(act->id);
						}
					}
				}
			}

			else if(genericConstraint->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM){
				ConstraintSubjectActivityTagPreferredRoom* ctr=(ConstraintSubjectActivityTagPreferredRoom*)genericConstraint;
				
				if(act->subjectName==ctr->subjectName && currentActivityTagsSet.contains(ctr->activityTagName)){
					if(ctr->weightPercentage==100.0){
						if(!activitiesWith100.contains(act->id)){
							activitiesWith100.insert(act->id);
						}
					}
					else if(ctr->weightPercentage>=0.0 && ctr->weightPercentage<100.0){
						if(!activitiesWithUnder100.contains(act->id)){
							activitiesWithUnder100.insert(act->id);
						}
					}
				}
			}

			else if(genericConstraint->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS){
				ConstraintSubjectActivityTagPreferredRooms* ctr=(ConstraintSubjectActivityTagPreferredRooms*)genericConstraint;
				
				if(act->subjectName==ctr->subjectName && currentActivityTagsSet.contains(ctr->activityTagName)){
					if(ctr->weightPercentage==100.0){
						if(!activitiesWith100.contains(act->id)){
							activitiesWith100.insert(act->id);
						}
					}
					else if(ctr->weightPercentage>=0.0 && ctr->weightPercentage<100.0){
						if(!activitiesWithUnder100.contains(act->id)){
							activitiesWithUnder100.insert(act->id);
						}
					}
				}
			}

			else if(genericConstraint->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM){
				ConstraintActivityTagPreferredRoom* ctr=(ConstraintActivityTagPreferredRoom*)genericConstraint;
				
				if(currentActivityTagsSet.contains(ctr->activityTagName)){
					if(ctr->weightPercentage==100.0){
						if(!activitiesWith100.contains(act->id)){
							activitiesWith100.insert(act->id);
						}
					}
					else if(ctr->weightPercentage>=0.0 && ctr->weightPercentage<100.0){
						if(!activitiesWithUnder100.contains(act->id)){
							activitiesWithUnder100.insert(act->id);
						}
					}
				}
			}

			else if(genericConstraint->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS){
				ConstraintActivityTagPreferredRooms* ctr=(ConstraintActivityTagPreferredRooms*)genericConstraint;
				
				if(currentActivityTagsSet.contains(ctr->activityTagName)){
					if(ctr->weightPercentage==100.0){
						if(!activitiesWith100.contains(act->id)){
							activitiesWith100.insert(act->id);
						}
					}
					else if(ctr->weightPercentage>=0.0 && ctr->weightPercentage<100.0){
						if(!activitiesWithUnder100.contains(act->id)){
							activitiesWithUnder100.insert(act->id);
						}
					}
				}
			}

			else if(genericConstraint->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
				ConstraintActivityPreferredRoom* ctr=(ConstraintActivityPreferredRoom*)genericConstraint;
				
				if(act->id==ctr->activityId){
					if(ctr->weightPercentage==100.0){
						if(!activitiesWith100.contains(act->id)){
							activitiesWith100.insert(act->id);
						}
					}
					else if(ctr->weightPercentage>=0.0 && ctr->weightPercentage<100.0){
						if(!activitiesWithUnder100.contains(act->id)){
							activitiesWithUnder100.insert(act->id);
						}
					}
				}
			}

			else if(genericConstraint->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOMS){
				ConstraintActivityPreferredRooms* ctr=(ConstraintActivityPreferredRooms*)genericConstraint;
				
				if(act->id==ctr->activityId){
					if(ctr->weightPercentage==100.0){
						if(!activitiesWith100.contains(act->id)){
							activitiesWith100.insert(act->id);
						}
					}
					else if(ctr->weightPercentage>=0.0 && ctr->weightPercentage<100.0){
						if(!activitiesWithUnder100.contains(act->id)){
							activitiesWithUnder100.insert(act->id);
						}
					}
				}
			}
		}
	}
	
	activitiesWithUnder100.subtract(activitiesWith100);
	
	QList<int> certainlyWithUnspecifiedRoom;
	QList<int> possiblyWithUnspecifiedRoom;
	
	foreach(Activity* act, gt.rules.activitiesList){
		if(!act->active)
			continue;

		if(!activitiesWith100.contains(act->id)){
			if(activitiesWithUnder100.contains(act->id)){
				possiblyWithUnspecifiedRoom.append(act->id);
			}
			else{
				certainlyWithUnspecifiedRoom.append(act->id);
			}
		}
	}
	
	QString strPossiblyWithoutRoom;
	if(possiblyWithUnspecifiedRoom.count()>0){
		strPossiblyWithoutRoom=tr("You have %1 activities which might end up in an unspecified room"
		 " (not considering those which will certainly end up in an unspecified room). They are listed below:",
		 "Instructions for translators: take care, it is 'might', so it is only a possibility.")
		 .arg(possiblyWithUnspecifiedRoom.count());
		strPossiblyWithoutRoom+=QString("\n\n");
		
		for(int i=0; i<possiblyWithUnspecifiedRoom.count(); i++){
			int id=possiblyWithUnspecifiedRoom.at(i);
			strPossiblyWithoutRoom+=tr("No: %1, Id: %2 (%3)", "%1 is the current number, %2 is the activity id, %3 is the detailed activity description")
			 .arg(i+1).arg(id).arg(getActivityDetailedDescription(gt.rules, id));
			strPossiblyWithoutRoom+=QString("\n");
		}
	}
	else{
		strPossiblyWithoutRoom=tr("You do not have activities which might end up in an unspecified room"
		" (not considering those which will certainly end up in an unspecified room).",
		 "Instructions for translators: take care, it is 'might', so it is only a possibility.");
		strPossiblyWithoutRoom+=QString("\n");
	}

	QString strCertainlyWithoutRoom;
	if(certainlyWithUnspecifiedRoom.count()>0){
		strCertainlyWithoutRoom=tr("You have %1 activities which will certainly end up in an unspecified room. They are listed below:",
		 "Instructions for translators: take care, it is 'certainly'.")
		 .arg(certainlyWithUnspecifiedRoom.count());
		strCertainlyWithoutRoom+=QString("\n\n");
		
		for(int i=0; i<certainlyWithUnspecifiedRoom.count(); i++){
			int id=certainlyWithUnspecifiedRoom.at(i);
			strCertainlyWithoutRoom+=tr("No: %1, Id: %2 (%3)", "%1 is the current number, %2 is the activity id, %3 is the detailed activity description")
			 .arg(i+1).arg(id).arg(getActivityDetailedDescription(gt.rules, id));
			strCertainlyWithoutRoom+=QString("\n");
		}
	}
	else{
		strCertainlyWithoutRoom=tr("You do not have activities which will certainly end up in an unspecified room.",
		 "Instructions for translators: take care, it is 'certainly'.");
		strCertainlyWithoutRoom+=QString("\n");
	}
	
	possiblyPlainTextEdit->setPlainText(strPossiblyWithoutRoom);
	certainlyPlainTextEdit->setPlainText(strCertainlyWithoutRoom);
}

ActivitiesRoomsStatisticsForm::~ActivitiesRoomsStatisticsForm()
{
	saveFETDialogGeometry(this);
}
