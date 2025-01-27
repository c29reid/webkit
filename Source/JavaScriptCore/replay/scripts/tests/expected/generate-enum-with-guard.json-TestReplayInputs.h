/*
 * Copyright (C) 2014 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// DO NOT EDIT THIS FILE. It is automatically generated from generate-enum-with-guard.json
// by the script: JavaScriptCore/replay/scripts/CodeGeneratorReplayInputs.py

#ifndef generate_enum_with_guard_json_TestReplayInputs_h
#define generate_enum_with_guard_json_TestReplayInputs_h

#if ENABLE(WEB_REPLAY)
#include "InternalNamespaceHeaderIncludeDummy.h"
#include <platform/ExternalNamespaceHeaderIncludeDummy.h>

namespace Test {
enum PlatformWheelPhase : uint64_t;
}

namespace WebCore {
class PlatformWheelEvent;
}


namespace Test {
class HandleWheelEvent;
} // namespace Test

namespace JSC {
template<> struct TEST_EXPORT_MACRO InputTraits<Test::HandleWheelEvent> {
    static InputQueue queue() { return InputQueue::EventLoopInput; }
    static const String& type();

    static void encode(JSC::EncodedValue&, const Test::HandleWheelEvent&);
    static bool decode(JSC::EncodedValue&, std::unique_ptr<Test::HandleWheelEvent>&);
};
#if ENABLE(DUMMY_FEATURE)
template<> struct TEST_EXPORT_MACRO EncodingTraits<Test::PlatformWheelPhase> {
    typedef Test::PlatformWheelPhase DecodedType;

    static EncodedValue encodeValue(const Test::PlatformWheelPhase& value);
    static bool decodeValue(EncodedValue&, Test::PlatformWheelPhase& value);
};
#endif // ENABLE(DUMMY_FEATURE)
} // namespace JSC

namespace Test {
class HandleWheelEvent : public EventLoopInput<HandleWheelEvent> {
public:
    TEST_EXPORT_MACRO HandleWheelEvent(std::unique_ptr<PlatformWheelEvent> platformEvent, PlatformWheelPhase phase);
    virtual ~HandleWheelEvent();

    // EventLoopInput API
    virtual void dispatch(ReplayController&) final;
    const PlatformWheelEvent& platformEvent() const { return *m_platformEvent; }
    PlatformWheelPhase phase() const { return m_phase; }
private:
    std::unique_ptr<PlatformWheelEvent> m_platformEvent;
    PlatformWheelPhase m_phase;
};
} // namespace Test

SPECIALIZE_TYPE_TRAITS_BEGIN(Test::HandleWheelEvent)
    static bool isType(const NondeterministicInputBase& input) { return input.type() == InputTraits<Test::HandleWheelEvent>::type(); }
SPECIALIZE_TYPE_TRAITS_END()

#define TEST_REPLAY_INPUT_NAMES_FOR_EACH(macro) \
    macro(HandleWheelEvent) \
    \
// end of TEST_REPLAY_INPUT_NAMES_FOR_EACH

#endif // ENABLE(WEB_REPLAY)

#endif // generate-enum-with-guard.json-TestReplayInputs_h
