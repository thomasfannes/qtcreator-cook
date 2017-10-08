#ifndef HEADER_cook_project_ProjectNodes_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_ProjectNodes_hpp_ALREADY_INCLUDED

#include "cook/project/info/Types.hpp"
#include <projectexplorer/projectnodes.h>

namespace cook { namespace project {

class Project;

class CookNode : public ProjectExplorer::ProjectNode
{
public:
    explicit CookNode(Project * project);
    CookNode(Project * project, const Utils::FileName & dir);

    virtual bool showInSimpleTree() const override;
    virtual QString tooltip() const override;

private:
    Project * project_;
};

class RecipeNode : public ProjectExplorer::ProjectNode
{
public:
    explicit RecipeNode(const info::Recipe & recipe);

    void compress();
};

class ChaiScriptNode : public ProjectExplorer::FileNode
{
public:
    explicit ChaiScriptNode(const Utils::FileName & fn);

    virtual bool showWhenEmpty() const { return true; }
};

} }

#endif
