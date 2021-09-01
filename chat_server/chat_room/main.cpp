#include"server/server.h"
int main() {
	server s1(6666, 10, 10);
	s1.start();
	return 0;

}