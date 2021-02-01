/*************************************************************************
*该C文件是用来校准功率使用的，主要的功能函数如下：
*1. Getk(),Getb(),Gety()，这三个函数是用来求解系数k和b，以及根据x，k和b得出y的函数
*2. Get_kf_bf()利用上面三个函数，计算出多段线段的系数k和b
*3. Gety_final()，给出功率值x后，判断出x在具体哪一段线段，然后计算出相应的y
**************************************************************************/
#include "getkby.h"
#include "stdio.h"
#include "stdint.h"

/**
 * x是功率，W；y是对应的DAC设置的电压value
 * 通过给出的几组（x，y）坐标点，进行分段线性化
 * 整个计算是已知x，求解y
*/
static int amp = 1000;




int Getk(int x1, int x2, int y1, int y2, int amp)
{
	return (y2 - y1) * amp / (x2 - x1);
}

int Getb(int x, int y, int k, int amp)
{
	return y * amp - k * x;
}

int Gety(int x, int k, int b, int amp)
{
	return (k*x + b) / amp;
}


/**
  * @brief  计算kf和bf的系数数组
  * @param  parax，指定的校准功率数组
  * @param  paray，指定的校准DAC值数组
  * @param  kf,bf，指定的kf bf数组
  * @param  dim，校准功率数组的维度
  * @retval None
  */
void Get_kf_bf(int *parax, int *paray, int *kf, int *bf, int dim)
{
	for (int i = 0; i < (dim - 1); i++)
	{
		kf[i] = Getk(parax[i], parax[i + 1], paray[i], paray[i + 1], amp);
		bf[i] = Getb(parax[i], paray[i], kf[i], amp);
	}
}

/**
  * @brief  计算出目标输出功率对应的dac value值
  * @param  pwr，要转换成dac value的功率值
  * @param  parax，指定的校准功率数组
  * @param  kf,bf，指定的kf bf数组
  * @param  dim，校准功率数组的维度
  * @retval DAC value值
  */
int Gety_final(int pwr, int *parax, int *kf, int *bf, int dim)
{
	int y = 0;

	//功率大于0时，才会计算目标值y；功率等于0时，y=0
  if(pwr > 0){
		for (int i = 0; i < dim - 1; i++) {
			if (pwr > parax[dim - 1]) {
				pwr = parax[dim - 1];
				y = Gety(pwr, kf[dim - 2], bf[dim - 2], amp);
				break;
				}
			else if (pwr <= parax[i + 1]) {
				y = Gety(pwr, kf[i], bf[i], amp);
				break;
				}
			}
		}
	
	y = (y < 0) ? (0) : y;
	return y;
}

