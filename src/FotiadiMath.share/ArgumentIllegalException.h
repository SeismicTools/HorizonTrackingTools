#include <exception>

namespace FotiadiMath_Exceptions
{
	class ArgumentIllegalException : public std::exception
	{
	public:
		// Переопределяем метод what()
		virtual const char* what() const noexcept override {
			return "Passed incorrect argument.";
		}
	};
}