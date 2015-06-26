#pragma once

#include "AThread.h"
#include "Scheduler.h"
#include "TEXTask.h"
#include "WaitForEvent.h"
#include "WaitForTime.h"
#include "WaitForCondition.h"
#include "WaitForIO.h"

#include <concurrent_queue.h>
#include <concurrent_unordered_map.h>

#include <functional>
#include <map>
#include <memory>

namespace fiberdyne {

// Task EXecution environment
class TEX
{
private:
	static unsigned int __stdcall Main(void *);

	bool m_ContinueRunning;
	std::unique_ptr<AThread> m_Thread;
	void * m_MainFiber;

	Scheduler m_Scheduler;
	WaitForEvent m_Alerter;
	WaitForTime	m_TimeHandler;
	WaitForCondition m_ConditionHandler;
	std::shared_ptr<WaitForIO> m_IOHandler;

	// Don't allow copy 
	// Don't allow assignment
	TEX(const TEX &);
	TEX & operator= (const TEX &);

protected:
	//WaitForEvent & GetAlertHandler() {return m_Alerter;}
	WaitForTime & GetTimeHandler() {return m_TimeHandler;}
	//WaitForCondition & GetConditionHandler() {return m_ConditionHandler;}

	virtual void OnIdle();

public:	
	TEX();
	virtual ~TEX();

	virtual bool setup();
	virtual void run();

	// Task management
	Scheduler & GetScheduler() {return m_Scheduler;}


public:
	// Task management interface
	std::shared_ptr<TEXTask> currentTask();
	void scheduleTask(const std::shared_ptr<TEXTask> &atask);
	std::shared_ptr<TEXTask> spawn(std::function<void(TEXTask *)> func);
	std::shared_ptr<TEXTask> spawnTask(const std::shared_ptr<TEXTask> &atask);

	// Tasicle management
	void waitForExit();
	void exit();

	// Convenience methods
	void sleep(const double millis);
	std::shared_ptr<TEXTask> delay(std::function<void(TEXTask *)> func, const double delay);
	std::shared_ptr<TEXTask> periodic(std::function<void(TEXTask *)> func, const double delay);

	// Alert management
	void waitForAlert(const std::string &alertName);
	void alertOne(const std::string &alertName);
	void alertAll(const std::string &alertName);

	// Conditionals
	void waitForCondition(std::function<bool()> func);
	std::shared_ptr<TEXTask> once(const std::function<bool()> &predicate, std::function<void()> func);
	std::shared_ptr<TEXTask> whenever(const std::function<bool()> &predicate, std::function<void()> func);

	// IO
	void watchForIO(HANDLE rawhandle, uintptr_t key, int &error);
	void waitForIO(HANDLE rawhandle, const std::shared_ptr<IOOverlapped> &overlap);

public:
	static TEX * getSingleton();
	static std::shared_ptr<TEX> createSingleton();

protected:
	static concurrency::concurrent_unordered_map<DWORD, TEX *> *m_map;


};

}
