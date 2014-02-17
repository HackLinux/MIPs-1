#include<stdio.h>

int main() {
	char s[] = "372 -2";
	char* end;
	int t1   = strtol(s, &end, 10);
	printf("%d\n", t1);
	int t2   = strtol(end, &end, 10);
	printf("%d\n", t2);
}
