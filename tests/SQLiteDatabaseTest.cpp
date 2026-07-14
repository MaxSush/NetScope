
#include <iostream>

#include <database/SQLiteDatabase.h>
#include <logger/Logger.h>
#include <schema/SchemaManager.h>
#include <ProcessInfo.h>
#include <repository/ProcessRepository.h>

using namespace netscope;

int main()
{
    storage::SQLiteDatabase database;

    auto res = database.Open("netscope.db");

    if (!res)
    {
        LOG_ERROR(res.message);
        return 1;
    }

    storage::SchemaManager schema;

    res = schema.Initialize(database);
    if (!res)
    {
        LOG_ERROR(res.message);
        return 1;
    }

    ProcessInfo p{};
    p.pid = 12345;
    p.name = "chrome";
    p.exe_path = "/usr/bin/chrome";
    p.start_time = 1234567895;
    
    storage::ProcessRepository repo(database);

    database.BeginTransaction();
    res = repo.Upsert(p);
    if (!res)
    {
        LOG_ERROR(res.message);
    }
    database.Rollback();

    Expected<ProcessInfo, storage::DatabaseResult> result = repo.Find(p.pid, p.start_time);
    if (!result.HasValue())
    {
        LOG_WARN(result.Error().message);
    }
    else
    {
        ProcessInfo process = result.Value();
        fmt::print("pid: {}, name: {}, exe_path: {}, start_time: {}\n", process.pid, process.name, process.exe_path, process.start_time);
    }

    return 0;
}