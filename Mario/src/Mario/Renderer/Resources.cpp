#include "mmpch.h"
#include "Resources.hpp"

#include "Mario/Core/Core.hpp"
#include "Mario/Core/Logging.hpp"
#include "Mario/Core/Settings.hpp"

#include "Mario/Core/Game.hpp"

#include <format>

namespace Mario
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Constructor & Destructor
    ////////////////////////////////////////////////////////////////////////////////////
    Resources::Resources()
        : m_MarioLuigiSheet(Settings::MarioLuigiSheet), m_EnemiesBossesSheet(Settings::EnemiesBossesSheet), m_ItemsObjectsSheet(Settings::ItemsObjectsSheet), m_TileSheet(Settings::TileSheet)
    {
    }

    Resources::~Resources()
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Static getter
    ////////////////////////////////////////////////////////////////////////////////////
    Resources& Resources::Instance()
    {
        return Game::Instance().m_Resources;
    }


}