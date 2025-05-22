#include <vector>

// хранение матрицы (только значащей её части) 
// обеспечивает доступ к элементам за O(N) и O(log N)
template<typename T>
class ListMaze
{
private:
	std::vector<T> data;
	int max_shift; // максимальное смещение от диагонали
	int nrows, mcols;
	std::vector<std::pair<int, int>> dataRemoved; // на каждом уровне слева n точек не входят в матрицу и справа m точек не входят в матрицу
	int xWhereYEqMax_shift;
	std::vector<int> ids; // id элементов в представлении [y*n+x], если хранили бы полную матрицу

	// заполняет vector данными (каждой точке сопоставляет её индекс такой, если бы хранили всю матрицу)
	void fillData()
	{
		int x = 0, y = 0;
		for (int i = 0; i < nrows * mcols; i++)
		{
			int fromX = dataRemoved[y].first;
			int toX = fromX + mcols - dataRemoved[y].first - dataRemoved[y].second;
			if (x >= fromX && x < toX)
			{
				data.push_back(i);
				ids.push_back(i);
			}
			x++;
			if (x >= mcols)
			{
				x = 0;
				y++;
			}
		}
	}

public:
	inline ListMaze(int max_shift, int nrows, int mcols)
	{
		this->max_shift = max_shift;
		this->nrows = nrows;
		this->mcols = mcols;
		auto minSize = std::min(nrows, mcols);
		if(max_shift >= minSize)
		{
			// тогда будем хранить всю матрицу
		}

		double k = (double)(nrows-1) / (double)(mcols-1); // коэф прямой (b = 0 )
		int xWhereYEqMax_shift = (int)floor(max_shift / k); // x где y проходит через точку max_shift по y
		// xWhereYEqMax_shift столько точек за один шаг мы будем "терять" справа и "приобретать" слева
		this->xWhereYEqMax_shift = xWhereYEqMax_shift;

		int leftNum = -xWhereYEqMax_shift; // x координата окна слева при y = 0
		int rightNum = xWhereYEqMax_shift; // x координата окна справа при y = 0 
		for(int i = 0; i < nrows; i++)
		{
			int plusPointsLeft = 0;
			if(leftNum > 0)
			{
				plusPointsLeft += leftNum;
			}
			int plusPointsRight = 0;
			if(rightNum < mcols-1)
			{
				plusPointsRight += mcols - 1 - rightNum;
			}

			dataRemoved.push_back(std::make_pair(plusPointsLeft, plusPointsRight));

			leftNum += xWhereYEqMax_shift;
			rightNum += xWhereYEqMax_shift;
		}

		fillData();
	}

	// храним не всю матрицу + на каждую строку по 2 элемента (число пропущенных слева и число справа)
	// операция взятия элемента = O(N), где N - число строк
	inline T operator() (const int _row, const int _col) {
		if(dataRemoved[_row].first > _col)
		{
			return (T)0.0;
		}
		else if(_col > dataRemoved[_row].first + xWhereYEqMax_shift + dataRemoved[_row].second)
		{
			return (T)0.0;
		}

		int resultId = 0;
		for(int i = 0; i <= _row; i++)
		{
			int fromX = dataRemoved[i].first;
			int toX = fromX + xWhereYEqMax_shift + dataRemoved[i].second;
			if(i != _row)
			{
				resultId += mcols - fromX - dataRemoved[i].second;
			}
			else
			{
				resultId += _col - fromX;
			}
		}
		return data[resultId];
	}

	T getElem (const int _row, const int _col)
	{
		// можно на каждый элемент хранить его индекс и искать бин поиском O(log N)
		// это занимает меньше памяти и быстрее исполняется
		auto target = _row *mcols + _col;

		int left = 0;
		int right = ids.size() - 1;

		int answerId = -1;
		while (left <= right) {
			int mid = left + (right - left) / 2;

			if (ids[mid] == target) {
				answerId = mid;
				break;
			}
			else if (ids[mid] < target) {
				left = mid + 1;
			}
			else {
				right = mid - 1;
			}
		}

		if(answerId == -1)
		{
			return 0;
		}
		else
		{
			return data[answerId];
		}
	}
};
