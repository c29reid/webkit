/*
 * Copyright (C) 2011 Igalia S.L.
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

#include "config.h"
#include "ArgumentCodersGtk.h"

#include "DataReference.h"
#include "ShareableBitmap.h"
#include "WebCoreArgumentCoders.h"
#include <WebCore/DataObjectGtk.h>
#include <WebCore/GraphicsContext.h>
#include <WebCore/Image.h>
#include <gtk/gtk.h>
#include <wtf/glib/GUniquePtr.h>

using namespace WebCore;
using namespace WebKit;

namespace IPC {

static void encodeImage(Encoder& encoder, Image& image)
{
    RefPtr<ShareableBitmap> bitmap = ShareableBitmap::createShareable(IntSize(image.size()), ShareableBitmap::SupportsAlpha);
    bitmap->createGraphicsContext()->drawImage(image, IntPoint());

    ShareableBitmap::Handle handle;
    bitmap->createHandle(handle);

    encoder << handle;
}

static bool decodeImage(Decoder& decoder, RefPtr<Image>& image)
{
    ShareableBitmap::Handle handle;
    if (!decoder.decode(handle))
        return false;

    RefPtr<ShareableBitmap> bitmap = ShareableBitmap::create(handle);
    if (!bitmap)
        return false;
    image = bitmap->createImage();
    if (!image)
        return false;
    return true;
}

void ArgumentCoder<DataObjectGtk>::encode(Encoder& encoder, const DataObjectGtk& dataObject)
{
    bool hasText = dataObject.hasText();
    encoder << hasText;
    if (hasText)
        encoder << dataObject.text();

    bool hasMarkup = dataObject.hasMarkup();
    encoder << hasMarkup;
    if (hasMarkup)
        encoder << dataObject.markup();

    bool hasURL = dataObject.hasURL();
    encoder << hasURL;
    if (hasURL)
        encoder << dataObject.url().string();

    bool hasURIList = dataObject.hasURIList();
    encoder << hasURIList;
    if (hasURIList)
        encoder << dataObject.uriList();

    bool hasImage = dataObject.hasImage();
    encoder << hasImage;
    if (hasImage)
        encodeImage(encoder, *dataObject.image());

    bool hasUnknownTypeData = dataObject.hasUnknownTypeData();
    encoder << hasUnknownTypeData;
    if (hasUnknownTypeData)
        encoder << dataObject.unknownTypes();

    bool canSmartReplace = dataObject.canSmartReplace();
    encoder << canSmartReplace;
}

bool ArgumentCoder<DataObjectGtk>::decode(Decoder& decoder, DataObjectGtk& dataObject)
{
    dataObject.clearAll();

    bool hasText;
    if (!decoder.decode(hasText))
        return false;
    if (hasText) {
        String text;
        if (!decoder.decode(text))
            return false;
        dataObject.setText(text);
    }

    bool hasMarkup;
    if (!decoder.decode(hasMarkup))
        return false;
    if (hasMarkup) {
        String markup;
        if (!decoder.decode(markup))
            return false;
        dataObject.setMarkup(markup);
    }

    bool hasURL;
    if (!decoder.decode(hasURL))
        return false;
    if (hasURL) {
        String url;
        if (!decoder.decode(url))
            return false;
        dataObject.setURL(URL(URL(), url), String());
    }

    bool hasURIList;
    if (!decoder.decode(hasURIList))
        return false;
    if (hasURIList) {
        String uriList;
        if (!decoder.decode(uriList))
            return false;
        dataObject.setURIList(uriList);
    }

    bool hasImage;
    if (!decoder.decode(hasImage))
        return false;
    if (hasImage) {
        RefPtr<Image> image;
        if (!decodeImage(decoder, image))
            return false;
        dataObject.setImage(image.get());
    }

    bool hasUnknownTypeData;
    if (!decoder.decode(hasUnknownTypeData))
        return false;
    if (hasUnknownTypeData) {
        HashMap<String, String> unknownTypes;
        if (!decoder.decode(unknownTypes))
            return false;

        auto end = unknownTypes.end();
        for (auto it = unknownTypes.begin(); it != end; ++it)
            dataObject.setUnknownTypeData(it->key, it->value);
    }

    bool canSmartReplace;
    if (!decoder.decode(canSmartReplace))
        return false;
    dataObject.setCanSmartReplace(canSmartReplace);

    return true;
}

static void encodeGKeyFile(Encoder& encoder, GKeyFile* keyFile)
{
    gsize dataSize;
    GUniquePtr<char> data(g_key_file_to_data(keyFile, &dataSize, 0));
    encoder << DataReference(reinterpret_cast<uint8_t*>(data.get()), dataSize);
}

static bool decodeGKeyFile(Decoder& decoder, GUniquePtr<GKeyFile>& keyFile)
{
    DataReference dataReference;
    if (!decoder.decode(dataReference))
        return false;

    if (!dataReference.size())
        return true;

    keyFile.reset(g_key_file_new());
    if (!g_key_file_load_from_data(keyFile.get(), reinterpret_cast<const gchar*>(dataReference.data()), dataReference.size(), G_KEY_FILE_NONE, 0)) {
        keyFile.reset();
        return false;
    }

    return true;
}

void encode(Encoder& encoder, GtkPrintSettings* printSettings)
{
    GUniquePtr<GKeyFile> keyFile(g_key_file_new());
    gtk_print_settings_to_key_file(printSettings, keyFile.get(), "Print Settings");
    encodeGKeyFile(encoder, keyFile.get());
}

bool decode(Decoder& decoder, GRefPtr<GtkPrintSettings>& printSettings)
{
    GUniquePtr<GKeyFile> keyFile;
    if (!decodeGKeyFile(decoder, keyFile))
        return false;

    printSettings = adoptGRef(gtk_print_settings_new());
    if (!keyFile)
        return true;

    if (!gtk_print_settings_load_key_file(printSettings.get(), keyFile.get(), "Print Settings", 0))
        printSettings = 0;

    return printSettings;
}

void encode(Encoder& encoder, GtkPageSetup* pageSetup)
{
    GUniquePtr<GKeyFile> keyFile(g_key_file_new());
    gtk_page_setup_to_key_file(pageSetup, keyFile.get(), "Page Setup");
    encodeGKeyFile(encoder, keyFile.get());
}

bool decode(Decoder& decoder, GRefPtr<GtkPageSetup>& pageSetup)
{
    GUniquePtr<GKeyFile> keyFile;
    if (!decodeGKeyFile(decoder, keyFile))
        return false;

    pageSetup = adoptGRef(gtk_page_setup_new());
    if (!keyFile)
        return true;

    if (!gtk_page_setup_load_key_file(pageSetup.get(), keyFile.get(), "Page Setup", 0))
        pageSetup = 0;

    return pageSetup;
}

}
