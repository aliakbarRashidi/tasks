

Asynchronous programming in Qt/C++ using tasks,continuations and resumable functions.

This project is inspired by this[1] video on channel9.

The project seeks to do async based programming in Qt/C++ using modern C++ with lambdas.

The project provides two sets of APIs.

1. Task::run().then() API provides async based programming with continuation[4].

2. Task::await() API provides async based programming without "hanging" the current thread through usage of resumable functions[5].

Example use case for the Task::run().then() API can be found here[0]. Additional examples are [2] where an API is
declared and [3] where the API is used.

Example use case of the Task::await() is here[6] where a function call "blocks" waiting for a result without "hanging" the entire thread.

[0] https://github.com/mhogomchungu/tasks/blob/master/example.cpp

[1] http://channel9.msdn.com/Blogs/Charles/Asynchronous-Programming-for-C-Developers-PPL-Tasks-and-Windows-8

[2] https://github.com/mhogomchungu/zuluCrypt/blob/d0439a4e36521e42fa9392b82dcefd3224d53334/zuluMount-gui/zulumounttask.h#L61

[3] https://github.com/mhogomchungu/zuluCrypt/blob/d0439a4e36521e42fa9392b82dcefd3224d53334/zuluMount-gui/mainwindow.cpp#L812

[4] Disscussion about this can be found on the following link among other places: http://isocpp.org/files/papers/N3558.pdf

[5] Disscussion about this can be found on the following link among other places: http://isocpp.org/files/papers/N3564.pdf

[6] https://github.com/mhogomchungu/zuluCrypt/blob/5ecd607054d458afc7029287a2259ad3c5e7cd8a/zuluMount-gui/mainwindow.cpp#L560
