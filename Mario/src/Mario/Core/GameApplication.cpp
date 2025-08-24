#include "mmpch.h"
#include "GameApplication.hpp"

#include "Mario/Core/Core.hpp"
#include "Mario/Core/Logging.hpp"
#include "Mario/Core/Settings.hpp"

#include <format>

namespace
{
    static Mario::GameApplication* s_Instance = nullptr;
}

namespace Mario
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Constructor & Destructor
    ////////////////////////////////////////////////////////////////////////////////////
    GameApplication::GameApplication()
    {
        s_Instance = this;
        
        // Window
        m_Window.Construct(Obsidian::WindowSpecification()
            .SetWidthAndHeight(Settings::WindowWidth, Settings::WindowHeight)
            .SetTitle("Mario: Metal")
            .SetFlags(Obsidian::WindowFlags::Decorated | Obsidian::WindowFlags::Focused | Obsidian::WindowFlags::FocusOnShow | Obsidian::WindowFlags::Visible | Obsidian::WindowFlags::Resizable)
            .SetEventCallback([this](Obsidian::Event e) mutable { OnEvent(e); })
        );

        // Device
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

        // Swapchain
        m_Swapchain.Construct(m_Device.Get(), Obsidian::SwapchainSpecification()
            .SetWindow(m_Window.Get())
            .SetFormat(Obsidian::Format::RGBA8Unorm)
            .SetColourSpace(Obsidian::ColourSpace::SRGB)
#if defined(MM_PLATFORM_APPLE)
            .SetVSync(true) // Note: Vulkan via MoltenVK without VSync causes bad screen tearing (at least on my mac)
#else
            .SetVSync(false)
#endif
            .SetDebugName("Swapchain")
        );

        // CommandPools
        for (uint8_t i = 0; i < Obsidian::Information::FramesInFlight; i++)
        {
            m_CommandPools[i].Construct(m_Swapchain.Get(), Obsidian::CommandListPoolSpecification()
                .SetQueue(Obsidian::CommandQueue::Graphics)
                .SetDebugName(std::format("CommandPool for frame: {0}", i))
            );
        }

        // CommandBuffers
        for (uint8_t i = 0; i < Obsidian::Information::FramesInFlight; i++)
        {
            m_CommandLists[i].Construct(m_CommandPools[i].Get(), Obsidian::CommandListSpecification()
                .SetDebugName(std::format("CommandList for frame: {0}", i))
            );
        }

        // Game
        m_Game.Construct(m_Window->GetSize().x, m_Window->GetSize().y);
    }

    GameApplication::~GameApplication()
    {
        m_Game.Destroy();

        for (auto& pool : m_CommandPools)
            m_Swapchain->FreePool(pool.Get());

        m_Device->DestroySwapchain(m_Swapchain.Get());
        m_Device->Wait();
        DestroyQueue();

        s_Instance = nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Methods
    ////////////////////////////////////////////////////////////////////////////////////
    void GameApplication::Run()
    {
        double lastTime = m_Window->GetWindowTime();
        double currentTime = m_Window->GetWindowTime();

        while (m_Window->IsOpen())
        {
            m_Window->PollEvents();
            m_Swapchain->AcquireNextImage();

            // Rendering
            {
                m_CommandPools[m_Swapchain->GetCurrentFrame()]->Reset();
                auto& list = m_CommandLists[m_Swapchain->GetCurrentFrame()];
                list->Open();

                currentTime = m_Window->GetWindowTime();
                {
                    m_Game->OnUpdate(currentTime - lastTime);
                    m_Game->OnRender(list);
                }
                lastTime = currentTime;

                list->Close();
                list->Submit(Obsidian::CommandListSubmitArgs()
                    .SetWaitForSwapchainImage(true)
                    .SetOnFinishMakeSwapchainPresentable(true)
                );
            }

            m_Swapchain->Present();
            m_Window->SwapBuffers();
            DestroyQueue();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Static getter
    ////////////////////////////////////////////////////////////////////////////////////
    GameApplication& GameApplication::Instance()
    {
        return *s_Instance;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Private methods
    ////////////////////////////////////////////////////////////////////////////////////
    void GameApplication::OnMessage(MessageType type, const std::string& message)
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

    void GameApplication::OnEvent(Obsidian::Event& e)
    {
        EventHandler handler(e);
        handler.Handle<Obsidian::WindowResizeEvent>([this](Obsidian::WindowResizeEvent& wre) mutable
        {
            m_Swapchain->Resize(wre.GetWidth(), wre.GetHeight());
        });
        handler.Handle<Obsidian::WindowCloseEvent>([this](Obsidian::WindowCloseEvent&) mutable
        {
            m_Window->Close();
        });

        m_Game->OnEvent(e);
    }

    void GameApplication::DestroyQueue()
    {
        while (!m_DestroyQueue.empty())
        {
            m_DestroyQueue.front()();
            m_DestroyQueue.pop();
        }
    }

}