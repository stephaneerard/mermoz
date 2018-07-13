/*
 * MIT License
 *
 * Copyright (c) 2018 Qwant Research
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
 *
 * Author:
 * Noel Martin (n.martin@qwantresearch.com)
 *
 */

#ifndef MERMOZ_URLSERVER_H__
#define MERMOZ_URLSERVER_H__

#include <string>
#include <vector>
#include <set>
#include <map>
#include <thread>

#include "common/common.hpp"

#include "urlserver/robots.hpp"

namespace mermoz
{
namespace urlserver
{

void urlserver(mermoz::common::AsyncQueue<std::string>* inurls_queue,
               mermoz::common::AsyncQueue<std::string>* outurls_queue,
               std::string user_agent,
               mermoz::common::MemSec* mem_sec,
               bool* status);

void robot_manager(mermoz::common::AsyncQueue<std::string>* robots_to_fetch,
                   mermoz::common::AsyncMap<std::string, Robots>* robots,
                   std::string user_agent,
                   bool* status);

} // namespace urlserver
} // namespace mermoz

#endif // MERMOZ_URLSERVER_H__
