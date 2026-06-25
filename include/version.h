/*
 * MIT License
 *
 * Copyright (c) 2023-2026 Underview
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef UDO_VERSION_H
#define UDO_VERSION_H

#include "macros.h"

/*
 * @brief Returns version of library in C string format.
 *
 * @returns
 * 	Library version
 */
#define udo_version() \
	UDO_STRINGIFY(UDO_VERSION_MAJOR) "." \
	UDO_STRINGIFY(UDO_VERSION_MINOR) "." \
	UDO_STRINGIFY(UDO_VERSION_PATCH)


/*
 * @brief Returns library version major in integer format.
 *
 * @returns
 * 	Library version major
 */
#define udo_version_get_major() UDO_VERSION_MAJOR


/*
 * @brief Returns library version minor in integer format.
 *
 * @returns
 * 	Library version minor
 */
#define udo_version_get_minor() UDO_VERSION_MINOR


/*
 * @brief Returns library version patch in integer format.
 *
 * @returns
 * 	Library version patch
 */
#define udo_version_get_patch() UDO_VERSION_PATCH

#endif /* UDO_VERSION_H */
