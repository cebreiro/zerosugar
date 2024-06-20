#include "execute_command.h"

#include <boost/process.hpp>

namespace zerosugar
{
    auto ExecuteCommand(const std::string& cmd) -> std::vector<std::string>
    {
        boost::process::ipstream pipeStream;
        boost::process::child child(cmd, boost::process::std_out > pipeStream);

        std::vector<std::string> result;

        std::string line;
        while (pipeStream && std::getline(pipeStream, line) && !line.empty())
        {
            result.push_back(std::move(line));
        }

        child.wait();

        return result;
    }
}
