#include "Joint.h"

int Joint::jointFeedback(absDriver& driver)
{
	driver.Read();

	return 0;
}

int Joint::jointFeedback2(EPOS3& epos)
{
	epos.Read();

	return 0;
}
