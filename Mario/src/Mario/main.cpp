#include "mmpch.h"
#include <iostream>

#include <Obsidian/Core/Window.hpp>

#include <Photon/Server/Server.hpp>

int main(int argc, char* argv[])
{
    (void)argc; (void)argv;

    Obsidian::Window* windowPtr;
    Obsidian::Window window(Obsidian::WindowSpecification()
        .SetWidthAndHeight(1280, 720)
        .SetTitle("Mario: Metal")
        .SetFlags(Obsidian::WindowFlags::Decorated | Obsidian::WindowFlags::Focused | Obsidian::WindowFlags::FocusOnShow | Obsidian::WindowFlags::Visible | Obsidian::WindowFlags::Resizable)
        .SetEventCallback([&](Obsidian::Event e)
        {
            Nano::Events::EventHandler handler(e);
            handler.Handle<Obsidian::WindowCloseEvent>([&](Obsidian::WindowCloseEvent&) { windowPtr->Close(); });
        })
    );
    windowPtr = &window;

    Photon::Server server;
    server.SetMessageCallback([](void* userData, Photon::MessageType type, const std::string& message) { std::cout << message << std::endl; });

    server.Start(8080).Wait();

    while (window.IsOpen())
    {
        window.PollEvents();

        window.SwapBuffers();
    }

    server.Stop();
    return 0;
}