#include "TcpClient.h"

int main() {
	TcpClient client("127.0.0.1", 3000);
	client.run();
	return 0;
}
