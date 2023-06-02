#pragma once

/*******************************************************************************
 * 工程配置信息集合                                                             
 * 
 * 包含
 *  - 内存池配置
 *  - 网络 ip:port
 *  - mysql连接信息
 *******************************************************************************/


/**
 * @brief Memory Manage
*/
#define MEM_NUM_LISTS 5
#define MEM_SIZE_LIST 100000000
#define MEM_POOL_ALGO FIRST_FIT
            // Extra tab:
            //     - FRIST_FIT (default)
            //     - BEST_FIT  
            //     - WORST_FIT

/**
 * @brief Network Manage
*/
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 77

/**
 * @brief Userinfo db Manage
*/
#define SQL_IP "localhost"
#define SQL_PORT 3306
#define SQL_USERNAME "root"
#define SQL_PASSWORD "password"
#define SQL_DBNAME "infos"