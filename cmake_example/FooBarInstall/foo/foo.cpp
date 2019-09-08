
#include "foo.h"

#include <stdio.h>

void foo(void)
{
	printf("This is foo version %s\n", FOOBAR_VERSION_STRING);
}
