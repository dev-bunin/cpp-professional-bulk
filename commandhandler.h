#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <string>
#include <queue>

class QueueInterface {
public:
	virtual ~QueueInterface()  = default;

	virtual bool isFinished() = 0;
	virtual void exec() = 0;
	virtual void addCommand(const std::string &command) = 0;
};

class QueueBase : public QueueInterface {
public:
	virtual bool isFinished();
	virtual void exec();
	virtual void addCommand(const std::string &command);

protected:
	std::queue<std::string> m_commands;
};

class DynamicQueue : public QueueBase {
public:
	DynamicQueue() = default;

	bool isFinished() override;
	void addCommand(const std::string &command) override;
	void exec() override;
private:
	int m_deep = 0;
};

class LimitedQueue : public QueueBase {
public:
	LimitedQueue(int maxCommandCount);

	void addCommand(const std::string &command) override;
private:
	int m_maxCommandCount;
};

void parseCommand(const int maxCommandCount, std::istream &istream);

#endif // COMMANDHANDLER_H
