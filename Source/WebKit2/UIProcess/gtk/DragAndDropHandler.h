/*
 * Copyright (C) 2014 Igalia S.L.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#if ENABLE(DRAG_SUPPORT)

#include <WebCore/DataObjectGtk.h>
#include <WebCore/DragActions.h>
#include <WebCore/IntPoint.h>
#include <gtk/gtk.h>
#include <wtf/HashMap.h>
#include <wtf/Noncopyable.h>
#include <wtf/glib/GRefPtr.h>

typedef struct _GdkDragContext GdkDragContext;
typedef struct _GtkSelectionData GtkSelectionData;

namespace WebCore {
class DragData;
}

namespace WebKit {

class ShareableBitmap;
class WebPageProxy;

class DragAndDropHandler {
    WTF_MAKE_NONCOPYABLE(DragAndDropHandler);
public:
    DragAndDropHandler(WebPageProxy&);

    void startDrag(Ref<WebCore::DataObjectGtk>&&, WebCore::DragOperation, RefPtr<ShareableBitmap>&& dragImage);
    void fillDragData(GdkDragContext*, GtkSelectionData*, unsigned info);
    void finishDrag(GdkDragContext*);

    void dragEntered(GdkDragContext*, GtkSelectionData*, unsigned info, unsigned time);
    void dragMotion(GdkDragContext*, const WebCore::IntPoint& position, unsigned time);
    void dragLeave(GdkDragContext*);
    bool drop(GdkDragContext*, const WebCore::IntPoint& position, unsigned time);

private:
    struct DroppingContext {
        DroppingContext(GdkDragContext*, const WebCore::IntPoint& position);

        GdkDragContext* gdkContext;
        RefPtr<WebCore::DataObjectGtk> dataObject;
        WebCore::IntPoint lastMotionPosition;
        unsigned pendingDataRequests;
        bool dropHappened;
    };

    WebCore::DataObjectGtk* dataObjectForDropData(GdkDragContext*, GtkSelectionData*, unsigned info, WebCore::IntPoint& position);
    WebCore::DataObjectGtk* requestDragData(GdkDragContext*, const WebCore::IntPoint& position, unsigned time);

    WebPageProxy& m_page;
    HashMap<GdkDragContext*, std::unique_ptr<DroppingContext>> m_droppingContexts;

#if GTK_CHECK_VERSION(3, 16, 0)
    GRefPtr<GdkDragContext> m_dragContext;
    RefPtr<WebCore::DataObjectGtk> m_draggingDataObject;
#else
    // We don't have gtk_drag_cancel() in GTK+ < 3.16, so we use the old code.
    // See https://bugs.webkit.org/show_bug.cgi?id=138468
    HashMap<GdkDragContext*, RefPtr<WebCore::DataObjectGtk>> m_draggingDataObjects;
#endif
};

} // namespace WebKit

#endif // ENABLE(DRAG_SUPPORT)
