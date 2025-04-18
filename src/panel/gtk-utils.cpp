#include <gtk-utils.hpp>
#include <glibmm.h>
#include <gtkmm/icontheme.h>
#include <gdk/gdkcairo.h>
#include <iostream>

extern "C" {
#include "lxutils.h"
}

Glib::RefPtr<Gdk::Pixbuf> load_icon_pixbuf_safe(std::string icon_path, int size)
{
    try {
        auto pb = Gdk::Pixbuf::create_from_file(icon_path, size, size);
        return pb;
    } catch (Glib::FileError&)
    {
        std::cerr << "Error loading file: " << icon_path << std::endl;
        return {};
    } catch (Gdk::PixbufError&)
    {
        std::cerr << "Pixbuf error: " << icon_path << std::endl;
        return {};
    } catch (...)
    {
        std::cerr << "Failed to load: " << icon_path << std::endl;
        return {};
    }
}

Glib::RefPtr<Gtk::CssProvider> load_css_from_path(std::string path)
{
    try {
        auto css = Gtk::CssProvider::create();
        css->load_from_path(path);
        return css;
    } catch (Glib::Error& err)
    {
        std::cerr << "Failed to load CSS: " << err.what() << std::endl;
        return {};
    } catch (...)
    {
        std::cerr << "Failed to load CSS at: " << path << std::endl;
        return {};
    }
}

void invert_pixbuf(Glib::RefPtr<Gdk::Pixbuf>& pbuff)
{
    int channels = pbuff->get_n_channels();
    int stride   = pbuff->get_rowstride();

    auto data = pbuff->get_pixels();
    int w     = pbuff->get_width();
    int h     = pbuff->get_height();

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            auto p = data + j * stride + i * channels;
            p[0] = 255 - p[0];
            p[1] = 255 - p[1];
            p[2] = 255 - p[2];
        }
    }
}

void set_image_pixbuf(Gtk::Image & image, Glib::RefPtr<Gdk::Pixbuf> pixbuf, int scale)
{
    auto pbuff = pixbuf->gobj();
    auto cairo_surface = gdk_cairo_surface_create_from_pixbuf(pbuff, scale, NULL);

    gtk_image_set_from_surface(image.gobj(), cairo_surface);
    cairo_surface_destroy(cairo_surface);
}

void set_image_icon(Gtk::Image& image, std::string icon_name, int size,
    const WfIconLoadOptions& options,
    const Glib::RefPtr<Gtk::IconTheme>& icon_theme)
{
    int scale = ((options.user_scale == -1) ?
        image.get_scale_factor() : options.user_scale);
    int scaled_size = size * scale;

    if (icon_theme->lookup_icon(icon_name, scaled_size))
    {
        auto pbuff = icon_theme->load_icon(icon_name, scaled_size, Gtk::ICON_LOOKUP_FORCE_SIZE)
        ->scale_simple(scaled_size, scaled_size, Gdk::INTERP_BILINEAR);

        if (options.invert) invert_pixbuf (pbuff);
        set_image_pixbuf (image, pbuff, scale);
    }
    else
    {
        auto pbuff = load_icon_pixbuf_safe(icon_name, scaled_size);
        if (pbuff)
        {
            if (options.invert) invert_pixbuf (pbuff);
            set_image_pixbuf (image, pbuff, scale);
        }
    }
}

Glib::RefPtr<Gtk::GestureLongPress> detect_long_press (Gtk::Widget& target)
{
    Glib::RefPtr<Gtk::GestureLongPress> gesture = Gtk::GestureLongPress::create (target);
    gesture->set_propagation_phase (Gtk::PHASE_BUBBLE);
    gesture->signal_pressed ().connect ([=] (double x, double y) {pressed = PRESS_LONG;});
    gesture->set_touch_only (touch_only);
    return gesture;
}

Glib::RefPtr<Gtk::GestureLongPress> add_longpress_default (Gtk::Widget& target)
{
    Glib::RefPtr<Gtk::GestureLongPress> gesture = Gtk::GestureLongPress::create (target);
    GtkWidget *wid = target.gobj ();
    gesture->set_propagation_phase (Gtk::PHASE_BUBBLE);
    gesture->signal_pressed ().connect ([=] (double x, double y) {pressed = PRESS_LONG; press_x = x; press_y = y;});
    gesture->signal_end ().connect ([=] (GdkEventSequence *) {if (pressed == PRESS_LONG) pass_right_click (wid, press_x, press_y);});
    gesture->set_touch_only (touch_only);
    return gesture;
}
