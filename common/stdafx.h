// stdafx.h: 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 项目特定的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容



// 在此处引用程序需要的其他标头

#include <windows.h>
#include <strsafe.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <map>

#include "include/curl/curl.h"
#include "include/csv/reader.hpp"
#include "include/json/json.hpp"
#include "include/librabbitmq/amqp.h"
#include "include/librabbitmq/amqp_tcp_socket.h"
#include "include/detours/detours.h"
#include "Wincrypt.h"
#include "sqlite3.h"
#include "convert.h"
#include "log.h"
#include "troycrawler.h"
#include "http.h"
#include "msg.h"
#include "ITXData.h"
#include "sqlite3.h"
using json = nlohmann::json;

namespace fs = std::filesystem;
