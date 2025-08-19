#include "mmpch.h"
#include "Game.hpp"

#include "Mario/Core/Core.hpp"
#include "Mario/Core/Logging.hpp"
#include "Mario/Core/Settings.hpp"

namespace Mario
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Constructor & Destructor
    ////////////////////////////////////////////////////////////////////////////////////
    Game::Game()
    {
        m_Window.Construct(Obsidian::WindowSpecification()
            .SetWidthAndHeight(Settings::WindowWidth, Settings::WindowHeight)
            .SetTitle("Mario: Metal")
            .SetFlags(Obsidian::WindowFlags::Decorated | Obsidian::WindowFlags::Focused | Obsidian::WindowFlags::FocusOnShow | Obsidian::WindowFlags::Visible | Obsidian::WindowFlags::Resizable)
            .SetEventCallback([this](Obsidian::Event e) mutable { OnEvent(e); })
        );

        m_Device.Construct(Obsidian::DeviceSpecification()
            .SetNativeWindow(m_Window->GetNativeWindow())
            .SetMessageCallback([this](Obsidian::DeviceMessageType type, const std::string& message)
            {
                static_assert(static_cast<uint8_t>(Obsidian::DeviceMessageType::Trace) == static_cast<uint8_t>(MessageType::Trace), "Obsidian Trace message type doesn't match our message type.");
                static_assert(static_cast<uint8_t>(Obsidian::DeviceMessageType::Info) == static_cast<uint8_t>(MessageType::Info), "Obsidian Info message type doesn't match our message type.");
                static_assert(static_cast<uint8_t>(Obsidian::DeviceMessageType::Warn) == static_cast<uint8_t>(MessageType::Warn), "Obsidian Warn message type doesn't match our message type.");
                static_assert(static_cast<uint8_t>(Obsidian::DeviceMessageType::Error) == static_cast<uint8_t>(MessageType::Error), "Obsidian Error message type doesn't match our message type.");
                
                // Note: It is safe to cast, since the types are the same for now
                OnMessage(static_cast<MessageType>(type), message);
            })
            .SetDestroyCallback([this](Obsidian::DeviceDestroyFn fn) { m_DestroyQueue.push(fn); })
        );
    }

    Game::~Game()
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Methods
    ////////////////////////////////////////////////////////////////////////////////////
    void Game::Run()
    {
        while (m_Window->IsOpen())
        {
            m_Window->PollEvents();

            m_Window->SwapBuffers();
            
            // Handle destroys
            {
                while (!m_DestroyQueue.empty())
                {
                    m_DestroyQueue.front()();
                    m_DestroyQueue.pop();
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Private methods
    ////////////////////////////////////////////////////////////////////////////////////
    void Game::OnMessage(MessageType type, const std::string& message)
    {
        switch (type)
        {
        case MessageType::Trace:
            MM_LOG_TRACE("{0}", message);
            break;
        case MessageType::Info:
            MM_LOG_INFO("{0}", message);
            break;
        case MessageType::Warn:
            MM_LOG_WARN("{0}", message);
            break;
        case MessageType::Error:
            MM_LOG_ERROR("{0}", message);
            break;
        case MessageType::Fatal:
            MM_LOG_FATAL("{0}", message);
            break;

        default:
            MM_UNREACHABLE();
            break;
        }
    }

    void Game::OnEvent(Obsidian::Event& e)
    {
        Nano::Events::EventHandler handler(e);
        handler.Handle<Obsidian::WindowResizeEvent>([this](Obsidian::WindowResizeEvent& wre) mutable
        {
            // ...
        });
        handler.Handle<Obsidian::WindowCloseEvent>([this](Obsidian::WindowCloseEvent&) mutable
        {
            m_Window->Close();
        });
    }

}