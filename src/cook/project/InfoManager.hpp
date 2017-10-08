#ifndef HEADER_cook_project_InfoManager_hpp_ALREADY_INCLUDED
#define HEADER_cook_project_InfoManager_hpp_ALREADY_INCLUDED

#include "cook/project/InfoManagerType.hpp"
#include "cook/project/info/Types.hpp"
#include "cook/project/info/RecipesManager.hpp"
#include "cook/project/info/BuildNinjaManager.hpp"
#include <QFuture>
#include <QFlags>

namespace cook { namespace project {

class BuildConfiguration;

class InfoManager : public QObject
{
    Q_OBJECT

public:
    InfoManager(BuildConfiguration * config);
    ~InfoManager();

    info::RecipesManager & recipes()                { return *recipes_mgr_; }
    info::BuildNinjaManager & ninja_build()         { return *ninja_build_mgr_; }

signals:
    void started(InfoRequestType type);
    void error_occurred(const QString & error, InfoRequestType type);
    void finished(bool ok, InfoRequestType type);

private:
    using ParserFlags = QFlags<InfoRequestType>;
    info::RecipesManager * recipes_mgr_;
    info::BuildNinjaManager * ninja_build_mgr_;
};

} }

#endif
