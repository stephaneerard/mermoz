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

#include "urlserver/urlserver.hpp"

#include <curl/curl.h>
#include <ctime>

namespace mc = mermoz::common;

namespace mermoz
{
namespace urlserver
{

void urlserver(mermoz::common::AsyncQueue<std::string>* content_queue,
               mermoz::common::AsyncQueue<std::string>* url_queue,
               std::string user_agent,
               mc::MemSec* mem_sec,
               bool* status)
{
  std::set<std::string> visited;
  std::set<std::string> to_visit;

  std::map<std::string, Robots> robots;
  size_t counter {0};

  while (*status)
  {
    std::string content;
    content_queue->pop(content);
    (*mem_sec) -= content.size();

    std::string url;
    std::string text;
    std::string links;
    std::string http_status;
    mc::unpack(content, {&url, &text, &links, &http_status});

    mc::UrlParser root(url);

    std::set<std::string>::iterator it;
    if ((it = to_visit.find(url)) != to_visit.end())
      to_visit.erase(it);

    visited.insert(url);

    std::string link;
    for (auto c : links)
    {
      if ((c == ' ' || c == ',') && !link.empty())
      {
        if (visited.find(link) == visited.end()
            && to_visit.find(link) == to_visit.end())
        {
          mc::UrlParser up(link);

          if (robots.find(up.domain) == robots.end())
            robots.insert(std::pair<std::string, Robots>(up.domain,
                          Robots(up.scheme + "://" + up.domain, "Qwantify", user_agent)));

          bool is_allowed {false};

          try
          {
            is_allowed = robots[up.domain].is_allowed(link);
          }
          catch (...)
          {
            std::cerr << "Error for Robots check of " << link << std::endl;
          }

          if (is_allowed)
          {
            url_queue->push(link);
            (*mem_sec) += link.size();

            to_visit.insert(link);
          }
        }

        link = {""};
        continue;
      }

      link.push_back(c);
    }
  }
}

} // namespace urlserver
} // namespace mermoz
