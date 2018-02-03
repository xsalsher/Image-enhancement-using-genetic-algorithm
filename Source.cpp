#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include<stdlib.h>
#include <algorithm>
#include<time.h>
#define array_size 1024
#define size 1048576
using namespace cv;
using namespace std;
int imarr[array_size][array_size], final[array_size][array_size], final1[array_size][array_size], canny_array[array_size][array_size], edge_cheak[array_size][array_size], cromosome[array_size][array_size], final_image_array1[array_size][array_size];
int arr[size], final_image_array[size], random[size], other[size], arr1[size], image[size], image1[size], image2[size], first_cross[size], second_cross[size], remove_duplicate[size];
double fit_array[size];
int l, k, pos1, pos2, image_size, rows, cols, edge;
double fit_value, fit_value1, largest = 0, second_largest = 0;
int dx[] = { -1,-1,-1,0,0,1,1,1 };
int dy[] = { -1,0,1,-1,1,-1,0,1 };
Mat matOut = imread("../data/img4.jpg", CV_LOAD_IMAGE_GRAYSCALE); // rename this as your image name
int initial_population = 5, perform_crossover = 5;
int total_population = initial_population + (2 * perform_crossover);
int *random_value()
{
	srand((int)time(NULL));
	random[0] = 0;
	random[l - 1] = 255;
	for (int i = 1; i < l - 1; i++)
	{
		int ran = rand() % 255;
		random[i] = ran;

	}
	sort(random, random + (l - 1));
	return random;
}

void dfs(int i, int j)
{
	int k;
	for (k = 0; k < 8; k++)
	{
		int fx = i + dx[k];
		int fy = j + dy[k];

		if (fx >= 0 && fx<rows && fy >= 0 && fy<cols && canny_array[fx][fy])
			canny_array[fx][fy] = 0, dfs(fx, fy);
	}
}

double fit()
{
	//transfer 1D array to 2D
	int count1 = 1;
	for (int i = 1; i <= rows; i++)
	{
		for (int j = 1; j <= cols; j++)
		{
			if (count1 <= image_size)
			{
				final[i][j] = image1[count1];
				count1++;
			}
		}
	}
	for (int i = 1; i <= rows; i++)
	{
		for (int j = 1; j <= cols; j++)
		{
			if (j == 1)
			{
				final[i][j - 1] = final[i][j];
			}
			else if (j == cols)
			{
				final[i][j + 1] = final[i][j];
			}
		}
		for (int j = 1; j <= cols; j++)
		{
			if (i == 1)
			{
				final[i - 1][j] = final[i][j];
			}
			else if (i == rows)
			{
				final[i + 1][j] = final[i][j];
			}
		}
	}

	final[0][0] = final[1][1];
	final[0][cols + 1] = final[1][cols];
	final[rows + 1][0] = final[rows][1];
	final[rows + 1][cols + 1] = final[rows][cols];

	//fitnrss function 
	double ans = 0;
	for (int i = 1; i <= rows; i++)
	{
		for (int j = 0; j <= cols; j++)
		{
			int val = (final[i - 1][j + 1] + (2 * final[i][j + 1]) + final[i + 1][j + 1]) - (final[i - 1][j - 1] + (2 * final[i][j - 1]) + final[i + 1][j - 1]);
			int val1 = (final[i + 1][j - 1] + (2 * final[i + 1][j]) + final[i + 1][j + 1]) - (final[i - 1][j - 1] + (2 * final[i - 1][j]) + final[i - 1][j + 1]);
			double res = sqrt((val*val) + (val1*val1));
			ans = ans + res;
		}
	}
	//canny image


	Mat canny_image;
	unsigned char canny_value;
	Canny(matOut, canny_image, 50, 150, 3);

	//namedWindow("pic2", WINDOW_AUTOSIZE);
	//imshow("pic2", canny_image);
	//waitKey(0);
	//canny image to array
	for (int x = 0; x < canny_image.rows; x++)
	{
		for (int y = 0; y < canny_image.cols; y++)
		{
			uchar canny_value = canny_image.at<uchar>(Point(y, x));
			canny_array[x][y] = canny_value;
		}
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (canny_array[i][j])
			{
				edge++, dfs(i, j);
			}
		}
	}
	double fit = log(log(ans))*edge;
	//printf("\n%lf\n", fit);
	return fit;
}


