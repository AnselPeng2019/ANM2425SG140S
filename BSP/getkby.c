/*************************************************************************
*��C�ļ�������У׼����ʹ�õģ���Ҫ�Ĺ��ܺ������£�
*1. Getk(),Getb(),Gety()���������������������ϵ��k��b���Լ�����x��k��b�ó�y�ĺ���
*2. Get_kf_bf()���������������������������߶ε�ϵ��k��b
*3. Gety_final()����������ֵx���жϳ�x�ھ�����һ���߶Σ�Ȼ��������Ӧ��y
**************************************************************************/
#include "getkby.h"
#include "stdio.h"
#include "stdint.h"

/**
 * x�ǹ��ʣ�W��y�Ƕ�Ӧ��DAC���õĵ�ѹvalue
 * ͨ�������ļ��飨x��y������㣬���зֶ����Ի�
 * ������������֪x�����y
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
  * @brief  ����kf��bf��ϵ������
  * @param  parax��ָ����У׼��������
  * @param  paray��ָ����У׼DACֵ����
  * @param  kf,bf��ָ����kf bf����
  * @param  dim��У׼���������ά��
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
  * @brief  �����Ŀ��������ʶ�Ӧ��dac valueֵ
  * @param  pwr��Ҫת����dac value�Ĺ���ֵ
  * @param  parax��ָ����У׼��������
  * @param  kf,bf��ָ����kf bf����
  * @param  dim��У׼���������ά��
  * @retval DAC valueֵ
  */
int Gety_final(int pwr, int *parax, int *kf, int *bf, int dim)
{
	int y = 0;

	//���ʴ���0ʱ���Ż����Ŀ��ֵy�����ʵ���0ʱ��y=0
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

