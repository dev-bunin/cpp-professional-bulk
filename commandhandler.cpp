#include "commandhandler.h"

#include <memory>
#include <iostream>
#include <fstream>

using namespace std;

QueueBase::QueueBase()
{
	m_startTime = chrono::system_clock::now();
}

bool QueueBase::isFinished()
{
	return m_commands.empty();
}

void QueueBase::exec()
{
	if (m_commands.empty()) {
		return;
	}
	bool firstIt = true;
	string result;
	for (; !m_commands.empty(); m_commands.pop()) {
		if (!firstIt) {
			result += ", ";
		}
		firstIt = false;
		result += m_commands.front();
	}
	cout << result << endl;
	saveToFile(result);
}

void QueueBase::addCommand(const std::string &command)
{
	m_commands.push(command);
}

void QueueBase::saveToFile(const std::string &result)
{
	using namespace chrono;
	std::ofstream outFile;
	string fileName(to_string(duration_cast<seconds>
							  (m_startTime.time_since_epoch()).count()) + ".log");
	outFile.open(fileName);
	if (outFile.is_open()) {
		outFile << result;
	}
	outFile.close();
}

LimitedQueue::LimitedQueue(int maxCommandCount)
	: m_maxCommandCount(maxCommandCount)
{
}

void LimitedQueue::addCommand(const string &command)
{
	QueueBase::addCommand(command);
	if (m_commands.size() >= m_maxCommandCount) {
		exec();
	}
}

bool DynamicQueue::isFinished()
{
	return (m_deep == 0) && QueueBase::isFinished();
}

void DynamicQueue::addCommand(const std::string &command)
{
	if (command == "{") {
		// Вложенные блоки игнорируется, но надо отслеживать что все они завершены
		++m_deep;
		return;
	}
	if (command == "}") {
		--m_deep;
		if (m_deep == 0) {
			exec();
		}
		return;
	}
	QueueBase::addCommand(command);
}

void DynamicQueue::exec()
{
	if (m_deep != 0) {
		// Если логический блок не закончен, то блок игнорируется
		return;
	}
	QueueBase::exec();
}

void parseCommand(const int maxCommandCount, std::istream &istream)
{
	enum CommandType {
		None,
		Queue,
		Limited
	};

	unique_ptr<QueueBase> command; // Переделать на умные указатели
	CommandType m_type = CommandType::None;

	string line;
	while(getline(istream, line)) {
		if (line.empty()) {
			break;
		} else if (line == "{") {
			// Инициализация при необходимости
			switch (m_type) {
			case CommandType::Limited:
				command->exec();
//				FALLTHROUGH

			case CommandType::None:
				m_type = CommandType::Queue;
				command.reset(new DynamicQueue());
				break;

			case CommandType::Queue:
				// Ничего не делаем
				break;
			}
		} else if (line == "}") {
			switch (m_type) {
			case CommandType::Limited:
			case CommandType::None:
				// Что то пошло не так
				return;

			case CommandType::Queue:
				break;
			}
		} else {
			if (!command) {
				m_type = CommandType::Limited;
				command.reset(new LimitedQueue(maxCommandCount));
			}
		}
		command->addCommand(line);

		// Если очередь команд завершилась, удаляем
		if (command && command->isFinished()) {
			m_type = CommandType::None;
			command.reset();
		}
	}

	if (command) {
		command->exec();
	}
}