void max()
{
	for (int i = 0; i<total_population; ++i)
	{
		if (fit_array[i]>largest)
		{
			largest = fit_array[i];
			pos1 = i;
		}
	}

	for (int i = 0; i<total_population; ++i)
	{
		if (fit_array[i]>second_largest)
		{
			if (fit_array[i] == largest)
				continue;              //Ignoring largest in order to get second largest
			second_largest = fit_array[i];
			pos2 = i;
		}
	}

}
void final_image()
{
	largest = 0;
	for (int i = 0; i<total_population; ++i)
	{
		if (fit_array[i]>largest)
		{
			largest = fit_array[i];
			pos1 = i;
		}
	}

	for (int i = 0; i < image_size; i++)
	{
		final_image_array[i] = cromosome[pos1][i];
	}

	int count1 = 0;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (count1 < image_size)
			{
				final_image_array1[i][j] = final_image_array[count1];
				count1++;
			}

		}
	}

	for (int x = 0; x < matOut.rows; x++)
	{
		for (int y = 0; y < matOut.cols; y++)
		{
			matOut.at<uchar>(Point(y, x)) = final_image_array1[x][y];
		}
	}
	//show image
	namedWindow("Output Image", WINDOW_AUTOSIZE);
	imshow("Output Image", matOut);
	waitKey(0);
}

void crossover()
{
	srand((int)time(NULL));

	for (int i = 0; i<perform_crossover; i++)
	{
		max();
		//waitKey(0);
		for (int j = 0; j < image_size; j++)
		{
			first_cross[j] = cromosome[pos1][j];
			second_cross[j] = cromosome[pos2][j];
		}
		int ran = rand() % image_size;
		for (int i = ran; i<image_size; i++)
		{
			int initial = first_cross[i];
			first_cross[i] = second_cross[i];
			second_cross[i] = initial;
		}
		for (int j = 0; j < image_size; j++)
		{
			image1[j + 1] = first_cross[j];
			cromosome[(2 * i) + perform_crossover][j] = first_cross[j];
			first_cross[j] = 0;

		}
		fit_value = fit();
		fit_array[(2 * i) + perform_crossover] = fit_value;
		for (int j = 0; j < image_size; j++)
		{
			image1[j + 1] = second_cross[j];
			cromosome[(2 * i) + (perform_crossover+1)][j] = second_cross[j];
			second_cross[j] = 0;
		}
		fit_value1 = fit();
		fit_array[(2 * i) + (perform_crossover+1)] = fit_value1;
	}
	final_image();
}
int main()
{

	int row, col, m;
	Mat mat = imread("../data/img4.jpg", CV_LOAD_IMAGE_GRAYSCALE); // rename this as your image name
	namedWindow("original image", WINDOW_AUTOSIZE);
	imshow("original image", mat);
	waitKey(0);
	for (k = 0; k<initial_population; k++)
	{
		for (int x = 0; x < mat.rows; x++)
		{
			for (int y = 0; y < mat.cols; y++)
			{
				uchar intensity = mat.at<uchar>(Point(y, x));
				imarr[x][y] = intensity;
			}
		}
		row = mat.rows;
		col = mat.cols;
		rows = row;
		cols = col;
		int count = 0;

		//transform 2d image array to 1d array
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{

				arr[count++] = imarr[i][j];
			}
		}

		//remove_duplicate array form image
		int n = row * col;
		m = n;
		image_size = m;
		for (int i = 0; i < n; i++)
		{
			arr1[i] = arr[i]; //save  imege array value
			image[i] = arr[i];
		}
		int val = 1;
		for (int i = 0; i<n; i++)
		{
			for (int j = i + 1; j<n;)
			{
				if (arr1[i] == arr1[j])
				{
					for (int k = j; k < n - 1; k++)
					{
						arr1[k] = arr1[k + 1];

					}
					n--;
				}
				else
					j++;
			}
		}
		// count remove_duplicate number;

		for (l = 0; l < n; l++)
		{
			other[l] = arr1[l];			//store value of remove_duplicate array to other array
			remove_duplicate[l] = arr1[l];
		}
		sort(remove_duplicate, remove_duplicate + (l - 1));
		//generate random number
		int *random_array;
		random_array = random_value();
		for (int i = 0; i < l; i++)
		{
			other[i] = random_array[i];
		}

		//replace random value to the image array
		for (int i = 0; i<l; i++)
		{
			int x = remove_duplicate[i];
			int y = other[i];
			for (int j = 0; j < m; j++)
			{
				if (image[j] == x)
				{
					image[j] = y;
				}

			}
		}

		for (int i = 0; i < m; i++)
		{
			image2[i] = image[i];
			image1[i + 1] = image[i];
		}

		int count2 = 0;
		for (int i = 0; i <row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				if (count2 <m)
				{
					final1[i][j] = image2[count2];
					count2++;
				}
			}
		}

		for (int x = 0; x < mat.rows; x++)
		{
			for (int y = 0; y < mat.cols; y++)
			{
				matOut.at<uchar>(Point(y, x)) = final1[x][y];

			}
		}

		//show image

		namedWindow("Initial Population", WINDOW_AUTOSIZE);
		imshow("Initial Population", matOut);
		//waitKey(0);

		for (int j = 0; j < m; j++)
		{
			cromosome[k][j] = image2[j];
		}

		//calculate fit value
		fit_value = fit();
		fit_array[k] = fit_value;
	}

	crossover();

	//waitKey(0);
	//ps and pc calculate
	int ps = l;
	double pc = 0.8;
	double result = ps * pc;
	int ran = rand() % l;
	//printf("%d", ran);

	return 0;
}