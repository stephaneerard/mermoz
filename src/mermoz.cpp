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

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <unistd.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "common/common.hpp"
namespace mc = mermoz::common;

#include "spider/spider.hpp"
namespace ms = mermoz::spider;

#include "urlserver/urlserver.hpp"
namespace mu = mermoz::urlserver;

int main (int argc, char** argv)
{
  po::options_description desc("Allowed options");
  desc.add_options()
  ("help", "displays this message")
  ("fetchers", po::value<int>(), "number of fetchers")
  ("parsers", po::value<int>(), "number of parsers")
  ("seed", po::value<std::string>(), "URL from witch to begin")
  ("user-agent", po::value<std::string>(), "announce youself")
  ("kafka-file", po::value<std::string>(), "config file for the Kafka consumer & producer")
  ;

  po::variables_map vmap;
  po::store(po::parse_command_line(argc, argv, desc), vmap);
  po::notify(vmap);

  if (vmap.count("help"))
  {
    std::cout << desc << std::endl;
    return 1;
  }

  bool status = true;

  mc::AsyncQueue<std::string> url_queue;
  url_queue.push(vmap["seed"].as<std::string>());

  mc::AsyncQueue<std::string> content_queue;

  std::thread urlserver(mu::urlserver,
                        &content_queue,
                        &url_queue,
                        vmap["user-agent"].as<std::string>(),
                        &status);

  std::atomic<uint64_t> nfetched;
  nfetched = 0;

  std::atomic<uint64_t> nparsed;
  nparsed = 0;

  std::thread spider(ms::spider,
                     &url_queue,
                     &content_queue,
                     vmap["fetchers"].as<int>(),
                     vmap["parsers"].as<int>(),
                     vmap["user-agent"].as<std::string>(),
                     &nfetched,
                     &nparsed,
                     &status);

  std::ofstream ofp("log.out");
  
  ofp << "# time urls contents fetched parsed" << std::endl;

  while (status)
  {
    sleep(2);
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    ofp << tm.tm_hour*3600 + tm.tm_min*60 + tm.tm_sec << " ";
    ofp << url_queue.size() << " ";
    ofp << content_queue.size() << " ";
    ofp << nfetched << " ";
    ofp << nparsed << std::endl;
  }

  urlserver.join();
  spider.join();

  return 0;
}

