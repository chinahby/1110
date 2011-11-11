#ifndef SETUPDRIVER_LIB_H
#define SETUPDRIVER_LIB_H

enum ESetupResult
{
	SETUP_OK,
	SETUP_PARAM_ERROR,
	SETUP_COPYINF_FAILED,
	SETUP_REG_ERROR
};

ESetupResult SetupDriver(const char* strInfFile);


#endif