#include <nova/cli.h>

using nova::cli::launcher;
using nova::cli::option;

int main(int argc, const char** argv) {
	return launcher("$name", "$version", {
		$options
	}).start(argc, argv);
}
