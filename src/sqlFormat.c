#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "curl.h"
#include "cstring.h"
#include "linked_list.h"
#include "cJSON.h"

static char errorBuffer[CURL_ERROR_SIZE];

static char * readFile(FILE * pfile)  
{  
	char * data;  
	int length;
	fseek(pfile, 0, SEEK_END);  
	length = ftell(pfile);  
	data = (char *)malloc((length + 1) * sizeof(char));  
	rewind(pfile);  
	length = fread(data, 1, length, pfile);  
	data[length] = '\0';  
	return data;  
}

static void printResult(FILE *file, const char * format, ...){
	va_list argp;
	char* arg;

	va_start(argp, format);
	arg = va_arg(argp, char*);

	if(file==NULL){
		fprintf(stdout, format, arg);
	}
	else
		fprintf(file, format, arg);
	va_end(argp);
}


static int readData(char *data, size_t size, size_t nmemb, CString *writerData)
{
	unsigned long sizes = size * nmemb;
	if (writerData == NULL) return 0;
	CStringNAppend(writerData, data, sizes);
	return sizes;
}

static BOOL initCurl(CURL *conn, char *url, char* param, CString *buffer)
{
	CURLcode code;

	code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
	if (code != CURLE_OK)
	{
		fprintf(stderr, "Failed to set error buffer [%d]\n", code);
		return FALSE;
	}
	code = curl_easy_setopt(conn, CURLOPT_URL, url);
	if (code != CURLE_OK)
	{
		fprintf(stderr, "Failed to set URL [%s]\n", errorBuffer);
		return FALSE;
	}

	if(param!=NULL){
		code = curl_easy_setopt(conn, CURLOPT_POSTFIELDS, param);
		if (code != CURLE_OK)
		{
			fprintf(stderr, "Failed to set post data [%s]\n", errorBuffer);
			return FALSE;
		}
	}

	code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1);
	if (code != CURLE_OK)
	{
		fprintf(stderr, "Failed to set redirect option [%s]\n", errorBuffer);
		return FALSE;
	}
	code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, readData);
	if (code != CURLE_OK)
	{
		fprintf(stderr, "Failed to set writer [%s]\n", errorBuffer);
		return FALSE;
	}
	code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, buffer);
	if (code != CURLE_OK)
	{
		fprintf(stderr, "Failed to set write data [%s]\n", errorBuffer);
		return FALSE;
	}

	return TRUE;
}

static char* sendPost(char* url, char* formatSql, char* formatOptions){
	CURL *conn = NULL;
	CURLcode code;
	CString* formatResultBuffer = CStringNew();
	CString* paramBuffer;
	char *formatResult;
	char *escapeFormatSql, *escapeFormatOptions;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	conn = curl_easy_init();
	if (conn == NULL)
	{
		fprintf(stderr, "Failed to create CURL connection\n");
		return NULL;
	}

	paramBuffer= CStringNew();
	CStringAppend(paramBuffer, "rqst_db_vendor=oracle&rqst_isf_client=ultraedit_client&rqst_input_sql=");
	escapeFormatSql = curl_easy_escape(conn, formatSql, 0);
	CStringAppend(paramBuffer, escapeFormatSql);
	curl_free(escapeFormatSql);

	if(formatOptions!=NULL){
		CStringAppend(paramBuffer, "&rqst_formatOptions=");
		escapeFormatOptions = curl_easy_escape(conn, formatOptions, 0);
		CStringAppend(paramBuffer, escapeFormatOptions);
		curl_free(escapeFormatOptions);
	}

	if (!initCurl(conn, url, paramBuffer->buffer, formatResultBuffer))
	{
		fprintf(stderr, "Connection initialization failed.\n");
		CStringDelete(formatResultBuffer);
		return NULL;
	}


	code = curl_easy_perform(conn);

	if (code != CURLE_OK)
	{
		fprintf(stderr, "Failed to get '%s' [%s]\n", url, errorBuffer);
		CStringDelete(formatResultBuffer);
		return NULL;
	}

	curl_easy_cleanup(conn);

	formatResult = formatResultBuffer->buffer;

	CStringDelete(paramBuffer);
	CStringDeleteWithoutBuffer(formatResultBuffer);

	return formatResult;
}

int main(int argc,char *argv[])
{
	int argIndex, index;
	List *argList;
	FILE *sqlFile = NULL, *formatOptionsFile = NULL;
	char *sqlText = NULL;
	CString *formatSql=NULL, *formatOptions = NULL;
	char* response;
	cJSON *json, *formatItem;

	argList = createStringList(FALSE);

	for(index=0;index<argc;index++){
		argList->add(argList, argv[index]);
	}

	argIndex = argList->indexOf(argList, "-f");
	if(argIndex!=-1 && argList->size(argList) >argIndex+1){
		sqlFile=fopen(argv[argIndex+1],"r");
		if(sqlFile == NULL){
			fprintf(stderr,"script file %s doesn't exist!\n", argv[1] );
			argList->dispose(argList);
			return 1;
		}
		else{
			char* content = readFile(sqlFile);
			formatSql = CStringNew();
			CStringAppend(formatSql, content);
			free(content);
			fclose(sqlFile);
		}
	}

	argIndex = argList->indexOf(argList, "-s");
	if(argIndex!=-1 && argList->size(argList) >argIndex+1){
		formatSql = CStringNew();
		CStringAppend(formatSql, argv[argIndex+1]);
	}

	if(formatSql == NULL){ 
		fprintf(stderr,"Invalid argument!\n", argv[1] );
		argList->dispose(argList);
		return 1;
	}

	argIndex = argList->indexOf(argList, "-o");
	if(argIndex!=-1 && argList->size(argList) >argIndex+1){
		formatOptionsFile=fopen(argv[argIndex+1],"r");
	}else{
		formatOptionsFile=fopen("./fo.json","r");
	} 

	if(formatOptionsFile != NULL){
		char* content = readFile(formatOptionsFile);
		formatOptions = CStringNew();
		CStringAppend(formatOptions, content);
		free(content);
		fclose(formatOptionsFile);
	}

	argList->dispose(argList);

	response = sendPost("http://www.gudusoft.com/format.php",formatSql->buffer, formatOptions==NULL? NULL: formatOptions->buffer);

	if(response!=NULL){
		json = cJSON_Parse(response);
		if(json!=NULL){
			formatItem = cJSON_GetObjectItem(json, "rspn_formatted_sql");
			if(formatItem!=NULL){
				printResult(NULL, "%s", formatItem->valuestring);
				free(formatItem);
			}
			else{
				fprintf(stderr,"Failed to format sql!");
			}
			free(json);
		}
		else{
			fprintf(stderr,"Failed to format sql!" );
		}
		free(response);
	}
	
	if(formatOptions!=NULL)
		CStringDelete(formatOptions);

	CStringDelete(formatSql);

	return 0;
}