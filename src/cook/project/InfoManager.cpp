#include "cook/project/InfoManager.hpp"

namespace cook { namespace project {

namespace  {

template <typename T>
void safe_destruct(info::AsyncManager<T> & t)
{
    QFuture<void> f = t.future();
    if (!f.isFinished())
    {
        f.cancel();
        f.waitForFinished();
        t.deleteLater();
    }
}

}

InfoManager::InfoManager(BuildConfiguration * config)
    : recipes_mgr_(new info::RecipesManager(config)),
      build_recipes_mgr_(new info::BuildRecipesManager(config)),
      ninja_build_mgr_(new info::BuildNinjaManager(config))
{
    connect(recipes_mgr_, &info::RecipesManager::started,                           [this]() {emit started(InfoRequestType::Recipes); } );
    connect(build_recipes_mgr_, &info::BuildRecipesManager::started,                [this]() {emit started(InfoRequestType::Build_Recipes); } );
    connect(ninja_build_mgr_, &info::BuildNinjaManager::started,                    [this]() {emit started(InfoRequestType::Ninja ); } );

    connect(recipes_mgr_, &info::RecipesManager::error_occured,                     [this](const QString & error) { emit error_occurred(error, InfoRequestType::Recipes); } );
    connect(build_recipes_mgr_, &info::BuildRecipesManager::error_occured,          [this](const QString & error) { emit error_occurred(error, InfoRequestType::Build_Recipes); } );
    connect(ninja_build_mgr_, &info::BuildNinjaManager::error_occured,              [this](const QString & error) { emit error_occurred(error, InfoRequestType::Ninja); } );

    connect(recipes_mgr_, &info::RecipesManager::finished,                          [this](bool ok) { emit finished(ok, InfoRequestType::Recipes); } );
    connect(build_recipes_mgr_, &info::BuildRecipesManager::finished,               [this](bool ok) { emit finished(ok, InfoRequestType::Build_Recipes); } );
    connect(ninja_build_mgr_, &info::BuildNinjaManager::finished,                   [this](bool ok) { emit finished(ok, InfoRequestType::Ninja); } );
}

InfoManager::~InfoManager()
{
    safe_destruct(*recipes_mgr_);
    safe_destruct(*build_recipes_mgr_);
}

} }
