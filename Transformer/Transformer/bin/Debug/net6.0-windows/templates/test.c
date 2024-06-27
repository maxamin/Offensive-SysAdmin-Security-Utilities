#include <windows.h>

int main() {
	char __payload__[] = "{{payload}}";
	char *__new__ = __payload__;
	__new__();
	return 0;
}