/**
 * @file main.c
 *
 * @description This file defines WebPA's main function
 *
 * Copyright (c) 2015  Comcast
 */
#include "stdlib.h"
#include "signal.h"
#include <curl/curl.h>
#ifdef INCLUDE_BREAKPAD
#include "breakpad_wrapper.h"
#endif
#include "webcfg.h"
#include "webcfg_log.h"
#include "webcfg_rbus.h"

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
#ifndef INCLUDE_BREAKPAD
static void sig_handler(int sig);
#endif

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

int main()
{
	char RfcEnable[64];
	memset(RfcEnable, 0, sizeof(RfcEnable));
	char* strValue = NULL;
	int ret = 0;
#ifdef INCLUDE_BREAKPAD
    breakpad_ExceptionHandler();
#else
	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);
	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);
	signal(SIGSEGV, sig_handler);
	signal(SIGBUS, sig_handler);
	signal(SIGKILL, sig_handler);
	signal(SIGFPE, sig_handler);
	signal(SIGILL, sig_handler);
	signal(SIGQUIT, sig_handler);
	signal(SIGHUP, sig_handler);
	signal(SIGALRM, sig_handler);
#endif
	WebcfgInfo("********** Starting component: %s **********\n ", WEBCFG_COMPONENT_NAME);
	curl_global_init(CURL_GLOBAL_DEFAULT);

	if(isRbusEnabled())
	{
		WebcfgInfo("webconfigRbusInit\n");
		webconfigRbusInit(WEBCFG_COMPONENT_NAME);
		regWebConfigDataModel();
		ret = rbus_GetValueFromDB( PARAM_RFC_ENABLE, &strValue );
		if (ret == 0)
		{
			WebcfgInfo("RFC strValue %s\n", strValue);
			if(strValue != NULL)
			{
				webcfgStrncpy(RfcEnable, strValue, sizeof(RfcEnable));
			}
		}
		if(RfcEnable[0] != '\0' && strncmp(RfcEnable, "true", strlen("true")) == 0)
		{
			if(get_global_mpThreadId() == NULL)
			{
				WebcfgInfo("WebConfig Rfc is enabled, starting initWebConfigTask\n");
				initWebConfigTask(0);
			}
			else
			{
				WebcfgInfo("Webconfig is already started, so not starting again.\n");
			}
		}
		else
		{
			WebcfgInfo("WebConfig Rfc Flag is not enabled\n");
		}
	}

	WebcfgInfo("curl_global_cleanup\n");
	curl_global_cleanup();
	while(1);
	WebcfgInfo("Exiting webconfig main thread!!\n");
	return 1;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
#ifndef INCLUDE_BREAKPAD
static void sig_handler(int sig)
{

	if ( sig == SIGINT ) 
	{
		signal(SIGINT, sig_handler); /* reset it to this function */
		WebcfgInfo("SIGINT received!\n");
		exit(0);
	}
	else if ( sig == SIGUSR1 ) 
	{
		signal(SIGUSR1, sig_handler); /* reset it to this function */
		WebcfgInfo("SIGUSR1 received!\n");
	}
	else if ( sig == SIGUSR2 ) 
	{
		WebcfgInfo("SIGUSR2 received!\n");
	}
	else if ( sig == SIGCHLD ) 
	{
		signal(SIGCHLD, sig_handler); /* reset it to this function */
		WebcfgInfo("SIGHLD received!\n");
	}
	else if ( sig == SIGPIPE ) 
	{
		signal(SIGPIPE, sig_handler); /* reset it to this function */
		WebcfgInfo("SIGPIPE received!\n");
	}
	else if ( sig == SIGALRM ) 
	{
		signal(SIGALRM, sig_handler); /* reset it to this function */
		WebcfgInfo("SIGALRM received!\n");
	}
	else 
	{
		WebcfgInfo("Signal %d received!\n", sig);
		exit(0);
	}
	
}
#endif