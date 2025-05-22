#include <cmath>
#include <queue>
#include <vector>
#include <stack>
#include <limits>
#include "wave_alg.h"

using namespace std;

bool isStepInLine(int n, int m, int height, int width, const pair<int, int>& cur)
{
	int tmp1 = (n - height) * cur.second - (m - width) * (cur.first - height);
	int tmp2 = (n - height) * (cur.second - width) - (m - width) * cur.first;
	if (tmp1 >= 0 && tmp2 <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool isStepInMatrix(int n, int m, const pair<int, int>& cur)
{
	if (cur.first >= 0 && cur.second >= 0 && cur.first < n && cur.second < m)
	{
		return true;
	}
	else
	{
		return false;
	}
}

template <typename T> int sgn(T type)
{
	if (static_cast<int>(type) < 0)
	{
		return -1;
	}
	else
	{
		if (static_cast<int>(type) == 0)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
}

list<pair<int, int>> PossibleSteps(pair<int, int> start, pair<int, int> finish)
{
	list<pair<int, int>> res;
	int sgn_y = sgn(finish.first - start.first);
	int sgn_x = sgn(finish.second - start.second);
	if (sgn_y)
	{
		res.push_back(pair<int, int>(sgn_y, 0));
	}
	if (sgn_x)
	{
		res.push_back(pair<int, int>(0, sgn_x));
	}
	if (sgn_y && sgn_x)
	{
		res.push_back(pair<int, int>(sgn_y, sgn_x));
	}
	return res;
}

const pair<int, int> AddPair(const pair<int, int>& p1, const pair<int, int>& p2)
{
	pair<int, int> res(p1.first + p2.first, p1.second + p2.second);
	return res;
}

const bool IsCanDoStep(const pair<int, int>& p, list<pair<int, int>> direction, double* matrix, const double approximation_bad_step, const pair<int, int>& stp, int n, int m)
{
	bool flag = true;
	for (list<pair<int, int>>::const_iterator k = direction.cbegin(); k != direction.end(); k++)
	{
		pair<int, int> next_step = AddPair(stp, *k);
		if (isStepInMatrix(n, m, next_step))
		{
			if (matrix[next_step.first * m + next_step.second] < approximation_bad_step)
			{
				flag = false;
				break;
			}
		}
	}
	if (abs(p.first) + abs(p.second) == 1 && flag)
	{
		return true;
	}
	return false;
}

const bool IsBasicPair(const pair<int, int>& p)
{
	if (abs(p.first) + abs(p.second) == 1)
	{
		return true;
	}
	return false;
}

const pair<int, int> InvPair(const pair<int, int>& p)
{
	pair<int, int> res(-p.first, -p.second);
	return res;
}

inline bool isFinish(const pair<int, int>& cur_stp, const pair<int, int>& finish, bool f_any)
{
	if (!f_any)
	{
		return cur_stp == finish;
	}
	else
	{
		return cur_stp.first == finish.first || cur_stp.second == finish.second;
	}
}

inline pair<int, int> SearchFinish(int n, int m, float* w_matrix, const pair<int, int>& finish, bool f_any)
{
	if (!f_any)
	{
		return finish;
	}
	else
	{
		pair<int, int> res;
		res = finish;
		float record = w_matrix[finish.first * m + finish.second];
		for (int i = 0; i < m; i++)
		{
			if (w_matrix[finish.first * m + i] < record)
			{
				record = w_matrix[finish.first * m + i];
				res = pair<int, int>(finish.first, i);
			}
		}
		for (int i = 0; i < n; i++)
		{
			if (w_matrix[i * m + finish.second] < record)
			{
				record = w_matrix[i * m + finish.second];
				res = pair<int, int>(i, finish.second);
			}
		}
		return res;
	}
}

double WaveStdAlgMaze(int n, int m, double* matrix, double approximation, pair<int, int> start, pair<int, int> finish, bool f_any, list< pair<int, int> >* path, double approximation_bad_step, double diagParam)
{
	if (path)
	{
		path->clear();
	}
	bool f_exit = false;
	float* matrix_weight = new float[n * m];
	for (int i = 0; i < n * m; i++)
	{
		matrix_weight[i] = (float)approximation;
	}
	float cur_approximation = (float)approximation;
	queue< pair<int, int> > steps;
	steps.push(start);
	matrix_weight[start.first * m + start.second] = (float)matrix[start.first * m + start.second];
	list<pair<int, int>> direction = PossibleSteps(start, finish);
	while (!steps.empty())
	{
		pair<int, int> stp = steps.front();
		steps.pop();
		if (isFinish(stp, finish, f_any))
		{
			float weight_cur_path = matrix_weight[stp.first * m + stp.second];
			if (weight_cur_path < cur_approximation)
			{
				f_exit = true;
				cur_approximation = weight_cur_path;
			}
		}
		for (list<pair<int, int>>::const_iterator k = direction.cbegin(); k != direction.end(); k++)
		{
			pair<int, int> next_step = AddPair(stp, *k);
			if (isStepInMatrix(n, m, next_step))
			{
				bool f_use = true;
				if (matrix[next_step.first * m + next_step.second] > approximation_bad_step)
				{
					if (!IsCanDoStep(*k, direction, matrix, approximation_bad_step, stp, n, m))
					{
						f_use = false;
					}
				}
				if (f_use)
				{
					double param = 1;
					if (k->first == 1 && k->second == 1)
					{
						param = diagParam;
					}
					float weight_cur_path = (float)(matrix_weight[stp.first * m + stp.second] + matrix[next_step.first * m + next_step.second] * param);
					if (weight_cur_path < matrix_weight[next_step.first * m + next_step.second])
					{
						matrix_weight[next_step.first * m + next_step.second] = weight_cur_path;
						steps.push(next_step);
					}
				}
			}
		}
	}
	if (f_exit && path)
	{
		list<pair<int, int>> inv_direction;
		for (list<pair<int, int>>::const_iterator k = direction.cbegin(); k != direction.end(); k++)
		{
			inv_direction.push_back(InvPair(*k));
		}
		pair<int, int> cur_pos = SearchFinish(n, m, matrix_weight, finish, f_any);
		path->push_back(cur_pos);
		while (cur_pos != start)
		{
			float min_val = matrix_weight[cur_pos.first * m + cur_pos.second];
			pair<int, int> step = cur_pos;
			double min = std::numeric_limits<double>::max();
			for (list<pair<int, int>>::const_iterator k = inv_direction.cbegin(); k != inv_direction.end(); k++)
			{
				bool f_use = true;
				pair<int, int> tmp_step = AddPair(cur_pos, *k);
				if (matrix[cur_pos.first * m + cur_pos.second] > approximation_bad_step)
				{
					if (!IsBasicPair(*k))
					{
						f_use = false;
					}
				}
				double param = 1;
				if (!IsBasicPair(*k))
				{
					param = diagParam;
				}
				if (tmp_step.first >= 0 && tmp_step.second >= 0 && f_use)
				{
					double diff = abs(min_val - matrix[cur_pos.first * m + cur_pos.second] * param - matrix_weight[tmp_step.first * m + tmp_step.second]);
					if (diff < min)
					{
						min = diff;
						step = tmp_step;
					}
				}
			}
			min_val = matrix_weight[step.first * m + step.second];
			cur_pos = step;
			path->push_back(cur_pos);
		}
	}
	double res= matrix_weight[finish.first * m + finish.second];
	delete[] matrix_weight;
	return res;
}

//double WaveStdAlgMaze_ret(int n, int m, double *matrix, double approximation, pair<int, int> start, pair<int, int> finish, bool f_any, list< pair<int, int> > *path, double approximation_bad_step, double diagParam)
double WaveStdAlgMaze_ret(int n, int m, double* matrix, double approximation_bad_step, double diagParam)
{
	list< pair<int, int> > path;
	path.clear();
	double approximation = 1.e100;
	pair<int, int> start(0, 0);
	pair<int, int> finish(n - 1, m - 1);
	bool f_any = true;
	bool f_exit = false;
	float* matrix_weight = new float[n * m];
	for (int i = 0; i < n * m; i++)
	{
		matrix_weight[i] = (float)approximation;
	}
	float cur_approximation = (float)approximation;
	queue< pair<int, int> > steps;
	steps.push(start);
	matrix_weight[start.first * m + start.second] = (float)matrix[start.first * m + start.second];
	list<pair<int, int>> direction = PossibleSteps(start, finish);
	double res = 0.;
	while (!steps.empty())
	{
		pair<int, int> stp = steps.front();
		steps.pop();
		if (isFinish(stp, finish, f_any))
		{
			float weight_cur_path = matrix_weight[stp.first * m + stp.second];
			if (weight_cur_path < cur_approximation)
			{
				f_exit = true;
				cur_approximation = weight_cur_path;
			}
		}
		for (list<pair<int, int>>::const_iterator k = direction.cbegin(); k != direction.end(); k++)
		{
			pair<int, int> next_step = AddPair(stp, *k);
			if (isStepInMatrix(n, m, next_step))
			{
				bool f_use = true;
				if (matrix[next_step.first * m + next_step.second] > approximation_bad_step)
				{
					if (!IsCanDoStep(*k, direction, matrix, approximation_bad_step, stp, n, m))
					{
						f_use = false;
					}
				}
				if (f_use)
				{
					double param = 1;
					if (k->first == 1 && k->second == 1)
					{
						param = diagParam;
					}
					float weight_cur_path = (float)(matrix_weight[stp.first * m + stp.second] + matrix[next_step.first * m + next_step.second] * param);
					if (weight_cur_path < matrix_weight[next_step.first * m + next_step.second])
					{
						matrix_weight[next_step.first * m + next_step.second] = weight_cur_path;
						//						res= weight_cur_path; закоментировал 8.02.2019
						steps.push(next_step);
					}
				}
			}
		}
	}

	//if (f_exit && path.size()>0)
	//{
	//	list<pair<int, int>> inv_direction;
	//	for (list<pair<int, int>>::const_iterator k = direction.cbegin(); k != direction.end(); k++)
	//	{
	//		inv_direction.push_back(InvPair(*k));
	//	}
	//	pair<int, int> cur_pos = SearchFinish(n, m, matrix_weight, finish, f_any);
	//	path->push_back(cur_pos);
	//	while (cur_pos != start)
	//	{
	//		float min_val = matrix_weight[cur_pos.first*m + cur_pos.second];
	//		pair<int, int> step = cur_pos;
	//		double min = std::numeric_limits<double>::max();
	//		for (list<pair<int, int>>::const_iterator k = inv_direction.cbegin(); k != inv_direction.end(); k++)
	//		{
	//			bool f_use = true;
	//			pair<int, int> tmp_step = AddPair(cur_pos, *k);
	//			if (matrix[cur_pos.first*m + cur_pos.second]>approximation_bad_step)
	//			{
	//				if (!IsBasicPair(*k))
	//				{
	//					f_use = false;
	//				}
	//			}
	//			double param = 1;
	//			if (!IsBasicPair(*k))
	//			{
	//				param = diagParam;
	//			}
	//			if (tmp_step.first >= 0 && tmp_step.second >= 0 && f_use)
	//			{
	//				double diff = abs(min_val - matrix[cur_pos.first*m + cur_pos.second] * param - matrix_weight[tmp_step.first*m + tmp_step.second]);
	//				if (diff < min)
	//				{
	//					min = diff;
	//					step = tmp_step;
	//				}
	//			}
	//		}
			//min_val = matrix_weight[step.first*m + step.second];
			//cur_pos = step;
			//path->push_back(cur_pos);
	//	}
	//}
	res = matrix_weight[n * m - 1]; // добавил 08.02.2019
	delete[] matrix_weight;
	return res;
}


void WaveStdAlgMazeLine(int n, int m, double* matrix, double approximation, pair<int, int> start, pair<int, int> finish, bool f_any, list< pair<int, int> >* path, double approximation_bad_step, double diagParam, int height, int width)
{
	if (path)
	{
		path->clear();
	}
	bool f_exit = false;
	float* matrix_weight = new float[n * m];
	for (int i = 0; i < n * m; i++)
	{
		matrix_weight[i] = (float)approximation;
	}
	float cur_approximation = (float)approximation;
	queue< pair<int, int> > steps;
	steps.push(start);
	matrix_weight[start.first * m + start.second] = (float)matrix[start.first * m + start.second];
	list<pair<int, int>> direction = PossibleSteps(start, finish);
	while (!steps.empty())
	{
		pair<int, int> stp = steps.front();
		steps.pop();
		if (isFinish(stp, finish, f_any))
		{
			float weight_cur_path = matrix_weight[stp.first * m + stp.second];
			if (weight_cur_path < cur_approximation)
			{
				f_exit = true;
				cur_approximation = weight_cur_path;
			}
		}
		for (list<pair<int, int>>::const_iterator k = direction.cbegin(); k != direction.end(); k++)
		{
			pair<int, int> next_step = AddPair(stp, *k);
			if (isStepInMatrix(n, m, next_step) && isStepInLine(n, m, height, width, next_step))
			{
				bool f_use = true;
				if (matrix[next_step.first * m + next_step.second] > approximation_bad_step)
				{
					if (!IsCanDoStep(*k, direction, matrix, approximation_bad_step, stp, n, m))
					{
						f_use = false;
					}
				}
				if (f_use)
				{
					double param = 1;
					if (k->first == 1 && k->second == 1)
					{
						param = diagParam;
					}
					float weight_cur_path = (float)(matrix_weight[stp.first * m + stp.second] + matrix[next_step.first * m + next_step.second] * param);
					if (weight_cur_path < matrix_weight[next_step.first * m + next_step.second])
					{
						matrix_weight[next_step.first * m + next_step.second] = weight_cur_path;
						steps.push(next_step);
					}
				}
			}
		}
	}
	if (f_exit && path)
	{
		list<pair<int, int>> inv_direction;
		for (list<pair<int, int>>::const_iterator k = direction.cbegin(); k != direction.end(); k++)
		{
			inv_direction.push_back(InvPair(*k));
		}
		pair<int, int> cur_pos = SearchFinish(n, m, matrix_weight, finish, f_any);
		path->push_back(cur_pos);
		while (cur_pos != start)
		{
			float min_val = matrix_weight[cur_pos.first * m + cur_pos.second];
			pair<int, int> step = cur_pos;
			for (list<pair<int, int>>::const_iterator k = inv_direction.cbegin(); k != inv_direction.end(); k++)
			{
				bool f_use = true;
				pair<int, int> tmp_step = AddPair(cur_pos, *k);
				if (matrix[cur_pos.first * m + cur_pos.second] > approximation_bad_step)
				{
					if (!IsBasicPair(*k))
					{
						f_use = false;
					}
				}
				if (min_val >= matrix_weight[tmp_step.first * m + tmp_step.second] && tmp_step.first >= 0 && tmp_step.second >= 0 && f_use)
				{
					step = tmp_step;
					min_val = matrix_weight[step.first * m + step.second];
				}
			}
			cur_pos = step;
			path->push_back(cur_pos);
		}
	}
	delete[] matrix_weight;
}

int makeWave(double* grid, int n, int m, int* xCoord, int* yCoord, int type, double diagParam, int& count, int* points, int pointCount)
{
	//	SECURITY(DEF_WEIGHT, SEC_GEN_MASK)
	bool f_any;
	if (1 == type)
	{
		f_any = true;
	}
	else
	{
		f_any = false;
	}
	int index = 0;
	if (pointCount > 2)
	{
		for (int i = 0; i < pointCount - 1; i++)
		{
			int newN = points[2 * (i + 1)] - points[2 * i] + 1;
			int newM = points[2 * (i + 1) + 1] - points[2 * i + 1] + 1;
			double* newGrid = new double[newN * newM];
			for (int j = 0; j < newN; j++)
			{
				for (int k = 0; k < newM; k++)
				{
					newGrid[j * newM + k] = grid[(points[2 * i] + j) * m + (points[2 * i + 1] + k)];
				}
			}
			list< pair<int, int> > path;
			WaveStdAlgMaze(newN, newM, newGrid, 1.e100, pair<int, int>(0, 0), pair<int, int>(newN - 1, newM - 1), f_any, &path, 0.999, diagParam);
			path.reverse();
			for (list<pair<int, int>>::const_iterator k = path.cbegin(); k != path.end(); k++)
			{
				xCoord[index] = k->first + points[2 * i];
				yCoord[index] = k->second + points[2 * i + 1];
				index++;
			}
			index--;
			delete[] newGrid;
		}
		index++;
		count = index;
	}
	else
	{
		list< pair<int, int> > path;
		WaveStdAlgMaze(n, m, grid, 1.e100, pair<int, int>(0, 0), pair<int, int>(n - 1, m - 1), f_any, &path, 0.999, diagParam);
		path.reverse();
		for (list<pair<int, int>>::const_iterator k = path.cbegin(); k != path.end(); k++)
		{
			xCoord[index] = k->first;
			yCoord[index] = k->second;
			index++;
		}
		count = (int)path.size();
	}
	return 0;
}

