#include "lxutils.h"

/*----------------------------------------------------------------------------*/
/* Plug-in global data                                                        */
/*----------------------------------------------------------------------------*/

typedef unsigned long long CPUTick;     /* Value from /proc/stat */

struct cpu_stat
{
    CPUTick u, n, s, i;             /* User, nice, system, idle */
};

/* Private context for plugin */

typedef struct
{
    GtkWidget *plugin;                      /* Back pointer to the widget */
    int icon_size;                          /* Variables used under wf-panel */
    gboolean bottom;
    GtkGesture *gesture;
    PluginGraph graph;
    GdkRGBA foreground_colour;              /* Foreground colour for drawing area */
    GdkRGBA background_colour;              /* Background colour for drawing area */
    gboolean show_percentage;               /* Display usage as a percentage */
    guint timer;                            /* Timer for periodic update */
    struct cpu_stat previous_cpu_stat;      /* Previous value of cpu_stat */
} CPUPlugin;

extern void cpu_init (CPUPlugin *up);
extern void cpu_update_display (CPUPlugin *up);
extern void cpu_destructor (gpointer user_data);
