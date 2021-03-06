/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights
 * reserved.
 * Copyright (C) 2008, 2009, 2010, 2011 Google Inc. All rights reserved.
 * Copyright (C) 2011 Igalia S.L.
 * Copyright (C) 2011 Motorola Mobility. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "core/editing/serializers/StyledMarkupAccumulator.h"

#include "core/css/StylePropertySet.h"
#include "core/dom/Text.h"
#include "core/editing/EditingUtilities.h"
#include "core/editing/iterators/TextIterator.h"
#include "platform/wtf/text/StringBuilder.h"

namespace blink {

namespace {

size_t TotalLength(const Vector<String>& strings) {
  size_t length = 0;
  for (const auto& string : strings)
    length += string.length();
  return length;
}

}  // namespace

using namespace HTMLNames;

StyledMarkupAccumulator::StyledMarkupAccumulator(
    EAbsoluteURLs should_resolve_urls,
    const TextOffset& start,
    const TextOffset& end,
    Document* document,
    EAnnotateForInterchange should_annotate,
    ConvertBlocksToInlines convert_blocks_to_inlines)
    : formatter_(should_resolve_urls),
      start_(start),
      end_(end),
      document_(document),
      should_annotate_(should_annotate),
      convert_blocks_to_inlines_(convert_blocks_to_inlines) {}

void StyledMarkupAccumulator::AppendEndTag(const Element& element) {
  AppendEndMarkup(result_, element);
}

void StyledMarkupAccumulator::AppendStartMarkup(Node& node) {
  formatter_.AppendStartMarkup(result_, node, nullptr);
}

void StyledMarkupAccumulator::AppendEndMarkup(StringBuilder& result,
                                              const Element& element) {
  formatter_.AppendEndMarkup(result, element);
}

void StyledMarkupAccumulator::AppendText(Text& text) {
  const String& str = text.data();
  unsigned length = str.length();
  unsigned start = 0;
  if (end_.IsNotNull()) {
    if (text == end_.GetText())
      length = end_.Offset();
  }
  if (start_.IsNotNull()) {
    if (text == start_.GetText()) {
      start = start_.Offset();
      length -= start;
    }
  }
  MarkupFormatter::AppendCharactersReplacingEntities(
      result_, str, start, length, formatter_.EntityMaskForText(text));
}

void StyledMarkupAccumulator::AppendTextWithInlineStyle(
    Text& text,
    EditingStyle* inline_style) {
  if (inline_style) {
    // wrappingStyleForAnnotatedSerialization should have removed
    // -webkit-text-decorations-in-effect.
    DCHECK(!ShouldAnnotate() || PropertyMissingOrEqualToNone(
                                    inline_style->Style(),
                                    CSSPropertyWebkitTextDecorationsInEffect));
    DCHECK(document_);

    result_.Append("<span style=\"");
    MarkupFormatter::AppendAttributeValue(
        result_, inline_style->Style()->AsText(), document_->IsHTMLDocument());
    result_.Append("\">");
  }
  if (!ShouldAnnotate()) {
    AppendText(text);
  } else {
    const bool use_rendered_text = !EnclosingElementWithTag(
        Position::FirstPositionInNode(text), selectTag);
    String content =
        use_rendered_text ? RenderedText(text) : StringValueForRange(text);
    StringBuilder buffer;
    MarkupFormatter::AppendCharactersReplacingEntities(
        buffer, content, 0, content.length(), kEntityMaskInPCDATA);
    result_.Append(ConvertHTMLTextToInterchangeFormat(buffer.ToString(), text));
  }
  if (inline_style)
    result_.Append("</span>");
}

void StyledMarkupAccumulator::AppendElementWithInlineStyle(
    const Element& element,
    EditingStyle* style) {
  AppendElementWithInlineStyle(result_, element, style);
}

void StyledMarkupAccumulator::AppendElementWithInlineStyle(
    StringBuilder& out,
    const Element& element,
    EditingStyle* style) {
  const bool document_is_html = element.GetDocument().IsHTMLDocument();
  formatter_.AppendOpenTag(out, element, nullptr);
  AttributeCollection attributes = element.Attributes();
  for (const auto& attribute : attributes) {
    // We'll handle the style attribute separately, below.
    if (attribute.GetName() == styleAttr)
      continue;
    formatter_.AppendAttribute(out, element, attribute, nullptr);
  }
  if (style && !style->IsEmpty()) {
    out.Append(" style=\"");
    MarkupFormatter::AppendAttributeValue(out, style->Style()->AsText(),
                                          document_is_html);
    out.Append('\"');
  }
  formatter_.AppendCloseTag(out, element);
}

void StyledMarkupAccumulator::AppendElement(const Element& element) {
  AppendElement(result_, element);
}

void StyledMarkupAccumulator::AppendElement(StringBuilder& out,
                                            const Element& element) {
  formatter_.AppendOpenTag(out, element, nullptr);
  AttributeCollection attributes = element.Attributes();
  for (const auto& attribute : attributes)
    formatter_.AppendAttribute(out, element, attribute, nullptr);
  formatter_.AppendCloseTag(out, element);
}

void StyledMarkupAccumulator::WrapWithStyleNode(StylePropertySet* style) {
  // wrappingStyleForSerialization should have removed
  // -webkit-text-decorations-in-effect.
  DCHECK(PropertyMissingOrEqualToNone(
      style, CSSPropertyWebkitTextDecorationsInEffect));
  DCHECK(document_);

  StringBuilder open_tag;
  open_tag.Append("<div style=\"");
  MarkupFormatter::AppendAttributeValue(open_tag, style->AsText(),
                                        document_->IsHTMLDocument());
  open_tag.Append("\">");
  reversed_preceding_markup_.push_back(open_tag.ToString());

  result_.Append("</div>");
}

String StyledMarkupAccumulator::TakeResults() {
  StringBuilder result;
  result.ReserveCapacity(TotalLength(reversed_preceding_markup_) +
                         result_.length());

  for (size_t i = reversed_preceding_markup_.size(); i > 0; --i)
    result.Append(reversed_preceding_markup_[i - 1]);

  result.Append(result_);

  // We remove '\0' characters because they are not visibly rendered to the
  // user.
  return result.ToString().Replace(0, "");
}

String StyledMarkupAccumulator::RenderedText(Text& text_node) {
  int start_offset = 0;
  int end_offset = text_node.length();
  if (start_.GetText() == text_node)
    start_offset = start_.Offset();
  if (end_.GetText() == text_node)
    end_offset = end_.Offset();
  return PlainText(EphemeralRange(Position(&text_node, start_offset),
                                  Position(&text_node, end_offset)));
}

String StyledMarkupAccumulator::StringValueForRange(const Text& node) {
  if (start_.IsNull())
    return node.data();

  String str = node.data();
  if (start_.GetText() == node)
    str.Truncate(end_.Offset());
  if (end_.GetText() == node)
    str.Remove(0, start_.Offset());
  return str;
}

bool StyledMarkupAccumulator::ShouldAnnotate() const {
  return should_annotate_ == kAnnotateForInterchange;
}

void StyledMarkupAccumulator::PushMarkup(const String& str) {
  reversed_preceding_markup_.push_back(str);
}

void StyledMarkupAccumulator::AppendInterchangeNewline() {
  DEFINE_STATIC_LOCAL(const String, interchange_newline_string,
                      ("<br class=\"" AppleInterchangeNewline "\">"));
  result_.Append(interchange_newline_string);
}

}  // namespace blink
