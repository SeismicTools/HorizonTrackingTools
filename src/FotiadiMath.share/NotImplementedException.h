#include <exception>

namespace FotiadiMath_Exceptions
{
	class NotImplementedException : public std::exception
	{
		public:
			// Переопределяем метод what()
			virtual const char* what() const noexcept override {
				return "Method/function still was not implemented.";
			}
	};
}
