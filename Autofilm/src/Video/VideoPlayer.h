#pragma once

#include <gst/gst.h>
#include "Core/File.h"

namespace Autofilm
{
    class GstVideoPlayer
    {
        GstVideoPlayer() { gst_init(NULL, NULL); }
        ~GstVideoPlayer();
    };
}

int AUTOFILM_API hello_gst()
{
    GstElement* pipeline;
    GstBus* bus;
    GstMessage *msg;
    gst_init(NULL, NULL);

    pipeline = gst_parse_launch(
        "playbin uri=file:///C:/dev/auto-vid/Autofilm/src/Video/example.mp4",
        NULL
    );

    /* Start playing */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(
        bus, 
        GST_CLOCK_TIME_NONE,
        (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS)
    );

    // /* See next tutorial for proper error message handling/parsing */
    if (msg != NULL) {
        if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
            AF_CORE_ERROR("An error occurred! Re-run with the GST_DEBUG=*:WARN "
                       "environment variable set for more details.");
        }
        gst_message_unref(msg);
    }

    // /* Free resources */
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
    return 0;
}