
/* ���ζ��� ����˵��
head ͷָ��ָ���ͷ
end  βָ��ָ���β
Max_Size ���еĿռ����
data[Max_Size] ���������buff


in  ��ӵ�����
out ���ӵ�����

���г�ʼ����head =0 ��end =0��

��ӷ�ʽ:��endָ���1�����in�����ݸ�ֵ��endָ��Ŀռ䣻Ϊ�˱�֤��Χ��0~Max_Size ����ȡ��ķ�ʽ

    end = (end+1)%Max_Size;
    data[end] = in;

���ӷ�ʽ:��headָ���1�����headָ��Ŀռ����ݸ�ֵ��out��Ϊ�˱�֤head��Χ��0~Max_Size ����ȡ��ķ�ʽ

    head = (head+1)%Max_Size;
    out = data[head];

�����ж�: end+1 == head  ��Ϊ�˱�֤end��Χ��0~Max_Size ����ȡ��ķ�ʽ

    (end+1)%Max_Size == head

�ӿ��ж�: end == head ��

     end == head��

*/
#include "stdio.h"
#include "stdint.h"

//���п�������Ϊ��󳤶ȼ�һ 

typedef struct
{
    uint32_t head;
    uint32_t end;
    uint32_t Max_Size;
    uint8_t *data;
} Fifo_t;


void FifoInit(Fifo_t *f,uint8_t *data,uint32_t Max_Size);
uint32_t FifoIn(Fifo_t *f, uint8_t *in, uint32_t len);
uint8_t FifoOut(Fifo_t *f, uint8_t *out, uint32_t len);

static uint8_t isEmpty(Fifo_t *f)
{
    return (f->end == f->head);
}

static uint8_t isFull(Fifo_t *f)
{
    return ((f->end + 1) % f->Max_Size == f->head);
}


/**
 * @brief ���ζ��г�ʼ��
 * @param Fifo_t *f ����ʼ�����нṹ��ָ��
 */
void FifoInit(Fifo_t *f,uint8_t *data,uint32_t Max_Size)
{
    f->end = 0;
    f->head = 0;
    f->data = data;
    f->Max_Size = Max_Size;
}

/**
 * @brief ��Ӻ���
 * @param Fifo_t *f ���нṹ��ָ��
 * @param uint8_t *in �������ָ��
 * @param uint32_t len ������ݳ���
 * @retval 0 ��ʾ�������˴治�������ʧ�� �������ֱ�ʾ�ɹ���ӵ����ݸ���
 */
uint32_t FifoIn(Fifo_t *f, uint8_t *in, uint32_t len)
{
    uint32_t i;
    for (i = 0; i < len; i++)
    {
        if (isFull(f))
        {
            printf(" in: %d ���ʧ��\n", in[i]);
            return 0;
        }
        f->end = (f->end + 1) % f->Max_Size;
        f->data[f->end] = in[i];
        printf("in: %d\n", in[i]);
    }
     
    return i;
}


/**
 * @brief ���Ӻ���
 * @param Fifo_t *f ���нṹ��ָ��
 * @param uint8_t *out ��������ָ��
 * @param uint32_t len �������ݳ���
 * @retval 0 ��ʾ���п���û����������ʧ�� �������ֱ�ʾ�ɹ����ӵ����ݸ���
 */
uint8_t FifoOut(Fifo_t *f, uint8_t *out, uint32_t len)
{
    uint32_t i;
    for (i = 0; i < len; i++)
    {
        if (isEmpty(f))
        {
             printf("����ʧ��\n");
            return 0;
        }
        f->head = (f->head + 1) % f->Max_Size;
        out[i] = f->data[f->head];
        printf("out: %d\n", out[i]);
    }
    return i;
}


static Fifo_t g_fifo;
#define fifomaxlen (10)
uint8_t fifobuf[fifomaxlen];

int main(void)
{
    FifoInit(&g_fifo,fifobuf,fifomaxlen);

    uint8_t in[10] = {1,2,3,4,5,6,7,8,9,10};
    uint8_t out[10] = {0};
    uint8_t num =0;
    //��� 3������ 1 2 3
    num = FifoIn(&g_fifo,in,3);
  

    //���� 2������ 1 2
    FifoOut(&g_fifo,out,2);

    //��� 4 5 6 7 8 ��ʱ��������6������
    FifoIn(&g_fifo,&in[3],5);

    //���Ӷ�2������ Ӧ���� 3 4 ��ʱ��������4 ������
    FifoOut(&g_fifo,out,2);

    //���6 ������ 1 2 3 4 5 6   ��ʱ�������� 10 ������ 
    //�������һ�����ݻ� ���ʧ��һ��  ��Ϊ���е��������ΪMax_Size -1 == 9�� 
   num =  FifoIn(&g_fifo,&in[0],6);
  printf("���%d������\n",num);

    //����8 ������Ӧ����  5 6 7 8 1 2 3 4 ��ʱ�������� 1 ������ 
    num =  FifoOut(&g_fifo,out,8);
   
    //����2 ������Ӧ���� 5 Ȼ����п� ����ʧ��һ��
  num =   FifoOut(&g_fifo,out,8);
 printf("����%d������\n",num);

    return 0;
}