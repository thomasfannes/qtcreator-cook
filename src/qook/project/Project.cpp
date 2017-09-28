#include "qook/project/Project.hpp"
#include "qook/project/ProjectNodes.hpp"
#include "qook/project/BuildConfiguration.hpp"
#include "qook/Constants.hpp"
#include "qook/toolset/KitInformation.hpp"
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>
#include <projectexplorer/taskhub.h>
#include <coreplugin/icontext.h>
#include <utils/qtcassert.h>
#include <QDebug>


namespace qook { namespace project {

Project::Project(const Utils::FileName & filename)
    : ProjectExplorer::Project(constants::QOOK_MIME_TYPE, filename, [this]() { refresh(InfoRequestType::Detailed_Recipes); }),
      connected_target_(nullptr),
      current_build_config_(nullptr),
      cpp_code_model_updater_(new CppTools::CppProjectUpdater(this))
{
    setId(constants::QOOK_PROJECT_ID);
    setProjectLanguages(Core::Context(ProjectExplorer::Constants::CXX_LANGUAGE_ID));
    setDisplayName(filename.toFileInfo().baseName());

    connect(this, &ProjectExplorer::Project::activeTargetChanged, this, &Project::active_target_changed);
}

Project::~Project()
{
    delete cpp_code_model_updater_;
}

void Project::active_target_changed(ProjectExplorer::Target *target)
{
    if (connected_target_)
    {
        disconnect(connected_target_, &ProjectExplorer::Target::activeBuildConfigurationChanged, this, &Project::refresh_all);
        disconnect(connected_target_, &ProjectExplorer::Target::kitChanged, this, &Project::refresh_all);
    }

    connected_target_ = target;

    if (connected_target_)
    {
        connect(connected_target_, &ProjectExplorer::Target::activeBuildConfigurationChanged, this, &Project::refresh_all);
        connect(connected_target_, &ProjectExplorer::Target::kitChanged, this, &Project::refresh_all);
    }

    refresh_all();
}

void Project::refresh_all()
{
    refresh(InfoRequestType::Recipes | InfoRequestType::Detailed_Recipes);
}

void Project::refresh(RequestFlags flags)
{
    QTC_ASSERT(!isParsing(), return);
    QTC_ASSERT(connected_target_, return);
    current_build_config_ = static_cast<BuildConfiguration*>(connected_target_->activeBuildConfiguration());
    QTC_ASSERT(current_build_config_, return);
    QTC_CHECK(current_build_config_->refresh(flags));
}

void Project::handle_parsing_started_(BuildConfiguration * configuration, RequestFlags flags)
{
    auto * t = activeTarget();
    if(!t || t->activeBuildConfiguration() != configuration)
        return;

    emitParsingStarted();
}

void Project::handle_parsing_finished_(BuildConfiguration * configuration, RequestFlags succeeded, RequestFlags failed)
{
    auto * t = activeTarget();
    if(!t || t->activeBuildConfiguration() != configuration)
        return;

    emitParsingFinished(failed == RequestFlags());
}

void Project::handle_sub_parsing_finished(BuildConfiguration * configuration, InfoRequestType request, bool success)
{
    // only emit if this is the active build configuration
    auto * t = activeTarget();
    if (!t || t->activeBuildConfiguration() != configuration)
        return;

    if (success)
    {
        switch(request)
        {
            case InfoRequestType::Recipes:
                emit recipes_available();
                break;

            case InfoRequestType::Detailed_Recipes:
                generate_project_tree_();
                refresh_cpp_code_model_();
                emit detailed_recipes_available();
                break;
        }
    }

    if(!success)
        ProjectExplorer::TaskHub::addTask(ProjectExplorer::Task::Error, configuration->error(), ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM);
}

bool Project::supportsKit(ProjectExplorer::Kit *k, QString *errorMessage) const
{
    if (!toolset::KitInformation::tool(k))
    {
        if(errorMessage)
            *errorMessage = tr("No Cook tool set.");
        return false;
    }

    return true;
}

bool Project::needsConfiguration() const
{
    return targets().isEmpty();
}


ProjectExplorer::Project::RestoreResult Project::fromMap(const QVariantMap &map, QString *errorMessage)
{
    return ProjectExplorer::Project::fromMap(map, errorMessage);
}

void Project::generate_project_tree_()
{
    setRootProjectNode(current_build_config_->generate_tree());
}

void Project::refresh_cpp_code_model_()
{
    QTC_ASSERT(current_build_config_, return);
    current_build_config_->refresh_cpp_code_model(cpp_code_model_updater_);
}



} }
