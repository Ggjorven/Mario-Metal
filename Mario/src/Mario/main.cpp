#include "mmpch.h"
#include <iostream>

#include <Obsidian/Core/Window.hpp>

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

    while (window.IsOpen())
    {
        window.PollEvents();

        window.SwapBuffers();
    }

    return 0;
}