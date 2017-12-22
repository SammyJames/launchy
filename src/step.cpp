#include "step.hpp"
#include "spdlog/spdlog.h"

namespace spd = spdlog;
using std::tie;

i32 StepMan::RunAll()
{
    auto log = spd::get("Launchy");

    auto total_steps = m_steps.size();
    bool b_success   = false;
    any extra;

    for (size_t step_id = 0; step_id < total_steps; ++step_id)
    {
        const auto& step = m_steps[ step_id ];
        log->info("[{0}/{1}] running step `{2}`", step_id + 1, total_steps, step->NameOf());
        tie(b_success, extra) = step->RunStep(extra);
        if (!b_success)
        {
            return 1;
        }
    }

    return 0;
}