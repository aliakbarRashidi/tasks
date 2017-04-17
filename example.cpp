/*
 * copyright: 2014-2017
 * name : Francis Banyikwa
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "task.h"
#include "example.h"

#include <QString>
#include <QMetaObject>
#include <QCoreApplication>
#include <iostream>

static void _testing_task_await() ;
static void _testing_task_future_all() ;
static void _testing_multiple_tasks() ;

void example::start()
{
	QMetaObject::invokeMethod( this,"run",Qt::QueuedConnection ) ;
}

QString _longRunningTask()
{
	return "abc" ;
}

static void _printThreadID()
{
	std::cout << "Thread id: " << QThread::currentThreadId() << std::endl ;
}

static void _useResult( const QString& e )
{
	Q_UNUSED( e ) ;
}

/*
 * A sample use where a task is run on separate thread and return a value and the returned
 * value is used on another task run on the original thread
 */
static void _test_run_then()
{
	std::cout<< "Testing Task::run().then()" << std::endl ;

	/*
	 * print the thread id to know we are on what thread.
	 * We are on the original thread here.
	 */
	_printThreadID() ;

	Task::run<QString>( [](){

		/*
		 * print the thread id to know we are on what thread.
		 * We are on a separate thread here
		 */
		_printThreadID() ;

		/*
		 * Do a time consuming process on a separate thread and return its result
		 */
		return _longRunningTask() ;

	} ).then( []( QString r ){

		/*
		 * use the returned value by the previous task
		 */
		_useResult( r ) ;
		/*
		 * print the thread id to know we are on what thread.
		 * We are back on the original thread and we will get here as a continuation of the task
		 * that completed above
		 */
		_printThreadID() ;

		/*
		 * moving on to the next test.
		 */
		_testing_task_await() ;
	} ) ;
}

/*
 * Task::await() function below does the following:
 * 1. suspends the "_testing_task_await" method at a point where Task::await() method is called.
 * 2. creates a new thread.
 * 3. runs the _longRunningTask method in the new thread.
 * 4. store the result of  _longRunningTask function in r.
 * 5. resumes "_testing_task_await" method.
 */
static void _testing_task_await()
{
	std::cout<< "Testing Task::run().then()" << std::endl ;

	QString e = Task::await<QString>( _longRunningTask ) ;

	std::cout << e.toLatin1().constData() << std::endl ;

	/*
	 * moving on to the next test.
	 */
	_testing_task_future_all() ;
}

/*
 * Task::run() function below does the following:
 * 1. Collects a bunch of tasks.
 * 2. Runs each task on its own thread.
 * 3. Returns a future that holds all above tasks.
 * 4. .await() can be called on the future to suspend the current thread at a point
 *    where this medhod is called to wait for all tasks to finish.
 * 5. .then() can be called on the future to register an event to be called when all
 *    tasks finish running.
 */
static void _testing_task_future_all()
{
	auto fn1 = [](){ _printThreadID() ; } ;
	auto fn2 = [](){ _printThreadID() ; } ;
	auto fn3 = [](){ _printThreadID() ; } ;

	std::cout<< "Testing Task::run().await() multiple tasks" << std::endl ;

	Task::future<void>& e = Task::run( fn1,fn2,fn3 ) ;

	e.await() ;

	std::cout<< "Testing Task::run().then() multiple tasks" << std::endl ;

	Task::future<void>& f1 = Task::run( fn1 ) ;
	Task::future<void>& f2 = Task::run( fn2 ) ;
	Task::future<void>& f3 = Task::run( fn3 ) ;

	Task::future<void>& s = Task::run( f1,f2,f3 ) ;

	s.then( [](){

		/*
		 * moving on to the next test.
		 */
		_testing_multiple_tasks() ;
	} ) ;
}

/*
 * Task::run() function below does the following:
 * 1. Collects a bunch of tasks and their continuations.
 * 2. Runs each task on its own thread.
 * 3. On completion of each task,run its continuation on the current thread.
 * 4. Returns a future that holds all above tasks.
 * 5. .await() can be called on the future to suspend the current thread at a point
 *    where this medhod is called to wait for all tasks and their continuations to finish.
 * 6. .then() can be called on the future to register an event to be called when all
 *    tasks and their continuations finish.
 */
static void _testing_multiple_tasks()
{
	std::cout<< "Testing multiple tasks without continuation arguments" << std::endl ;

	auto fna1 = [](){ _printThreadID(); } ;
	auto fna2 = [](){ _printThreadID(); } ;
	auto fna3 = [](){ _printThreadID(); } ;

	auto ra1 = [](){ std::cout << "r1" << std::endl ; } ;
	auto ra2 = [](){ std::cout << "r2" << std::endl ; } ;
	auto ra3 = [](){ std::cout << "r3" << std::endl ; } ;

	Task::future<void>& e = Task::run( Task::void_pair{ fna1,ra1 },
					   Task::void_pair{ fna2,ra2 },
					   Task::void_pair{ fna3,ra3 } ) ;

	e.await() ;

	std::cout<< "Testing multiple tasks with continuation arguments" << std::endl ;

	auto fn1 = [](){ _printThreadID() ; return 0 ; } ;
	auto fn2 = [](){ _printThreadID() ; return 0 ; } ;
	auto fn3 = [](){ _printThreadID() ; return 0 ; } ;

	auto r1 = []( int ){ std::cout << "r1" << std::endl ; } ;
	auto r2 = []( int ){ std::cout << "r2" << std::endl ; } ;
	auto r3 = []( int ){ std::cout << "r3" << std::endl ; } ;

	Task::future<int>& s = Task::run( Task::pair<int>{ fn1,r1 },
					  Task::pair<int>{ fn2,r2 },
					  Task::pair<int>{ fn3,r3 } ) ;

	s.then( [](){ QCoreApplication::quit() ; } ) ;
}

void example::run()
{
	_test_run_then() ;
}

