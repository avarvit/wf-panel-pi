#ifndef WIDGETS_CPU_HPP
#define WIDGETS_CPU_HPP

#include <widget.hpp>
#include <gtkmm/button.h>

extern "C" {
#include "cpu.h"
}

class WayfireCPU : public WayfireWidget
{
    std::unique_ptr <Gtk::Button> plugin;

    WfOption <int> icon_size {"panel/icon_size"};
    WfOption <std::string> bar_pos {"panel/position"};
    sigc::connection icon_timer;

    WfOption <bool> show_percentage {"panel/cpu_show_percentage"};
    WfOption <std::string> foreground_colour {"panel/cpu_foreground"};
    WfOption <std::string> background_colour {"panel/cpu_background"};

    /* plugin */
    CPUPlugin *cpu;

  public:

    void init (Gtk::HBox *container) override;
    virtual ~WayfireCPU ();
    void icon_size_changed_cb (void);
    void bar_pos_changed_cb (void);
    bool set_icon (void);
    void settings_changed_cb (void);
};

#endif /* end of include guard: WIDGETS_CPU_HPP */
