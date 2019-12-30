#include <$header>

using commline::command;
using commline::option;
using commline::origin;

namespace commline {
	program& current_program() {
		static program instance("$name", "$version");
		return instance;
	}
}

int main(int argc, char** argv) {
	return origin({
		$options
	}, {
		$commands
	}).run(argc, argv);
}
