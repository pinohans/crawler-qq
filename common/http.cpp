#include "stdafx.h"
#include "http.h"

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

BOOL troycrawler::http::init()
{
	curl_global_init(CURL_GLOBAL_ALL);
	return TRUE;
}

std::string troycrawler::http::get(std::string _sUrl)
{
	CURL *pCurl;
	CURLcode cRes;
	std::stringstream ssOut;

	pCurl = curl_easy_init();
	if (pCurl) {
		curl_easy_setopt(pCurl, CURLOPT_URL, _sUrl.c_str());
		curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, WriteData);
		curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &ssOut);

		cRes = curl_easy_perform(pCurl);
		if (cRes != CURLE_OK)
		{
			log::error(std::string("curl_easy_perform() failed: ") + curl_easy_strerror(cRes));
			curl_easy_cleanup(pCurl);
			return "";
		}
		curl_easy_cleanup(pCurl);
		return ssOut.str();
	}
	log::error(WS2U8(L"curl初始化失败！"));
	return "";
}

std::string troycrawler::http::post(std::string _sUrl, std::string _sData)
{
	CURL *pCurl;
	CURLcode cRes;
	std::stringstream ssOut;

	pCurl = curl_easy_init();
	if (pCurl) {
		curl_easy_setopt(pCurl, CURLOPT_URL, _sUrl.c_str());
		curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, _sData.c_str());
		curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, WriteData);
		curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &ssOut);

		cRes = curl_easy_perform(pCurl);
		if (cRes != CURLE_OK)
		{
			log::error(std::string("curl_easy_perform() failed: ") + curl_easy_strerror(cRes));

			curl_easy_cleanup(pCurl);
			return "";
		}
		curl_easy_cleanup(pCurl);
		return ssOut.str();
	}
	log::error(WS2U8(L"curl初始化失败！"));
	return "";
}

BOOL troycrawler::http::quit()
{
	curl_global_cleanup();
	return TRUE;
}
