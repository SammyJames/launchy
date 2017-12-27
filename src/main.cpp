#include "types.hpp"

#include "bytebuffer.hpp"
#include "client.hpp"
#include "launcher.hpp"
#include "step.hpp"
#include "utils.hpp"

#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

#include <boost/filesystem.hpp>

namespace spd = spdlog;
using std::make_tuple;

namespace fs = boost::filesystem;

struct AuthInfo
{
    string m_username;
    string m_password;
};

class ProcessCommandLine : public IStep
{
public:
    tuple<bool, any> RunStep(any&) override
    {
        ByteBuffer test;
        test.Write(string{"Hello World"});
        auto str = test.ToString();

        string res;
        test.Read(res);

        return make_tuple(true, any(AuthInfo{"username", "password"}));
    }
    const char* NameOf() const override { return "Process Command Line"; }
};

class FetchLauncher : public IStep
{
public:
    tuple<bool, any> RunStep(any& prevStep) override
    {
        auto auth_info           = any_cast<AuthInfo>(&prevStep);
        const auto launcher_info = RequestLauncherInfo();

        return make_tuple(!launcher_info.m_address.empty() && launcher_info.m_port != 0,
                          any(make_tuple(launcher_info, *auth_info)));
    }

    const char* NameOf() const override { return "Fetch Launcher Info"; }

private:
};

class OpenConnection : public IStep
{
public:
    tuple<bool, any> RunStep(any& prevStep) override
    {
        if (auto merged_steps = any_cast<tuple<LauncherInfo, AuthInfo>>(&prevStep))
        {
            const auto& launcher_info = std::get<0>(*merged_steps);

            if (!launcher_info.m_address.empty() && launcher_info.m_port != 0)
            {
                auto client = make_shared<Client>(launcher_info.m_address, launcher_info.m_port);
                client->Connect();

                return make_tuple(true, any(client));
            }
        }

        return make_tuple(false, any());
    }
    const char* NameOf() const override { return "Open Connection"; }
};

class CheckClient : public IStep
{
public:
    tuple<bool, any> RunStep(any& prevStep) override
    {
        if (auto p_client = any_cast<shared_ptr<Client>>(&prevStep))
        {
            const auto& client = *p_client;

            fs::path login_xml = fs::current_path() / "mythloginserviceconfig.xml";
            auto file_size     = fs::file_size(login_xml);

            auto buffer = ByteBuffer();
            buffer.Write(0u);
            buffer.Write<u8>(1u);
            buffer.Write(1u);
            buffer.Write<u8>(1u);
            buffer.Write(file_size);

            client->Write(buffer);

            ByteBuffer dest;
            client->Read(dest);

            spd::get("Launchy")->info("{0}", dest.ToString());
        }
        return make_tuple(false, any());
    }
    const char* NameOf() const override { return "Check Client"; }
};

class AttemptLogin : public IStep
{
public:
    tuple<bool, any> RunStep(any& prevStep) override { return make_tuple(false, any()); }
    const char* NameOf() const override { return "Attempt Login"; }
};

class LaunchClient : public IStep
{
public:
    tuple<bool, any> RunStep(any& prevStep) override { return make_tuple(false, any()); }
    const char* NameOf() const override { return "Launch Client"; }
};

i32 main(i32 argc, char** argv)
{
    auto log = spd::stdout_color_mt("Launchy");
    log->info("Welcome to Launchy! The command line Warhammer Online launcher.");

    StepMan manager;
    manager.SetSteps<ProcessCommandLine,
                     FetchLauncher,
                     OpenConnection,
                     CheckClient,
                     AttemptLogin,
                     LaunchClient>();
    return manager.RunAll();
}