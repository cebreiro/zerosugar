#include <cstdint>
#include <array>
#include <span>
#include <string>
#include <format>
#include <iostream>
#include <boost/lexical_cast.hpp>

#include "zerosugar/sl/database/connection/connection_pool.h"
#include "zerosugar/sl/database/connection/connection_pool_option.h"
#include "zerosugar/sl/database/helper/transaction.h"
#include "zerosugar/sl/database/generated/account_table.h"
#include "zerosugar/sl/database/generated/character_table.h"
#include "zerosugar/sl/database/generated/item_table.h"
#include "zerosugar/sl/database/generated/skill_table.h"

bool InitializeDatabase(boost::mysql::tcp_ssl_connection& connection);

int main(int argc, char* argv[])
{
    using namespace zerosugar;
    using namespace zerosugar::sl;

    std::span<char*> args(argv, argc);
    if (std::ssize(args) < 6)
    {
        std::cout << "usage: this.exe <user> <password> <database> <address> <port>\n";

        ::system("pause");
        return 1;
    }

    std::string user(args[1]);
    std::string password(args[2]);
    std::string database(args[3]);
    std::string address(args[4]);
    std::optional<uint16_t> port = [](const std::string& str) -> std::optional<uint16_t>
        {
            try
            {
                return boost::lexical_cast<uint16_t>(str);
            }
            catch (...)
            {
                return std::nullopt;
            }
        }(args[5]);

    if (!port.has_value())
    {
        std::cout << "invalid port: " << args[5] << '\n';

        ::system("pause");
        return 1;
    }

    constexpr int64_t connectionCount = 1;
    const auto endPoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(address), *port);
    const boost::mysql::handshake_params params(user, password, database);

    execution::IExecutor& executor = StaticThreadPool::GetInstance();
    auto connectionPool = std::make_shared<db::ConnectionPool>(executor.SharedFromThis(),
        db::ConnectionPoolOption(std::move(endPoint), params, connectionCount));
    connectionPool->Start();

    db::ConnectionPool::BorrowedConnection connection = connectionPool->Lend().Get();
    if (!InitializeDatabase(*connection))
    {
        std::cout << "fail to initialize database\n";

        ::system("pause");
        return 1;
    }

    ::system("pause");
    return 0;
}

template <typename T>
bool CreateTable(boost::mysql::tcp_ssl_connection& connection)
{
    try
    {
        T table(connection);
        table.CreateTable();

        return true;
    }
    catch (const boost::mysql::error_with_diagnostics& e)
    {
        const boost::mysql::diagnostics& diagnostics = e.get_diagnostics();

        std::ostringstream oss;

        if (diagnostics.server_message().data())
        {
            oss << "server error: " << diagnostics.server_message();
        }
        else if (diagnostics.client_message().data())
        {
            oss << "client error: " << diagnostics.client_message();
        }

        oss << '\n';

        std::cout << oss.str();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what();
    }

    return false;
}

template <typename T>
bool DropTable(boost::mysql::tcp_ssl_connection& connection)
{
    try
    {
        T table(connection);
        table.DropTable();

        return true;
    }
    catch (const boost::mysql::error_with_diagnostics& e)
    {
        const boost::mysql::diagnostics& diagnostics = e.get_diagnostics();

        std::ostringstream oss;

        if (diagnostics.server_message().data())
        {
            oss << "server error: " << diagnostics.server_message();
        }
        else if (diagnostics.client_message().data())
        {
            oss << "client error: " << diagnostics.client_message();
        }

        oss << '\n';

        std::cout << oss.str();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what();
    }

    return false;
}

#define MAKE_CRATE_TABLE(table) std::make_tuple(CreateTable<table>, DropTable<table>, std::string(#table))

bool InitializeDatabase(boost::mysql::tcp_ssl_connection& connection)
{
    using namespace zerosugar;
    using namespace zerosugar::sl;

    const std::array initializers{
        MAKE_CRATE_TABLE(db::AccountTable),
        MAKE_CRATE_TABLE(db::CharacterTable),
        MAKE_CRATE_TABLE(db::CharacterStatTable),
        MAKE_CRATE_TABLE(db::CharacterJobTable),
        MAKE_CRATE_TABLE(db::ItemTable),
        MAKE_CRATE_TABLE(db::SkillTable),
    };

    db::Transaction transaction(connection);
    transaction.Start();

    for (const auto& [create, drop, tableName] : initializers | std::views::reverse)
    {
        if (!drop(connection))
        {
            std::cout << std::format("{} drop table fail\n", tableName);

            transaction.Rollback();
            return false;
        }

        std::cout << std::format("{} drop table success\n", tableName);
    }

    for (const auto& [create, drop, tableName] : initializers)
    {
        if (!create(connection))
        {
            std::cout << std::format("{} create table fail\n", tableName);

            transaction.Rollback();
            return false;
        }

        std::cout << std::format("{} create table success\n", tableName);
    }

    transaction.Commit();
    return true;
}
