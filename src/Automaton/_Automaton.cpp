/*
 * State.cpp
 *
 *  Created on: Aug 27, 2016
 *      Author: root
 */

#include "_Automaton.h"

namespace kai
{

_Automaton::_Automaton()
{
	_ThreadBase();

	m_numState = 0;
	m_iState = 0;

}

_Automaton::~_Automaton()
{
	// TODO Auto-generated destructor stub
}

bool _Automaton::init(JSON* pJson, string automatonName)
{
	if (!pJson)return false;

	double FPS = DEFAULT_FPS;
	CHECK_INFO(pJson->getVal("AM_"+automatonName+"_FPS", &FPS));
	this->setTargetFPS(FPS);

	//init basic params
	m_numState = 0;

	//read in configuration
	int i,k;
	string name;
	string condStr;
	string typeStr;
	string nameS;
	string nameST;
	string nameSTC;

	do
	{
		nameS = "AM_"+automatonName+"_STATE" + i2str(m_numState);
		if(!pJson->getVal(nameS+"_NAME", &name))break;

		State* pS = addState();
		if(pS==NULL)return false;
		pS->m_name = name;

		do
		{
			nameST = nameS+"_T"+i2str(pS->m_numTransition);
			if(!pJson->getVal(nameST+"_TO", &k))break;

			Transition* pT = pS->addTransition();
			if(pT==NULL)return false;
			pT->m_transitToID = k;

			do
			{
				nameSTC = nameST+"_COND"+i2str(pT->m_numCond);

				if(!pJson->getVal(nameSTC+"_TYPE", &typeStr))break;
				if(!pJson->getVal(nameSTC+"_COND", &condStr))break;

				ConditionBase* pTC;

				if(typeStr=="ii")
					pTC = pT->addConditionII();
				else if(typeStr=="ff")
					pTC = pT->addConditionII();

				if(pTC==NULL)return false;

				if(condStr=="bt")pTC->m_condition=bt;
				else if(condStr=="beq")pTC->m_condition=beq;
				else if(condStr=="st")pTC->m_condition=st;
				else if(condStr=="seq")pTC->m_condition=seq;
				else if(condStr=="eq")pTC->m_condition=eq;
				else if(condStr=="neq")pTC->m_condition=neq;
				else pTC->m_condition=DEFAULT;

				pJson->getVal(nameSTC+"_NAME1", &pTC->m_namePtr1);
				pJson->getVal(nameSTC+"_NAME2", &pTC->m_namePtr2);
//				if(!pJson->getVal(nameSTC+"_CONST2F", &pTC->m_pFConst2))
//				{
//					pJson->getVal(nameSTC+"_CONST2I", &pTC->m_pIConst2);
//				}

			}while(1);

		}while(1);

	}while(1);

	return true;
}

bool _Automaton::start(void)
{
	//Start thread
	m_bThreadON = true;
	int retCode = pthread_create(&m_threadID, 0, getUpdateThread, this);
	if (retCode != 0)
	{
		LOG(ERROR)<< "Return code: "<< retCode << " in _State::start().pthread_create()";
		m_bThreadON = false;
		return false;
	}

	LOG(INFO)<< "AutoPilot.start()";

	return true;
}

void _Automaton::update(void)
{
	while (m_bThreadON)
	{
		this->autoFPSfrom();

		updateAll();

		this->autoFPSto();
	}

}

void _Automaton::updateAll(void)
{

}


State* _Automaton::addState(void)
{
	if(m_numState >= NUM_STATE)return NULL;

	State** ppS = &m_pState[m_numState];
	*ppS = new State();
	if(*ppS==NULL)return NULL;

	m_numState++;
	return *ppS;
}

bool _Automaton::setState(int iState)
{
	if(iState >= m_numState)return false;
	if(iState < 0)return false;

	m_iState = iState;
	return true;
}

bool _Automaton::checkDiagram(void)
{
	return true;
}



} /* namespace kai */
