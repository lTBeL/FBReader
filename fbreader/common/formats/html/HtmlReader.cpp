/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <abstract/ZLInputStream.h>

#include "HtmlReader.h"

HtmlReader::HtmlTag HtmlReader::tag(std::string &name) {
	if (name.length() == 0) {
		return HtmlTag(_UNKNOWN, true);
	}

	bool start = name[0] != '/';
	if (!start) {
		name = name.substr(1);
	}

	if (name.length() > 6) {
		return HtmlTag(_UNKNOWN, start);
	}

	for (unsigned int i = 0; i < name.length(); i++) {
		name[i] = tolower(name[i]);
	}

	if (name == "body") {
		return HtmlTag(_P, start);
	}
	if (name == "p") {
		return HtmlTag(_P, start);
	}
	if (name == "br") {
		return HtmlTag(_BR, start);
	}
	if (name == "li") {
		return HtmlTag(_LI, start);
	}
	if (name == "title") {
		return HtmlTag(_TITLE, start);
	}
	if (name == "h1") {
		return HtmlTag(_H1, start);
	}
	if (name == "h2") {
		return HtmlTag(_H2, start);
	}
	if (name == "h3") {
		return HtmlTag(_H3, start);
	}
	if (name == "h4") {
		return HtmlTag(_H4, start);
	}
	if (name == "h5") {
		return HtmlTag(_H5, start);
	}
	if (name == "h6") {
		return HtmlTag(_H6, start);
	}
	if (name == "tt") {
		return HtmlTag(_TT, start);
	}
	if (name == "b") {
		return HtmlTag(_B, start);
	}
	if (name == "i") {
		return HtmlTag(_I, start);
	}
	if (name == "strong") {
		return HtmlTag(_STRONG, start);
	}
	if (name == "cite") {
		return HtmlTag(_CITE, start);
	}
	if (name == "style") {
		return HtmlTag(_STYLE, start);
	}
	return HtmlTag(_UNKNOWN, start);
}

void HtmlReader::readDocument(ZLInputStream &stream) {
	if (!stream.open()) {
		return;
	}

	startDocumentHandler();

	bool insideTag = false;
	bool insideTagName = false;
	std::string tagName;
	
	const size_t BUFSIZE = 2048;
	char buffer[BUFSIZE];
	size_t length;
	bool doBreak = false;
	do {
		length = stream.read(buffer, BUFSIZE);
		char *start = buffer;
		char *endOfBuffer = buffer + length;
		for (char *ptr = buffer; ptr < endOfBuffer; ptr++) {
			if (insideTag) {
				if (*ptr == '>') {
					insideTag = false;
					if (insideTagName) {
						tagName.append(start, ptr - start);
					}
					if (!tagHandler(tag(tagName))) {
						doBreak = true;
						break;
					}
					tagName.clear();
					start = ptr + 1;
				} if (insideTagName && isspace(*ptr)) {
					tagName.append(start, ptr - start);
					insideTagName = false;
				}
			} else {
				if (*ptr == '<') {
					insideTag = true;
					insideTagName = true;
					if (!characterDataHandler(start, ptr - start)) {
						doBreak = true;
						break;
					}
					start = ptr + 1;
				}
			}
		}
		if (doBreak) {
			break;
		}
		if (start != endOfBuffer) {
			if (!insideTag) {
				if (!characterDataHandler(start, endOfBuffer - start)) {
					break;
				}
			} else if (insideTagName) {
				tagName.append(start, endOfBuffer - start);
			}
		}
  } while (length == BUFSIZE);

	endDocumentHandler();

	stream.close();
}
