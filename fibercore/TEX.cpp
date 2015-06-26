
#include "TEX.h"

#include <iostream>

//
// References
//
namespace fiberdyne {

// Thread singleton TEX environment for a thread
concurrency::concurrent_unordered_map<DWORD, TEX *> * TEX::m_map = new concurrency::concurrent_unordered_map<DWORD, TEX *>();

unsigned int __stdcall TEX::Main(void *context)
{
#ifdef _DEBUG_VERBOSE
	std::cout << "TEX::Main() - BEGIN" << std::endl;
#endif

	TEX *self = reinterpret_cast<TEX*>(context);

#ifdef _DEBUG_VERBOSE
	std::cout << "  Thread Start Signalled" << std::endl;
#endif

	try
	{
        // Indicate to sub-classes that the main thread 
        // routine has started.
		if (!self->setup()) {
			return -1;
		}

		self->run();
	}
	catch (...)
	{
        // TODO: Do some more intelligent handling here
      # ifdef _DEBUG
        std::cout << "Caught exception in TEX." << std::endl;
      # endif
	}	
	
	// When using the stdc library thread routines,
	// endthreadex is called automatically once this
	// routine returns.  This is necessary to do proper cleanup 
	// putting handle into signaled state in particular
	// we could call endthreadex here explicitly, but it is not actually necessary
	//_endthreadex( 0 );

	return 0;
}


std::shared_ptr<TEX> TEX::createSingleton()
{
	// Create an instance
	auto anInstance = std::make_shared<TEX>();
	anInstance->m_Thread.reset( new AThread(TEX::Main, anInstance.get()) );

	return anInstance;
}

TEX * TEX::getSingleton()
{
	DWORD myThreadId = ::GetCurrentThreadId();
	TEX *retVal = nullptr;
	auto iter = TEX::m_map->find(myThreadId);
	if (iter != m_map->end()) {
		return iter->second;
	} 
	
	return nullptr;
/*
	// Turn current thread into a fiber
	auto rawhandle = ConvertThreadToFiber(nullptr);

	// Wrap the fiber handle up into AFiberProcessor object
	retVal = new TEX();

	m_map->insert(std::make_pair(myThreadId, retVal)); 

	return retVal;
*/
}

TEX::TEX()
	: m_TimeHandler(m_Scheduler)
	, m_Alerter(m_Scheduler)
	, m_ConditionHandler(m_Scheduler)
	, m_IOHandler(nullptr)
	, m_ContinueRunning(true)
{
	m_IOHandler = std::make_shared<WaitForIO>(m_Alerter);
}


TEX::~TEX()
{
}

bool TEX::setup()
{
	// Turn current thread into a fiber
	m_MainFiber = ConvertThreadToFiber(nullptr);

	// Add current instance to table of thread singletons
	DWORD myThreadId = ::GetCurrentThreadId();
	m_map->insert(std::make_pair(myThreadId, this)); 

	return true;
}


void TEX::OnIdle()
{
	// do nothing
}



void TEX::run()
{
	std::cout << "TEX::run() - BEGIN" << std::endl;

	while (m_ContinueRunning) {
		m_TimeHandler();
		m_ConditionHandler();
		//m_IOHandler->operator()();		// not needed in primary tasicle

		m_Scheduler();

		OnIdle();
	}

	std::cout << "TEX::run() - EXITING" << std::endl;
}


void TEX::exit()
{
	m_ContinueRunning = false;
}

void TEX::waitForExit()
{
	m_Thread->join();
}


// Task management
std::shared_ptr<TEXTask> TEX::currentTask()
{
	return m_Scheduler.GetCurrentTask();
}

void TEX::scheduleTask(const std::shared_ptr<TEXTask> &atask)
{
	m_Scheduler.ScheduleTask(atask);
}

std::shared_ptr<TEXTask> TEX::spawnTask(const std::shared_ptr<TEXTask> &atask)
{
	if (nullptr == atask) {
		return nullptr;
	}
	
	scheduleTask(atask);

	return atask;
}


std::shared_ptr<TEXTask> TEX::spawn(std::function<void(TEXTask *)> func)
{
	auto atask = std::make_shared<RoutineTask>(func);
	return spawnTask(atask);
}

/*
void TEX::yield()
{
	auto atask = currentTask();
	if (nullptr == atask) {
		return ;
	}
	
	atask->yield();
}
*/

void TEX::sleep(const double millis)
{
	GetTimeHandler().yieldForTime(millis, currentTask());
}

std::shared_ptr<TEXTask> TEX::delay(std::function<void(TEXTask *)> func, const double millis)
{
	auto atask = std::make_shared<RoutineTask>([=](TEXTask *atask){
		sleep(millis);
		func(atask);	
	});

	return spawnTask(atask);
}

std::shared_ptr<TEXTask> TEX::periodic(std::function<void(TEXTask *)> func, const double millis)
{
	auto atask = std::make_shared<RoutineTask>([=](TEXTask *atask){
		while(true) {
			sleep(millis);
			func(atask);
		}
	});

	return spawnTask(atask);
}


// Alert Management
void TEX::waitForAlert(const std::string &alertName)
{
	m_Alerter.yieldForEvent(alertName, currentTask());
}

void TEX::alertOne(const std::string &alertName)
{
	m_Alerter.alertOne(alertName);
}

void TEX::alertAll(const std::string &alertName)
{
	m_Alerter.alertAll(alertName);
}


// Conditionals
void TEX::waitForCondition(std::function<bool()> pred)
{
	m_ConditionHandler.yieldForCondition(pred, currentTask());
}


std::shared_ptr<TEXTask> TEX::once(const std::function<bool()> &predicate, std::function<void()> func)
{
	return spawn([=](TEXTask *tsk){
		waitForCondition(predicate);
		func();
	});
}

std::shared_ptr<TEXTask> TEX::whenever(const std::function<bool()> &predicate, std::function<void()> func)
{
	return spawn([=](TEXTask *tsk){
		while(true) {
			waitForCondition(predicate);
			func();
		}
	});
}



// IO
void TEX::watchForIO(HANDLE rawhandle, uintptr_t key, int &error)
{
	m_IOHandler->WatchForIOEvents(rawhandle, key, error);
}

void TEX::waitForIO(HANDLE rawhandle, const std::shared_ptr<IOOverlapped> &overlap)
{
	m_IOHandler->yieldForIO(rawhandle, overlap, currentTask());
}

}
