#pragma once

#include "types.hpp"

#include <vector>

using std::vector;
class IStep
{
public:
    virtual ~IStep() {}
    virtual tuple<bool, any> RunStep(any&) = 0;
    virtual const char* NameOf() const     = 0;
};

class StepMan
{
public:
    template <class... Steps>
    void SetSteps()
    {
        AddStep<Steps...>(m_steps);
    }

    i32 RunAll();

private:
    template <class Step, class Head, class... Tail>
    static void AddStep(vector<unique_ptr<IStep>>& dest)
    {
        AddStep<Step>(dest);
        AddStep<Head, Tail...>(dest);
    }

    template <class Step>
    static void AddStep(vector<unique_ptr<IStep>>& dest)
    {
        dest.push_back(make_unique<Step>());
    }

private:
    vector<unique_ptr<IStep>> m_steps;
};