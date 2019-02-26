//@note:	������С���˷����ж���ʽ������ϣ����������ؼ������꼴��������ƺ���
//@Author:	Season
//@date:	2019/2/26

#ifndef POLYFIT_H_
#define POLYFIT_H_
#include <vector>

namespace curvefit {

//������ʾ
template<typename T>
struct Point {
	T x;
	T y;
};

class PolyFit {
public:
	PolyFit() = default;
	~PolyFit() = default;

	template<typename T>
	void Fit(const std::vector<Point<T>> &key_points, int poly_n);

	//@berif ������С���˷����ж���ʽ���
	//@param key_points �ؼ�����������
	//@param length		�ؼ������
	//@param poly_n		��ϴ���������Խ��Խ��ȷ
	template<typename T>
	void Fit(const Point<T> *key_points, size_t length, int poly_n);

	std::vector<double> GetFactor();
private:
	std::vector<double> factor;	//��Ϻ�ķ���ϵ��

	template<typename T>
	void GaussSolve(int n, std::vector<T> &a, std::vector<T> &x, std::vector<T> &b);

	//@berif ��˹��Ԫ�������Է������,��AX=B������ʽ
	//@param n ����Ľ���
	template<typename T>
	void GaussSolve(int n, T *a, T *x, T *b);

};

template<typename T>
void PolyFit::GaussSolve(int n, std::vector<T> &a, std::vector<T> &x, std::vector<T> &b) {
	GaussSolve(n, &a[0], &x[0], &b[0]);
}

//TODO ���������Ҫ���������������0
//����������Ԫ�����
template<typename T>
void PolyFit::GaussSolve(int n, T *a, T *x, T *b) {
	int i, j, k;
	double max;
	int index;
	for (k = 0; k < n - 1; k++) {
		max = std::fabs(a[k*n + k]);
		index = k;
		for (i = k; i <= n; i++) {
			if (std::fabs(a[i*n + i]) > max) {
				max = fabs(a[i*n + i]);
				index = i;
			}
		}
		if (index != k) {
			for (i = 0; i < n; i++) {
				max = a[k*n + i];
				a[k*n + i] = a[index*n + i];
				a[index*n + i] = max;
			}
		}
		max = b[k];
		b[k] = b[index];
		b[index] = max;
		for (i = k + 1; i < n; i++) {
			for (j = k + 1; j < n; j++) {
				a[i*n + j] -= a[i*n + k] * a[k*n + j] / a[k*n + k];
			}
			b[i] -= a[i*n + k] * b[k] / a[k*n + k];
		}
	}
	for (i = n - 1; i >= 0; x[i] /= a[i*n + i], i--) {
		for (j = i + 1, x[i] = b[i]; j < n; j++) {
			x[i] -= a[i*n + j] * x[j];
		}
	}
}

template<typename T>
void PolyFit::Fit(const std::vector<Point<T>> &key_points, int poly_n) {
	Fit(&key_points[0], key_points.size(), poly_n);
}

template<typename T>
void PolyFit::Fit(const Point<T> *key_points, size_t length, int poly_n) {
	factor.resize(poly_n + 1, 0);
	int i, j;
	std::vector<double> tempx(length, 1.0);
	std::vector<double> tempy;
	std::vector<double> sumxx(poly_n * 2 + 1);
	std::vector<double> ata((poly_n + 1)*(poly_n + 1));
	std::vector<double> sumxy(poly_n + 1);

	for (i = 0; i < length; i++) {
		tempy.push_back(key_points[i].y);
	}

	for (i = 0; i < 2 * poly_n + 1; i++) {
		for (sumxx[i] = 0, j = 0; j < length; j++)
		{
			sumxx[i] += tempx[j];
			tempx[j] *= key_points[j].x;
		}
	}
	for (i = 0; i < poly_n + 1; i++) {
		for (sumxy[i] = 0, j = 0; j < length; j++)
		{
			sumxy[i] += tempy[j];
			tempy[j] *= key_points[j].x;
		}
	}
	for (i = 0; i < poly_n + 1; i++)
		for (j = 0; j < poly_n + 1; j++)
			ata[i*(poly_n + 1) + j] = sumxx[i + j];
	GaussSolve(poly_n + 1, ata, factor, sumxy);
}

std::vector<double> PolyFit::GetFactor() {
	return factor;
}

}

#endif
