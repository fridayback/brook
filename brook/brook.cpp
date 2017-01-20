/*************************************************
  Copyright (C), 2017-2027, Enerbos. Co., Ltd.
  File name:      brook.cpp
  Author:       liulin
  Version:   1.0     
  Date: 2017/01/12
  Description:    // 用于详细说明此程序文件完成的主要功能，与其他模块
                  // 或函数的接口，输出值、取值范围、含义及参数间的控
                  // 制、顺序、独立或依赖等关系
  Others:         // 其它内容的说明
  History:        // 修改历史记录列表，每条修改记录应包括修改日期、修改
                  // 者及修改内容简述  
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
  Description:    // 函数功能、性能等的描述
  Author:         liulin
  Param:          
        Input:
        Output:
  Return:         void 返回值说明：
  Others:         // 其它说明
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

	//TODO:加载运行配置
	if (false == load_config())
	{
		LOG_ERROR("加载运行配置失败");
		return 0;
	}
	//TODO:初始化

	//TODO:启动消息循环和心跳
    return 0;
}


