#include "stdafx.h"

size_t WriteData(void *ptr, size_t size, size_t nmemb, void *stream)
{
	std::string data((const char*)ptr, size * nmemb);
	*((std::stringstream*)stream) << data << std::endl;
	return size * nmemb;
}

size_t DownloadData(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}
httpIO::httpIO(logger* pLog)
{
	this->pLog = pLog;
	curl_global_init(CURL_GLOBAL_ALL);
}

httpIO::~httpIO()
{
	curl_global_cleanup();
}

std::string httpIO::Get(std::string sUrl)
{
	CURL *pCurl;
	CURLcode cRes;
	std::stringstream ssOut;

	pCurl = curl_easy_init();
	if (pCurl) {
		curl_easy_setopt(pCurl, CURLOPT_URL, sUrl.c_str());
		curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, WriteData);
		curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &ssOut);

		cRes = curl_easy_perform(pCurl);
		if (cRes != CURLE_OK)
		{
			this->pLog->doLog("error", std::string("curl_easy_perform() failed: ") + curl_easy_strerror(cRes));
			curl_easy_cleanup(pCurl);
			return "";
		}
		curl_easy_cleanup(pCurl);
		return ssOut.str();
	}
	this->pLog->doLog("ERROR", WS2U8(L"curl初始化失败！"));
	return "";
}
std::string httpIO::Post(std::string sUrl, std::string sData)
{
	CURL *pCurl;
	CURLcode cRes;
	std::stringstream ssOut;

	pCurl = curl_easy_init();
	if (pCurl) {
		curl_easy_setopt(pCurl, CURLOPT_URL, sUrl.c_str());
		curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, sData.c_str());
		curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, WriteData);
		curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &ssOut);

		cRes = curl_easy_perform(pCurl);
		if (cRes != CURLE_OK)

		{
			this->pLog->doLog("error", std::string("curl_easy_perform() failed: ") + curl_easy_strerror(cRes));

			curl_easy_cleanup(pCurl);
			return "";
		}
		curl_easy_cleanup(pCurl);
		return ssOut.str();
	}
	this->pLog->doLog("error", WS2U8(L"curl初始化失败！"));
	return "";
}
BOOL httpIO::Download(std::string sUrl, std::string sFilename)
{
	CURL *pCurl;
	CURLcode cRes;
	FILE* fpFile;
	std::stringstream ssOut;

	pCurl = curl_easy_init();
	if (pCurl) {
		_wfopen_s(&fpFile, U82WS(sFilename).c_str(), L"wb");
		if (fpFile)
		{
			curl_easy_setopt(pCurl, CURLOPT_URL, sUrl.c_str());
			curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L);
			curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, DownloadData);
			curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, fpFile);
			curl_easy_setopt(pCurl, CURLOPT_NOPROGRESS, 1L);

			cRes = curl_easy_perform(pCurl);
			if (cRes != CURLE_OK)
			{
				this->pLog->doLog("error", std::string("curl_easy_perform() failed: ") + curl_easy_strerror(cRes));
				fclose(fpFile);
				curl_easy_cleanup(pCurl);
				return FALSE;
			}
			fclose(fpFile);
			curl_easy_cleanup(pCurl);
			return TRUE;
		}
		this->pLog->doLog("error", WS2U8(L"下载资源目标文件被占用！"));
		curl_easy_cleanup(pCurl);
	}
	this->pLog->doLog("error", WS2U8(L"curl初始化失败！"));
	return FALSE;
}



bool httpIO::SendRabbitmq(std::string sHostname, 
	std::string sPort,
	std::string sVhost,
	std::string sUsername,
	std::string sPassword,
	std::string sQueue,
	std::string sExchange,
	std::string sRoutingkey,
	std::string sMessage)
{
	// TODO: 在此处添加实现代码.
	int iStatus;
	amqp_socket_t *pSocket = NULL;
	amqp_connection_state_t cConn;
	cConn = amqp_new_connection();
	pSocket = amqp_tcp_socket_new(cConn);
	if (!pSocket)
	{
		this->pLog->doLog("[ERROR]", WS2U8(L"发送至rabbitmq，socket建立错误"));
		return false;
	}

	iStatus = amqp_socket_open(pSocket, sHostname.c_str(), std::stoi(sPort));
	if (iStatus)
	{
		this->pLog->doLog("[ERROR]", WS2U8(L"发送至rabbitmq，status错误"));
		return false;
	}

	if (amqp_login(cConn, sVhost.c_str(), 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, sUsername.c_str(), sPassword.c_str()).reply_type != 1)
	{
		this->pLog->doLog("[ERROR]", WS2U8(L"发送至rabbitmq，login错误"));
		return false;
	}

	amqp_channel_open(cConn, 1);
	if (amqp_get_rpc_reply(cConn).reply_type != 1)
	{
		this->pLog->doLog("[ERROR]", WS2U8(L"发送至rabbitmq，reply错误"));
		return false;
	}
	amqp_queue_bind(cConn, 1, amqp_cstring_bytes(sQueue.c_str()), amqp_cstring_bytes(sExchange.c_str()), amqp_cstring_bytes(sRoutingkey.c_str()), amqp_empty_table);
	{
		amqp_basic_properties_t props;
		props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
		props.content_type = amqp_cstring_bytes("text/plain");
		props.delivery_mode = 2;

		if (amqp_basic_publish(cConn, 1, amqp_cstring_bytes(sExchange.c_str()), amqp_cstring_bytes(sRoutingkey.c_str()), 0, 0, &props, amqp_cstring_bytes(sMessage.c_str())) < 0)
		{
			this->pLog->doLog("[ERROR]", WS2U8(L"发送至rabbitmq，publish错误"));
			return false;
		}
	}

	if (amqp_channel_close(cConn, 1, AMQP_REPLY_SUCCESS).reply_type != 1)
	{
		this->pLog->doLog("[ERROR]", WS2U8(L"rabbitmq，channel关闭错误"));
		return false;
	}
	if (amqp_connection_close(cConn, AMQP_REPLY_SUCCESS).reply_type != 1) {
		this->pLog->doLog("[ERROR]", WS2U8(L"rabbitmq，connection关闭错误"));
		return false;
	}
	if (amqp_destroy_connection(cConn) < 0)
	{
		this->pLog->doLog("[ERROR]", WS2U8(L"rabbitmq，destroy关闭错误"));
		return false;
	}
	return true;
}
