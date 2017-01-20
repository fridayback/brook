#include "bk_cfg.h"


/*************************************************
  Function:       load_sys_defaul_config
  Description:    加载默认运行参数
  Author:         liulin
  Param:          
        Input:
        Output:
  Return:         void
  Others:         // 其它说明
*************************************************/
void load_sys_defaul_config()
{

}

bool load_sys_config()
{
	load_sys_defaul_config();
	//从zookeeper中加载公共运行配置,覆盖冲突的默认配置

	//从本地配置文件加载个人配置(如果和公共配置冲突则覆盖公共配置)

}

