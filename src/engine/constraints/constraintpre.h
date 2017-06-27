#ifndef CONSTRAINTPRE_H
#define CONSTRAINTPRE_H

#include "rules.h"

class ConstraintPre
{
public:
	ConstraintPre();

	virtual bool prepare(const Rules &rules) = 0;

	virtual QStringList getErrors() const;

protected:
	QStringList errors;
};

#endif // CONSTRAINTPRE_H
