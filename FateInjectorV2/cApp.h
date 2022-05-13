#pragma once


#include "cMain.h"


class cApp : public wxApp
{
public:
	cApp();
	~cApp();


	cMain* m_frame1;

private:

public:
	virtual bool OnInit();
};
 
extern cMain* ref;