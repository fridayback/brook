/*************************************************
  Copyright (C), 2017-2027, Enerbos. Co., Ltd.
  File name:      brook.cpp
  Author:       liulin
  Version:   1.0     
  Date: 2017/01/12
  Description:    // ������ϸ˵���˳����ļ���ɵ���Ҫ���ܣ�������ģ��
                  // �����Ľӿڣ����ֵ��ȡֵ��Χ�����弰������Ŀ�
                  // �ơ�˳�򡢶����������ȹ�ϵ
  Others:         // �������ݵ�˵��
  History:        // �޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸�
                  // �߼��޸����ݼ���  
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/

#include "stdafx.h"
#include <stdlib.h>
#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>
#include "comm/fdlog.h"
#include "comm/crash_process.h"
#include "bk_cfg.h "



/*************************************************
  Function:       finally_process
  Description:    // �������ܡ����ܵȵ�����
  Author:         liulin
  Param:          
        Input:
        Output:
  Return:         void ����ֵ˵����
  Others:         // ����˵��
*************************************************/
void finally_process()
{
	log_uninit();
}


int main()
{
	atexit(finally_process);
	CRASH_PROC();
	log_init();

	//TODO:������������
	if (false == load_config())
	{
		LOG_ERROR("������������ʧ��");
		return 0;
	}
	//TODO:��ʼ��

	//TODO:������Ϣѭ��������
    return 0;
}


