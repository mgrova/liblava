// file      : liblava/frame/render_thread.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/frame/renderer.hpp>

#include <thread>

namespace lava {

struct render_thread {

    ~render_thread() { destroy(); }

    bool create(swapchain* swapchain) {

        plotter.on_destroy = [&]() { stop(); };

        return plotter.create(swapchain);
    }

    void destroy() { plotter.destroy(); }

    void start() { thread = std::thread(&render_thread::render, this); }

    void stop() {

        if (!active)
            return;

        active = false;
        thread.join();
    }

    using render_func = std::function<VkCommandBuffers(ui32)>;
    render_func on_render;

    renderer* get_renderer() { return &plotter; }
    bool running() const { return active; }

private:
    void render() {

        active = true;

        while (active) {

            if (!plotter.active)
                continue;

            if (!on_render)
                continue;

            auto frame_index = plotter.begin_frame();
            if (!frame_index)
                continue;

            plotter.end_frame(on_render(*frame_index));
        }
    }

    std::thread thread;
    bool active = false;

    renderer plotter;
};

} // lava
